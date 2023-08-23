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
// all_of
// 检查[first, last)内是否全部元素都满足一元操作 unary_pred 为 true 的情况，满足则返回 true
/*****************************************************************************************/

/// @brief 检查[first, last)内是否全部元素都满足一元操作 unary_pred 为 true 的情况，满足则返回 true
template <class InputIterator, class UnaryPredicate>
bool all_of(InputIterator first, InputIterator last, UnaryPredicate unary_pred) {
    for (; first != last; ++first) {
        if (!unary_pred(*first)) return false;
    }
    return true;
}

/*****************************************************************************************/
// any_of
// 检查[first, last)内是否存在某个元素满足一元操作 unary_pred 为 true 的情况，满足则返回 true
/*****************************************************************************************/

/// @brief 检查[first, last)内是否存在某个元素满足一元操作 unary_pred 为 true 的情况，满足则返回 true
template <class InputIterator, class UnaryPredicate>
bool any_of(InputIterator first, InputIterator last, UnaryPredicate unary_pred) {
    for (; first != last; ++first) {
        if (unary_pred(*first)) return true;
    }
    return false;
}

/*****************************************************************************************/
// none_of
// 检查[first, last)内是否全部元素都不满足一元操作 unary_pred 为 true 的情况，满足则返回 true
/*****************************************************************************************/

/// @brief 检查[first, last)内是否全部元素都不满足一元操作 unary_pred 为 true 的情况，满足则返回 true
template <class InputIter, class UnaryPredicate>
bool none_of(InputIter first, InputIter last, UnaryPredicate unary_pred) {
    for (; first != last; ++first) {
        if (unary_pred(*first)) return false;
    }
    return true;
}

/*****************************************************************************************/
// count
// 对[first, last)区间内的元素与给定值进行比较，缺省使用 operator==，返回元素相等的个数
/*****************************************************************************************/

/// @brief 对[first, last)区间内的元素与给定值进行比较，缺省使用 operator==，返回元素相等的个数
template <class InputIterator, class T>
size_t count(InputIterator first, InputIterator last, const T& value) {
    size_t n = 0;
    for (; first != last; ++first) {
        if (*first == value) ++n;
    }
    return n;
}

/*****************************************************************************************/
// count_if
// 对[first, last)区间内的每个元素都进行一元 unary_pred 操作，返回结果为 true 的个数
/*****************************************************************************************/

/// @brief 对[first, last)区间内的每个元素都进行一元 unary_pred 操作，返回结果为 true 的个数
template <class InputIterator, class UnaryPredicate>
size_t count_if(InputIterator first, InputIterator last, UnaryPredicate unary_pred) {
    size_t n = 0;
    for (; first != last; ++first) {
        if (unary_pred(*first)) ++n;
    }
    return n;
}

/*****************************************************************************************/
// find
// 在[first, last)区间内找到等于 value 的元素，返回指向该元素的迭代器
/*****************************************************************************************/

/// @brief 在[first, last)区间内找到等于 value 的元素，返回指向该元素的迭代器
template <class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value) {
    for (; first != last; ++first) {
        if (*first == value) return first;
    }
    return last;
}

/*****************************************************************************************/
// find_if
// 在[first, last)区间内找到第一个令一元操作 unary_pred 为 true 的元素并返回指向该元素的迭代器
/*****************************************************************************************/

/// @brief 在[first, last)区间内找到第一个令一元操作 unary_pred 为 true 的元素并返回指向该元素的迭代器
template <class InputIterator, class UnaryPredicate>
InputIterator find_if(InputIterator first, InputIterator last, UnaryPredicate unary_pred) {
    for (; first != last; ++first) {
        if (unary_pred(*first)) return first;
    }
    return last;
}

/*****************************************************************************************/
// find_if_not
// 在[first, last)区间内找到第一个令一元操作 unary_pred 为 false 的元素并返回指向该元素的迭代器
/*****************************************************************************************/

/// @brief 在[first, last)区间内找到第一个令一元操作 unary_pred 为 false 的元素并返回指向该元素的迭代器
template <class InputIterator, class UnaryPredicate>
InputIterator find_if_not(InputIterator first, InputIterator last, UnaryPredicate unary_pred) {
    for (; first != last; ++first) {
        if (!unary_pred(*first)) return first;
    }
    return last;
}

/*****************************************************************************************/
// search
// 在[first1, last1)中查找[first2, last2)的首次出现点
/*****************************************************************************************/

/// @brief 在[first1, last1)中查找[first2, last2)的首次出现点
template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1, 
                        ForwardIterator2 first2, ForwardIterator2 last2) {
    auto d1 = tinystl::distance(first1, last1);
    auto d2 = tinystl::distance(first2, last2);
    if (d1 < d2) return last1;
    auto current1 = first1;
    auto current2 = first2;
    while (current2 != last2) {
        // 如果 *current1 == *current2，则继续比较下一个元素
        if (*current1 == *current2) {
            ++current1;
            ++current2;
        }
        // 否则，从 first1 的下一个元素开始重新匹配
        else {
            // 如果剩余元素个数小于 first2 的长度，则不可能匹配成功
            if (d1 == d2) return last1;
            current1 = ++first1;  // 从 first1 的下一个元素开始重新匹配
            current2 = first2;    // current2 重新指向 first2
            --d1;
        }
    }
    return first1;
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1, 
                        ForwardIterator2 first2, ForwardIterator2 last2, 
                        BinaryPredicate comp) {
    auto d1 = tinystl::distance(first1, last1);
    auto d2 = tinystl::distance(first2, last2);
    if (d1 < d2) return last1;
    auto current1 = first1;
    auto current2 = first2;
    while (current2 != last2) {
        // 如果 *current1 == *current2，则继续比较下一个元素
        if (comp(*current1, *current2)) {
            ++current1;
            ++current2;
        }
        // 否则，从 first1 的下一个元素开始重新匹配
        else {
            // 如果剩余元素个数小于 first2 的长度，则不可能匹配成功
            if (d1 == d2) return last1;
            current1 = ++first1;  // 从 first1 的下一个元素开始重新匹配
            current2 = first2;    // current2 重新指向 first2
            --d1;
        }
    }
    return first1;
}

/*****************************************************************************************/
// search_n
// 在[first, last)中查找连续 n 个 value 所形成的子序列，返回一个迭代器指向该子序列的起始处
/*****************************************************************************************/

/// @brief 在[first, last)中查找连续 n 个 value 所形成的子序列，返回一个迭代器指向该子序列的起始处
template <class ForwardIterator, class Size, class T>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last, 
                         Size n, const T& value) {
    if (n <= 0) return first;
    first = tinystl::find(first, last, value);
    while (first != last) {
        auto m = n - 1;
        auto i = first;
        ++i;
        while (i != last && m != 0 && *i == value) {
            ++i;
            --m;
        }
        if (m == 0) return first;
        first = tinystl::find(i, last, value);
    }
    return last;
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class ForwardIterator, class Size, class T, class BinaryPredicate>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last, 
                         Size n, const T& value, BinaryPredicate comp) {
    if (n <= 0) return first;
    // 代替 find 的作用
    while (first != last) {
        if (comp(*first, value)) break;
        ++first;
    }
    while (first != last) {
        auto m = n - 1;
        auto i = first;
        ++i;
        while (i != last && m != 0 && comp(*i, value)) {
            ++i;
            --m;
        }
        if (m == 0) return first;
        // 代替 find 的作用
        while (i != last) {
            if (comp(*i, value)) break;
            ++i;
        }
        first = i;
    }
    return last;
}

/*****************************************************************************************/
// find_end
// 在[first1, last1)区间中查找[first2, last2)最后一次出现的地方，若不存在返回 last1
/*****************************************************************************************/

/// @brief 在[first1, last1)区间中查找[first2, last2)最后一次出现的地方，若不存在返回 last1
template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator1 find_end_dispatch(ForwardIterator1 first1, ForwardIterator1 last1, 
                                   ForwardIterator2 first2, ForwardIterator2 last2, 
                                   forward_iterator_tag, forward_iterator_tag) {
    if (first2 == last2) return last1;
    auto result = last1;
    // 思路为保留上一次的查找结果，因为最终情况一定是本次越界，那么上一次的查找结果
    // 就是 [first2, last2) 最后一次出现的地方
    while (true) {
        // 利用 search 查找 [first1, last1) 中 [first2, last2) 首次出现的位置
        // 起点不断更新为上一次查找结果的下一个位置
        auto new_result = tinystl::search(first1, last1, first2, last2);
        if (new_result == last1) return result;
        else {
            result = new_result;
            first1 = new_result;  // 本次出现的起点
            ++first1;             // 下一次查找的起点
        }
    }
}

/// @brief find_end_dispatch 针对双向迭代器的版本
template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator1 find_end_dispatch(BidirectionalIterator1 first1, BidirectionalIterator1 last1, 
                                         BidirectionalIterator2 first2, BidirectionalIterator2 last2, 
                                         bidirectional_iterator_tag, bidirectional_iterator_tag) {
    using reviter1 = reverse_iterator<BidirectionalIterator1>;
    using reviter2 = reverse_iterator<BidirectionalIterator2>;
    // 逆序查找
    auto rlast1 = reviter1(first1);
    auto rlast2 = reviter2(first2);
    auto rresult = tinystl::search(reviter1(last1), rlast1, reviter2(last2), rlast2);
    if (rresult == rlast1) return last1;
    else {
        // 转换为正向迭代器
        auto result = rresult.base();
        // 此时是在末尾处，需要回退第二个序列的长度
        tinystl::advance(result, -tinystl::distance(first2, last2));
        return result;
    }
}

template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1, 
                          ForwardIterator2 first2, ForwardIterator2 last2) {
    return tinystl::find_end_dispatch(first1, last1, first2, last2, 
        iterator_category(first1), iterator_category(first2));
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1, 
                          ForwardIterator2 first2, ForwardIterator2 last2, 
                          forward_iterator_tag,    forward_iterator_tag,
                          BinaryPredicate comp) {
    if (first2 == last2) return last1;
    auto result = last1;
    while (true) {
        auto new_result = tinystl::search(first1, last1, first2, last2, comp);
        if (new_result == last1) return result;
        else {
            result = new_result;
            first1 = new_result;
            ++first1;
        }
    }
}

/// @brief find_end_dispatch 针对双向迭代器的版本
template <class BidirectionalIterator1, class BidirectionalIterator2, class BinaryPredicate>
BidirectionalIterator1 find_end_dispatch(BidirectionalIterator1 first1, BidirectionalIterator1 last1, 
                                         BidirectionalIterator2 first2, BidirectionalIterator2 last2,  
                                         bidirectional_iterator_tag, bidirectional_iterator_tag, 
                                         BinaryPredicate comp) {
    using reviter1 = reverse_iterator<BidirectionalIterator1>;
    using reviter2 = reverse_iterator<BidirectionalIterator2>;
    auto rlast1 = reviter1(first1);
    auto rlast2 = reviter2(first2);
    auto rresult = tinystl::search(reviter1(last1), rlast1, reviter2(last2), rlast2, comp);
    if (rresult == rlast1) return last1;
    else {
        auto result = rresult.base();
        tinystl::advance(result, -tinystl::distance(first2, last2));
        return result;
    }
}

template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1, 
                          ForwardIterator2 first2, ForwardIterator2 last2, 
                          BinaryPredicate comp) {
    return tinystl::find_end_dispatch(first1, last1, first2, last2, 
        iterator_category(first1), iterator_category(first2), comp);
}



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
template <class ForwardIter1, class ForwardIter2, class BinaryPred>
bool is_permutation_aux(ForwardIter1 first1, ForwardIter1 last1,
                        ForwardIter2 first2, ForwardIter2 last2,
                        BinaryPred pred) {
    constexpr bool is_ra_it = tinystl::is_random_access_iterator<ForwardIter1>::value
        && tinystl::is_random_access_iterator<ForwardIter2>::value;
    if (is_ra_it) {
        auto len1 = last1 - first1;
        auto len2 = last2 - first2;
        if (len1 != len2) return false;
    }

    // 先找出相同的前缀段
    for (; first1 != last1 && first2 != last2; ++first1, (void) ++first2) {
        if (!pred(*first1, *first2)) break;
    }
    if (is_ra_it) {
        if (first1 == last1) return true;
    }
    else {
        auto len1 = tinystl::distance(first1, last1);
        auto len2 = tinystl::distance(first2, last2);
        if (len1 == 0 && len2 == 0) return true;
        if (len1 != len2) return false;
    }

    // 判断剩余部分相同元素的数目是否相等
    for (auto i = first1; i != last1; ++i) {
        // 判断 *i 是否已经出现过
        bool is_repeated = false;
        for (auto j = first1; j != i; ++j) {
            if (pred(*j, *i)) {
                is_repeated = true;
                break;
            }
        }
        // 如果 *i 没有出现过，则分别计算它在两个区间中的数目
        if (!is_repeated) {
            // 计算 *i 在 [first2, last2) 的数目
            auto c2 = 0;
            for (auto j = first2; j != last2; ++j) {
                if (pred(*i, *j)) ++c2;
            }
            if (c2 == 0) return false;

            // 计算 *i 在 [first1, last1) 的数目
            auto c1 = 1;
            auto j = i;
            for (++j; j != last1; ++j) {
                if (pred(*i, *j)) ++c1;
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