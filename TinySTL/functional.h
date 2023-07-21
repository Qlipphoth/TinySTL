#ifndef TINYSTL_FUNCTIONAL_H
#define TINYSTL_FUNCTIONAL_H

// 这个头文件包含了 mystl 的仿函数与哈希函数

#include <cstddef>

namespace tinystl {

// 定义一元函数的参数型别和返回值型别
template <class Arg, class Result>
struct unarg_function
{
  typedef Arg       argument_type;
  typedef Result    result_type;
};

// 定义二元函数的参数型别的返回值型别
template <class Arg1, class Arg2, class Result>
struct binary_function
{
  typedef Arg1      first_argument_type;
  typedef Arg2      second_argument_type;
  typedef Result    result_type;
};

/// @brief 仿函数：等于
/// @tparam T 
template <class T>
struct equal_to :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x == y; }
};

/// @brief 仿函数：小于
/// @tparam T 
template <class T>
struct less :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x < y; }
};

/// @brief 仿函数：大于
/// @tparam T 
template <class T>
struct greater :public binary_function<T, T, bool> {
  bool operator()(const T& x, const T& y) const { return x > y; }
};

}


#endif //TINYSTL_FUNCTIONAL_H