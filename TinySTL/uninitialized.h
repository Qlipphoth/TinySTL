#ifndef TINYSTL_UNINITIALIZED_H_
#define TINYSTL_UNINITIALIZED_H_

// 这个头文件用于对未初始化空间构造元素

// 涉及知识点：note3.md
// 1. typename
// 2. std::is_trivially_copy_assignable

#include <new>          // std::true_type, std::false_type
#include "algobase.h"   // copy, copy_n, copy_backward 

namespace tinystl {

// TODO: 对 char* 与 wchar_t* 的特化

/*****************************************************************************************/
// uninitialized_copy
// 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
/*****************************************************************************************/

/// @brief 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
/// @tparam InputIterator  输入迭代器
/// @tparam ForwardIterator  输出迭代器
/// @param first  输入迭代器的首位置
/// @param last  输入迭代器的尾位置
/// @param result  输出迭代器的首位置
/// @param   //std::true_type  用于区分是否为 POD 类型
/// @return  返回复制结束的位置
template <class InputIterator, class  ForwardIterator>
ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, 
    ForwardIterator result, std::true_type) {
        return tinystl::copy(first, last, result);
    }

/// @brief 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
/// @tparam InputIterator  输入迭代器
/// @tparam ForwardIterator  输出迭代器
/// @param first  输入迭代器的首位置
/// @param last  输入迭代器的尾位置
/// @param result  输出迭代器的首位置
/// @param   //std::false_type  用于区分是否为 POD 类型
/// @return  返回复制结束的位置
template <class InputIterator, class ForwardIterator>
ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, 
    ForwardIterator result, std::false_type) {
        auto cur = result;
        try {
            for (; first != last; ++first, ++cur) {
                // 移动构造
                tinystl::construct(&*cur, *first);
            }
        } 
        // commit or rollback，要么产生出所有的元素，要么一个都不产生
        // catch(...) 会捕获所有异常
        catch(...) {
            for (; result != cur; ++result) {
                // 析构
                tinystl::destroy(&*result);
            }
        }
        return cur;
    }

template <class InputIterator, class ForwardIteraotr>
ForwardIteraotr uninitialized_copy(InputIterator first, InputIterator last, 
    ForwardIteraotr result) {
        // 判断是否为 POD 类型，这里使用了 std::is_trivially_copy_assignable
        return tinystl::__uninitialized_copy(first, last, result, 
            std::is_trivially_copy_assignable<typename iterator_traits<InputIterator>::value_type>{});
        // TInySTL 中这里判断的是 ForwardIterator 的 value_type，感觉有点问题
    }


/*****************************************************************************************/
// uninitialized_copy_n
// 把 [first, first + n) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
/*****************************************************************************************/

template <class InputIterator, class Size, class ForwardIterator>
ForwardIterator __uninitialized_copy_n(InputIterator first, Size n, 
    ForwardIterator result, std::true_type) {
        return tinystl::copy_n(first, n, result);
    }

template <class InputIterator, class Size, class ForwardIterator>
ForwardIterator __uninitialized_copy_n(InputIterator first, Size n, 
    ForwardIterator result, std::false_type) {
        auto cur = result;
        try {
            for (; n > 0; --n, ++first, ++cur) {
                mystl::construct(&*cur, *first);  // 移动构造
            }
            catch (...) {
                for (; result != cur; ++result) {
                    mystl::destroy(&*result);  // 析构
                }
            }
        }
    }

template <class InputIterator, class Size, class ForwardIterator>
ForwardIterator uninitialized_copy_n(InputIterator first, Size n, 
    ForwardIterator result) {
        return tinystl::__uninitialized_copy_n(first, n, result, 
            std::is_trivially_copy_assignable<typename iterator_traits<InputIterator>::value_type>{});
    }


/*****************************************************************************************/
// uninitialized_fill
// 在 [first, last) 区间内填充元素值
/*****************************************************************************************/

template <class ForwardIterator, class T>
void __uninitialized_fill(ForwardIterator first, ForwardIterator last, 
    const T& value, std::true_type) {
        tinystl::fill(first, last, value);
    }

template <class ForwardIterator, class T>
void __uninitialized_fill(ForwardIterator first, ForwardIterator last, 
    const T& value, std::false_type) {
        auto cur = first;
        try {
            for (; cur != last; ++cur) {
                tinystl::construct(&*cur, value);  // 带参数构造，也是移动构造
            }
        }
        catch (...) {
            for (; first != cur; ++first) {
                tinystl::destroy(&*first);  // 析构
            }
        }
    }

template <class ForwardIterator, class T>
void uninitialized_fill(ForwardIterator first, ForwardIterator last, 
    const T& value) {
        tinystl::__uninitialized_fill(first, last, value, 
            std::is_trivially_copy_assignable<typename iterator_traits<ForwardIterator>::value_type>{});
    }


/*****************************************************************************************/
// uninitialized_fill_n
// 从 first 位置开始，填充 n 个元素值，返回填充结束的位置
/*****************************************************************************************/

template <class ForwardIterator, class Size, class T>
ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, 
    const T& value, std::true_type) {
        return tinystl::fill_n(first, n, value);
    }

template <class ForwardIterator, class Size, class T>
ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, 
    const T& value, std::false_type) {
        auto cur = first;
        try {
            for (; n > 0; --n, ++cur) {
                tinystl::construct(&*cur, value);
            }
        }
        catch (...) {
            for (; first != cur; ++first) {
                tinystl::destroy(&*first);
            }
        }
        return cur;
    }

template <class ForwardIterator, class Size, class T>
ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& value) {
    return tinystl::__uninitialized_fill_n(first, n, value, 
        std::is_trivially_copy_assignable<typename iterator_traits<ForwardIterator>::value_type>{});
    }


/*****************************************************************************************/
// uninitialized_move
// 把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
/*****************************************************************************************/

template <class InputIterator, class ForwardIterator>
ForwardIterator __uninitialized_move(InputIterator first, InputIterator last, 
    ForwardIterator result, std::true_type) {
        return tinystl::move(first, last, result);  // algobase.h
    }

template <class InputIterator, class ForwardIterator>
ForwardIterator __uninitialized_move(InputIterator first, InputIterator last, 
    ForwardIterator result, std::false_type) {
        auto cur = result;
        try {
            for (; first != last; ++first, ++cur) {
                tinystl::construct(&*cur, tinystl::move(*first));  // 转化为右值引用，移动构造
            }
        }
        catch (...) {
            for (; result != cur; ++result) {
                tinystl::destroy(&*result);  // 析构
            }
        }
        return cur;
    }
   
template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_move(InputIterator first, InputIterator last, 
    ForwardIterator result) {
        return tinystl::__uninitialized_move(first, last, result, 
            std::is_trivially_move_assignable<typename iterator_traits<InputIterator>::value_type>{});
    }


/*****************************************************************************************/
// uninitialized_move_n
// 把[first, first + n)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
/*****************************************************************************************/

template <class InputIterator, class Size, class ForwardIterator>
ForwardIterator __uninitialized_move_n(InputIterator first, Size n, 
    ForwardIterator result, std::true_type) {
        return tinystl::move(first, first + n, result);
    }

template <class InputIterator, class Size, class ForwardIterator>
ForwardIterator __uninitialized_move_n(InputIterator first, Size n, 
    ForwardIterator result, std::false_type) {
        auto cur = result;
        try {
            for (; n > 0; --n, ++first, ++cur) {
                tinystl::construct(&*cur, tinystl::move(*first));
            }
        }
        catch (...) {
            for (; result != cur; ++result) {
                tinystl::destroy(&*result);
            }
        }
        return cur;
    }

template <class InputIterator, class Size, class ForwardIterator>
ForwardIterator uninitialized_move_n(InputIterator first, Size n, 
    ForwardIterator result) {
        return tinystl::__uninitialized_move_n(first, n, result, 
            std::is_trivially_move_assignable<typename iterator_traits<InputIterator>::value_type>{});
    }


}

#endif