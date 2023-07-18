#ifndef TINYSTL_ALGOBASE_H_
#define TINYSTL_ALGOBASE_H_

// 这个头文件包含了 tinystl 的基本算法

#include <cstring>      // memmove

#include "iterator.h"
#include "util.h"

// max
// copy
// copy_backward
// move
// equal
// lexicographical_compare

namespace tinystl {

    #ifdef max
    #pragma message("#undefing macro max")
    #undef max
    #endif  // max

    #ifdef min
    #pragma message("#undefing macro min")
    #undef min
    #endif  // min


    // ========================== max ========================== // 
    // max: 返回两个参数中的较大者，语义相等时返回第一个参数
    
    template <class T>
    const T& max(const T& lhs, const T& rhs) {
        return lhs < rhs ? rhs : lhs;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class T, class Compare>
    const T& max(const T& lhs, const T& rhs, Compare comp) {
        return comp(lhs, rhs) ? rhs : lhs;
    }


    // ========================== min ========================== //
    // min: 返回两个参数中的较小者，语义相等时返回第一个参数

    template <class T>
    const T& min(const T& lhs, const T& rhs) {
        return rhs < lhs ? rhs : lhs;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class T, class Compare>
    const T& min(const T& lhs, const T& rhs, Compare comp) {
        return comp(rhs, lhs) ? rhs : lhs;
    }


    // ========================== iter_swap ========================== //
    // iter_swap: 将两个迭代器所指对象对调
    template <class ForwardIterator1, class ForwardIterator2>
    void iter_swap(ForwardIterator1 lhs, ForwardIterator2 rhs) {
        tinystl::swap(*lhs, *rhs);  // util.h
    }


    // ========================== copy ========================== //
    // copy: 将 [first, last) 区间内的元素拷贝到 [result, result + (last - first)) 区间内

    // input_iterator_tag 版本
    template <class InputIterator, class OutputIterator>
    OutputIterator unchecked_copy_cat(InputIterator first, InputIterator last, 
        OutputIterator result, tinystl::input_iterator_tag) {
        for (; first != last; ++first, ++result) {
            *result = *first;
        }
        return result;
    }

    // random_access_iterator_tag 版本
    template <class RandomAccessIterator, class OutputIterator>
    OutputIterator unchecked_copy_cat(RandomAccessIterator first, RandomAccessIterator last, 
        OutputIterator result, tinystl::random_access_iterator_tag) {
        for (auto n = last - first; n > 0; --n, ++first, ++result) {
            *result = *first;
        }
        return result;
    }

    template <class InputIterator, class OutputIterator>
    OutputIterator unchecked_copy(InputIterator first, InputIterator last, 
        OutputIterator result) {
        return unchecked_copy_cat(first, last, result, iterator_category(first));
    }

    // 为 trivally_copy_assignable 类型提供特化版本
    template <class Tp, class Up>
    typename std::enable_if<
        std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
        std::is_trivially_copy_assignable<Up>::value,
        Up*>::type
    unchecked_copy(Tp* first, Tp* last, Up* result) {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0) {
            std::memmove(result, first, n * sizeof(Up));
        }
        return result + n;
    }

    template <class InputIterator, class OutputIterator>
    OutputIterator copy(InputIterator first, InputIterator last, 
        OutputIterator result) {
        return unchecked_copy(first, last, result);
    }


    // ========================== copy_backward ========================== //
    // copy_backward: 将 [first, last) 区间内的元素拷贝到 [result - (last - first), result) 区间内

    // bidirectional_iterator_tag 版本
    template <class BidirectionalIterator1, class BidirectionalIterator2>
    BidirectionalIterator2 unchecked_copy_backward_cat(BidirectionalIterator1 first, 
        BidirectionalIterator1 last, BidirectionalIterator2 result, 
        tinystl::bidirectional_iterator_tag) {
        while (first != last) {
            *(--result) = *(--last);
        }
        return result;
    }

    // random_access_iterator_tag 版本
    template <class RandomAccessIterator1, class RandomAccessIterator2>
    RandomAccessIterator2 unchecked_copy_backward_cat(RandomAccessIterator1 first, 
        RandomAccessIterator1 last, RandomAccessIterator2 result, 
        tinystl::random_access_iterator_tag) {
        for (auto n = last - first; n > 0; --n) {
            *(--result) = *(--last);
        }
        return result;
    }

    template <class BidirectionalIterator1, class BidirectionalIterator2>
    BidirectionalIterator2 unchecked_copy_backward(BidirectionalIterator1 first, 
        BidirectionalIterator1 last, BidirectionalIterator2 result) {
        return unchecked_copy_backward_cat(first, last, result, iterator_category(first));
    }

    // 为 trivally_copy_assignable 类型提供特化版本
    template <class Tp, class Up>
    typename std::enable_if<
        std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
        std::is_trivially_copy_assignable<Up>::value,
        Up*>::type
    unchecked_copy_backward(Tp* first, Tp* last, Up* result) {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0) {
            result -= n;
            std::memmove(result, first, n * sizeof(Up));
        }
        return result;
    }

    template <class BidirectionalIterator1, class BidirectionalIterator2>
    BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, 
        BidirectionalIterator1 last, BidirectionalIterator2 result) {
        return unchecked_copy_backward(first, last, result);
    }


    // ========================== copy_if ========================== //
    // copy_if: 将 [first, last) 区间内满足一元操作 unary_pred 的元素拷贝到 [result, result + (last - first)) 区间内

    template <class InputIterator, class OutputIterator, class UnaryPredicate>
    OutputIterator unchecked_copy_if_cat(InputIterator first, InputIterator last, 
        OutputIterator result, UnaryPredicate unary_pred) {
        for (; first != last; ++first) {
            if (unary_pred(*first)) {
                *result = *first;
                ++result;
            }
        }
        return result;
    }


    // ========================== copy_n ========================== //
    // copy_n: 将 [first, first + n) 区间内的元素拷贝到 [result, result + n) 区间内
    // 返回一个 pair 分别指向拷贝结束的尾部

    // input_iterator_tag 版本
    // template <class InputIterator, class Size, class OutputIterator>














}



#endif