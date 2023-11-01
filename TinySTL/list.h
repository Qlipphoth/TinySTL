#ifndef TINYSTL_LIST_H_
#define TINYSTL_LIST_H_

// 这个头文件包含了一个模板类 list
// list : 双向链表

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "functional.h"
#include "util.h"
#include "exceptdef.h"

namespace tinystl {

template <class T> struct list_node_base;  // node 的 prev 与 next 指针结构
template <class T> struct list_node;       // node 的结构，继承自 list_node_base，增加了 data 成员

/// @brief 用于萃取 node 的型别
/// @tparam T 
template <class T>
struct node_traits {
    typedef list_node_base<T>* base_ptr;
    typedef list_node<T>*      node_ptr;
};


// ==================================== list node 结构 ==================================== //

/// @brief list node 的指针结构
/// @tparam T 
template <class T>
struct list_node_base {
    typedef typename node_traits<T>::base_ptr base_ptr;
    typedef typename node_traits<T>::node_ptr node_ptr;

    base_ptr prev;  // 前一个节点
    base_ptr next;  // 后一个节点

    list_node_base() = default;  

    /// @brief 将自身转化为 list_node<T>*
    /// @return 
    node_ptr as_node () {
        return static_cast<node_ptr>(self());
    }

    /// @brief 取消自身与链表的关联，将 prev 与 next 指向自身
    void unlink() {
        prev = next = self();
    }

    /// @brief 将自身转化为 list_node_base<T>* 
    /// @return 
    base_ptr self() {
        return static_cast<base_ptr>(&*this);
    }

};

/// @brief  list node 结构，继承自 list_node_base，增加了 data 成员
/// @tparam T 
template <class T>
struct list_node : public list_node_base<T> {
    typedef typename node_traits<T>::base_ptr base_ptr;
    typedef typename node_traits<T>::node_ptr node_ptr;

    T data;  // 节点的值

    list_node() = default;
    
    list_node(const T& value) : data(value) {}

    list_node(T&& value) : data(tinystl::move(value)) {}

    base_ptr as_base() {
        return static_cast<base_ptr>(&*this);
    }

    node_ptr self() {
        return static_cast<node_ptr>(&*this);
    }

};

// ==================================== list 迭代器 ==================================== //

template <class T>
struct list_iterator : public tinystl::iterator<tinystl::bidirectional_iterator_tag, T> {
    typedef T                                 value_type;
    typedef T*                                pointer;
    typedef T&                                reference;
    typedef typename node_traits<T>::base_ptr base_ptr;
    typedef typename node_traits<T>::node_ptr node_ptr;
    typedef list_iterator<T>                  self;

    base_ptr node_;  // 指向当前节点

    // 构造函数
    
    list_iterator() = default;
    list_iterator(base_ptr x) : node_(x) {}
    list_iterator(node_ptr x) : node_(x->as_base()) {}
    list_iterator(const list_iterator& x) noexcept : node_(x.node_) {}

    // 重载操作符

    reference operator*() const { return node_->as_node()->data; }
    pointer  operator->() const { return &(operator*()); }

    self& operator++() {
        TINYSTL_DEBUG(node_ != nullptr);
        node_ = node_->next;
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--() {
        TINYSTL_DEBUG(node_ != nullptr);
        node_ = node_->prev;
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    // 重载比较操作符
    bool operator==(const self& x) const { return node_ == x.node_; }
    bool operator!=(const self& x) const { return node_ != x.node_; }

};

template <class T>
struct list_const_iterator : public iterator<bidirectional_iterator_tag, T> {
    typedef T                                 value_type;
    typedef const T*                          pointer;
    typedef const T&                          reference;
    typedef typename node_traits<T>::base_ptr base_ptr;
    typedef typename node_traits<T>::node_ptr node_ptr;
    typedef list_const_iterator<T>            self;

    base_ptr node_;

    list_const_iterator() = default;
    list_const_iterator(base_ptr x) :node_(x) {}
    list_const_iterator(node_ptr x) :node_(x->as_base()) {}
    list_const_iterator(const list_iterator<T>& rhs) :node_(rhs.node_) {}
    list_const_iterator(const list_const_iterator& rhs) :node_(rhs.node_) {}

    reference operator*()  const { return node_->as_node()->data; }
    pointer   operator->() const { return &(operator*()); }

    self& operator++() {
        TINYSTL_DEBUG(node_ != nullptr);
        node_ = node_->next;
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--() {
        TINYSTL_DEBUG(node_ != nullptr);
        node_ = node_->prev;
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    // 重载比较操作符
    bool operator==(const self& rhs) const { return node_ == rhs.node_; }
    bool operator!=(const self& rhs) const { return node_ != rhs.node_; }

};


// ==================================== list 结构 ==================================== //
// SGI-STL 中的 list 为双向环形链表，其中尾节点为空节点，头节点为尾节点的前一个节点

template <class T>
class list {

public:
    // list 的嵌套型别定义
    typedef tinystl::allocator<T>                     allocator_type;
    typedef tinystl::allocator<T>                     data_allocator;
    typedef tinystl::allocator<list_node_base<T>>     base_allocator;
    typedef tinystl::allocator<list_node<T>>          node_allocator;

    typedef typename allocator_type::value_type       value_type;
    typedef typename allocator_type::pointer          pointer;
    typedef typename allocator_type::const_pointer    const_pointer;
    typedef typename allocator_type::reference        reference;
    typedef typename allocator_type::const_reference  const_reference;
    typedef typename allocator_type::size_type        size_type;
    typedef typename allocator_type::difference_type  difference_type;

    typedef list_iterator<T>                          iterator;
    typedef list_const_iterator<T>                    const_iterator;
    typedef tinystl::reverse_iterator<iterator>       reverse_iterator;
    typedef tinystl::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef typename node_traits<T>::base_ptr         base_ptr;
    typedef typename node_traits<T>::node_ptr         node_ptr;

    allocator_type get_allocator() { return node_allocator(); }

private:
    
    base_ptr node_;  // 指向末尾节点
    size_type size_; // list 的大小

public:  // 构造、复制、移动、析构函数
    
    list() { fill_init(0, value_type()); }

    explicit list(size_type n) { fill_init(n, value_type()); }

    list(size_type n, const T& value) { fill_init(n, value); }
    
    template <class Iter, typename std::enable_if<
        tinystl::is_input_iterator<Iter>::value, int>::type = 0>
    list(Iter first, Iter last) { copy_init(first, last); }

    list(std::initializer_list<T> ilist) { copy_init(ilist.begin(), ilist.end()); }

    list(const list& x) { copy_init(x.cbegin(), x.cend()); }

    list(list&& x) noexcept : node_(x.node_), size_(x.size_) {
        x.node_ = nullptr;
        x.size_ = 0;
    }

    list& operator=(const list& x) {
        if (this != &x) assign(x.begin(), x.end());
        return *this;
    }

    list& operator=(list&& x) noexcept {
        clear();
        splice(end(), x);
        return *this;
    }

    list& operator=(std::initializer_list<value_type> ilist) {
        list tmp(ilist.begin(), ilist.end());
        swap(tmp);
        return *this;
    }

    ~list() {
        if (node_ != nullptr) {
            clear();
            base_allocator::deallocate(node_);
            node_ = nullptr;
            size_ = 0;
        }
    }

public: // 迭代器相关操作

    iterator               begin()   noexcept        { return node_->next; }
    const_iterator         begin()   const noexcept  { return node_->next; }
    const_iterator         cbegin()  const noexcept  { return begin(); }
    reverse_iterator       rbegin()  noexcept        { return reverse_iterator(end()); }
    const_reverse_iterator rbegin()  const noexcept  { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept  { return rbegin(); }

    iterator               end()     noexcept        { return node_; }
    const_iterator         end()     const noexcept  { return node_; }
    const_iterator         cend()    const noexcept  { return end(); }
    reverse_iterator       rend()    noexcept        { return reverse_iterator(begin()); }
    const_reverse_iterator rend()    const noexcept  { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend()   const noexcept  { return rend(); }

public: // 容量相关操作

    bool      empty()    const noexcept  { return node_->next == node_; }
    size_type size()     const noexcept  { return size_; }
    size_type max_size() const noexcept  { return static_cast<size_type>(-1); }

public: // 访问元素相关操作

    reference front() {
        TINYSTL_DEBUG(!empty());
        return *begin();
    }

    const_reference front() const {
        TINYSTL_DEBUG(!empty());
        return *begin();
    }

    reference back() {
        TINYSTL_DEBUG(!empty());
        return *(--end());
    }

    const_reference back() const {
        TINYSTL_DEBUG(!empty());
        return *(--end());
    }

public: // 调整元素容器相关操作

    // assign

    void assign(size_type n, const value_type& value) {
        fill_assign(n, value);
    }

    template <class Iter, typename std::enable_if<
        tinystl::is_input_iterator<Iter>::value, int>::type = 0>
    void assign(Iter first, Iter last) {
        copy_assign(first, last);
    }

    void assign(std::initializer_list<value_type> ilist) {
        copy_assign(ilist.begin(), ilist.end());
    }

    // emplace_front / emplace_back / emplace

    /// @brief 在 list 的头部插入一个元素
    /// @tparam ...Args 
    /// @param ...args 
    template <class ...Args>
    void emplace_front(Args&&... args) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        auto link_node = create_node(std::forward<Args>(args)...);
        link_nodes_at_front(link_node->as_base(), link_node->as_base());
        ++size_;
    }

    /// @brief 在 list 的尾部插入一个元素
    /// @tparam ...Args 
    /// @param ...args 
    template <class ...Args>
    void emplace_back(Args&&... args) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        auto link_node = create_node(std::forward<Args>(args)...);
        link_nodes_at_back(link_node->as_base(), link_node->as_base());
        ++size_;
    }

    /// @brief 在 pos 的位置插入一个元素
    /// @tparam ...Args 
    /// @param pos  插入的位置
    /// @param ...args 
    /// @return  返回指向新插入元素的迭代器
    template <class ...Args>
    iterator emplace(const_iterator pos, Args&& ...args) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        auto link_node = create_node(std::forward<Args>(args)...);
        link_nodes(pos.node_, link_node->as_base(), link_node->as_base());
        ++size_;
        return iterator(link_node);
    }

    // insert

    /// @brief 在 pos 处插入一个节点
    /// @param pos  插入的位置
    /// @param value  节点的值
    /// @return  返回指向新插入元素的迭代器
    iterator insert(const_iterator pos, const value_type& value) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        auto link_node = create_node(value);
        ++size_;
        return link_iter_node(pos, link_node->as_base());
    }

    iterator insert(const_iterator pos, value_type&& value) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        auto link_node = create_node(std::move(value));
        ++size_;
        return link_iter_node(pos, link_node->as_base());
    }

    iterator insert(const_iterator pos, size_type n, const value_type& value) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");
        return fill_insert(pos, n, value);
    }

    template <class Iter, typename std::enable_if<
        tinystl::is_input_iterator<Iter>::value, int>::type = 0>
    iterator insert(const_iterator pos, Iter first, Iter last) {
        size_type n = tinystl::distance(first, last);
        THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");
        return copy_insert(pos, n, first);
    }

    // push_front / push_back

    void push_front(const value_type& value) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        auto link_node = create_node(value);
        link_nodes_at_front(link_node->as_base(), link_node->as_base());
        ++size_;
    }

    void push_front(value_type&& value) {
        emplace_front(std::move(value));
    }

    void push_back(const value_type& value) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        auto link_node = create_node(value);
        link_nodes_at_back(link_node->as_base(), link_node->as_base());
        ++size_;
    }

    void push_back(value_type&& value) {
        emplace_back(std::move(value));
    }

    // pop_front / pop_back

    void pop_front() {
        TINYSTL_DEBUG(!empty());
        auto del_node = node_->next;
        unlink_nodes(del_node, del_node);
        destroy_node(del_node->as_node());
        --size_;
    }

    void pop_back() {
        TINYSTL_DEBUG(!empty());
        auto del_node = node_->prev;
        unlink_nodes(del_node, del_node);
        destroy_node(del_node->as_node());
        --size_;
    }

    // erase / clear

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    void     clear();

    // resize
    // 将 resize 包装一层，实际的逻辑由 resize(size_type new_size, const value_type& value) 实现，以获取类型信息

    void     resize(size_type new_size) { resize(new_size, value_type()); }
    void     resize(size_type new_size, const value_type& value);

    // swap

    void swap(list& x) noexcept {
        tinystl::swap(node_, x.node_);
        tinystl::swap(size_, x.size_);
    }

public:  // list 相关操作

    void transfer(const_iterator pos, const_iterator first, const_iterator last);

    void splice(const_iterator pos, list& x);
    void splice(const_iterator pos, list& x, const_iterator it);
    void splice(const_iterator pos, list& x, const_iterator first, const_iterator last);

    void remove(const value_type& value) {
        remove_if([&](const value_type& x) { return x == value; });  // 用 lambda 表达式实现一元谓词
    }

    template <class UnaryPredicate>
    void remove_if(UnaryPredicate pred);

    void unique() { unique(tinystl::equal_to<T>()); }

    template <class BinaryPredicate>
    void unique(BinaryPredicate pred);

    void merge(list& x) { merge(x, tinystl::less<T>()); }

    template <class Compare>
    void merge(list& x, Compare comp);

    void sort() { list_sort(tinystl::less<T>()); }

    template <class Compare>
    void sort(Compare comp) { list_sort(comp); }

    void reverse() noexcept;

private:  // 辅助函数

    // create_node / destroy_node

    template <class ...Args>
    node_ptr create_node(Args&&... args);

    void destroy_node(node_ptr p);

    // initialize

    void fill_init(size_type n, const value_type& value);

    template <class Iter>
    void copy_init(Iter first, Iter last);

    // link_nodes / unlink_nodes

    iterator link_iter_node(const_iterator pos, base_ptr node);
    void     link_nodes(base_ptr pos, base_ptr first, base_ptr last);
    void     link_nodes_at_front(base_ptr first, base_ptr last);
    void     link_nodes_at_back(base_ptr first, base_ptr last);
    void     unlink_nodes(base_ptr first, base_ptr last);

    // assign

    void fill_assign(size_type n, const value_type& value);

    template <class Iter>
    void copy_assign(Iter first, Iter last);

    // insert

    iterator fill_insert(const_iterator pos, size_type n, const value_type& value);

    template <class Iter>
    iterator copy_insert(const_iterator pos, size_type n, Iter first);

    // sort

    template <class Compare>
    void list_sort(Compare comp);

};


// ==================================== 函数实现 ==================================== //

/// @brief 删除 pos 处的元素
/// @tparam T  元素的类型
/// @param pos  删除的位置
/// @return  返回指向被删除元素的下一个元素的迭代器
template <class T>
typename list<T>::iterator
list<T>::erase(const_iterator pos) {
    TINYSTL_DEBUG(pos != cend());
    auto del_node = pos.node_;
    auto next_node = del_node->next;
    unlink_nodes(del_node, del_node);
    destroy_node(del_node->as_node());
    --size_;
    return iterator(next_node);
}

/// @brief 删除 [first, last) 区间内的元素
/// @tparam T  元素的类型
/// @param first  删除的起始位置
/// @param last  删除的结束位置
/// @return  返回指向被删除元素的下一个元素的迭代器
template <class T>
typename list<T>::iterator
list<T>::erase(const_iterator first, const_iterator last) {
    if (first != last) {
        unlink_nodes(first.node_, last.node_->prev);
        while (first != last) {
            auto del_node = first.node_;
            ++first;
            destroy_node(del_node->as_node());
            --size_;
        }
    }
    return iterator(last.node_);
}

/// @brief 将 list 清空
/// @tparam T 
template<class T>
void list<T>::clear() {
    if (size_ != 0) {
        auto cur = node_->next;
        for (base_ptr next = cur->next; cur != node_; cur = next, next = cur->next) {
            destroy_node(cur->as_node());
        }
        node_->unlink();
        size_ = 0;
    }
}

/// @brief 重置容器大小，若 new_size > size_，则在尾部插入元素，否则删除尾部元素
/// @tparam T  元素的类型
/// @param new_size  新的大小
/// @param value  新插入元素的值
template <class T>
void list<T>::resize(size_type new_size, const value_type& value) {
    auto i = begin();
    size_type len = 0;
    while (i != end() && len < new_size) {
        ++i; ++len;
    }
    // 跳出循环后只有两种情况
    // 1. 遍历到 new_size 个元素，此时需要删除后面的元素
    if (len == new_size) erase(i, node_);
    // 2. 遍历到 end()，此时需要在尾部插入元素
    else insert(node_, new_size - len, value);

}

template <class T>
void list<T>::transfer(const_iterator pos, const_iterator first, const_iterator last) {
    if (pos != last) {
        last.node_->prev->next = pos.node_;        // (1) 将 last 之前的节点与 pos 连接
        first.node_->prev->next = last.node_;      // (2) 将 first 之前的节点与 last 连接
        pos.node_->prev->next = first.node_;       // (3) 将 pos 之前的节点与 first 连接
        auto tmp = pos.node_->prev;                // (4) 存储 pos 之前的节点
        pos.node_->prev = last.node_->prev;        // (5) 将 pos 之前的节点与 last 之前的节点连接
        last.node_->prev = first.node_->prev;      // (6) 将 last 之前的节点与 first 之前的节点连接
        first.node_->prev = tmp;                   // (7) 将 first 之前的节点与 pos 之前的节点连接
    }
}

/// @brief 将 x 合并到 pos 之前
/// @tparam T  元素的类型
/// @param pos  合并的位置
/// @param x  要合并的 list
template <class T>
void list<T>::splice(const_iterator pos, list& x) {
    TINYSTL_DEBUG(this != &x);
    if (!x.empty()) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - x.size_, "list<T>'s size too big");
        
        // auto first = x.node_->next;
        // auto  last = x.node_->prev;

        // x.unlink_nodes(first, last);
        // link_nodes(pos.node_, first, last);

        // size_ += x.size_;
        // x.size_ = 0;

        transfer(pos, x.begin(), x.end());
        size_ += x.size_;
        x.size_ = 0;
    }
}

/// @brief 将 x list 的 it 位置处的 node 合并到 pos 之前
/// @tparam T  元素的类型
/// @param pos  合并的位置
/// @param x  要合并的 list
/// @param it  要合并的位置
template <class T>
void list<T>::splice(const_iterator pos, list& x, const_iterator it) {
    if (pos.node_ != it.node_ && pos.node_ != it.node_->next) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
        // auto first = it.node_;
        // x.unlink_nodes(first, first);
        // link_nodes(pos.node_, first, first);
        // ++size_;
        // --x.size_;

        transfer(pos, it, it.node_->next);
        ++size_;
        --x.size_;
    }
}

/// @brief 将 x list 的 [first, last) 区间内的 node 合并到 pos 之前
/// @tparam T  元素的类型
/// @param pos  合并的位置
/// @param x  要合并的 list
/// @param first  要合并的起始位置
/// @param last   要合并的结束位置
template <class T>
void list<T>::splice(const_iterator pos, list& x, const_iterator first, const_iterator last) {
    if (first != last && this != &x) {
        size_type n = tinystl::distance(first, last);
        THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");
        // auto first_node = first.node_;
        // auto last_node = last.node_->prev;
        // x.unlink_nodes(first_node, last_node);
        // link_nodes(pos.node_, first_node, last_node);
        // size_ += n;
        // x.size_ -= n;

        transfer(pos, first, last);
        size_ += n;
        x.size_ -= n;
    }
}

/// @brief 将 list 中所有满足一元谓词 pred 的元素删除
/// @tparam T  元素的类型
/// @param pred  一元谓词
template <class T>
template <class UnaryPredicate>
void list<T>::remove_if(UnaryPredicate pred) {
    auto first = begin();
    auto last = end();
    for (auto next = first; first != last; first = next) {
        ++next;
        if (pred(*first)) erase(first);
    }
}

/// @brief 将 list 中所有满足二元谓词 pred 的重复元素删除
/// @tparam T 元素的类型
/// @param pred  二元谓词
template <class T>
template <class BinaryPredicate>
void list<T>::unique(BinaryPredicate pred) {
    auto i = begin();
    auto e = end();
    auto j = i;
    ++j;

    while (j != e) {
        if (pred(*i, *j)) erase(j);
        else { i = j; }
        j = i;
        ++j;
    }
}

/// @brief 与另一个 list x 合并，顺序按照给定的 comp
/// @tparam T 
/// @param x 
/// @param comp 
template <class T>
template <class Compare>
void list<T>::merge(list& x, Compare comp) {
    if (this == &x) return;
    THROW_LENGTH_ERROR_IF(size_ > max_size() - x.size_, "list<T>'s size too big");

    auto f1 = begin();
    auto l1 = end();
    auto f2 = x.begin();
    auto l2 = x.end();

    // while (f1 != l1 && f2 != l2) {
    //   if (comp(*f2, *f1)) {
    //     // 使 comp 为 true 的一段区间
    //     auto next = f2;
    //     ++next;
    //     for (; next != l2 && comp(*next, *f1); ++next);
    //     auto f = f2.node_;
    //     auto l = next.node_->prev;
    //     f2 = next;

    //     // link node
    //     x.unlink_nodes(f, l);
    //     link_nodes(f1.node_, f, l);
    //     ++f1;
    //   }
    //   else ++f1;
    // }

    // // 连接剩余部分
    // if (f2 != l2) {
    //   auto f = f2.node_;
    //   auto l = l2.node_->prev;
    //   x.unlink_nodes(f, l);
    //   link_nodes(l1.node_, f, l);
    // }

    // size_ += x.size_;
    // x.size_ = 0;

    while (f1 != l1 && f2 != l2) {
        if (comp(*f2, *f1)) {
            auto next = f2;
            transfer(f1, f2, ++next);
            f2 = next;
        }
        else ++f1;
    }
    if (f2 != l2) transfer(l1, f2, l2);

    size_ += x.size_;
    x.size_ = 0;
}

template <class T>
void list<T>::reverse() noexcept {
    // if (size_ <= 1) return;
    // auto i = begin();
    // auto e = end();
    // while (i.node_ != e.node_) {
    //     tinystl::swap(i.node_->prev, i.node_->next);
    //     i.node_ = i.node_->prev;
    // }
    // tinystl::swap(e.node_->prev, e.node_->next);

    if (node_->next == node_ || node_->next->next == node_) return;
    auto first = begin();
    ++first;
    while (first != end()) {
        auto old = first;
        ++first;
        transfer(begin(), old, first);
    }
}


// ==================================== 辅助函数实现 ==================================== //

/// @brief 创建一个 list 节点
/// @tparam T  节点的类型
/// @param ...args  节点的构造参数
/// @return  返回指向新节点的指针
template <class T>
template <class ...Args>
typename list<T>::node_ptr
list<T>::create_node(Args&&... args) {
    node_ptr p = node_allocator::allocate(1);
    try {
        data_allocator::construct(tinystl::address_of(p->data), std::forward<Args>(args)...);
        p->prev = nullptr;
        p->next = nullptr;
    }
    catch (...) {
        node_allocator::deallocate(p);
        throw;
    }
    return p;
}

/// @brief 销毁一个 list 节点
/// @tparam T  节点的类型
/// @param p  指向要销毁的节点的指针
template <class T>
void list<T>::destroy_node(node_ptr p) {
    data_allocator::destroy(tinystl::address_of(p->data));
    node_allocator::deallocate(p);
}

/// @brief 用 n 个 value 初始化 list
/// @tparam T  节点的类型
/// @param n  初始化的元素个数
/// @param value  初始化的元素值
template <class T>
void list<T>::fill_init(size_type n, const value_type& value) {
    node_ = base_allocator::allocate(1);
    node_->unlink();
    size_ = n;
    try {
        for (; n > 0; --n) {
            auto link_node = create_node(value);
            link_nodes_at_back(link_node->as_base(), link_node->as_base());
        }
    }
    catch (...) {
        clear();
        base_allocator::deallocate(node_);
        node_ = nullptr;
        throw;
    }
}

/// @brief 用 [first, last) 区间内的元素初始化 list
/// @tparam T  节点的类型
/// @param first  初始化的起始位置
/// @param last  初始化的结束位置
template <class T>
template <class Iter>
void list<T>::copy_init(Iter first, Iter last) {
    node_ = base_allocator::allocate(1);
    node_->unlink();
    size_type n = tinystl::distance(first, last);
    size_ = n;
    try {
        for (; n > 0; --n, ++first) {
            auto link_node = create_node(*first);
            link_nodes_at_back(link_node->as_base(), link_node->as_base());
        }
    }
    catch (...) {
        clear();
        base_allocator::deallocate(node_);
        node_ = nullptr;
        throw;
    }
}

/// @brief 在 pos 之前插入一个 node
/// @tparam T  节点的类型
/// @param pos  插入的位置
/// @param node  要插入的节点
/// @return  返回指向新插入元素的迭代器
template <class T>
typename list<T>::iterator
list<T>::link_iter_node(const_iterator pos, base_ptr node) {
    if (pos == node_->next) link_nodes_at_front(node, node);
    else if (pos == node_) link_nodes_at_back(node, node);
    else link_nodes(pos.node_, node, node);
    return iterator(node);
}

/// @brief 在 pos 之前连接 [first, last] 区间内的节点
/// @tparam T  节点的类型
/// @param pos  连接的位置
/// @param first  连接的起始位置
/// @param last  连接的结束位置
template <class T>
void list<T>::link_nodes(base_ptr pos, base_ptr first, base_ptr last) {
    pos->prev->next = first;
    first->prev = pos->prev;
    pos->prev = last;
    last->next = pos;
}

/// @brief 在 list 的头部连接 [first, last] 区间内的节点
/// @tparam T 
/// @param first 
/// @param last 
template <class T>
void list<T>::link_nodes_at_front(base_ptr first, base_ptr last) {
    // 将新的链表加入
    first->prev = node_;
    last->next = node_->next;
    // 断开原有连接
    node_->next->prev = last;
    node_->next = first;
}

/// @brief 在 list 的尾部连接 [first, last] 区间内的节点
/// @tparam T 
/// @param first 
/// @param last 
template <class T>
void list<T>::link_nodes_at_back(base_ptr first, base_ptr last) {
    // 将新的链表加入
    first->prev = node_->prev;
    last->next = node_;
    // 断开原有连接
    node_->prev->next = first;
    node_->prev = last;
}

/// @brief 断开链表与 [first, last] 之间的部分的连接 
/// @tparam T 
/// @param first 
/// @param last 
template <class T>
void list<T>::unlink_nodes(base_ptr first, base_ptr last) {
    first->prev->next = last->next;
    last->next->prev = first->prev;
}

/// @brief 用 n 个 value 为容器赋值
/// @tparam T 
/// @param n 
/// @param value 
template <class T>
void list<T>::fill_assign(size_type n, const value_type& value) {
    auto i = begin();
    auto e = end();
    for (; i != e && n > 0; ++i, --n) *i = value;
    if (n > 0) insert(e, n, value);
    else erase(i, e);
}

/// @brief 复制 [first, last) 区间内的元素为容器赋值
/// @tparam T 
/// @param first 
/// @param last 
template <class T>
template <class Iter>
void list<T>::copy_assign(Iter first, Iter last) {
    auto i = begin();
    auto e = end();
    for (; i != e && first != last; ++i, ++first) *i = *first;
    if (first != last) insert(e, first, last);
    else erase(i, e);
}

/// @brief 插入 n 个 value 在 pos 之前
/// @tparam T 
/// @param pos 
/// @param n 
/// @param value 
/// @return  返回指向新插入的最后一个元素的迭代器，若 n == 0，返回 pos
template <class T>
typename list<T>::iterator
list<T>::fill_insert(const_iterator pos, size_type n, const value_type& value) {
    iterator r(pos.node_);
    if (n != 0) {
        const auto add_size = n;
        auto node = create_node(value);
        node->prev = nullptr;
        r = iterator(node);
        iterator end = r;
        try {
            // 前面已经创建了一个节点，还需 n - 1 个
            for (--n; n > 0; --n, ++end) {
                auto next = create_node(value);
                end.node_->next = next->as_base();  // link node
                next->prev = end.node_;
            }
            size_ += add_size;
        }
        catch (...) {
            auto enode = end.node_;
            while (true) {
                auto prev = enode->prev;
                destroy_node(enode->as_node());
                if (prev == nullptr)
                break;
                enode = prev;
            }
            throw;
        }
        link_nodes(pos.node_, r.node_, end.node_);
    }
    return r;
}

/// @brief 插入 [first, last) 区间内的元素在 pos 之前
/// @tparam T 
/// @param pos 
/// @param n 
/// @param first 
/// @return 
template <class T>
template <class Iter>
typename list<T>::iterator
list<T>::copy_insert(const_iterator pos, size_type n, Iter first) {
    iterator r(pos.node_);
    if (n != 0)
    {
        const auto add_size = n;
        auto node = create_node(*first);
        node->prev = nullptr;
        r = iterator(node);
        iterator end = r;
        try {
            for (--n, ++first; n > 0; --n, ++first, ++end) {
                auto next = create_node(*first);
                end.node_->next = next->as_base();  // link node
                next->prev = end.node_;
            }
            size_ += add_size;
        }
        catch (...) {
            auto enode = end.node_;
            while (true) {
                auto prev = enode->prev;
                destroy_node(enode->as_node());
                if (prev == nullptr)
                break;
                enode = prev;
            }
            throw;
        }
        link_nodes(pos.node_, r.node_, end.node_);
    }
    return r;
}

template <class T>
template <class Compare>
void list<T>::list_sort(Compare comp) {
    if (node_->next == node_ || node_->next->next == node_) return;

    // 一些新的 lists 用于存储分割后的 lists
    list<T> carry;
    list<T> counter[64];  // 缓存表，第 i 层 list 存储长度为 2^i
    int fill = 0;         // 记录最深层数，counter[fill] 为空

    while (!empty()) {
        // 运行到此处 carry 必然为空，因此一定可以处理下一个元素
        // 1. 将当前 list 的头部元素取出，放入 carry 中
        carry.splice(carry.begin(), *this, begin());
        int i = 0;  // 记录经过的层数
        
        // 2. 从小往大不断合并非空层直至遇到空层或者到达当前最大归并层次
        // 出这个循环，无论 i 是否超过最大层，counter[i] 必然是空的
        // !counter[i].empty() 决定了 counter[i] 中存放的 list 的长度为 2^i
        while (i < fill && !counter[i].empty()) {
            // 将 carry 的元素与 counter[i] 的元素按顺序合并
            counter[i].merge(carry, comp);
            // 将合并后的结果放入 carry 中，i + 1，继续下一层的合并
            // 这里使用了一些小技巧，实际上相当于：
            // carry.swap(counter[i]);
            // ++i;
            carry.swap(counter[i++]);
        }

        // 3. 将合并出的结果放入下一层中
        // 根据上面循环的退出条件，count[i]必然是空的，
        // 这里相当于将 carry 中归并的结果放入下一层，由于经过了 swap，因此 carry 必然为空
        carry.swap(counter[i]);

        // 如果 i == fill，说明当前层是最大层，需要增加一层
        if (i == fill) ++fill;
    }

    // 将每一层的 list 依次合并，每一层保证有序
    for (int i = 1; i < fill; ++i) {
        counter[i].merge(counter[i - 1], comp);
    }

    // 将最终的结果交换到当前 list 中
    swap(counter[fill - 1]);
}

// ==================================== 重载比较操作符 ==================================== //

template <class T>
bool operator==(const list<T>& lhs, const list<T>& rhs) {
    auto f1 = lhs.begin();
    auto f2 = rhs.begin();
    auto l1 = lhs.end();
    auto l2 = rhs.end();
    for (; f1 != l1 && f2 != l2 && *f1 == *f2; ++f1, ++f2);
    return f1 == l1 && f2 == l2;
}

template <class T>
bool operator<(const list<T>& lhs, const list<T>& rhs) {
    return tinystl::lexicographical_compare(
        lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T>
bool operator!=(const list<T>& lhs, const list<T>& rhs) {
    return !(lhs == rhs);
}

template <class T>
bool operator>(const list<T>& lhs, const list<T>& rhs) {
    return rhs < lhs;
}

template <class T>
bool operator<=(const list<T>& lhs, const list<T>& rhs) {
    return !(rhs < lhs);
}

template <class T>
bool operator>=(const list<T>& lhs, const list<T>& rhs) {
    return !(lhs < rhs);
}

// ==================================== 重载 swap ==================================== //

template <class T>
void swap(list<T>& lhs, list<T>& rhs) noexcept {
    lhs.swap(rhs);
}


}  // namespace tinystl

#endif // TINYSTL_LIST_H_