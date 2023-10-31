#ifndef TINYSTL_VECTOR_H_
#define TINYSTL_VECTOR_H_

#include <initializer_list>  // std::initializer_list

#include "iterator.h"
#include "memory.h"          // address_of 
#include "util.h"          
#include "exceptdef.h"
#include "algo.h"
#include "alloc.h"

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
template <class T, class Alloc = alloc>
class vector {

// 静态断言，用于在编译期间判断 T 是否为 bool 类型
static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in tinystl");

public:
    // vector 的嵌套型别定义
    typedef simple_alloc<T, Alloc>                       data_allocator;
    typedef simple_alloc<T, Alloc>                       allocator_type;

    // 已弃用
    // typedef typename allocator_type::value_type           value_type;
    // typedef typename allocator_type::pointer              pointer;
    // typedef typename allocator_type::const_pointer        const_pointer;
    // typedef typename allocator_type::reference            reference;
    // typedef typename allocator_type::const_reference      const_reference;
    // typedef typename allocator_type::size_type            size_type;
    // typedef typename allocator_type::difference_type      difference_type;

    typedef T                  value_type;
    typedef value_type*        pointer;
    typedef const value_type*  const_pointer;
    typedef value_type&        reference;
    typedef const value_type&  const_reference;
    typedef size_t             size_type;
    typedef ptrdiff_t          difference_type;

    // vector 的迭代器是普通指针
    typedef value_type*                                   iterator;
    typedef const value_type*                             const_iterator;
    typedef tinystl::reverse_iterator<iterator>           reverse_iterator;
    typedef tinystl::reverse_iterator<const_iterator>     const_reverse_iterator;

    allocator_type get_allocator() { return data_allocator(); }

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
    vector(std::initializer_list<T> ilist) { range_init(ilist.begin(), ilist.end()); };

    // =========================  赋值运算符  ========================= //

    vector& operator=(const vector& rhs);
    vector& operator=(vector&& rhs) noexcept;  // 移动赋值不会抛出异常
    
    vector& operator=(std::initializer_list<value_type> ilist) {
        vector tmp(ilist.begin(), ilist.end());
        swap(tmp);
        return *this;
    }


    // =========================  析构函数  ========================= //

    ~vector() {
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = end_ = cap_ = nullptr;
    }

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

    // 判断容器是否为空
    bool empty() const noexcept { return begin_ == end_; }   

    // 返回元素个数
    size_type size() const noexcept 
        { return static_cast<size_type>(end_ - begin_); }       

    // static_cast<size_type>(-1) 将 -1 转换为 size_type 类型，
    // 即将其转换为容器能够表示的最大无符号整数值。
    
    // 返回最大容量
    size_type max_size() const noexcept 
        { return static_cast<size_type>(-1) / sizeof(T); }      
    
    // 返回当前可用空间的大小
    size_type capacity() const noexcept 
        { return static_cast<size_type>(cap_ - begin_); }       
    
    // 重新分配空间
    void reserve(size_type n);  

    // 释放多余空间                                
    void shrink_to_fit();                                       

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
    
    // 返回指向首元素的指针
    pointer data() noexcept { return begin_; }
    // 返回指向首元素的常量指针                  
    const_pointer data() const noexcept { return begin_; }      

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
    };
    
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
    };

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
    template <class... Args>
    void reallocate_emplace(iterator pos, Args&& ...args);
    void reallocate_insert(iterator pos, const value_type& value);

    // insert
    iterator fill_insert(iterator pos, size_type n, const value_type& value);
    template <class Iter>
    void     copy_insert(iterator pos, Iter first, Iter last);

    // shrink_to_fit
    void     reinsert(size_type size);

};  // class vector

// =========================  函数实现  ====================== //

// 复制赋值操作符
template <class T, class Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(const vector& rhs) {
    if (this != &rhs) {
        const auto len = rhs.size();
        // 如果 rhs 比当前容量大，则重新分配内存
        if (len > capacity()) {
            vector tmp(rhs.begin(), rhs.end());
            swap(tmp);
        }
        // 如果 rhs 比当前容量小，则回收多余内存
        else if (size() >= len) {
            auto i = tinystl::copy(rhs.begin(), rhs.end(), begin());
            // data_allocator::destroy(i, end_);
            tinystl::destroy(i, end_);
            end_ = begin_ + len;
        }
        // 如果 rhs 比当前容量小，但是比当前元素多，则需要重新分配内存，并复制元素
        else {
            tinystl::copy(rhs.begin(), rhs.begin() + size(), begin_);
            tinystl::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
            cap_ = end_ = begin_ + len;
        }
    }
    return *this;
}

// 移动赋值操作符
template <class T, class Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(vector&& rhs) noexcept {
    destroy_and_recover(begin_, end_, cap_ - begin_);  // 回收内存
    begin_ = rhs.begin_;  // 移动资源
    end_ = rhs.end_;
    cap_ = rhs.cap_;
    rhs.begin_ = nullptr;  // 将 rhs 置于可析构状态
    rhs.end_ = nullptr;
    rhs.cap_ = nullptr;
    return *this;  // 返回自身
}

/// @brief 重新分配空间
/// @tparam T  元素类型
/// @param n  新的空间大小
template <class T, class Alloc>
void vector<T, Alloc>::reserve(size_type n) {
    if (capacity() < n) {
        THROW_LENGTH_ERROR_IF(n > max_size(), 
            "n can not larger than max_size() in vector<T>::reserve(n)");
        const auto old_size = size();
        auto tmp = data_allocator::allocate(n);  // 重新分配内存
        tinystl::uninitialized_move(begin_, end_, tmp);  // 移动元素
        destroy_and_recover(begin_, end_, cap_ - begin_);  // 回收内存
        // 重新设置迭代器
        begin_ = tmp;
        end_ = tmp + old_size;
        cap_ = begin_ + n;
    }
}

/// @brief 释放多余空间
/// @tparam T 元素类型 
template <class T, class Alloc>
void vector<T, Alloc>::shrink_to_fit() {
    if (end_ < cap_) {
        reinsert(size());
    }
}


/// @brief 在 pos 处就地构造元素
/// @tparam T  元素类型
/// @param pos  插入位置
/// @param ...args  元素的构造参数
/// @return  返回指向新构造元素的迭代器
template <class T, class Alloc>
template <class... Args>
typename vector<T, Alloc>::iterator vector<T, Alloc>::emplace(
    const_iterator pos, Args&& ...args) {
    TINYSTL_DEBUG(pos >= begin() && pos <= end());
    iterator xpos = const_cast<iterator>(pos);
    const size_type n = xpos - begin_;

    // 构造位置为 end_，直接构造元素
    if (end_ != cap_ && xpos == end_) {
        // data_allocator::construct(tinystl::address_of(*end_), tinystl::forward<Args>(args)...);
        tinystl::construct(tinystl::address_of(*end_), tinystl::forward<Args>(args)...);
        ++end_;
    }
    // 构造位置不为 end_，则需要移动元素
    else if (end_ != cap_) {
        auto new_end = end_;
        // data_allocator::construct(tinystl::address_of(*end_), *(end_ - 1));
        tinystl::construct(tinystl::address_of(*end_), *(end_ - 1));
        ++new_end;
        tinystl::copy_backward(xpos, end_ - 1, end_);
        *xpos = value_type(tinystl::forward<Args>(args)...);
        end_ = new_end;
    }
    // 构造位置为 end_，但是空间已满，则需要重新分配内存
    else {
        reallocate_emplace(xpos, tinystl::forward<Args>(args)...);
    }
    return begin() + n;
} 

/// @brief 在尾部就地构造元素
/// @tparam T  元素类型
/// @param ...args  元素的构造参数
template <class T, class Alloc>
template <class ...Args>
void vector<T, Alloc>::emplace_back(Args&& ...args) {
    if (end_ < cap_) {
        // data_allocator::construct(tinystl::address_of(*end_), tinystl::forward<Args>(args)...);
        tinystl::construct(tinystl::address_of(*end_), tinystl::forward<Args>(args)...);
        ++end_;
    }
    else {
        reallocate_emplace(end_, tinystl::forward<Args>(args)...);
    }
}

/// @brief 在尾部插入元素
/// @tparam T  元素类型
/// @param value  元素的值
template <class T, class Alloc>
void vector<T, Alloc>::push_back(const value_type& value) {
    // 如果空间足够就直接构造元素
    if (end_ != cap_) {
        data_allocator::construct(tinystl::address_of(*end_), value);
        ++end_;
    }
    // 否则重新分配内存，并构造元素
    else {
        reallocate_insert(end_, value);
    }
}

/// @brief 弹出尾部元素
/// @tparam T  元素类型
template <class T, class Alloc>
void vector<T, Alloc>::pop_back() {
    TINYSTL_DEBUG(!empty());
    // data_allocator::destroy(end_ - 1);
    tinystl::destroy(end_ - 1);
    --end_;
}

/// @brief 在 pos 处插入元素
/// @tparam T  元素类型
/// @param pos  插入位置
/// @param value  元素的值
/// @return  返回指向新构造元素的迭代器
template <class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(const_iterator pos, const value_type& value) {
    // 确保 pos 在 [begin(), end()) 内
    TINYSTL_DEBUG(pos >= begin() && pos <= end());
    iterator xpos = const_cast<iterator>(pos);
    const size_type n = xpos - begin_;
    // 构造位置为 end_，直接构造元素
    if (end_ != cap_ && xpos == end_) {
        data_allocator::construct(tinystl::address_of(*end_), value);
        ++end_;
    }
    // 构造位置不为 end_，则需要移动元素
    else if (end_ != cap_) {
        auto new_end = end_;
        // 在 end_ 处构造元素
        data_allocator::construct(tinystl::address_of(*end_), *(end_ - 1));
        ++new_end;
        auto value_copy = value;  // 避免元素因以下复制操作而被改变
        // 将 [xpos, end_ - 1) 的元素向后移动一位
        tinystl::copy_backward(xpos, end_ - 1, end_);
        // 在 xpos 处构造元素
        *xpos = tinystl::move(value_copy);
        // 更新 end_ 的位置
        end_ = new_end;
    }
    // 空间已满，则需要重新分配内存并构造元素
    else {
        reallocate_insert(xpos, value);
    }
    return begin() + n;
} 

/// @brief 删除 pos 位置上的元素
/// @tparam T  元素类型
/// @param pos  删除位置
/// @return  返回指向被删除元素的下一个元素的迭代器
template <class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(const_iterator pos) {
    // 确保 pos 在 [begin(), end()) 内
    TINYSTL_DEBUG(pos >= begin() && pos < end());
    iterator xpos = begin_ + (pos - begin());
    // 将 [xpos + 1, end_) 的元素向前移动一位
    tinystl::move(xpos + 1, end_, xpos);
    // 销毁 end_ - 1处的元素
    // data_allocator::destroy(end_ - 1);
    tinystl::destroy(end_ - 1);
    // 更新 end_ 的位置
    --end_;
    return xpos;
}

/// @brief 删除 [first, last) 区间上的元素
/// @tparam T  元素类型
/// @param first  删除区间的起始位置
/// @param last  删除区间的终止位置
/// @return  返回指向被删除元素（区间）的下一个元素的迭代器
template <class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(const_iterator first, const_iterator last) {
    // 确保 first 和 last 在 [begin(), end()) 内，且 first 不大于 last
    TINYSTL_DEBUG(first >= begin() && first <= end() && !(last < first));
    const auto n = first - begin();
    iterator r = begin_ + (first - begin());
    // tinystl::move(r + (last - first), end_, r) 将被删除区间后面的元素向前移动，返回移动后的尾部位置
    // destroy(pos, end_) 销毁 [pos, end_) 区间上的元素
    // data_allocator::destroy(tinystl::move(r + (last - first), end_, r), end_);
    tinystl::destroy(tinystl::move(r + (last - first), end_, r), end_);
    // 更新 end_ 的位置
    end_ = end_ - (last - first);
    return begin_ + n;
}

/// @brief 重置容器大小，如果新的大小大于当前大小，则在尾部插入元素，否则删除尾部元素
/// @tparam T  元素类型
/// @param new_size  新的容器大小
/// @param value  新增元素的值
template <class T, class Alloc>
void vector<T, Alloc>::resize(size_type new_size, const value_type& value) {
    if (new_size < size()) {
        erase(begin() + new_size, end());
    }
    else {
        insert(end(), new_size - size(), value);
    }
}

/// @brief 交换两个 vector
/// @tparam T  元素类型
/// @param rhs  交换的另一个 vector
template <class T, class Alloc>
void vector<T, Alloc>::swap(vector<T, Alloc>& rhs) noexcept {
    if (this != &rhs) {
        tinystl::swap(begin_, rhs.begin_);
        tinystl::swap(end_, rhs.end_);
        tinystl::swap(cap_, rhs.cap_);
    }
}

// =========================  辅助函数实现  ====================== //

/// @brief 初始化 vector, 具有 commit or rollback 机制
/// @tparam T  元素类型
template <class T, class Alloc>
void vector<T, Alloc>::try_init() noexcept {
    try {
        begin_ = data_allocator::allocate(16);
        end_ = begin_;
        cap_ = begin_ + 16;
    }
    catch (...) {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
    }
}

/// @brief 初始化 vector, 并设置初始元素个数与容量, 失败则抛出异常
/// @tparam T 元素类型
/// @param size  元素个数
/// @param cap  容量
template <class T, class Alloc>
void vector<T, Alloc>::init_space(size_type size, size_type cap) {
    try {
        begin_ = data_allocator::allocate(cap);
        end_ = begin_ + size;
        cap_ = begin_ + cap;
    }
    catch (...) {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
        throw;
    }
}

/// @brief 以 n 个 value 初始化 vector
/// @tparam T  元素类型
/// @param n  元素个数
/// @param value  元素的值
template <class T, class Alloc>
void vector<T, Alloc>::fill_init(size_type n, const value_type& value) {
    const size_type init_size = tinystl::max(static_cast<size_type>(16), n);
    init_space(n, init_size);
    tinystl::uninitialized_fill_n(begin_, n, value);
}

/// @brief 以 [first, last) 区间初始化 vector
/// @tparam T  元素类型
/// @param first  区间起始位置
/// @param last  区间终止位置
template <class T, class Alloc>
template <class Iter>
void vector<T, Alloc>::range_init(Iter first, Iter last) {
    const size_type len = tinystl::distance(first, last);
    const size_type init_size = tinystl::max(static_cast<size_type>(16), len);
    init_space(len, init_size);
    tinystl::uninitialized_copy(first, last, begin_);
}

/// @brief 清除 [first, last) 区间上的元素，并回收内存
/// @tparam T  元素类型
/// @param first  区间起始位置
/// @param last  区间终止位置
/// @param n  区间长度
template <class T, class Alloc>
void vector<T, Alloc>::destroy_and_recover(iterator first, iterator last, size_type n) {
    // data_allocator::destroy(first, last);
    // data_allocator::deallocate(first, n);
    tinystl::destroy(first, last);
    data_allocator::deallocate(first, n);
}

/// @brief 增加 vector 容量大小，每一次默认增加 1.5 倍
/// @tparam T  元素类型
/// @param add_size  增加的大小
/// @return  返回新的容量大小
template <class T, class Alloc>
typename vector<T, Alloc>::size_type vector<T, Alloc>::get_new_cap(size_type add_size) {
    const auto old_size = capacity();
    THROW_LENGTH_ERROR_IF(old_size > max_size() - add_size, 
        "vector<T>'s size too big");
    if (old_size > max_size() - old_size / 2) {
        return old_size + add_size > max_size() - 16 ? 
            old_size + add_size : old_size + add_size + 16;
    }
    // 每一次默认增加 1.5 倍
    const auto new_size = old_size == 0 ? 
        tinystl::max(add_size, static_cast<size_type>(16)) : 
        tinystl::max(old_size + old_size / 2, old_size + add_size);
    return new_size;
}

/// @brief 以 n 个 value 赋值 vector
/// @tparam T  元素类型
/// @param n  元素个数
/// @param value  元素的值
template <class T, class Alloc>
void vector<T, Alloc>::fill_assign(size_type n, const value_type& value) {
    if (n > capacity()) {
        vector tmp(n, value);
        swap(tmp);
    }
    else if (n > size()) {
        tinystl::fill(begin(), end(), value);
        end_ = tinystl::uninitialized_fill_n(end_, n - size(), value);
    }
    else {
        erase(tinystl::fill_n(begin(), n, value), end());
    }
}

/// @brief 以 [first, last) 区间赋值 vector
/// @tparam T  元素类型
/// @param first  区间起始位置
/// @param last  区间终止位置
/// @param   区间长度
template <class T, class Alloc>
template <class Iter>
void vector<T, Alloc>::copy_assign(Iter first, Iter last, tinystl::input_iterator_tag) {
    auto cur = begin_;
    for (; first != last && cur != end_; ++first, ++cur) {
        *cur = *first;
    }
    if (first == last) {
        erase(cur, end_);
    }
    else {
        insert(end_, first, last);
    }
}

template <class T, class Alloc>
template <class Iter>
void vector<T, Alloc>::copy_assign(Iter first, Iter last, tinystl::forward_iterator_tag) {
    const auto len = tinystl::distance(first, last);
    if (len > capacity()) {
        vector tmp(first, last);
        swap(tmp);
    }
    else if (size() >= len) {
        auto new_end = tinystl::copy(first, last, begin_);
        // data_allocator::destroy(new_end, end_);
        tinystl::destroy(new_end, end_);
        end_ = new_end;
    }
    else {
        auto mid = first;
        tinystl::advance(mid, size());
        tinystl::copy(first, mid, begin_);
        auto new_end = tinystl::uninitialized_copy(mid, last, end_);
        end_ = new_end;
    }
}

/// @brief 重新分配内存，并在 pos 处就地构造元素
/// @tparam T  元素类型
/// @param pos  构造位置
/// @param ...args  元素的构造参数
template <class T, class Alloc>
template <class... Args>
void vector<T, Alloc>::reallocate_emplace(iterator pos, Args&& ...args) {
    const auto new_size = get_new_cap(1);
    auto new_begin = data_allocator::allocate(new_size);
    auto new_end = new_begin;
    try {
        new_end = tinystl::uninitialized_move(begin_, pos, new_begin);
        // data_allocator::construct(tinystl::address_of(*new_end), tinystl::forward<Args>(args)...);
        tinystl::construct(tinystl::address_of(*new_end), tinystl::forward<Args>(args)...);
        ++new_end;
        new_end = tinystl::uninitialized_move(pos, end_, new_end);
    }
    catch (...) {
        data_allocator::deallocate(new_begin, new_size);
        throw;
    }
    destroy_and_recover(begin_, end_, cap_ - begin_);  // 回收内存
    begin_ = new_begin;
    end_ = new_end;
    cap_ = new_begin + new_size;
}

/// @brief 重新分配内存，并在 pos 处插入元素
/// @tparam T  元素类型
/// @param pos  插入位置
/// @param value  元素的值
template <class T, class Alloc>
void vector<T, Alloc>::reallocate_insert(iterator pos, const value_type& value) {
    const auto new_size = get_new_cap(1);
    auto new_begin = data_allocator::allocate(new_size);
    auto new_end = new_begin;
    const value_type& value_copy = value;
    try {
        // 将 pos 之前的元素移动到新的位置
        new_end = tinystl::uninitialized_move(begin_, pos, new_begin);
        // 在 pos 处构造元素
        data_allocator::construct(tinystl::address_of(*new_end), value_copy);
        ++new_end;
        // 将 pos 之后的元素移动到新的位置
        new_end = tinystl::uninitialized_move(pos, end_, new_end);
    }
    catch (...) {
        data_allocator::deallocate(new_begin, new_size);
        throw;
    }
    // 回收旧的内存
    destroy_and_recover(begin_, end_, cap_ - begin_);
    // 重新设置迭代器
    begin_ = new_begin;
    end_ = new_end;
    cap_ = new_begin + new_size;
}

/// @brief 在 pos 处插入 n 个 value
/// @tparam T  元素类型
/// @param pos  插入位置
/// @param n  插入元素个数
/// @param value  元素的值
/// @return  返回指向新构造元素的迭代器
template <class T, class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::fill_insert(
    iterator pos, size_type n, const value_type& value) {
    if (n == 0) return pos;
    const size_type xpos = pos - begin_;
    const value_type value_copy = value;  // 避免被覆盖

    // 剩余空间大于等于新增元素个数
    if (static_cast<size_type>(cap_ - end_) >= n) {
        const size_type after_elems = end_ - pos;
        auto old_end = end_;
        if (after_elems > n) {
            tinystl::uninitialized_copy(end_ - n, end_, end_);
            end_ += n;
            tinystl::move_backward(pos, old_end - n, old_end);
            tinystl::uninitialized_fill_n(pos, n, value_copy);
        }
        else {
            end_ = tinystl::uninitialized_fill_n(end_, n - after_elems, value_copy);
            end_ = tinystl::uninitialized_move(pos, old_end, end_);
            tinystl::uninitialized_fill_n(pos, after_elems, value_copy);
        }
    }
    // 剩余空间不足
    else {
        const auto new_size = get_new_cap(n);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try {
            new_end = tinystl::uninitialized_move(begin_, pos, new_begin);
            new_end = tinystl::uninitialized_fill_n(new_end, n, value_copy);
            new_end = tinystl::uninitialized_move(pos, end_, new_end);
        }
        catch (...) {
            destroy_and_recover(new_begin, new_end, new_size);
            throw;
        }
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = new_begin + new_size;
    }
    return begin_ + xpos;
}

/// @brief 在 pos 处插入 [first, last) 区间上的元素
/// @tparam T  元素类型
/// @param pos  插入位置
/// @param first  区间起始位置
/// @param last  区间终止位置
template <class T, class Alloc>
template <class Iter>
void vector<T, Alloc>::copy_insert(iterator pos, Iter first, Iter last) {
    if (first == last) return;
    const auto n = tinystl::distance(first, last);
    // 剩余空间大于等于新增元素个数
    if (static_cast<size_type>(cap_ - end_) >= n) {
        const auto after_elems = end_ - pos;
        auto old_end = end_;
        if (after_elems > n) {
            end_ = tinystl::uninitialized_copy(end_ - n, end_, end_);
            tinystl::move_backward(pos, old_end - n, old_end);
            tinystl::uninitialized_copy(first, last, pos);
        }
        else {
            auto mid = first;
            tinystl::advance(mid, after_elems);
            end_ = tinystl::uninitialized_copy(mid, last, end_);
            end_ = tinystl::uninitialized_move(pos, old_end, end_);
            tinystl::uninitialized_copy(first, mid, pos);
        }
    }
    // 剩余空间不足
    else {
        const auto new_size = get_new_cap(n);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try {
            new_end = tinystl::uninitialized_move(begin_, pos, new_begin);
            new_end = tinystl::uninitialized_copy(first, last, new_end);
            new_end = tinystl::uninitialized_move(pos, end_, new_end);
        }
        catch (...) {
            destroy_and_recover(new_begin, new_end, new_size);
            throw;
        }
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = new_begin + new_size;
    }
}

/// @brief 重新分配内存
/// @tparam T  元素类型
/// @param size  新的内存大小
template <class T, class Alloc>
void vector<T, Alloc>::reinsert(size_type size) {
    auto new_begin = data_allocator::allocate(size);
    try {
        tinystl::uninitialized_move(begin_, end_, new_begin);
    }
    catch (...) {
        data_allocator::deallocate(new_begin, size);
        throw;
    }
    data_allocator::deallocate(begin_, cap_ - begin_);
    begin_ = new_begin;
    end_ = begin_ + size;
    cap_ = begin_ + size;
}

// =========================  比较操作符  ====================== //

template <class T, class Alloc>
bool operator==(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return lhs.size() == rhs.size() && 
        tinystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, class Alloc>
bool operator<(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return tinystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T, class Alloc>
bool operator!=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(lhs == rhs);
}

template <class T, class Alloc>
bool operator>(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return rhs < lhs;
}

template <class T, class Alloc>
bool operator<=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(rhs < lhs);
}

template <class T, class Alloc>
bool operator>=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(lhs < rhs);
}

// =========================  重载 swap  ====================== //

template <class T, class Alloc>
void swap(vector<T, Alloc>& lhs, vector<T, Alloc>& rhs) noexcept {
    lhs.swap(rhs);  // 可以方便的交换两个 vector
}

}  // namespace tinystl

#endif  // TINYSTL_VECTOR_H_