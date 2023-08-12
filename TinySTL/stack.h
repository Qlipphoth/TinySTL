#ifndef TINYSTL_STACK_H_
#define TINYSTL_STACK_H_

// 这个头文件包含了一个模板类 stack
// stack : 栈
// SGI-STL 中，stack 是以 deque 为底层容器的适配器

#include "deque.h"

namespace tinystl {

/// @brief 模板类 stack
/// @tparam T 数据类型
/// @tparam Container 底层容器类型，默认为 tinystl::deque<T>
template <class T, class Container = tinystl::deque<T>>
class stack {

public:
    typedef Container                           container_type;
    // 使用底层容器的型别
    typedef typename Container::value_type      value_type;
    typedef typename Container::size_type       size_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;

    static_assert(std::is_same<T, value_type>::value, "the value_type of Container should be same with T");

private:
    container_type c_;  // 底层容器

public:  // 构造、复制、移动、析构函数

    stack() = default;  // 默认构造函数
    // 构造一个有 n 个元素的 stack
    explicit stack(size_type n) : c_(n) {}  
    // 构造一个有 n 个元素的 stack，每个元素都是 value
    stack(size_type n, const value_type& value) : c_(n, value) {}  

    template <class InputIterator>
    stack(InputIterator first, InputIterator last) : c_(first, last) {}

    stack(std::initializer_list<value_type> ilist) : c_(ilist.begin(), ilist.end()) {}

    stack(const Container& c) : c_(c) {}
    // 见 stack.md
    stack(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value) : c_(std::move(c)) {}
    
    stack(const stack& rhs) : c_(rhs.c_) {}
    stack(stack&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value) : c_(std::move(rhs.c_)) {}

    stack& operator=(const stack& rhs) {
        c_ = rhs.c_;
        return *this;
    }

    stack& operator=(stack&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value) {
        c_ = std::move(rhs.c_);
        return *this;
    }

    stack& operator=(std::initializer_list<value_type> ilist) {
        c_ = ilist;
        return *this;
    }

    ~stack() = default;

public:  // 元素相关操作

    reference       top()                  { return c_.back(); }
    const_reference top() const            { return c_.back(); }

    bool            empty() const noexcept { return c_.empty(); }
    size_type       size() const noexcept  { return c_.size(); }

    template<class... Args>
    void emplace(Args&&... args) { c_.emplace_back(tinystl::forward<Args>(args)...); }

    void push(const value_type& value) { c_.push_back(value); }
    void push(value_type&& value)      { c_.push_back(std::move(value)); }

    void pop() { c_.pop_back(); }

    void clear() {while (!empty()) pop();}

    void swap(stack& rhs) noexcept(noexcept(tinystl::swap(c_, rhs.c_))) { tinystl::swap(c_, rhs.c_); }

public: 
    // 为了访问 private 的 c_，声明为友元
    friend bool operator==(const stack& lhs, const stack& rhs) { return lhs.c_ == rhs.c_; }
    friend bool operator< (const stack& lhs, const stack& rhs) { return lhs.c_ <  rhs.c_; }

};

// ====================================== 重载比较操作符 ====================================== //

template <class T, class Container>
bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return lhs.c_ == rhs.c_;
}

template <class T, class Container>
bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return !(lhs == rhs);
}

template <class T, class Container>
bool operator< (const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return lhs.c_ < rhs.c_;
}

template <class T, class Container>
bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return !(rhs < lhs);
}

template <class T, class Container>
bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return !(lhs < rhs);
}

template <class T, class Container>
bool operator> (const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return rhs < lhs;
}

// ====================================== 重载 swap ====================================== //

template <class T, class Container>
void swap(stack<T, Container>& lhs, stack<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

}  // namespace tinystl

#endif // !TINYSTL_STACK_H_