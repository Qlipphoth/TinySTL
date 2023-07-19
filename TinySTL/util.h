#ifndef TINYSTL_UTIL_H_
#define TINYSTL_UTIL_H_

// 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 

#include <cstddef>      // ptrdiff_t, size_t
#include "type_traits.h"  // remove_reference

namespace tinystl {

// move: 将左值或右值转换为右值引用
// 通过 std::remove_reference<T>::type 获取 T 的类型，然后将其转换为右值引用

/// @brief 将左值转换为右值引用
/// @tparam T  一个类型
/// @param arg  一个左值
/// @return  一个右值引用
template <class T>
typename std::remove_reference<T>::type&& move(T&& arg) noexcept {
    return static_cast<typename std::remove_reference<T>::type&&>(arg);
}

// forward: 用于完美转发，保留参数的左值或右值属性，用作模板函数的参数

/// @brief 将左值引用转发为左值引用
/// @tparam T  一个类型
/// @param arg  一个左值引用
/// @return  一个左值引用
template <class T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept {
    return static_cast<T&&>(arg);
}

/// @brief 将右值引用转发为右值引用
/// @tparam T  一个类型
/// @param arg  一个右值引用
/// @return  一个右值引用
template <class T>
T&& forward(typename std::remove_reference<T>::type&& arg) noexcept {
    // 静态断言，T 不是左值引用
    static_assert(!std::is_lvalue_reference<T>::value, "bad forward");  
    return static_cast<T&&>(arg);
}

// swap: 交换两个对象的值

template <class Tp>
void swap(Tp& lhs, Tp& rhs) {
    // auto tmp = tinystl::move(lhs);  // 拷贝构造
    auto tmp(tinystl::move(lhs));   // 移动构造
    lhs = tinystl::move(rhs);
    rhs = tinystl::move(tmp);
}

template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator2 swap_range(ForwardIterator1 first1, ForwardIterator1 last1, 
    ForwardIterator2 first2) {
    for (; first1 != last1; ++first1, (void) ++first2) {  // TODO: 为什么要转成 void
        tinystl::swap(*first1, *first2);
    }
    return first2;
}

// 针对数组的特化版本
// 通过模板参数推导，可以推导出数组的大小
template <class Tp, size_t N>
void swap(Tp(&lhs)[N], Tp(&rhs)[N]) {
    tinystl::swap_range(lhs, lhs + N, rhs);
}


// ========================== pair ========================== //
// pair: 一个模板结构体，用于存储一对值

template <class T1, class T2>
struct pair {
    typedef T1 first_type;
    typedef T2 second_type;

    first_type first;
    second_type second;

    // 默认构造
    template <class U1 = T1, class U2 = T2, 
        typename std::enable_if<std::is_default_constructible<U1>::value &&
        std::is_default_constructible<U2>::value, void>::type>
        constexpr pair() : first(), second() {}

    // 拷贝构造

    // implicit constructiable for this type
    template <class U1 = T1, class U2 = T2, 
        typename std::enable_if<
        std::is_copy_constructible<U1>::value &&
        std::is_copy_constructible<U2>::value &&
        std::is_convertible<const U1&, T1>::value &&
        std::is_convertible<const U2&, T2>::value, int>::type = 0>
        constexpr pair(const T1& a, const T2& b) : first(a), second(b) {}
    
    // explicit constructiable for this type
    template <class U1 = T1, class U2 = T2, 
        typename std::enable_if<
        std::is_copy_constructible<U1>::value &&
        std::is_copy_constructible<U2>::value &&
        (!std::is_convertible<const U1&, T1>::value ||
         !std::is_convertible<const U2&, T2>::value), int>::type = 0>
        explicit constexpr pair(const T1& a, const T2& b) : first(a), second(b) {}

    // 使用默认的实现
    pair(const pair& rhs) = default;
    pair(pair&& rhs) = default;

    // implicit constructiable for this type
    template <class U1, class U2, 
        typename std::enable_if<
        std::is_constructible<T1, U1>::value &&
        std::is_constructible<T2, U2>::value &&
        std::is_convertible<U1&&, T1>::value &&
        std::is_convertible<U2&&, T2>::value, int>::type = 0>
        constexpr pair(U1&& a, U2&& b) : 
        first(tinystl::forward<U1>(a)),
        second(tinystl::forward<U2>(b)) {}

    // explicit constructiable for this type
    template <class U1, class U2, 
        typename std::enable_if<
        std::is_constructible<T1, U1>::value &&
        std::is_constructible<T2, U2>::value &&
        (!std::is_convertible<U1, T1>::value ||
         !std::is_convertible<U2, T2>::value), int>::type = 0>
        explicit constexpr pair(U1&& a, U2&& b) : 
        first(tinystl::forward<U1>(a)),
        second(tinystl::forward<U2>(b)) {}

    // implicit constructiable for this type
    template <class U1, class U2, 
        typename std::enable_if<
        std::is_constructible<T1, const U1&>::value &&
        std::is_constructible<T2, const U2&>::value &&
        std::is_convertible<const U1&, T1>::value &&
        std::is_convertible<const U2&, T2>::value, int>::type = 0>
        constexpr pair(const pair<U1, U2>& rhs) : 
        first(rhs.first),
        second(rhs.second) {}

    // explicit constructiable for this type
    template <class U1, class U2, 
        typename std::enable_if<
        std::is_constructible<T1, const U1&>::value &&
        std::is_constructible<T2, const U2&>::value &&
        (!std::is_convertible<const U1&, T1>::value ||
         !std::is_convertible<const U2&, T2>::value), int>::type = 0>
        explicit constexpr pair(const pair<U1, U2>& rhs) : 
        first(rhs.first),
        second(rhs.second) {}

    // implicit constructiable for this type
    template <class U1, class U2, 
        typename std::enable_if<
        std::is_constructible<T1, U1>::value &&
        std::is_constructible<T2, U2>::value &&
        std::is_convertible<U1, T1>::value &&
        std::is_convertible<U2, T2>::value, int>::type = 0>
        constexpr pair(pair<U1, U2>&& rhs) : 
        first(tinystl::forward<U1>(rhs.first)),
        second(tinystl::forward<U2>(rhs.second)) {}

    // explicit constructiable for this type
    template <class U1, class U2, 
        typename std::enable_if<
        std::is_constructible<T1, U1>::value &&
        std::is_constructible<T2, U2>::value &&
        (!std::is_convertible<U1, T1>::value ||
         !std::is_convertible<U2, T2>::value), int>::type = 0>
        explicit constexpr pair(pair<U1, U2>&& rhs) : 
        first(tinystl::forward<U1>(rhs.first)),
        second(tinystl::forward<U2>(rhs.second)) {}

    // copy assign
    pair& operator=(const pair& rhs) {
        if (this != &rhs) {
            first = rhs.first;
            second = rhs.second;
        }
        return *this;
    }

    // move assign
    pair& operator=(pair&& rhs) {
        if (this != &rhs) {
            first = tinystl::move(rhs.first);
            second = tinystl::move(rhs.second);
        }
        return *this;
    }

    // copy assign for pair with different types
    template <class U1, class U2>
    pair& operator=(const pair<U1, U2>& rhs) {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }

    // move assign for pair with different types
    template <class U1, class U2>
    pair& operator=(pair<U1, U2>&& rhs) {
        first = tinystl::forward<U1>(rhs.first);
        second = tinystl::forward<U2>(rhs.second);
        return *this;
    }

    ~pair() = default;

    // swap

    void swap(pair& rhs) {
        if (this != &rhs) {
            tinystl::swap(first, rhs.first);
            tinystl::swap(second, rhs.second);
        }
    }

};


// ========================== 重载比较运算符 ========================== //

template <class T1, class T2>
bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <class T1, class T2>
bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return (lhs.first < rhs.first) || ((lhs.first == rhs.first) && (lhs.second < rhs.second));
}

template <class T1, class T2>
bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(lhs == rhs);
}

template <class T1, class T2>
bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return rhs < lhs;
}

template <class T1, class T2>
bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(rhs < lhs);
}

template <class T1, class T2>
bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(lhs < rhs);
}

// 重载 tinystl 的 swap

template <class T1, class T2>
void swap(pair<T1, T2>& lhs, pair<T1, T2>& rhs) {
    lhs.swap(rhs);
}

// ========================== make_pair ========================== //
// make_pair: 用于生成一个 pair 对象

template <class T1, class T2>
pair<T1, T2> make_pair(T1&& first, T2&& second) {
    return pair<T1, T2>(tinystl::forward<T1>(first), tinystl::forward<T2>(second));
}

}

#endif  // TINYSTL_UTIL_H_