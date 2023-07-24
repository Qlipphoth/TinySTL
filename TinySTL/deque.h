#ifndef TINYSTL_DEQUE_H_
#define TINYSTL_DEQUE_H_

// 这个头文件包含了一个模板类 deque
// deque: 双端队列

// notes:
//
// 异常保证：
// mystl::deque<T> 满足基本异常保证，部分函数无异常保证，并对以下等函数做强异常安全保证：
//   * emplace_front
//   * emplace_back
//   * emplace
//   * push_front
//   * push_back
//   * insert

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "util.h"
#include "exceptdef.h"

namespace tinystl {

#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

// deque map 初始化的大小
#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

/// @brief 确定 deque 的缓冲区大小
/// @tparam T 
template <class T>
struct deque_buf_size {
    static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
};

template <class T, class Ref, class Ptr>
struct deque_iterator : public iterator<random_access_iterator_tag, T> {
    
    typedef deque_iterator<T, T&, T*>             iterator;
    typedef deque_iterator<T, const T&, const T*> const_iterator;
    typedef deque_iterator                        self;

    typedef T              value_type;
    typedef Ptr            pointer;
    typedef Ref            reference;
    typedef size_t         size_type;
    typedef ptrdiff_t      difference_type;
    typedef T*             value_pointer;
    typedef T**            map_pointer;

    static const size_type buffer_size = deque_buf_size<T>::value;

    // 迭代器所含成员数据
    value_pointer cur;      // 指向当前缓冲区的当前元素
    value_pointer first;    // 指向当前缓冲区的头
    value_pointer last;     // 指向当前缓冲区的尾（最后一个元素的下一个位置）
    map_pointer   node;     // 指向当前缓冲区的管控中心

    // =================================== 构造、复制、移动 =============================== //
    
    /// @brief 默认构造函数，将四个指针都置为 nullptr
    deque_iterator() noexcept 
        : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

    /// @brief 构造函数，初始化迭代器
    deque_iterator(value_pointer v, map_pointer n) noexcept
        : cur(v), first(*n), last(*n + buffer_size), node(n) {}

    /// @brief 拷贝构造函数
    deque_iterator(const iterator& rhs) noexcept
        : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

    /// @brief 拷贝构造函数
    /// @param rhs 
    deque_iterator(const const_iterator& rhs) noexcept
        : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

    /// @brief 移动构造函数
    deque_iterator(iterator&& rhs) noexcept
        : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {
        rhs.cur = nullptr;
        rhs.first = nullptr;
        rhs.last = nullptr;
        rhs.node = nullptr;
    }

    // 转到另一个缓冲区
    void set_node(map_pointer new_node) {
        node = new_node;
        first = *new_node;
        last = first + difference_type(buffer_size);
    }

    // =================================== 重载操作符 =================================== //

    self& operator=(const iterator& rhs) noexcept {
        if (this != &rhs) {
            cur = rhs.cur;
            first = rhs.first;
            last = rhs.last;
            node = rhs.node;
        }
        return *this;
    }

    self& operator=(const const_iterator& rhs) noexcept {
        if (this != &rhs) {
            cur = rhs.cur;
            first = rhs.first;
            last = rhs.last;
            node = rhs.node;
        }
        return *this;
    }

    reference operator*() const noexcept { return *cur; }
    pointer operator->() const noexcept { return cur; }

    difference_type operator-(const self& x) {
        return static_cast<difference_type>(buffer_size) * (node - x.node - 1) + 
            (cur - first) + (x.last - x.cur);
    }

    self& operator++() {
        ++cur;
        // 到达当前缓冲区的尾部，切换到下一个缓冲区
        if (cur == last) {
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--() {
        // 到达当前缓冲区的头部，切换到上一个缓冲区
        if (cur == first) {
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    self& operator+=(difference_type n) {
        const auto offset = n + (cur - first);
        // 如果偏移量在当前缓冲区内
        if (offset >= 0 && offset < static_cast<difference_type>(buffer_size)) {
            cur += n;
        } 
        // 否则要跳转到其他缓冲区
        else {
            const auto node_offset = offset > 0 ? offset / static_cast<difference_type>(buffer_size)
                : -static_cast<difference_type>((-offset - 1) / buffer_size) - 1;
            set_node(node + node_offset);
            cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size));
        }
        return *this;
    }

    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }

    self& operator-=(difference_type n) { 
        return *this += -n; 
    }

    self operator-(difference_type n) const {
        self tmp = *this;
        return tmp -= n;
    }

    reference operator[](difference_type n) const { 
        return *(*this + n); 
    }

    bool operator==(const self& rhs) const noexcept { return cur == rhs.cur; }
    bool operator!=(const self& rhs) const noexcept { return !(*this == rhs); }
    bool operator<(const self& rhs) const noexcept {
        return (node == rhs.node) ? (cur < rhs.cur) : (node < rhs.node);
    }
    bool operator>(const self& rhs) const noexcept { return rhs < *this; }
    bool operator<=(const self& rhs) const noexcept { return !(rhs < *this); }
    bool operator>=(const self& rhs) const noexcept { return !(*this < rhs); }

};  // struct deque_iterator

template <class T>
class deque {

public: // deque 的型别定义

    typedef tinystl::allocator<T>                     allocator_type;
    typedef tinystl::allocator<T>                     data_allocator;
    typedef tinystl::allocator<T*>                    map_allocator;

    typedef typename allocator_type::value_type       value_type;
    typedef typename allocator_type::pointer          pointer;
    typedef typename allocator_type::const_pointer    const_pointer;
    typedef typename allocator_type::reference        reference;
    typedef typename allocator_type::const_reference  const_reference;
    typedef typename allocator_type::size_type        size_type;
    typedef typename allocator_type::difference_type  difference_type;

    typedef pointer*                                  map_pointer;
    typedef const pointer*                            const_map_pointer;

    typedef deque_iterator<T, T&, T*>                 iterator;
    typedef deque_iterator<T, const T&, const T*>     const_iterator;
    typedef tinystl::reverse_iterator<iterator>       reverse_iterator;
    typedef tinystl::reverse_iterator<const_iterator> const_reverse_iterator;

    allocator_type get_allocator() { return allocator_type(); }  // 返回一个 allocator

    static const size_type buffer_size = deque_buf_size<T>::value;

private: // deque 的成员变量
    
    iterator    start_;        // 指向第一个节点
    iterator    finish_;       // 指向最后一个节点
    map_pointer map_;          // 指向管控中心，管控中心是一个指针数组，每个指针指向一个缓冲区
    size_type   map_size_;     // 管控中心的大小

public:  // 构造、复制、移动、析构函数

    deque() { fill_init(0, value_type()); }
    explicit deque(size_type n) { fill_init(n, value_type()); }
    deque(size_type n, const value_type& value) { fill_init(n, value); }

    template <class InputIterator, typename std::enable_if<
        tinystl::is_input_iterator<InputIterator>::value, int>::type = 0>
    deque(InputIterator first, InputIterator last) {
        copy_init(first, last, iterator_category(first));
    }

    deque(std::initializer_list<value_type> ilist) {
        copy_init(ilist.begin(), ilist.end(), tinystl::forward_iterator_tag());
    }

    deque(const deque& rhs) {
        copy_init(rhs.begin(), rhs.end(), tinystl::forward_iterator_tag());
    }

    deque(deque&& rhs) noexcept
        : start_(rhs.start_), finish_(rhs.finish_), map_(rhs.map_), map_size_(rhs.map_size_) {
        rhs.start_ = iterator();
        rhs.finish_ = iterator();
        rhs.map_ = nullptr;
        rhs.map_size_ = 0;
    }

    deque& operator=(const deque& rhs);
    deque& operator=(deque&& rhs) noexcept;

    deque& operator=(std::initializer_list<value_type> ilist) {
        deque tmp(ilist);
        swap(tmp);
        return *this;
    }

    ~deque() {
        if (map_ != nullptr) {
            clear();
            data_allocator::deallocate(*start_.node, buffer_size);
            start_.node = nullptr;
            map_allocator::deallocate(map_, map_size_);
            map_ = nullptr;
        }
    }

public:  // 迭代器相关操作

    iterator               begin()   noexcept       { return start_; }
    const_iterator         begin()   const noexcept { return start_; }
    const_iterator         cbegin()  const noexcept { return start_; }

    iterator               end()     noexcept       { return finish_; }
    const_iterator         end()     const noexcept { return finish_; }
    const_iterator         cend()    const noexcept { return finish_; }

    reverse_iterator       rbegin()  noexcept       { return reverse_iterator(end()); }
    const_reverse_iterator rbegin()  const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

    reverse_iterator       rend()    noexcept       { return reverse_iterator(begin()); }
    const_reverse_iterator rend()    const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend()   const noexcept { return const_reverse_iterator(begin()); }

public:  // 容量相关操作

    bool       empty()    const noexcept  { return start_ == finish_; }
    size_type  size()     const noexcept  { return finish_ - start_; }
    size_type  max_size() const noexcept  { return static_cast<size_type>(-1); }
    void       resize(size_type new_size) { resize(new_size, value_type()); }
    void       resize(size_type new_size, const value_type& value);
    void       shrink_to_fit()  noexcept;

public:  // 访问元素相关操作

    reference operator[](size_type n) {
        TINYSTL_DEBUG(n < size());
        return start_[static_cast<difference_type>(n)];
    }

    const_reference operator[](size_type n) const {
        TINYSTL_DEBUG(n < size());
        return start_[static_cast<difference_type>(n)];
    }

    reference at(size_type n) {
        THROW_OUT_OF_RANGE_IF(!(n < size()), "deque<T>::at() subscript out of range");
        return (*this)[n];
    }

    const_reference at(size_type n) const {
        THROW_OUT_OF_RANGE_IF(!(n < size()), "deque<T>::at() subscript out of range");
        return (*this)[n];
    }

    reference front() {
        TINYSTL_DEBUG(!empty());
        return *begin();  // 这里不宜用 start_，需要利用多态返回不同属性的值（const / not const）
    }

    const_reference front() const {
        TINYSTL_DEBUG(!empty());
        return *begin();
    }

    reference back() {
        TINYSTL_DEBUG(!empty());
        return *(end() - 1);
    }

    const_reference back() const {
        TINYSTL_DEBUG(!empty());
        return *(end() - 1);
    }

public:  // 修改容器相关操作

    void assign(size_type n, const value_type& value) { fill_assign(n, value); }

    template <class InputIterator, typename std::enable_if<
        tinystl::is_input_iterator<InputIterator>::value, int>::type = 0>
    void assign(InputIterator first, InputIterator last) {
        copy_assign(first, last, iterator_category(first));
    }

    void assign(std::initializer_list<value_type> ilist) {
        copy_assign(ilist.begin(), ilist.end(), tinystl::forward_iterator_tag());
    }


    template <class ...Args>
    void emplace_front(Args&& ...args);

    template <class ...Args>
    void emplace_back(Args&& ...args);

    template <class ...Args>
    iterator emplace(iterator pos, Args&& ...args);


    void push_front(const value_type& value) { emplace_front(value); }  // TODO:这样做不知道对不对
    void push_front(value_type&& value)      { emplace_front(tinystl::move(value)); }

    void push_back(const value_type& value)  { emplace_back(value); }
    void push_back(value_type&& value)       { emplace_back(tinystl::move(value)); }


    void pop_front();
    void pop_back();


    iterator insert(iterator pos, const value_type& value);
    iterator insert(iterator pos, value_type&& value);
    iterator insert(iterator pos, size_type n, const value_type& value);
    
    template <class InputIterator, typename std::enable_if<
        tinystl::is_input_iterator<InputIterator>::value, int>::type = 0>
    void insert(iterator pos, InputIterator first, InputIterator last) {
        insert_dispatch(pos, first, last, iterator_category(first));
    }


    iterator erase(iterator pos);
    iterator erase(iterator first, iterator last);
    void     clear();


    void     swap(deque& rhs) noexcept;

private:  // help functions

    map_pointer create_map(size_type size);
    void        create_buffer(map_pointer nstart, map_pointer nfinish);
    void        destroy_buffer(map_pointer nstart, map_pointer nfinish);

    
    void        map_init(size_type n);
    void        fill_init(size_type n, const value_type& value);
    
    template <class InputIterator>
    void        copy_init(InputIterator first, InputIterator last, tinystl::input_iterator_tag);

    template <class ForwardIterator>
    void        copy_init(ForwardIterator first, ForwardIterator last, tinystl::forward_iterator_tag);


    void        fill_assign(size_type n, const value_type& value);

    template <class InputIterator>
    void        copy_assign(InputIterator first, InputIterator last, tinystl::input_iterator_tag);

    template <class ForwardIterator>
    void        copy_assign(ForwardIterator first, ForwardIterator last, tinystl::forward_iterator_tag);


    template <class ...Args>
    iterator    insert_aux(iterator pos, Args&& ...args);

    void        fill_insert(iterator pos, size_type n, const value_type& value);

    template <class ForwardIterator>
    void        copy_insert(iterator pos, ForwardIterator first, ForwardIterator last, size_type n);

    template <class InputIterator>
    void        insert_dispatch(iterator pos, InputIterator first, InputIterator last, 
                    tinystl::input_iterator_tag);

    template <class ForwardIterator>
    void        insert_dispatch(iterator pos, ForwardIterator first, ForwardIterator last, 
                    tinystl::forward_iterator_tag);


    void        require_capacity(size_type n, bool front);
    void        reallocate_map_at_front(size_type need);
    void        reallocate_map_at_back(size_type need);

};  // class deque


// =================================== 函数实现 =================================== //

/// @brief 复制赋值运算符
template <class T>
deque<T>& deque<T>::operator=(const deque& rhs) {
    if (this != &rhs) {
        const auto len = size();
        if (len >= rhs.size()) {
            erase(tinystl::copy(rhs.start_, rhs.finish_, start_), end_);
        }
        else {
            iterator mid = rhs.begin() + static_cast<difference_type>(len);
            tinystl::copy(rhs.start_, mid, start_);
            insert(end_, mid, rhs.finish_);
        }
    }
}

/// @brief 移动赋值运算符
template <class T>
deque<T>& deque<T>::operator=(deque&& rhs) noexcept {
    clear();
    start_ = tinystl::move(rhs.start_);
    finish_ = tinystl::move(rhs.finish_);
    map_ = rhs.map_;
    map_size_ = rhs.map_size_;

    rhs.map_ = nullptr;
    rhs.map_size_ = 0;

    return *this;
}

/// @brief 重置容器的大小
template <class T>
void deque<T>::resize(size_type new_size, const value_type& value) {
    const auto len = size();
    if (new_size < len) {
        erase(begin() + new_size, end());
    }
    else {
        insert(end(), new_size - len, value);
    }
}

/// @brief 减小容器容量，至少会留下头部缓冲区
/// @tparam T 
template <class T>
void deque<T>::shrink_to_fit() noexcept {
    for (auto cur = map_; cur < start_.node; ++cur) {
        data_allocator::deallocate(*cur, buffer_size);
        *cur = nullptr;
    }
    for (auto cur = finish_.node + 1; cur < map_ + map_size_; ++cur) {
        data_allocator::deallocate(*cur, buffer_size);
        *cur = nullptr;
    }
}

/// @brief 在头部就地构造元素
template <class T>
template <class ...Args>
void deque<T>::emplace_front(Args&& ...args) {
    // 如果头部缓冲区还有空间，就在头部就地构造元素
    if (start_.cur != start_.first) {
        data_allocator::construct(start_.cur - 1, tinystl::forward<Args>(args)...);
        --start_.cur;
    }
    // 否则需要重新分配空间
    else {
        require_capacity(1, true);
        try {
            --start_;
            data_allocator::construct(start_.cur, tinystl::forward<Args>(args)...);
        }
        catch (...) {
            ++start_;
            throw;
        }
    }
}

/// @brief 在尾部就地构造元素
template <class T>
template <class ...Args>
void deque<T>::emplace_back(Args&& ...args) {
    // 如果尾部缓冲区还有空间，就在尾部就地构造元素
    if (finish_.cur != finish_.last - 1) {
        data_allocator::construct(finish_.cur, tinystl::forward<Args>(args)...);
        ++finish_.cur;
    }
    // 否则需要重新分配空间
    else {
        require_capacity(1, false);
        data_allocator::construct(finish_.cur, tinystl::forward<Args>(args)...);
        ++finish_;
    }
}

/// @brief 在 pos 处就地构造元素
template <class T>
template <class ...Args>
typename deque<T>::iterator deque<T>::emplace(iterator pos, Args&& ...args) {
    if (pos.cur == start_.cur) {
        emplace_front(tinystl::forward<Args>(args)...);
        return start_;
    }
    else if (pos.cur == finish_.cur) {
        emplace_back(tinystl::forward<Args>(args)...);
        return finish_ - 1;
    }
    return insert_aux(pos, tinystl::forward<Args>(args)...);
}

/// @brief 弹出头部元素
template <class T>
void deque<T>::pop_front() {
    TINYSTL_DEBUG(!empty());
    // 如果头部缓冲区还有元素，就在头部就地销毁元素
    if (start_.cur != start_.last - 1) {
        data_allocator::destroy(start_.cur);
        ++start_.cur;
    }
    // 否则需要释放头部缓冲区
    else {
        data_allocator::destroy(start_.cur);
        ++start_;
        destroy_buffer(start_.node - 1, start_.node - 1);
    }
}

/// @brief 弹出尾部元素
template <class T>
void deque<T>::pop_back() {
    TINYSTL_DEBUG(!empty());
    // 如果尾部缓冲区还有元素，就在尾部就地销毁元素
    if (finish_.cur != finish_.first) {
        --finish_.cur;
        data_allocator::destroy(finish_.cur);
    }
    // 否则需要释放尾部缓冲区
    else {
        --finish_;
        data_allocator::destroy(finish_.cur);
        destroy_buffer(finish_.node + 1, finish_.node + 1);
    }
}

/// @brief 在 pos 处插入元素
template <class T>
typename deque<T>::iterator deque<T>::insert(iterator pos, const value_type& value) {
    emplace(pos, value);
}

/// @brief 在 pos 处插入元素
template <class T>
typename deque<T>::iterator deque<T>::insert(iterator pos, value_type&& value) {
    emplace(pos, tinystl::move(value));
}

/// @brief 在 pos 处插入 n 个元素
template <class T>
typename deque<T>::iterator deque<T>::insert(iterator pos, size_type n, const value_type& value) {
    if (pos.cur == start_.cur) {
        require_capacity(n, true);
        auto new_start = start_ - static_cast<difference_type>(n);
        tinystl::uninitialized_fill_n(new_start, n, value);
        start_ = new_start;
    }
    else if (pos.cur == finish_.cur) {
        require_capacity(n, false);
        auto new_finish = finish_ + static_cast<difference_type>(n);
        tinystl::uninitialized_fill_n(finish_, n, value);
        finish_ = new_finish;
    }
    else {
        fill_insert(pos, n, value);
    }
}

/// @brief 清除 pos 处的元素
template <class T>
typename deque<T>::iterator deque<T>::erase(iterator pos) {
    auto next = pos; ++next;  // 尽量不要使用 pos + 1
    const auto elem_before = pos - start_;
    // 如果 pos 前面的元素比较少，就从前面开始移动
    if (elem_before < (size() >> 1)) {
        tinystl::copy_backward(start_, pos, next);
        pop_front();
    }
    // 否则从后面开始移动
    else {
        tinystl::copy(next, finish_, pos);
        pop_back();
    }
    return start_ + elem_before;
}

/// @brief 清除 [first, last) 内的元素
template <class T>
typename deque<T>::iterator deque<T>::erase(iterator first, iterator last) {
    if (first == start_ && last == finish_) {
        clear();
        return finish_;
    }
    else {
        const auto n = last - first;
        const auto elems_before = first - start_;
        // 如果前面的元素比较少，就从前面开始移动
        if (elems_before < (size() - n) / 2) {
            tinystl::copy_backward(start_, first, last);
            auto new_start = start_ + n;
            data_allocator::destroy(start_.cur, new_start.cur);
            destroy_buffer(start_.node, new_start.node - 1);
            start_ = new_start;
        }
        // 否则从后面开始移动
        else {
            tinystl::copy(last, finish_, first);
            auto new_finish = finish_ - n;
            data_allocator::destroy(new_finish.cur, finish_.cur);
            destroy_buffer(new_finish.node + 1, finish_.node);
            finish_ = new_finish;
        }
        return start_ + elems_before;
    }
}

/// @brief 清空 deque，保留头部的缓冲区
template <class T>
void deque<T>::clear() {
    // 针对头尾以外的缓冲区，全部释放
    for (auto cur = start_.node + 1; cur < finish_.node; ++cur) {
        data_allocator::destroy(*cur, *cur + buffer_size);       // 析构缓冲区内的元素
    }
    // 至少有头尾两个缓冲区
    if (start_.node != finish_.node) {
        data_allocator::destroy(start_.cur, start_.last);        // 析构头部缓冲区内的元素
        data_allocator::destroy(finish_.first, finish_.cur);     // 析构尾部缓冲区内的元素
    }
    // 仅剩一个缓冲区
    else {
        data_allocator::destroy(start_.cur, finish_.cur);        // 只需析构头部缓冲区内的元素
    }
    shrink_to_fit();  // 释放内存由 shrink_to_fit() 完成
    finish_ = start_; // 重置迭代器
}

/// @brief 交换两个 deque
template <class T>
void deque<T>::swap(deque& rhs) noexcept {
    if (this != &rhs) {
        tinystl::swap(start_, rhs.start_);
        tinystl::swap(finish_, rhs.finish_);
        tinystl::swap(map_, rhs.map_);
        tinystl::swap(map_size_, rhs.map_size_);
    }
}

// =================================== help functions =================================== //

/// @brief 创建管控中心
template <class T>
typename deque<T>::map_pointer deque<T>::create_map(size_type size) {
    map_pointer map = nullptr;
    map = map_allocator::allocate(size);
    for (auto cur = map; cur < map + size; ++cur) {
        *cur = nullptr;
    }
    return map;
}

/// @brief 创建缓冲区
template <class T>
void deque<T>::create_buffer(map_pointer nstart, map_pointer nfinish) {
    map_pointer cur;
    try {
        for (cur = nstart; cur <= nfinish; ++cur) {
            *cur = data_allocator::allocate(buffer_size);
        }
    }
    catch (...) {
        while (cur != nstart) {
            --cur;
            data_allocator::deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
        throw;
    }
}

/// @brief 销毁缓冲区
template <class T>
void deque<T>::destroy_buffer(map_pointer nstart, map_pointer nfinish) {
    for (auto cur = nstart; cur <= nfinish; ++cur) {
        data_allocator::deallocate(*cur, buffer_size);
        *cur = nullptr;
    }
}

/// @brief 初始化一个容量为 nElem 的 deque
template <class T>
void deque<T>::map_init(size_type nElem) {
    const auto nNode = nElem / buffer_size + 1;  // 向下取整，因此需要加 1
    // map 中的节点个数，最少为 DEQUE_MAP_INIT_SIZE，最多为 nNode + 2，前后各预留一个，方便扩容
    map_size_ = tinystl::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), nNode + 2);
    
    try {
        map_ = create_map(map_size_);
    }
    catch (...) {
        map_ = nullptr;
        map_size_ = 0;
        throw;
    }

    // 让 nstart 和 nfinish 都指向 map_ 最中央的区域，方便向头尾扩充
    map_pointer nstart = map_ + (map_size_ - nNode) / 2;
    map_pointer nfinish = nstart + nNode - 1;

    try {
        create_buffer(nstart, nfinish);
    }
    catch (...) {
        map_allocator::deallocate(map_, map_size_);
        map_ = nullptr;
        map_size_ = 0;
        throw;
    }

    start_.set_node(nstart);
    finish_.set_node(nfinish);
    start_.cur = start_.first;
    finish_.cur = finish_.first + nElem % buffer_size;
}

/// @brief 初始化一个容量为 n 且每个元素都为 value 的 deque
template <class T>
void deque<T>::fill_init(size_type n, const value_type& value) {
    map_init(n);
    if (n != 0) {
        // 填充非尾节点的缓冲区
        for (auto cur = start_.node; cur <= finish_.node; ++cur) {
            tinystl::uninitialized_fill(*cur, *cur + buffer_size, value);
        }
        // 填充尾节点的缓冲区
        tinystl::uninitialized_fill(finish_.first, finish_.cur, value);
    }
}

/// @brief 利用 [first, last) 区间的元素初始化 deque
template <class T>
template <class InputIterator>
void deque<T>::copy_init(InputIterator first, InputIterator last, tinystl::input_iterator_tag) {
    const difference_type n = tinystl::distance(first, last);
    map_init(n);
    for (; first != last; ++first) {
        emplace_back(*first);
    }
}

/// @brief 利用 [first, last) 区间的元素初始化 deque
template <class T>
template <class ForwardIterator>
void deque<T>::copy_init(ForwardIterator first, ForwardIterator last, tinystl::forward_iterator_tag) {
    const difference_type n = tinystl::distance(first, last);
    map_init(n);
    for (auto cur = start_.node; cur < finish_.node; ++cur) {
        auto next = first;
        tinystl::advance(next, buffer_size);  // 前进一个缓冲区的大小
        tinystl::uninitialized_copy(first, next, *cur);
        first = next;
    }
    tinystl::uninitialized_copy(first, last, finish_.first);
}

/// @brief 为 deque 赋值 n 个 value
template <class T>
void deque<T>::fill_assign(size_type n, const value_type& value) {
    if (n > size()) {
        tinystl::fill(begin(), end(), value);
        insert(end(), n - size(), value);
    }
    else {
        erase(begin() + n, end());
        tinystl::fill(begin(), end(), value);
    }
}

/// @brief 将 [first, last) 区间的元素拷贝赋值给 deque
template <class T>
template <class InputIterator>
void deque<T>::copy_assign(InputIterator first, InputIterator last, tinystl::input_iterator_tag) {
    auto first1 = begin();
    auto last1 = end();

    for (; first != last && first1 != last1; ++first, ++first1) {
        *first1 = *first;
    }
    if (first1 != last1) {
        erase(first1, last1);
    }
    else {
        insert_dispatch(finish_, first, last, tinystl::input_iterator_tag());  // TODO: {} or ()
    }

}

/// @brief 将 [first, last) 区间的元素拷贝赋值给 deque
template <class T>
template <class ForwardIterator>
void deque<T>::copy_assign(ForwardIterator first, ForwardIterator last, tinystl::forward_iterator_tag) {
    const auto len = tinystl::distance(first, last);
    if (len > size()) {
        auto next = first;
        tinystl::advance(next, size());
        tinystl::copy(first, next, start_);
        insert_dispatch(finish_, next, last, tinystl::forward_iterator_tag());  // TODO: {} or ()
    }
    else {
        erase(tinystl::copy(first, last, start_), finish_);
    }
}

/// @brief 在 pos 处插入 1 个元素
template <class T>
template <class ...Args>
typename deque<T>::iterator deque<T>::insert_aux(iterator pos, Args&& ...args) {
    const auto elem_before = pos - start_;
    // 如果前面的元素比较少，就从前面开始移动
    if (elem_before < (size() >> 1)) {
        emplace_front(front());  // 现在最前方插入一个与头部相同的元素
        auto front1 = start_;  ++front1;
        auto front2 = front1;  ++front2;
        pos = start_ + elem_before;
        auto pos1 = pos;  ++pos1;
        tinystl::copy(front2, pos1, front1);  // 将 [front2, pos1) 区间的元素向前移动一个位置
    }
    // 否则从后面开始移动
    else {
        emplace_back(back());  // 现在最后方插入一个与尾部相同的元素
        auto back1 = finish_;  --back1;
        auto back2 = back1;  --back2;
        pos = start_ + elem_before;
        tinystl::copy_backward(pos, back2, back1);  // 将 [pos, back2) 区间的元素向后移动一个位置
    }
    *pos = value_type(tinystl::forward<Args>(args)...);
    return pos;
}

template <class T>
void deque<T>::fill_insert(iterator pos, size_type n, const value_type& value) {
    const auto elem_before = pos - start_;
    const auto len = size();
    auto value_copy = value;

    if (elem_before < (len >> 1)) {
        require_capacity(n, true);

        auto old_start = start_;  // 原始的 start_ 的位置
        auto new_start = start_ - static_cast<difference_type>(n);  // 插入新元素后的 start_ 的位置
        pos = start_ + elem_before;  // 要插入新元素在 pos 之前

        try {
            // 要填充的元素的个数小于 pos 之前的元素的个数
            if (elem_before >= n) {
                auto start_n = start_ + static_cast<difference_type>(n);
                tinystl::uninitialized_copy(start_, start_n, new_start);  // 复制前半部分
                start_ = new_start;
                tinystl::copy(start_n, pos, old_start);  // 复制后半部分
                tinystl::fill(pos - static_cast<difference_type>(n), pos, value_copy);  // 填充新元素
            }
            else {
                auto mid = tinystl::uninitialized_copy(start_, pos, new_start);  // 复制所有 pos 之前的元素
                tinystl::uninitialized_fill(mid, start_, value_copy);  // 填充前半部分新元素
                start_ = new_start; 
                tinystl::fill(old_start, pos, value_copy);  // 填充剩余的新元素
            }
        }
        catch (...) {
            destroy_buffer(new_start.node, new_start.node);
            throw;
        }

    }
    // TODO:

}



















}   // namespace tinystl

#endif // !TINYSTL_DEQUE_H_