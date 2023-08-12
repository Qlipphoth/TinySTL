#ifndef TINYSTL_QUEUE_H_
#define TINYSTL_QUEUE_H_

// 这个头文件包含了两个模板类 queue 和 priority_queue
// queue          : 队列
// priority_queue : 优先队列

#include "deque.h"
#include "vector.h"
#include "functional.h"
#include "heap.h"

namespace tinystl {

// 模板类 queue
// 参数一代表数据类型，参数二代表底层容器类型，缺省使用 deque 作为底层容器
template <class T, class Container = tinystl::deque<T>>
class queue {

public:
    typedef Container                           container_type;
    // 使用底层容器的型别
    typedef typename Container::value_type      value_type;
    typedef typename Container::size_type       size_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;

    static_assert(std::is_same<T, value_type>::value, "the value_type of Container should be same with T");

private:
    // 用底层容器的型别来定义一个底层容器对象
    container_type c_;

public:  // 构造、复制、移动、析构函数
    queue() = default;
    explicit queue(size_type n) : c_(n) {}
    queue(size_type n, const value_type& value) : c_(n, value) {}

    template <class InputIterator>
    queue(InputIterator first, InputIterator last) : c_(first, last) {}

    queue(std::initializer_list<value_type> ilist) : c_(ilist.begin(), ilist.end()) {}

    queue(Container& c) : c_(c) {}

    queue(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value) : c_(tinystl::move(c)) {}
 
    queue(const queue& rhs) : c_(rhs.c_) {}

    queue(queue&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value) : c_(tinystl::move(rhs.c_)) {}

    queue& operator=(const queue& rhs) {
        c_ = rhs.c_;
        return *this;
    }

    queue& operator=(queue&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value) {
        c_ = tinystl::move(rhs.c_);
        return *this;
    }

    queue& operator=(std::initializer_list<value_type> ilist) {
        c_ = ilist;
        return *this;
    }

    ~queue() = default;

public:  // 元素相关操作
    reference       front()        { return c_.front(); }
    const_reference front() const  { return c_.front(); }
    reference       back()         { return c_.back(); }
    const_reference back() const   { return c_.back(); }

    bool            empty() const noexcept { return c_.empty(); }
    size_type       size()  const noexcept { return c_.size(); }

    template <class... Args>
    void emplace(Args&&... args) { c_.emplace_back(tinystl::forward<Args>(args)...); }

    void push(const value_type& value) { c_.push_back(value); }
    void push(value_type&& value)      { c_.push_back(tinystl::move(value)); }

    void pop() { c_.pop_front(); }

    void clear() { while (!empty()) pop(); }

    void swap(queue& rhs) noexcept(noexcept(tinystl::swap(c_, rhs.c_))) { tinystl::swap(c_, rhs.c_); }

public: 
    friend bool operator==(const queue& lhs, const queue& rhs) { return lhs.c_ == rhs.c_; }
    friend bool operator< (const queue& lhs, const queue& rhs) { return lhs.c_ <  rhs.c_; }
};

// ========================== 重载比较操作符 ========================== // 

template <class T, class Container>
bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return lhs == rhs;
}

template <class T, class Container>
bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return !(lhs == rhs);
}

template <class T, class Container>
bool operator<(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return lhs < rhs;
}

template <class T, class Container>
bool operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return rhs < lhs;
}

template <class T, class Container>
bool operator<=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return !(rhs < lhs);
}

template <class T, class Container>
bool operator>=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class T, class Container>
void swap(queue<T, Container>& lhs, queue<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}


/*****************************************************************************************/

// 模板类 priority_queue
// 参数一代表数据类型，参数二代表容器类型，缺省使用 mystl::vector 作为底层容器
// 参数三代表比较权值的方式，缺省使用 mystl::less 作为比较方式

template <class T, class Container = tinystl::vector<T>,
          class Compare = tinystl::less<typename Container::value_type>>
class priority_queue {

public:
    typedef Container                           container_type;
    typedef Compare                             value_compare;
    // 使用底层容器的型别
    typedef typename Container::value_type      value_type;
    typedef typename Container::size_type       size_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;

    static_assert(std::is_same<T, value_type>::value, "the value_type of Container should be same with T");

private:
    container_type c_;     // 用底层容器来表现 priority_queue
    value_compare  comp_;  // 权值比较的标准

public:  // 构造、复制、移动函数
    priority_queue() = default;
    priority_queue(const Compare& c) : c_(), comp_(c) {}
    explicit priority_queue(size_type n) : c_(n) { tinystl::make_heap(c_.begin(), c_.end(), comp_); }
    priority_queue(size_type n, const value_type& value) : c_(n, value) {
        tinystl::make_heap(c_.begin(), c_.end(), comp_);
    }

    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last) : c_(first, last) {
        tinystl::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue(std::initializer_list<value_type> ilist) : c_(ilist) {
        tinystl::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue(const Container& c) : c_(c) {
        tinystl::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue(Container&& c) : c_(tinystl::move(c)) {
        tinystl::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue(const priority_queue& rhs) : c_(rhs.c_), comp_(rhs.comp_) {
        tinystl::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue(priority_queue&& rhs) : c_(tinystl::move(rhs.c_)), comp_(rhs.comp_) {
        tinystl::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue& operator=(const priority_queue& rhs) {
        c_ = rhs.c_;
        comp_ = rhs.comp_;
        tinystl::make_heap(c_.begin(), c_.end(), comp_);
        return *this;
    }

    priority_queue& operator=(priority_queue&& rhs) {
        c_ = tinystl::move(rhs.c_);
        comp_ = rhs.comp_;
        tinystl::make_heap(c_.begin(), c_.end(), comp_);
        return *this;
    }

    priority_queue& operator=(std::initializer_list<value_type> ilist) {
        c_ = ilist;
        tinystl::make_heap(c_.begin(), c_.end(), comp_);
        return *this;
    }

    ~priority_queue() = default;

public:  // 元素相关操作
    const_reference top()   const { return c_.front(); }
    bool            empty() const { return c_.empty(); }
    size_type       size()  const { return c_.size(); }

    template <class... Args>
    void emplace(Args&&... args) {
        c_.emplace_back(tinystl::forward<Args>(args)...);
        tinystl::push_heap(c_.begin(), c_.end(), comp_);
    }

    void push(const value_type& value) {
        c_.push_back(value);
        tinystl::push_heap(c_.begin(), c_.end(), comp_);
    }

    void push(value_type&& value) {
        c_.push_back(tinystl::move(value));
        tinystl::push_heap(c_.begin(), c_.end(), comp_);
    }

    void pop() {
        tinystl::pop_heap(c_.begin(), c_.end(), comp_);
        c_.pop_back();
    }

    void clear() {
        while (!empty()) {
            pop();
        }
    }

    void swap(priority_queue& rhs) noexcept(noexcept(tinystl::swap(c_, rhs.c_)) &&
                                            noexcept(tinystl::swap(comp_, rhs.comp_))) {
        tinystl::swap(c_, rhs.c_);
        tinystl::swap(comp_, rhs.comp_);
    }

public:
    friend bool operator==(const priority_queue& lhs, const priority_queue& rhs) {
        return lhs.c_ == rhs.c_;
    }

    friend bool operator!=(const priority_queue& lhs, const priority_queue& rhs) {
        return !(lhs == rhs);
    }
};


// ============================= 重载比较操作符 ============================= //

template <class T, class Container, class Compare>
bool operator==(const priority_queue<T, Container, Compare>& lhs,
                const priority_queue<T, Container, Compare>& rhs) {
    return lhs == rhs;
}

template <class T, class Container, class Compare>
bool operator!=(const priority_queue<T, Container, Compare>& lhs,
                const priority_queue<T, Container, Compare>& rhs) {
    return !(lhs == rhs);
}

template <class T, class Container, class Compare>
void swap(priority_queue<T, Container, Compare>& lhs,
          priority_queue<T, Container, Compare>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

}  // namespace tinystl

#endif  // TINYSTL_QUEUE_H_