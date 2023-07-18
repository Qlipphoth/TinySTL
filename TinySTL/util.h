#ifndef TINYSTL_UTIL_H_
#define TINYSTL_UTIL_H_

// 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 

#include <cstddef>      // ptrdiff_t, size_t
#include <type_traits>  // remove_reference

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








};
































}


#endif