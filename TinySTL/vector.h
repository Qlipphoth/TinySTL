#ifndef TINYSTL_VECTOR_H_
#define TINYSTL_VECTOR_H_

#include <initializer_list>  // std::initializer_list

#include "allocator.h"
#include "iterator.h"
#include "memory.h"
#include "util.h"
#include "exceptdef.h"
#include "algo.h"

namespace tinystl {

    // 取消对 max 和 min 的宏定义
    #ifdef max
    #pragma message("#undefing marco max")
    #undef max
    #endif // max

    #ifdef min
    #pragma message("#undefing marco min")
    #undef min
    #endif // min

    // 模板类 vector
    template <class T>
    class vector {
        // 静态断言，用于在编译期间判断 T 是否为 bool 类型
        static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in mystl");
        public:
            // vector 的嵌套型别定义
            typedef tinystl::allocator<T>                         allocator_type;
            typedef tinystl::allocator<T>                         data_allocator;

            typedef typename allocator_type::value_type           value_type;
            typedef typename allocator_type::pointer              pointer;
            typedef typename allocator_type::const_pointer        const_pointer;
            typedef typename allocator_type::reference            reference;
            typedef typename allocator_type::const_reference      const_reference;
            typedef typename allocator_type::size_type            size_type;
            typedef typename allocator_type::difference_type      difference_type;

            typedef value_type*                                   iterator;
            typedef const value_type*                             const_iterator;
            typedef tinystl::reverse_iterator<iterator>           reverse_iterator;
            typedef tinystl::reverse_iterator<const_iterator>     const_reverse_iterator;

            allocator_type get_allocator() { return allocator_type(); }

        private:
            iterator begin_;        // 表示目前使用空间的头
            iterator end_;          // 表示目前使用空间的尾
            iterator cap_;          // 表示目前可用空间的尾

        public:
            
            // =========================  构造函数  ========================= // 
            
            vector() noexcept { try_init(); }  // 不会引发异常
            explicit vector(size_type n) { fill_init(n, value_type()); }  // explicit 防止隐式转换
            vector(size_type n, const value_type& value) { fill_init(n, value); }
            
            // 这里使用 std::enable_if 来确保只有当迭代器类型满足输入迭代器的要求时，该函数模板才会被实例化。
            template <class Iter, typename std::enable_if<
                tinystl::is_input_iterator<Iter>::value, int>::type = 0>
            vector(Iter first, Iter last) {
                TINYSTL_DEBUG(!(last < first));
                range_init(first, last);
            }
            
            vector(const vector& rhs) { range_init(rhs.begin_, rhs.end_); }
            
            /// @brief 移动构造函数
            /// @param rhs  右值引用
            vector(vector&& rhs) noexcept : begin_(rhs.begin_), end_(rhs.end_), cap_(rhs.cap_) {
                rhs.begin_ = nullptr;
                rhs.end_ = nullptr;
                rhs.cap_ = nullptr;
            }

            /// @brief 列表初始化
            /// @param ilist  列表
            vector(std::initializer_list<T> ilist) { range_init(ilist.begin(), ilist.end()); }

            // =========================  析构函数  ========================= //

            ~vector() {
                destroy_and_recover(begin_, end_, cap_ - begin_);
                begin_ = end_ = cap_ = nullptr;
            }

            // =========================  赋值运算符  ========================= //

            vector& operator=(vector& rhs);
            vector& operator=(vector&& rhs) noexcept;  // 移动赋值不会抛出异常
            vector& operator=(std::initializer_list<T> ilist);

        public:
            // =========================  迭代器相关操作  ====================== //

            iterator begin() noexcept { return begin_; }                // 返回首元素的迭代器
            const_iterator begin() const noexcept { return begin_; }    // 返回首元素的常量迭代器
            iterator end() noexcept { return end_; }                    // 返回尾后元素的迭代器
            const_iterator end() const noexcept { return end_; }        // 返回尾后元素的常量迭代器

            reverse_iterator rbegin() noexcept 
                { return reverse_iterator(end()); }                     // 返回首元素的逆向迭代器
            const_reverse_iterator rbegin() const noexcept 
                { return const_reverse_iterator(end()); }               // 返回首元素的常量逆向迭代器
            reverse_iterator rend() noexcept 
                { return reverse_iterator(begin()); }                   // 返回尾后元素的逆向迭代器
            const_reverse_iterator rend() const noexcept 
                { return const_reverse_iterator(begin()); }             // 返回尾后元素的常量逆向迭代器

            const_iterator cbegin() const noexcept { return begin(); }  // 返回首元素的常量迭代器
            const_iterator cend() const noexcept { return end(); }      // 返回尾后元素的常量迭代器
            const_reverse_iterator crbegin() const noexcept 
                { return const_reverse_iterator(end()); }               // 返回首元素的常量逆向迭代器
            const_reverse_iterator crend() const noexcept 
                { return const_reverse_iterator(begin()); }             // 返回尾后元素的常量逆向迭代器

            // =========================  容量相关操作  ====================== //

            bool empty() const noexcept { return begin_ == end_; }      // 判断是否为空
            size_type size() const noexcept 
                { return static_cast<size_type>(end_ - begin_); }       // 返回元素个数

            // static_cast<size_type>(-1) 将 -1 转换为 size_type 类型，
            // 即将其转换为容器能够表示的最大无符号整数值。
            size_type max_size() const noexcept 
                { return static_cast<size_type>(-1) / sizeof(T); }      // 返回最大容量
            size_type capacity() const noexcept 
                { return static_cast<size_type>(cap_ - begin_); }       // 返回当前可用空间的大小
            void reserve(size_type n);                                  // 重新分配空间
            void shrink_to_fit();                                       // 释放多余空间

            // =========================  元素访问相关操作  ====================== //

            /// @brief 通过索引访问元素，返回对应元素的引用
            /// @param n  索引
            /// @return  对应元素的引用
            reference operator[](size_type n) {
                TINYSTL_DEBUG(n < size());
                return *(begin_ + n);
            }

            /// @brief 通过索引访问元素，返回对应元素的常量引用
            /// @param n  索引
            /// @return  对应元素的常量引用
            const_reference operator[](size_type n) const {
                TINYSTL_DEBUG(n < size());
                return *(begin_ + n);
            }

            /// @brief 另一种访问元素的方式，返回对应元素的引用
            /// @param n  索引
            /// @return  对应元素的引用
            reference at(size_type n) {
                THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
                return (*this)[n];
            }

            /// @brief 返回首元素的引用
            /// @return  首元素的引用
            reference front() {
                TINYSTL_DEBUG(!empty());
                return *begin_;
            }

            /// @brief 返回首元素的常量引用
            /// @return  首元素的常量引用
            const_reference front() const {
                TINYSTL_DEBUG(!empty());
                return *begin_;
            }

            /// @brief 返回尾元素的引用
            /// @return  尾元素的引用
            reference back() {
                TINYSTL_DEBUG(!empty());
                return *(end_ - 1);
            }

            /// @brief 返回尾元素的常量引用
            /// @return  尾元素的常量引用  
            const_reference back() const {
                TINYSTL_DEBUG(!empty());
                return *(end_ - 1);
            }

            pointer data() noexcept { return begin_; }                  // 返回指向首元素的指针
            const_pointer data() const noexcept { return begin_; }      // 返回指向首元素的常量指针

            // =========================  修改容器相关操作  ====================== //

            // assign

            void assign(size_type n, const value_type& value) {
                fill_assign(n, value);
            }

            template <class Iter, typename std::enable_if<
                tinystl::is_input_iterator<Iter>::value, int>::type = 0>
            void assign(Iter first, Iter last) {
                TINYSTL_DEBUG(!(last < first));
                copy_assign(first, last, tinystl::iterator_category(first));
            }

            void assign(std::initializer_list<value_type> ilist) {
                copy_assign(ilist.begin(), ilist.end(), tinystl::forward_iterator_tag{});
            }
            
            // emplace / emplace_back
            // emplace 用于在指定位置构造元素，emplace_back 用于在尾部构造元素

            template <class... Args>
            iterator emplace(const_iterator pos, Args&&... args);

            template <class... Args>
            void emplace_back(Args&&... args);

            // push_back / pop_back
            
            void push_back(const value_type& value);
            void push_back(value_type&& value) { emplace_back(tinystl::move(value)); }

            void pop_back();

            // insert

            iterator insert(const_iterator pos, const value_type& value);
            iterator insert(const_iterator pos, value_type&& value) {
                return emplace(pos, tinystl::move(value));
            }

            iterator insert(const_iterator pos, size_type n, const value_type& value) {
                TINYSTL_DEBUG(pos >= begin() && pos <= end());
                return fill_insert(const_cast<iterator>(pos), n, value);
            }

            template <class Iter, typename std::enable_if<
                tinystl::is_input_iterator<Iter>::value, int>::type = 0>
            void     insert(const_iterator pos, Iter first, Iter last) {
                TINYSTL_DEBUG(pos >= begin() && pos <= end() && !(last < first));
                copy_insert(const_cast<iterator>(pos), first, last);  // const_cast 用于去除 const
            }

            // erase / clear
            iterator erase(const_iterator pos);
            iterator erase(const_iterator first, const_iterator last);
            void     clear() { erase(begin(), end()); }

            // resize / reverse
            void resize(size_type new_size) { return resize(new_size, value_type()); }
            void resize(size_type new_size, const value_type& value);

            void reverse() { tinystl::reverse(begin(), end()); }

            // swap
            void swap(vector& rhs) noexcept;

        private:
            // =========================  辅助函数  ====================== //

            // initialize / destroy
            void try_init() noexcept;

            void init_space(size_type size, size_type cap);

            void fill_init(size_type n, const value_type& value);

            template <class Iter>
            void range_init(Iter first, Iter last);

            void destroy_and_recover(iterator first, iterator last, size_type n);

            // calculate the growth size
            size_type get_new_cap(size_type add_size);

            // assign
            void fill_assign(size_type n, const value_type& value);

            template <class Iter>
            void copy_assign(Iter first, Iter last, tinystl::input_iterator_tag);

            template <class Iter>
            void copy_assign(Iter first, Iter last, tinystl::forward_iterator_tag);

            // reallocate
            void reallocate_emplace(iterator pos, const value_type& value);
            void reallocate_insert(iterator pos, const value_type& value);

            // insert
            iterator fill_insert(iterator pos, size_type n, const value_type& value);
            template <class Iter>
            void     copy_insert(iterator pos, Iter first, Iter last);

            // shrink_to_fit
            void     reinsert(size_type size);

    };

    // =========================  函数实现  ====================== //

    // 赋值操作符
    template <class T>
    vector<T>& vector<T>::operator=(const vector& rhs) {
        if (this != &rhs) {
            const auto len = rhs.size();
            if (len > capacity()) {
                vector tmp(rhs.begin(), rhs.end());
                swap(tmp);
            }
            else if (size() >= len) {
                auto i = tinystl::copy(rhs.begin(), rhs.end(), begin());
                data_allocator::destory(i, end_);
                end_ = begin_ + len;
            }
            else {
                
            }
        }
    }









}




#endif