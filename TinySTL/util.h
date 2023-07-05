#ifndef TINYSTL_UTIL_H_
#define TINYSTL_UTIL_H_

// 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 

#include <cstddef>      // ptrdiff_t, size_t
#include <type_traits>  // remove_reference

namespace tinystl {

// move: 将左值转换为右值引用, 供移动构造使用
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

}



#endif