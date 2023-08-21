#ifndef TINYSTL_ALGO_H_
#define TINYSTL_ALGO_H_

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif

// 这个头文件包含了 tinystl 的一系列算法

#include <cstddef>
#include <ctime>

#include "algobase.h"
#include "memory.h"
// #include "heap_algo.h"
#include "functional.h"

namespace tinystl {

/*****************************************************************************************/
// reverse
// 将[first, last)区间内的元素反转
/*****************************************************************************************/

template <class BidirectionalIterator>
void reverse_dispatch(BidirectionalIterator first, BidirectionalIterator last, 
    bidirectional_iterator_tag) {
    while (true) {
        if (first == last || first == --last) return;
        tinystl::iter_swap(first++, last);
    }
}

template <class RandomAccessIterator>
void reverse_dispatch(RandomAccessIterator first, RandomAccessIterator last, 
    random_access_iterator_tag) {
    while (first < last) {
        tinystl::iter_swap(first++, --last);
    }
}

template <class BidirectionalIterator>
void reverse(BidirectionalIterator first, BidirectionalIterator last) {
    tinystl::reverse_dispatch(first, last, iterator_category(first));
}

/*****************************************************************************************/
// lower_bound
// 在[first, last)中查找第一个不小于 value 的元素，并返回指向它的迭代器，若没有则返回 last
/*****************************************************************************************/

/// @brief lower_bound 的 forward_iterator_tag 版本
template <class ForwardIterator, class T>
ForwardIterator lbond_dispatch(ForwardIterator first, ForwardIterator last, 
                               const T& value, forward_iterator_tag) {
    auto len = tinystl::distance(first, last);
    auto half = len;
    ForwardIterator middle;
    // 二分查找
    while (len > 0) {
        half = len >> 1;
        middle = first;
        tinystl::advance(middle, half);
        if (*middle < value) {
            first = middle;
            ++first;
            len = len - half - 1;
        } 
        else len = half;
    }
    return first;
} 

/// @brief lower_bound 的 random_access_iterator_tag 版本 
template <class RandomAccessIterator, class T>
RandomAccessIterator lbond_dispatch(RandomAccessIterator first, RandomAccessIterator last, 
                                    const T& value, random_access_iterator_tag) {
    auto len = last - first;
    auto half = len;
    RandomAccessIterator middle;
    // 二分查找
    while (len > 0) {
        half = len >> 1;
        middle = first + half;
        if (*middle < value) {
            first = middle + 1;
            len = len - half - 1;
        } 
        else len = half;
    }
    return first;
}

/// @brief 在[first, last)中查找第一个不小于 value 的元素，根据迭代器类型调用不同版本的 lbond_dispatch
/// @return  返回指向第一个不小于 value 的元素的迭代器，若没有则返回 last
template <class ForwardIterator, class T>
ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value) {
    return tinystl::lbond_dispatch(first, last, value, iterator_category(first));
}

template <class ForwardIterator, class T, class Compare>
ForwardIterator lbound_dispatch(ForwardIterator first, ForwardIterator last, 
                                const T& value, Compare comp, forward_iterator_tag) {
    auto len = tinystl::distance(first, last);
    auto half = len;
    ForwardIterator middle;
    // 二分查找
    while (len > 0) {
        half = len >> 1;
        middle = first;
        tinystl::advance(middle, half);
        if (comp(*middle, value)) {
            first = middle;
            ++first;
            len = len - half - 1;
        } 
        else len = half;
    }
    return first;
}

template <class RandomAccessIterator, class T, class Compare>
RandomAccessIterator lbound_dispatch(RandomAccessIterator first, RandomAccessIterator last, 
                                     const T& value, Compare comp, random_access_iterator_tag) {
    auto len = last - first;
    auto half = len;
    RandomAccessIterator middle;
    // 二分查找
    while (len > 0) {
        half = len >> 1;
        middle = first + half;
        if (comp(*middle, value)) {
            first = middle + 1;
            len = len - half - 1;
        } 
        else len = half;
    }
    return first;
}

/// @brief 在[first, last)中查找第一个不小于 value 的元素，使用 comp 比较元素大小
/// @return 返回指向第一个不小于 value 的元素的迭代器，若没有则返回 last
template <class ForwardIterator, class T, class Compare>
ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
    return tinystl::lbound_dispatch(first, last, value, comp, iterator_category(first));
}


/*****************************************************************************************/
// is_permutation
// 判断[first1,last1)是否为[first2, last2)的排列组合
/*****************************************************************************************/
// TODO: 理解这部分
template <class ForwardIter1, class ForwardIter2, class BinaryPred>
bool is_permutation_aux(ForwardIter1 first1, ForwardIter1 last1,
                        ForwardIter2 first2, ForwardIter2 last2,
                        BinaryPred pred) {
    constexpr bool is_ra_it = tinystl::is_random_access_iterator<ForwardIter1>::value
        && tinystl::is_random_access_iterator<ForwardIter2>::value;
    if (is_ra_it) {
        auto len1 = last1 - first1;
        auto len2 = last2 - first2;
        if (len1 != len2)
        return false;
    }

    // 先找出相同的前缀段
    for (; first1 != last1 && first2 != last2; ++first1, (void) ++first2) {
        if (!pred(*first1, *first2))
        break;
    }
    if (is_ra_it) {
        if (first1 == last1)
        return true;
    }
    else {
        auto len1 = tinystl::distance(first1, last1);
        auto len2 = tinystl::distance(first2, last2);
        if (len1 == 0 && len2 == 0)
        return true;
        if (len1 != len2)
        return false;
    }

    // 判断剩余部分
    for (auto i = first1; i != last1; ++i) {
        bool is_repeated = false;
        for (auto j = first1; j != i; ++j) {
            if (pred(*j, *i)) {
                is_repeated = true;
                break;
            }
        }

        if (!is_repeated) {
            // 计算 *i 在 [first2, last2) 的数目
            auto c2 = 0;
            for (auto j = first2; j != last2; ++j) {
                if (pred(*i, *j))
                ++c2;
            }
            if (c2 == 0) return false;

            // 计算 *i 在 [first1, last1) 的数目
            auto c1 = 1;
            auto j = i;
            for (++j; j != last1; ++j) {
                if (pred(*i, *j))
                ++c1;
            }
            if (c1 != c2) return false;
        }
    }
    return true;
}

template <class ForwardIter1, class ForwardIter2, class BinaryPred>
bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                    ForwardIter2 first2, ForwardIter2 last2,
                    BinaryPred pred) {
    return is_permutation_aux(first1, last1, first2, last2, pred);
}

template <class ForwardIter1, class ForwardIter2>
bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                    ForwardIter2 first2, ForwardIter2 last2) {
    typedef typename iterator_traits<ForwardIter1>::value_type v1;
    typedef typename iterator_traits<ForwardIter2>::value_type v2;
    static_assert(std::is_same<v1, v2>::value, "the type should be same in tinystl::is_permutation");
    return is_permutation_aux(first1, last1, first2, last2, tinystl::equal_to<v1>());
}


}  // namespace tinystl

#endif  // TINYSTL_ALGO_H_