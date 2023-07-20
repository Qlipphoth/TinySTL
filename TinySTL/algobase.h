#ifndef TINYSTL_ALGOBASE_H_
#define TINYSTL_ALGOBASE_H_

// 这个头文件包含了 tinystl 的基本算法

#include <cstring>      // memmove

#include "iterator.h"
#include "util.h"

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
    template <class InputIterator, class Size, class OutputIterator>
    tinystl::pair<InputIterator, OutputIterator> unchecked_copy_n_cat(InputIterator first, 
        Size n, OutputIterator result, tinystl::input_iterator_tag) {
        for (; n > 0; --n, ++first, ++result) {
            *result = *first;
        }
        return tinystl::make_pair<InputIterator, OutputIterator>(first, result);
    }

    // random_access_iterator_tag 版本
    template <class RandomAccessIterator, class Size, class OutputIterator>
    tinystl::pair<RandomAccessIterator, OutputIterator> unchecked_copy_n_cat(RandomAccessIterator first, 
        Size n, OutputIterator result, tinystl::random_access_iterator_tag) {
        auto last = first + n;
        return tinystl::make_pair(last, tinystl::copy(first, last, result));
    }

    template <class InputIterator, class Size, class OutputIterator>
    tinystl::pair<InputIterator, OutputIterator> unchecked_copy_n(InputIterator first, 
        Size n, OutputIterator result) {
        return unchecked_copy_n_cat(first, n, result, iterator_category(first));
    }

    // 为 trivally_copy_assignable 类型提供特化版本
    template <class Tp, class Size, class Up>
    typename std::enable_if<
        std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
        std::is_trivially_copy_assignable<Up>::value,
        tinystl::pair<Tp*, Up*>>::type
        unchecked_copy_n(Tp* first, Size n, Up* result) {
        const auto last = first + n;
        if (n != 0) {
            std::memmove(result, first, n * sizeof(Up));
        }
        return tinystl::make_pair(last, result + n);
    }

    template <class InputIterator, class Size, class OutputIterator>
    tinystl::pair<InputIterator, OutputIterator> copy_n(InputIterator first, 
        Size n, OutputIterator result) {
        return unchecked_copy_n(first, n, result);
    }

    
    // ========================== move ========================== //
    // move: 将 [first, last) 区间内的元素移动到 [result, result + (last - first)) 区间内

    // input_iterator_tag 版本
    template <class InputIterator, class OutputIterator>
    OutputIterator unchecked_move_cat(InputIterator first, InputIterator last, 
        OutputIterator result, tinystl::input_iterator_tag) {
        for (; first != last; ++first, ++result) {
            *result = tinystl::move(*first);
        }
        return result;
    }

    // random_access_iterator_tag 版本
    template <class RandomAccessIterator, class OutputIterator>
    OutputIterator unchecked_move_cat(RandomAccessIterator first, RandomAccessIterator last, 
        OutputIterator result, tinystl::random_access_iterator_tag) {
        for (auto n = last - first; n > 0; --n, ++first, ++result) {
            *result = tinystl::move(*first);
        }
        return result;
    }

    template <class InputIterator, class OutputIterator>
    OutputIterator unchecked_move(InputIterator first, InputIterator last, 
        OutputIterator result) {
        return unchecked_move_cat(first, last, result, iterator_category(first));
    }

    // 为 trivally_copy_assignable 类型提供特化版本
    template <class Tp, class Up>
    typename std::enable_if<
        std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
        std::is_trivially_copy_assignable<Up>::value,
        Up*>::type
    unchecked_move(Tp* first, Tp* last, Up* result) {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0) {
            std::memmove(result, first, n * sizeof(Up));
        }
        return result + n;
    }

    /// @brief 将 [first, last) 区间内的元素移动到 [result, result + (last - first)) 区间内
    /// @tparam InputIterator  输入迭代器
    /// @tparam OutputIterator  输出迭代器
    /// @param first  源区间的起始迭代器
    /// @param last  源区间的结束迭代器
    /// @param result  目标区间的起始迭代器
    /// @return  返回目标区间的结束迭代器
    template <class InputIterator, class OutputIterator>
    OutputIterator move(InputIterator first, InputIterator last, 
        OutputIterator result) {
        return unchecked_move(first, last, result);
    }

    // ========================== move_backward ========================== //
    // move_backward: 将 [first, last) 区间内的元素移动到 [result - (last - first), result) 区间内

    // bidirectional_iterator_tag 版本
    template <class BidirectionalIterator1, class BidirectionalIterator2>
    BidirectionalIterator2 unchecked_move_backward_cat(BidirectionalIterator1 first, 
        BidirectionalIterator1 last, BidirectionalIterator2 result, 
        tinystl::bidirectional_iterator_tag) {
        while (first != last) {
            *(--result) = tinystl::move(*(--last));
        }
        return result;
    }

    // random_access_iterator_tag 版本
    template <class RandomAccessIterator1, class RandomAccessIterator2>
    RandomAccessIterator2 unchecked_move_backward_cat(RandomAccessIterator1 first, 
        RandomAccessIterator1 last, RandomAccessIterator2 result, 
        tinystl::random_access_iterator_tag) {
        for (auto n = last - first; n > 0; --n) {
            *(--result) = tinystl::move(*(--last));
        }
        return result;
    }

    template <class BidirectionalIterator1, class BidirectionalIterator2>
    BidirectionalIterator2 unchecked_move_backward(BidirectionalIterator1 first, 
        BidirectionalIterator1 last, BidirectionalIterator2 result) {
        return unchecked_move_backward_cat(first, last, result, iterator_category(first));
    }

    // 为 trivally_copy_assignable 类型提供特化版本
    template <class Tp, class Up>
    typename std::enable_if<
        std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
        std::is_trivially_move_assignable<Up>::value,
        Up*>::type
    unchecked_move_backward(Tp* first, Tp* last, Up* result) {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0) {
            result -= n;
            std::memmove(result, first, n * sizeof(Up));
        }
        return result;
    }

    template <class BidirectionalIterator1, class BidirectionalIterator2>
    BidirectionalIterator2 move_backward(BidirectionalIterator1 first, 
        BidirectionalIterator1 last, BidirectionalIterator2 result) {
        return unchecked_move_backward(first, last, result);
    }


    // ========================== equal ========================== //
    // equal: 比较两个序列是否相等

    template <class InputIterator1, class InputIterator2>
    bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
        for (; first1 != last1; ++first1, ++first2) {
            if (*first1 != *first2) {
                return false;
            }
        }
        return true;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class InputIterator1, class InputIterator2, class BinaryPredicate>
    bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, 
        BinaryPredicate comp) {
        for (; first1 != last1; ++first1, ++first2) {
            if (!comp(*first1, *first2)) {
                return false;
            }
        }
        return true;
    }


    // ========================== fill_n ========================== //
    // fill_n: 从 first 位置开始，填充 n 个元素值，返回填充结束的位置

    template <class OutputIterator, class Size, class T>
    OutputIterator unchecked_fill_n(OutputIterator first, Size n, const T& value) {
        for (; n > 0; --n, ++first) {
            *first = value;
        }
        return first;
    }

    // 为 one-byte 类型提供特化版本
    template <class Tp, class Size, class Up>
    typename std::enable_if<
        std::is_integral<Tp>::value && sizeof(Tp) == 1 &&
        !std::is_same<Tp, bool>::value &&
        std::is_integral<Up>::value && sizeof(Up) == 1,
        Tp*>::type
    unchecked_fill_n(Tp* first, Size n, Up value) {
        if (n > 0) {
            std::memset(first, static_cast<unsigned char>(value), (size_t)n);
        }
        return first + n;
    }

    template <class OutputIterator, class Size, class T>
    OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
        return unchecked_fill_n(first, n, value);
    }


    // ========================== fill ========================== //
    // fill: 将 [first, last) 区间内的元素填充新值

    // forward_iterator_tag 版本
    template <class ForwardIterator, class T>
    void fill_cat(ForwardIterator first, ForwardIterator last, const T& value) {
        for (; first != last; ++first) {
            *first = value;
        }
    }

    // random_access_iterator_tag 版本
    template <class RandomAccessIterator, class T>
    void fill_cat(RandomAccessIterator first, RandomAccessIterator last, const T& value, 
        tinystl::random_access_iterator_tag) {
        fill_n(first, last - first, value);
    }

    template <class ForwardIterator, class T>
    void fill(ForwardIterator first, ForwardIterator last, const T& value) {
        fill_cat(first, last, value, iterator_category(first));
    }


    // ========================== lexicographical_compare ========================== //
    // lexicographical_compare: 按字典序比较两个序列
    // 以字典序排列对两个序列进行比较，当在某个位置发现第一组不相等元素时，有下列几种情况：
    // (1)如果第一序列的元素较小，返回 true ，否则返回 false
    // (2)如果到达 last1 而尚未到达 last2 返回 true
    // (3)如果到达 last2 而尚未到达 last1 返回 false
    // (4)如果同时到达 last1 和 last2 返回 false

    // 若序列一严格小于序列二，返回 true，否则返回 false
    template <class InputIterator1, class InputIterator2>
    bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                                 InputIterator2 first2, InputIterator2 last2) {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
            if (*first1 < *first2) {
                return true;
            }
            if (*first2 < *first1) {
                return false;
            }
        }
        return first1 == last1 && first2 != last2;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class InputIterator1, class InputIterator2, class Compare>
    bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                                 InputIterator2 first2, InputIterator2 last2,
                                 Compare comp) {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
            if (comp(*first1, *first2)) {
                return true;
            }
            if (comp(*first2, *first1)) {
                return false;
            }
        }
        return first1 == last1 && first2 != last2;
    }

    // 针对 const unsigned char* 的特化版本
    inline bool lexicographical_compare(const unsigned char* first1, const unsigned char* last1,
                                        const unsigned char* first2, const unsigned char* last2) {
        const auto len1 = last1 - first1;
        const auto len2 = last2 - first2;
        // 先比较相同长度的部分
        const auto result = std::memcmp(first1, first2, tinystl::min(len1, len2));
        // 若相同长度的部分相等，则长度较长的序列更大
        return result != 0 ? result < 0 : len1 < len2;
    }


    // ========================== mismatch ========================== //
    // mismatch: 比较两个序列的第一个不匹配点
    // 平行比较两个序列，找到第一处失配的元素，返回一对迭代器，分别指向两个序列中失配的元素

    template <class InputIterator1, class InputIterator2>
    tinystl::pair<InputIterator1, InputIterator2>
    mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
        while (first1 != last1 && *first1 == *first2) {
            ++first1; ++first2;
        }
        return tinystl::pair<InputIterator1, InputIterator2>(first1, first2);
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class InputIterator1, class InputIterator2, class Compare>
    tinystl::pair<InputIterator1, InputIterator2>
    mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, Compare comp) {
        while (first1 != last1 && comp(*first1, *first2)) {
            ++first1; ++first2;
        }
        return tinystl::pair<InputIterator1, InputIterator2>(first1, first2);
    }

}  // namespace tinystl

#endif  // !TINYSTL_ALGOBASE_H_