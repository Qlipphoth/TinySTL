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
#include "heap.h"
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
// find_first_of
// 在[first1, last1)中查找[first2, last2)中第一次出现的元素，返回指向第一次出现的元素的迭代器
/*****************************************************************************************/

/// @brief 在[first1, last1)中查找[first2, last2)中第一次出现的元素，返回指向第一次出现的元素的迭代器
template <class InputIterator, class ForwardIterator>
InputIterator find_first_of(InputIterator first1, InputIterator last1, 
                            ForwardIterator first2, ForwardIterator last2) {
    for (; first1 != last1; ++first1) {
        for (auto iter = first2; iter != last2; ++iter) {
            if (*first1 == *iter) return first1;
        }
    }
    return last1;
} 

/// @brief 重载版本使用函数对象 comp 代替比较操作       
template <class InputIterator, class ForwardIterator, class BinaryPredicate>
InputIterator find_first_of(InputIterator first1, InputIterator last1,
                            ForwardIterator first2, ForwardIterator last2,
                            BinaryPredicate comp) {
    for (; first1 != last1; ++first1) {
        for (auto iter = first2; iter != last2; ++iter) {
            if (comp(*first1, *iter)) return first1;
        }
    }
    return last1;
}

/*****************************************************************************************/
// for_each
// 使用一个函数对象 f 对[first, last)区间内的每个元素执行一个 operator() 操作，但不能改变元素内容
// f() 可返回一个值，但该值会被忽略
/*****************************************************************************************/

/// @brief 使用一个函数对象 f 对[first, last)区间内的每个元素执行一个 operator() 操作，但不能改变元素内容
template <class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f) {
    for (; first != last; ++first) {
        f(*first);
    }
    return f;
}

/*****************************************************************************************/
// adjacent_find
// 找出第一对匹配的相邻元素，缺省使用 operator== 比较，如果找到返回一个迭代器，指向这对元素的第一个元素
/*****************************************************************************************/

/// @brief 找出第一对匹配的相邻元素，缺省使用 operator== 比较，如果找到返回一个迭代器，指向这对元素的第一个元素
template <class ForwardIterator>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last) {
    if (first == last) return last;
    auto next = first;
    while (++next != last) {
        if (*first == *next) return first;
        first = next;
    }
    return last;
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class ForwardIterator, class BinaryPredicate>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last, BinaryPredicate comp) {
    if (first == last) return last;
    auto next = first;
    while (++next != last) {
        if (comp(*first, *next)) return first;
        first = next;
    }
    return last;
}

/*****************************************************************************************/
// lower_bound
// 在[first, last)中查找第一个不小于 value 的元素，并返回指向它的迭代器，若没有则返回 last
// 使用二分查找，所以要求[first, last)内的元素必须有序
// 相当于 python 中的 bisect.bisect_left，返回的位置即为可以被插入的第一个合适位置
/*****************************************************************************************/

/// @brief lower_bound 的 forward_iterator_tag 版本
template <class ForwardIterator, class T>
ForwardIterator lbound_dispatch(ForwardIterator first, ForwardIterator last, 
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
RandomAccessIterator lbound_dispatch(RandomAccessIterator first, RandomAccessIterator last, 
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
    return tinystl::lbound_dispatch(first, last, value, iterator_category(first));
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
// upper_bound
// 在[first, last)中查找第一个大于value 的元素，并返回指向它的迭代器，若没有则返回 last
// 使用二分查找，所以要求[first, last)内的元素必须有序
// 相当于 python 中的 bisect.bisect_right，返回的位置即为可以被插入的最后一个合适位置
/*****************************************************************************************/

/// @brief upper_bound 的 forward_iterator_tag 版本
template <class ForwardIterator, class T>
ForwardIterator ubound_dispatch(ForwardIterator first, ForwardIterator last, 
                                const T& value, forward_iterator_tag) {
    auto len = tinystl::distance(first, last);
    auto half = len;
    ForwardIterator middle;
    while (len > 0) {
        half = len >> 1;
        middle = first;
        tinystl::advance(middle, half);
        if (value < *middle) len = half;
        else {
            first = middle;
            ++first;
            len = len - half - 1;
        }
    }
    return first;
}

/// @brief upper_bound 的 random_access_iterator_tag 版本
template <class RandomAccessIterator, class T>
RandomAccessIterator ubound_dispatch(RandomAccessIterator first, RandomAccessIterator last, 
                                     const T& value, random_access_iterator_tag) {
    auto len = last - first;
    auto half = len;
    RandomAccessIterator middle;
    while (len > 0) {
        half = len >> 1;
        middle = first + half;
        if (value < *middle) len = half;
        else {
            first = middle + 1;
            len = len - half - 1;
        }
    }
    return first;
}

/// @brief 在[first, last)中查找第一个大于 value 的元素，根据迭代器类型调用不同版本的 ubound_dispatch
template <class ForwardIterator, class T>
ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value) {
    return tinystl::ubound_dispatch(first, last, value, iterator_category(first));
}

template <class ForwardIterator, class T, class Compare>
ForwardIterator ubound_dispatch(ForwardIterator first, ForwardIterator last, 
                                const T& value, forward_iterator_tag, Compare comp) {
    auto len = tinystl::distance(first, last);
    auto half = len;
    ForwardIterator middle;
    while (len > 0) {
        half = len >> 1;
        middle = first;
        tinystl::advance(middle, half);
        if (comp(value, *middle)) len = half;
        else {
            first = middle;
            ++first;
            len = len - half - 1;
        }
    }
    return first;
}

template <class RandomAccessIterator, class T, class Compare>
RandomAccessIterator ubound_dispatch(RandomAccessIterator first, RandomAccessIterator last, 
                                     const T& value, random_access_iterator_tag, Compare comp) {
    auto len = last - first;
    auto half = len;
    RandomAccessIterator middle;
    while (len > 0) {
        half = len >> 1;
        middle = first + half;
        if (comp(value, *middle)) len = half;
        else {
            first = middle + 1;
            len = len - half - 1;
        }
    }
    return first;
}

/// @brief 在[first, last)中查找第一个大于 value 的元素，使用 comp 比较元素大小
template <class ForwardIterator, class T, class Compare>
ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
    return tinystl::ubound_dispatch(first, last, value, iterator_category(first), comp);
}

/*****************************************************************************************/
// binary_search
// 二分查找，如果在[first, last)内有等同于 value 的元素，返回 true，否则返回 false
/*****************************************************************************************/

/// @brief 二分查找，如果在[first, last)内有等同于 value 的元素，返回 true，否则返回 false
template <class ForwardIterator, class T>
bool binary_search(ForwardIterator first, ForwardIterator last, const T& value) {
    auto i = tinystl::lower_bound(first, last, value);
    // 最终 i 指向的位置如果不为 last，那么其中的值必然是 >= value 的，因此去除掉 > value 的情况
    // 即可判断是否存在等于 value 的元素
    return i != last && !(value < *i);
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class ForwardIterator, class T, class Compare>
bool binary_search(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
    auto i = tinystl::lower_bound(first, last, value, comp);
    return i != last && !comp(value, *i);
}

/*****************************************************************************************/
// equal_range
// 查找[first,last)区间中与 value 相等的元素所形成的区间，返回一对迭代器指向区间首尾
// 第一个迭代器指向第一个不小于 value 的元素，第二个迭代器指向第一个大于 value 的元素
// 我觉得直接调用 lbound 和 ubound 就可以了，为什么还要先找到中间元素呢
// TODO: 讲解二分与这里的做法
/*****************************************************************************************/

/// @brief 查找[first,last)区间中与 value 相等的元素所形成的区间，返回一对迭代器指向区间首尾
template <class ForwardIterator, class T>
tinystl::pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T& value) {
    return tinystl::pair<ForwardIterator, ForwardIterator>(
        tinystl::lower_bound(first, last, value),
        tinystl::upper_bound(first, last, value));
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class ForwardIterator, class T, class Compare>
tinystl::pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
    return tinystl::pair<ForwardIterator, ForwardIterator>(
        tinystl::lower_bound(first, last, value, comp),
        tinystl::upper_bound(first, last, value, comp));
}

/*****************************************************************************************/
// generate
// 将函数对象 gen 的运算结果对[first, last)内的每个元素赋值
/*****************************************************************************************/

/// @brief 将函数对象 gen 的运算结果对[first, last)内的每个元素赋值
template <class ForwardIterator, class Generator>
void generate(ForwardIterator first, ForwardIterator last, Generator gen) {
    for (; first != last; ++first) {
        *first = gen();
    }
}

/*****************************************************************************************/
// generate_n
// 用函数对象 gen 连续对 n 个元素赋值
/*****************************************************************************************/

/// @brief 用函数对象 gen 连续对 n 个元素赋值
template <class ForwardIterator, class Size, class Generator>
void generate_n(ForwardIterator first, Size n, Generator gen) {
    for (; n > 0; --n, ++first) {
        *first = gen();
    }
}

/*****************************************************************************************/
// includes
// 判断序列一是否包含序列二，即序列二的元素是否全部出现在序列一中
// 两个序列必须为有序序列
/*****************************************************************************************/

/// @brief 判断序列一是否包含序列二，即序列二的元素是否全部出现在序列一中
template <class InputIterator1, class InputIterator2>
bool includes(InputIterator1 first1, InputIterator1 last1, 
              InputIterator2 first2, InputIterator2 last2) {
    while (first1 != last1 && first2 != last2) {
        if (*first2 < *first1) return false;
        else if (*first1 < *first2) ++first1;
        else ++first1, ++first2;
    }
    return first2 == last2;
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class InputIterator1, class InputIterator2, class Compare>
bool includes(InputIterator1 first1, InputIterator1 last1, 
              InputIterator2 first2, InputIterator2 last2, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first2, *first1)) return false;
        else if (comp(*first1, *first2)) ++first1;
        else ++first1, ++first2;
    }
    return first2 == last2;
}

/*****************************************************************************************/
// is_heap
// 检查[first, last)内的元素是否为一个堆，如果是，则返回 true
// 默认为大根堆，如果想要检查小根堆，需要传入一个比较函数
/*****************************************************************************************/

/// @brief 检查[first, last)内的元素是否为一个堆，如果是，则返回 true
template <class RandomAccessIterator>
bool is_heap(RandomAccessIterator first, RandomAccessIterator last) {
    auto n = last - first;
    auto parent = 0;
    for (auto child = 1; child < n; ++child) {
        if (first[parent] < first[child]) return false;
        if ((child & 1) == 0) ++parent;
    }
    return true;
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class RandomAccessIterator, class Compare>
bool is_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    auto n = last - first;
    auto parent = 0;
    for (auto child = 1; child < n; ++child) {
        if (comp(first[parent], first[child])) return false;
        if ((child & 1) == 0) ++parent;
    }
    return true;
}

/*****************************************************************************************/
// is_sorted
// 检查[first, last)内的元素是否升序，如果是升序，则返回 true
/*****************************************************************************************/

/// @brief 检查[first, last)内的元素是否升序，如果是升序，则返回 true
template <class ForwardIterator>
bool is_sorted(ForwardIterator first, ForwardIterator last) {
    if (first == last) return true;
    auto next = first;
    while (++next != last) {
        if (*next < *first) return false;
        first = next;
    }
    return true;
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class ForwardIterator, class Compare>
bool is_sorted(ForwardIterator first, ForwardIterator last, Compare comp) {
    if (first == last) return true;
    auto next = first;
    while (++next != last) {
        if (comp(*next, *first)) return false;
        first = next;
    }
    return true;
}

/*****************************************************************************************/
// median
// 找出三个值的中间值
/*****************************************************************************************/

/// @brief 找出三个值的中间值，主要用于快排中的 pivot 选择
template <class T>
const T& median(const T& left, const T& mid, const T& right) {
    if (left < mid)
        if (mid < right) return mid;          // left < mid < right
        else if (left < right) return right;  // left < right <= mid
        else return left;                     // right <= left < mid
        
    else if (left < right) return left;     // mid <= left < right
    else if (mid < right) return right;     // mid < right <= left
    else return mid;                        // right <= mid <= left
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class T, class Compared>
const T& median(const T& left, const T& mid, const T& right, Compared comp) {
    if (comp(left, mid))
        if (comp(mid, right)) return mid;
        else if (comp(left, right)) return right;
        else return left;
    else if (comp(left, right)) return left;
    else if (comp(mid, right)) return right;
    else return mid;
}

/*****************************************************************************************/
// max_element
// 返回一个迭代器，指向序列中最大的元素
/*****************************************************************************************/

/// @brief 返回一个迭代器，指向序列中最大的元素
template <class ForwardIterator>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last) {
    if (first == last) return first;
    auto result = first;
    while (++first != last) {
        if (*result < *first) result = first;
    }
    return result;
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class ForwardIterator, class Compared>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last, Compared comp) {
    if (first == last) return first;
    auto result = first;
    while (++first != last) {
        if (comp(*result, *first)) result = first;
    }
    return result;
}

/*****************************************************************************************/
// min_element
// 返回一个迭代器，指向序列中最小的元素
/*****************************************************************************************/

/// @brief 返回一个迭代器，指向序列中最小的元素
template <class ForwardIterator>
ForwardIterator min_elememt(ForwardIterator first, ForwardIterator last) {
    if (first == last) return first;
    auto result = first;
    while (++first != last) {
        if (*first < *result) result = first;
    }
    return result;
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class ForwardIterator, class Compared>
ForwardIterator min_elememt(ForwardIterator first, ForwardIterator last, Compared comp) {
    if (first == last) return first;
    auto result = first;
    while (++first != last) {
        if (comp(*first, *result)) result = first;
    }
    return result;
}

/*****************************************************************************************/
// swap_ranges
// 将 [first1, last1) 从 first2 开始，交换相同个数元素
// 交换的区间长度必须相同，两个序列不能互相重叠，返回一个迭代器指向序列二最后一个被交换元素的下一位置
/*****************************************************************************************/

/// @brief 将 [first1, last1) 从 first2 开始，交换相同个数元素
template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator2 swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1, 
                             ForwardIterator2 first2) {
    for (; first1 != last1; ++first1, ++first2) {
        tinystl::iter_swap(first1, first2);
    }
    return first2;
}

/*****************************************************************************************/
// transform
// 第一个版本以函数对象 unary_op 作用于[first, last)中的每个元素并将结果保存至 result 中
// 第二个版本以函数对象 binary_op 作用于两个序列[first1, last1)、[first2, last2)的相同位置
/*****************************************************************************************/

/// @brief 以函数对象 unary_op 作用于[first, last)中的每个元素并将结果保存至 result 中
template <class InputIterator, class OutputIterator, class UnaryOperation>
OutputIterator transform(InputIterator first, InputIterator last, 
                         OutputIterator result, UnaryOperation unary_op) {
    for (; first != last; ++first, ++result) {
        *result = unary_op(*first);
    }
    return result;
}

/// @brief 以函数对象 binary_op 作用于两个序列[first1, last1)、[first2, last2)的相同位置并将结果保存至 result 中
template <class InputIterator1, class InputIterator2, class OutputIterator, class BinaryOperation>
OutputIterator transform(InputIterator1 first1, InputIterator1 last1, 
                         InputIterator2 first2, OutputIterator result, BinaryOperation binary_op) {
    for (; first1 != last1; ++first1, ++first2, ++result) {
        *result = binary_op(*first1, *first2);
    }
    return result;
}

/*****************************************************************************************/
// remove_copy
// 移除区间内与指定 value 相等的元素，并将结果复制到以 result 标示起始位置的容器上
/*****************************************************************************************/

template <class InputIterator, class OutputIterator, class T>
OutputIterator remove_copy(InputIterator first, InputIterator last, 
                           OutputIterator result, const T& value) {
    for (; first != last; ++first) {
        if (*first != value) *result++ = *first;
    }
    return result;
}

/*****************************************************************************************/
// remove
// 移除所有与指定 value 相等的元素
// 实际的操作仅为将不与 value 相等的元素移动到前面，返回指向新的逻辑结尾的迭代器
// 并不从容器中删除这些元素，所以 remove 和 remove_if 不适用于 array （因为 array 的大小固定）
/*****************************************************************************************/

/// @brief 移除所有与指定 value 相等的元素
template <class ForwardIterator, class T>
ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T& value) {
    first = tinystl::find(first, last, value);
    auto next = first;
    return first == last ? first : tinystl::remove_copy(++next, last, first, value);
}

/*****************************************************************************************/
// remove_copy_if
// 移除区间内所有令一元操作 unary_pred 为 true 的元素，并将结果复制到以 result 为起始位置的容器上
/*****************************************************************************************/

/// @brief 移除区间内所有令一元操作 unary_pred 为 true 的元素，并将结果复制到以 result 为起始位置的容器上 
template <class InputIterator, class OutputIterator, class UnaryPredicate>
OutputIterator remove_copy_if(InputIterator first, InputIterator last, 
                              OutputIterator result, UnaryPredicate unary_pred) {
    for (; first != last; ++first) {
        if (!unary_pred(*first)) *result++ = *first;
    }
    return result;
}

/*****************************************************************************************/
// remove_if
// 移除区间内所有令一元操作 unary_pred 为 true 的元素
/*****************************************************************************************/

/// @brief 移除区间内所有令一元操作 unary_pred 为 true 的元素
template <class ForwardIterator, class UnaryPredicate>
ForwardIterator remove_if(ForwardIterator first, ForwardIterator last, UnaryPredicate unary_pred) {
    first = tinystl::find_if(first, last, unary_pred);
    auto next = first;
    return first == last ? first : tinystl::remove_copy_if(++next, last, first, unary_pred);
}

/*****************************************************************************************/
// replace
// 将区间内所有的 old_value 都以 new_value 替代
/*****************************************************************************************/

/// @brief 将区间内所有的 old_value 都以 new_value 替代
template <class ForwardIterator, class T>
void replace(ForwardIterator first, ForwardIterator last, const T& old_value, const T& new_value) {
    for (; first != last; ++first) {
        if (*first == old_value) *first = new_value;
    }
}

/*****************************************************************************************/
// replace_copy
// 行为与 replace 类似，不同的是将结果复制到 result 所指的容器中，原序列没有改变
/*****************************************************************************************/

/// @brief 行为与 replace 类似，不同的是将结果复制到 result 所指的容器中，原序列没有改变
template <class InputIterator, class OutputIterator, class T>
OutputIterator replace_copy(InputIterator first, InputIterator last, 
                            OutputIterator result, const T& old_value, const T& new_value) {
    for (; first != last; ++first, ++result) {
        *result = *first == old_value ? new_value : *first;
    }
    return result;
}

/*****************************************************************************************/
// replace_copy_if
// 行为与 replace_if 类似，不同的是将结果复制到 result 所指的容器中，原序列没有改变
/*****************************************************************************************/

/// @brief 行为与 replace_if 类似，不同的是将结果复制到 result 所指的容器中，原序列没有改变
template <class InputIterator, class OutputIterator, class UnaryPredicate, class T>
OutputIterator replace_copy_if(InputIterator first, InputIterator last, 
                               OutputIterator result, UnaryPredicate unary_pred, const T& new_value) {
    for (; first != last; ++first, ++result) {
        *result = unary_pred(*first) ? new_value : *first;
    }
    return result;
}

/*****************************************************************************************/
// replace_if
// 将区间内所有令一元操作 unary_pred 为 true 的元素都用 new_value 替代
/*****************************************************************************************/

/// @brief 将区间内所有令一元操作 unary_pred 为 true 的元素都用 new_value 替代
template <class ForwardIterator, class UnaryPredicate, class T>
void replace_if(ForwardIterator first, ForwardIterator last, 
                UnaryPredicate unary_pred, const T& new_value) {
    for (; first != last; ++first) {
        if (unary_pred(*first)) *first = new_value;
    }
}


/*****************************************************************************************/
// reverse
// 将[first, last)区间内的元素反转
/*****************************************************************************************/

/// @brief reverse 的 bidirectional_iterator_tag 版本
template <class BidirectionalIterator>
void reverse_dispatch(BidirectionalIterator first, BidirectionalIterator last, 
    bidirectional_iterator_tag) {
    while (true) {
        if (first == last || first == --last) return;
        tinystl::iter_swap(first++, last);
    }
}

/// @brief reverse 的 random_access_iterator_tag 版本 
template <class RandomAccessIterator>
void reverse_dispatch(RandomAccessIterator first, RandomAccessIterator last, 
    random_access_iterator_tag) {
    while (first < last) {
        tinystl::iter_swap(first++, --last);
    }
}

/// @brief 将[first, last)区间内的元素反转，根据迭代器类型调用不同版本的 reverse_dispatch
template <class BidirectionalIterator>
void reverse(BidirectionalIterator first, BidirectionalIterator last) {
    tinystl::reverse_dispatch(first, last, iterator_category(first));
}

/*****************************************************************************************/
// reverse_copy
// 行为与 reverse 类似，不同的是将结果复制到 result 所指容器中
/*****************************************************************************************/

/// @brief 行为与 reverse 类似，不同的是将结果复制到 result 所指容器中
template <class BidirectionalIterator, class OutputIterator>
OutputIterator reverse_copy(BidirectionalIterator first, BidirectionalIterator last, 
                            OutputIterator result) {
    while (first != last) {
        --last;
        *result++ = *last;
    }
    return result;
}

/*****************************************************************************************/
// random_shuffle
// 将[first, last)内的元素次序随机重排
// 重载版本使用一个产生随机数的函数对象 rand
/*****************************************************************************************/

/// @brief 将[first, last)内的元素次序随机重排
template <class RandomAccessIterator>
void random_shuffle(RandomAccessIterator first, RandomAccessIterator last) {
    if (first == last) return;
    srand((unsigned)time(0));  // 产生随机数种子，以当前时间为种子，使每次运行结果随机
    for (auto i = first + 1; i != last; ++i) {
        tinystl::iter_swap(i, first + rand() % ((i - first) + 1));
    }
}

/// @brief 重载版本使用一个产生随机数的函数对象 rand
template <class RandomAccessIterator, class RandomNumberGenerator>
void random_shuffle(RandomAccessIterator first, RandomAccessIterator last, 
                    RandomNumberGenerator rand) {
    if (first == last) return;
    auto len = tinystl::distance(first, last);
    for (auto i = first + 1; i != last; ++i) {
        tinystl::iter_swap(i, first + (rand(i - first + 1) % len));
    }
}

/*****************************************************************************************/
// rotate
// 将[first, middle)内的元素和 [middle, last)内的元素互换，可以交换两个长度不同的区间
// 返回交换后 middle 的位置
// STL 原始的实现太复杂了，这里使用 LeetCode 189 题的解法，翻转三次数组
// https://leetcode.cn/problems/rotate-array/
// 也不需要使用额外的空间，O(1) 空间复杂度，O(n) 时间复杂度
// TODO:讲解
/*****************************************************************************************/

// template <class ForwardIterator>
// ForwardIterator rotate_dispatch(ForwardIterator first, ForwardIterator middle, 
//                                 ForwardIterator last, forward_iterator_tag) {
//     auto new_middle = first;
//     tinystl::advance(new_middle, last - middle);
//     tinystl::reverse(first, last);
//     tinystl::reverse(first, new_middle);
//     tinystl::reverse(new_middle, last);
//     return new_middle;
// }

// template <class BidirectionalIterator>
// BidirectionalIterator rotate_dispatch(BidirectionalIterator first, BidirectionalIterator middle, 
//                                       BidirectionalIterator last, bidirectional_iterator_tag) {
//     auto new_middle = first;
//     tinystl::advance(new_middle, last - middle);
//     tinystl::reverse(first, last);
//     tinystl::reverse(first, new_middle);
//     tinystl::reverse(new_middle, last);
//     return new_middle;
// }


// 使用三次翻转的方法后 rotate 就完全不需要根据迭代器来设计不同的实现，只需使用 reverse 的不同实现即可

/// @brief 将[first, middle)内的元素和 [middle, last)内的元素互换，可以交换两个长度不同的区间
template <class ForwardIterator>
ForwardIterator rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last) {
    auto new_middle = first;
    tinystl::advance(new_middle, last - middle);
    tinystl::reverse(first, last);
    tinystl::reverse(first, new_middle);
    tinystl::reverse(new_middle, last);
    return new_middle;
}

/*****************************************************************************************/
// rotate_copy
// 行为与 rotate 类似，不同的是将结果复制到 result 所指的容器中
/*****************************************************************************************/

template <class ForwardIterator, class OutputIterator>
ForwardIterator rotate_copy(ForwardIterator first, ForwardIterator middle, 
                            ForwardIterator last, OutputIterator result) {
    // 先拷贝 [middle, last) 的元素至 result
    // 再拷贝 [first, middle) 的元素
    // 相当于做了一次 rotate
    return tinystl::copy(first, middle, tinystl::copy(middle, last, result));
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

/*****************************************************************************************/
// next_permutation
// 取得[first, last)所标示序列的下一个排列组合，如果没有下一个排序组合，返回 false，否则返回 true
// 算法流程为：
// 首先，从最尾端开始往前寻找两个相邻元素，令第一元素为 *i，第二元素为 *ii，且满足 *i < *ii。
// 找到这样一组相邻元素后﹐再从最尾端开始往前检验，找出第一个大于 *i 的元素，令为 *j，将 i，j 元素对调，
// 再将 ii之后的所有元素颠倒排列。此即所求之 “下一个” 排列组合。
// TODO:讲解
/*****************************************************************************************/

/// @brief 取得[first, last)所标示序列的下一个排列组合，如果没有下一个排序组合，返回 false，否则返回 true
template <class BidirectionalIterator>
bool next_permutation(BidirectionalIterator first, BidirectionalIterator last) {
    auto i = last;
    if (first == last || first == --i) return false;
    for (;;) {
        auto ii = i;
        if (*--i < *ii) {
            auto j = last;
            while (!(*i < *--j)) {}
            tinystl::iter_swap(i, j);
            tinystl::reverse(ii, last);
            return true;
        }
        if (i == first) {
            tinystl::reverse(first, last);
            return false;
        }
    }
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class BidirectionalIterator, class Compare>
bool next_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare comp) {
    auto i = last;
    if (first == last || first == --i) return false;
    for (;;) {
        auto ii = i;
        if (comp(*--i, *ii)) {
            auto j = last;
            while (!comp(*i, *--j)) {}
            tinystl::iter_swap(i, j);
            tinystl::reverse(ii, last);
            return true;
        }
        if (i == first) {
            tinystl::reverse(first, last);
            return false;
        }
    }
}

/*****************************************************************************************/
// prev_permutation
// 取得[first, last)所标示序列的上一个排列组合，如果没有上一个排序组合，返回 false，否则返回 true
/*****************************************************************************************/

/// @brief 取得[first, last)所标示序列的上一个排列组合，如果没有上一个排序组合，返回 false，否则返回 true
template <class BidirectionalIterator>
bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last) {
    auto i = last;
    if (first == last || first == --i) return false;
    for (;;) {
        auto ii = i;
        // 找到第一对相邻元素，满足 *i > *ii
        if (*ii < *--i) {
            auto j = last;
            while (!(*--j < *i)) {}
            tinystl::iter_swap(i, j);    // 交换 i 和 j
            tinystl::reverse(ii, last);  // 将 ii 之后的元素颠倒排列
            return true;
        }
        if (i == first) {
            tinystl::reverse(first, last);
            return false;
        }
    }
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class BidirectionalIterator, class Compare>
bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare comp) {
    auto i = last;
    if (first == last || first == --i) return false;
    for (;;) {
        auto ii = i;
        if (comp(*ii, *--i)) {
            auto j = last;
            while (!comp(*--j, *i)) {}
            tinystl::iter_swap(i, j);
            tinystl::reverse(ii, last);
            return true;
        }
        if (i == first) {
            tinystl::reverse(first, last);
            return false;
        }
    }
}

/*****************************************************************************************/
// merge
// 要求两个序列有序
// 将两个经过排序的集合 S1 和 S2 合并起来置于另一段空间，返回一个迭代器指向最后一个元素的下一位置
/*****************************************************************************************/

/// @brief 将两个经过排序的集合 S1 和 S2 合并起来置于另一段空间，返回一个迭代器指向最后一个元素的下一位置
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1, 
                     InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        if (*first2 < *first1) *result++ = *first2++;
        else *result++ = *first1++;
    }
    return tinystl::copy(first2, last2, tinystl::copy(first1, last1, result));
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1, 
                     InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first2, *first1)) *result++ = *first2++;
        else *result++ = *first1++;
    }
    return tinystl::copy(first2, last2, tinystl::copy(first1, last1, result));
}

/*****************************************************************************************/
// inplace_merge
// 把连接在一起的两个有序序列结合成单一序列并保持有序
// TODO: 讲解
/*****************************************************************************************/

/// @brief 在没有缓冲区的情况下合并
template <class BidirectionalIterator, class Distance>
void merge_without_buffer(BidirectionalIterator first, BidirectionalIterator middle, 
                          BidirectionalIterator last, Distance len1, Distance len2) {
    if (len1 == 0 || len2 == 0) return;  // 有一个序列为空，不需要合并
    if (len1 + len2 == 2) {  // 两个序列都只有一个元素，直接比较大小
        if (*middle < *first) tinystl::iter_swap(first, middle);
        return;
    }
    auto first_cut = first;
    auto second_cut = middle;
    Distance len11 = 0;
    Distance len22 = 0;
    // 序列一比较长，找到序列一的中点
    if (len1 > len2) {
        len11 = len1 >> 1;
        tinystl::advance(first_cut, len11);
        second_cut = tinystl::lower_bound(middle, last, *first_cut);
        len22 = tinystl::distance(middle, second_cut);
    }
    // 序列二比较长，找到序列二的中点
    else {  
        len22 = len2 >> 1;
        tinystl::advance(second_cut, len22);
        first_cut = tinystl::upper_bound(first, middle, *second_cut);
        len11 = tinystl::distance(first, first_cut);
    }
    // 将较小的部分旋转至前半段，较大的部分旋转至后半段
    // 这样就将问题转化为了将两个长度更小的序列合并
    auto new_middle = tinystl::rotate(first_cut, middle, second_cut);
    // 递归解决子问题
    tinystl::merge_without_buffer(first, first_cut, new_middle, len11, len22);
    tinystl::merge_without_buffer(new_middle, second_cut, last, len1 - len11, len2 - len22);
}

/// @brief 将两个有序序列合并到以 result 为结尾的区间上
template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator1 merge_backward(BidirectionalIterator1 first1, BidirectionalIterator1 last1, 
                                      BidirectionalIterator2 first2, BidirectionalIterator2 last2, 
                                      BidirectionalIterator1 result) {
    if (first1 == last1) return tinystl::copy_backward(first2, last2, result);
    if (first2 == last2) return tinystl::copy_backward(first1, last1, result);
    --last1;
    --last2;
    while (true) {
        if (*last2 < *last1) {
            *--result = *last1;
            if (first1 == last1) return tinystl::copy_backward(first2, ++last2, result);
            --last1;
        }
        else {
            *--result = *last2;
            if (first2 == last2) return tinystl::copy_backward(first1, ++last1, result);
            --last2;
        }
    }
}

/// @brief 对 [first, middle) 和 [middle, last) 两个区间进行旋转，根据 buffer 的情况选择效率最高的实现 
template <class BidirectionalIterator1, class BidirectionalIterator2, class Distance>
BidirectionalIterator1 rotate_adaptive(BidirectionalIterator1 first, BidirectionalIterator1 middle, 
                                       BidirectionalIterator1 last, Distance len1, Distance len2, 
                                       BidirectionalIterator2 buffer, Distance buffer_size) {
    BidirectionalIterator2 buffer_end;
    // 如果缓冲区能够装下序列二，就将序列二拷贝至缓冲区，
    // 再将序列一拷贝至区间后方，将缓冲区（序列二）拷贝至区间前方，完成 rotate
    // 思路类似于 rotate_copy
    if (len1 > len2 && len2 <= buffer_size) {
        buffer_end = tinystl::copy(middle, last, buffer);
        tinystl::copy_backward(first, middle, last);
        return tinystl::copy(buffer, buffer_end, first);
    }
    // 如果缓冲区能够装下序列一，就将序列一拷贝至缓冲区
    else if (len1 <= buffer_size) {
        buffer_end = tinystl::copy(first, middle, buffer);
        tinystl::copy(middle, last, first);
        return tinystl::copy_backward(buffer, buffer_end, last);
    }
    // 缓冲区装不下任何一个序列，使用原地 rotate，即 tinystl::rotate
    else {
        return tinystl::rotate(first, middle, last);
    }
}

/// @brief 把连接在一起的两个有序序列结合成单一序列并保持有序，根据 buffer 的情况选择效率最高的实现
template <class BidirectionalIterator, class Distance, class Pointer>
void merge_adaptive(BidirectionalIterator first, BidirectionalIterator middle, 
                    BidirectionalIterator last, Distance len1, Distance len2, 
                    Pointer buffer, Distance buffer_size) {
    // 缓冲区足够放进序列一
    if (len1 <= len2 && len1 <= buffer_size) {
        Pointer buffer_end = tinystl::copy(first, middle, buffer);
        tinystl::merge(buffer, buffer_end, middle, last, first);
    }
    // 缓冲区足够放进序列二
    else if (len2 <= buffer_size) {
        Pointer buffer_end = tinystl::copy(middle, last, buffer);
        tinystl::merge_backward(first, middle, buffer, buffer_end, last);
    }
    // 缓冲区放不下任何一个序列，分割递归处理，类似于 merge_without_buffer，但是在 rotate 时根据
    // buffer 与处理区间的关系选用效率最高的 rotate 方法
    else {
        auto first_cut = first;
        auto second_cut = middle;
        Distance len11 = 0;
        Distance len22 = 0;
        if (len1 > len2) {
            len11 = len1 >> 1;
            tinystl::advance(first_cut, len11);
            second_cut = tinystl::lower_bound(middle, last, *first_cut);
            len22 = tinystl::distance(middle, second_cut);
        }
        else {
            len22 = len2 >> 1;
            tinystl::advance(second_cut, len22);
            first_cut = tinystl::upper_bound(first, middle, *second_cut);
            len11 = tinystl::distance(first, first_cut);
        }
        auto new_middle = tinystl::rotate_adaptive(first_cut, middle, second_cut, len1 - len11, len22, 
                                                   buffer, buffer_size);
        tinystl::merge_adaptive(first, first_cut, new_middle, len11, len22, buffer, buffer_size);
        tinystl::merge_adaptive(new_middle, second_cut, last, len1 - len11, len2 - len22, buffer, buffer_size);
    }
    
}

template <class BidirectionalIterator, class T>
void inplace_merge_aux(BidirectionalIterator first, BidirectionalIterator middle, 
                       BidirectionalIterator last, T*) {
    auto len1 = tinystl::distance(first, middle);
    auto len2 = tinystl::distance(middle, last);
    temporary_buffer<BidirectionalIterator, T> buf(first, last);  // 申请缓冲区
    // 如果没有申请到缓冲区，就使用 merge_without_buffer
    if (buf.begin() == nullptr) {
        tinystl::merge_without_buffer(first, middle, last, len1, len2);
    }
    // 如果申请到了缓冲区，就使用 merge_adaptive
    else {
        tinystl::merge_adaptive(first, middle, last, len1, len2, buf.begin(), buf.size());
    }
}

/// @brief 把连接在一起的两个有序序列结合成单一序列并保持有序
template <class BidirectionalIterator>
void inplace_merge(BidirectionalIterator first, BidirectionalIterator middle, 
                   BidirectionalIterator last) {
    if (first == middle || middle == last) return;
    tinystl::inplace_merge_aux(first, middle, last, value_type(first));
}

// ============================ 重载版本使用函数对象 comp ================================= //

/// @brief 在没有缓冲区的情况下合并
template <class BidirectionalIterator, class Distance, class Compare>
void merge_without_buffer(BidirectionalIterator first, BidirectionalIterator middle, 
                          BidirectionalIterator last, Distance len1, Distance len2,
                          Compare comp) {
    if (len1 == 0 || len2 == 0) return;  // 有一个序列为空，不需要合并
    if (len1 + len2 == 2) {  // 两个序列都只有一个元素，直接比较大小
        if (comp(*middle, *first)) tinystl::iter_swap(first, middle);
        return;
    }
    auto first_cut = first;
    auto second_cut = middle;
    Distance len11 = 0;
    Distance len22 = 0;
    // 序列一比较长，找到序列一的中点
    if (len1 > len2) {
        len11 = len1 >> 1;
        tinystl::advance(first_cut, len11);
        second_cut = tinystl::lower_bound(middle, last, *first_cut, comp);
        len22 = tinystl::distance(middle, second_cut);
    }
    // 序列二比较长，找到序列二的中点
    else {  
        len22 = len2 >> 1;
        tinystl::advance(second_cut, len22);
        first_cut = tinystl::upper_bound(first, middle, *second_cut, comp);
        len11 = tinystl::distance(first, first_cut);
    }
    // 将较小的部分旋转至前半段，较大的部分旋转至后半段
    // 这样就将问题转化为了将两个长度更小的序列合并
    auto new_middle = tinystl::rotate(first_cut, middle, second_cut);
    // 递归解决子问题
    tinystl::merge_without_buffer(first, first_cut, new_middle, len11, len22, comp);
    tinystl::merge_without_buffer(new_middle, second_cut, last, len1 - len11, len2 - len22, comp);
}

/// @brief 将两个有序序列合并到以 result 为结尾的区间上
template <class BidirectionalIterator1, class BidirectionalIterator2, class Compare>
BidirectionalIterator1 merge_backward(BidirectionalIterator1 first1, BidirectionalIterator1 last1, 
                                      BidirectionalIterator2 first2, BidirectionalIterator2 last2, 
                                      BidirectionalIterator1 result, Compare comp) {
    if (first1 == last1) return tinystl::copy_backward(first2, last2, result);
    if (first2 == last2) return tinystl::copy_backward(first1, last1, result);
    --last1;
    --last2;
    while (true) {
        if (comp(*last2, *last1)) {
            *--result = *last1;
            if (first1 == last1) return tinystl::copy_backward(first2, ++last2, result);
            --last1;
        }
        else {
            *--result = *last2;
            if (first2 == last2) return tinystl::copy_backward(first1, ++last1, result);
            --last2;
        }
    }
}

template <class BidirectionalIterator, class Distance, class Pointer, class Compare>
void merge_adaptive(BidirectionalIterator first, BidirectionalIterator middle,
                    BidirectionalIterator last, Distance len1, Distance len2, 
                    Pointer buffer, Distance buffer_size, Compare comp) {
    // 缓冲区足够放进序列一
    if (len1 <= len2 && len1 <= buffer_size) {
        Pointer buffer_end = tinystl::copy(first, middle, buffer);
        tinystl::merge(buffer, buffer_end, middle, last, first, comp);
    }
    // 缓冲区足够放进序列二
    else if (len2 <= buffer_size) {
        Pointer buffer_end = tinystl::copy(middle, last, buffer);
        tinystl::merge_backward(first, middle, buffer, buffer_end, last, comp);
    }
    // 缓冲区放不下任何一个序列，分割递归处理，类似于 merge_without_buffer，但是在 rotate 时根据
    // buffer 与处理区间的关系选用效率最高的 rotate 方法
    else {
        auto first_cut = first;
        auto second_cut = middle;
        Distance len11 = 0;
        Distance len22 = 0;
        if (len1 > len2) {
            len11 = len1 >> 1;
            tinystl::advance(first_cut, len11);
            second_cut = tinystl::lower_bound(middle, last, *first_cut, comp);
            len22 = tinystl::distance(middle, second_cut);
        }
        else {
            len22 = len2 >> 1;
            tinystl::advance(second_cut, len22);
            first_cut = tinystl::upper_bound(first, middle, *second_cut, comp);
            len11 = tinystl::distance(first, first_cut);
        }
        auto new_middle = tinystl::rotate_adaptive(first_cut, middle, second_cut, len1 - len11, len22, 
                                                   buffer, buffer_size);
        tinystl::merge_adaptive(first, first_cut, new_middle, len11, len22, buffer, buffer_size, comp);
        tinystl::merge_adaptive(new_middle, second_cut, last, len1 - len11, len2 - len22, buffer, buffer_size, comp);
    }
}

template <class BidirectionalIterator, class T, class Compare>
void inplace_merge_aux(BidirectionalIterator first, BidirectionalIterator middle, 
                       BidirectionalIterator last, T*, Compare comp) {
    auto len1 = tinystl::distance(first, middle);
    auto len2 = tinystl::distance(middle, last);
    temporary_buffer<BidirectionalIterator, T> buf(first, last);  // 申请缓冲区
    // 如果没有申请到缓冲区，就使用 merge_without_buffer
    if (buf.begin() == nullptr) {
        tinystl::merge_without_buffer(first, middle, last, len1, len2, comp);
    }
    // 如果申请到了缓冲区，就使用 merge_adaptive
    else {
        tinystl::merge_adaptive(first, middle, last, len1, len2, buf.begin(), buf.size(), comp);
    }
}

/// @brief 把连接在一起的两个有序序列结合成单一序列并保持有序
template <class BidirectionalIterator, class Compare>
void inplace_merge(BidirectionalIterator first, BidirectionalIterator middle, 
                   BidirectionalIterator last, Compare comp) {
    if (first == middle || middle == last) return;
    tinystl::inplace_merge_aux(first, middle, last, value_type(first), comp);
}

/*****************************************************************************************/
// partial_sort
// 对整个序列做部分排序，保证 [first, middle) 内的元素有序且小于 [middle, last) 内的元素
// 思路：
// 1. 在区间 [first, midlle) 上构造大根堆（max-heap）
// 2. 遍历 [middle, last)，如果小于堆顶元素，就将堆顶元素与该元素交换，然后调整堆
// 经过了步骤 2 后可以保证 [first, middle) 内的元素一定小于 [middle, last) 内的元素
// 3. 最后将 [first, middle) 转化为有序序列
/*****************************************************************************************/

/// @brief 对整个序列做部分排序，保证 [first, middle) 内的元素有序且小于 [middle, last) 内的元素
/// 内部使用堆排序
template <class RandomAccessIterator>
void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, 
                  RandomAccessIterator last) {
    tinystl::make_heap(first, middle);
    for (auto i = middle; i != last; ++i) {
        if (*i < *first) tinystl::pop_heap_aux(first, middle, i, *i, distance_type(first));
    }
    tinystl::sort_heap(first, middle);
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class RandomAccessIterator, class Compare>
void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, 
                  RandomAccessIterator last, Compare comp) {
    tinystl::make_heap(first, middle, comp);
    for (auto i = middle; i != last; ++i) {
        if (comp(*i, *first)) tinystl::pop_heap_aux(first, middle, i, *i, distance_type(first), comp);
    }
    tinystl::sort_heap(first, middle, comp);
}

/*****************************************************************************************/
// partial_sort_copy
// 行为与 partial_sort 类似，不同的是把排序结果复制到 result 容器中
/*****************************************************************************************/

template <class InputIterator, class RandomAccessIterator, class Distance>
RandomAccessIterator psort_copy_aux(InputIterator first, InputIterator last, 
                                    RandomAccessIterator result_first, RandomAccessIterator result_last, 
                                    Distance*) {
    if (result_first == result_last) return result_last;
    auto result_iter = result_first;
    while (first != last && result_iter != result_last) {
        *result_iter++ = *first++;
    }
    tinystl::make_heap(result_first, result_iter);
    // 此时 first 已经移动到了后半段的位置
    while (first != last) {
        if (*first < *result_first) tinystl::adjust_heap(
            result_first, static_cast<Distance>(0), 
            result_iter - result_first, *first);
        ++first;
    }
    tinystl::sort_heap(result_first, result_iter);
    return result_iter;
}

/// @brief 将 [first, last) 中前 n 个最小元素复制到 [result_first, result_last) 中
template <class InputIterator, class RandomAccessIterator>
RandomAccessIterator partial_sort_copy(InputIterator first, InputIterator last, 
                                       RandomAccessIterator result_first, 
                                       RandomAccessIterator result_last) {
    return tinystl::psort_copy_aux(first, last, result_first, result_last, distance_type(result_first));
}

template <class InputIterator, class RandomAccessIterator, class Distance, class Compare>
RandomAccessIterator psort_copy_aux(InputIterator first, InputIterator last, 
                                    RandomAccessIterator result_first, RandomAccessIterator result_last, 
                                    Distance*, Compare comp) {
    if (result_first == result_last) return result_last;
    auto result_iter = result_first;
    while (first != last && result_iter != result_last) {
        *result_iter++ = *first++;
    }
    tinystl::make_heap(result_first, result_iter, comp);
    // 此时 first 已经移动到了后半段的位置
    while (first != last) {
        if (comp(*first, *result_first)) tinystl::adjust_heap(
            result_first, static_cast<Distance>(0), 
            result_iter - result_first, *first, comp);
        ++first;
    }
    tinystl::sort_heap(result_first, result_iter, comp);
    return result_iter;
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class InputIterator, class RandomAccessIterator, class Compare>
RandomAccessIterator partial_sort_copy(InputIterator first, InputIterator last, 
                                       RandomAccessIterator result_first, 
                                       RandomAccessIterator result_last, Compare comp) {
    return tinystl::psort_copy_aux(first, last, result_first, result_last, distance_type(result_first), comp);
}

/*****************************************************************************************/
// partition
// 对区间内的元素重排，被一元条件运算判定为 true 的元素会放到区间的前段
// 该函数不保证元素的原始相对位置
/*****************************************************************************************/

/// @brief 对区间内的元素重排，被一元条件运算判定为 true 的元素会放到区间的前段
template <class BidirectionalIterator, class UnaryPredicate>
BidirectionalIterator partition(BidirectionalIterator first, BidirectionalIterator last, 
                                UnaryPredicate pred) {
    while (true) {
        // 找到一个不满足 pred 的位于前端的元素
        while (first != last && pred(*first)) ++first;
        if (first == last) break;
        --last;
        // 找到一个满足 pred 的位于后端的元素
        while (first != last && !pred(*last)) --last;
        if (first == last) break;
        // 交换两个元素
        tinystl::iter_swap(first, last);
        ++first;
    }
    return first;
}

/*****************************************************************************************/
// partition_copy
// 行为与 partition 类似，不同的是，将被一元操作符判定为 true 的放到 result_true 的输出区间
// 其余放到 result_false 的输出区间，并返回一个 tinystl::pair 指向这两个区间的尾部
/*****************************************************************************************/

/// @brief 将被一元操作符判定为 true 的放到 result_true 的输出区间，
/// 其余放到 result_false 的输出区间，并返回一个 tinystl::pair 指向这两个区间的尾部
template <class InputIterator, class OutputIterator1, class OutputIterator2, class UnaryPredicate>
tinystl::pair<OutputIterator1, OutputIterator2>
partition_copy(InputIterator first, InputIterator last, OutputIterator1 result_true, 
               OutputIterator2 result_false, UnaryPredicate pred) {
    for (; first != last; ++first) {
        if (pred(*first)) *result_true++ = *first;
        else *result_false++ = *first;
    }
    return tinystl::pair<OutputIterator1, OutputIterator2>(result_true, result_false);
}

/*****************************************************************************************/
// sort
// 将[first, last)内的元素以递增的方式排序
// 使用 sort 算法的迭代器必须为 random_access_iterator
// TODO: 讲解
/*****************************************************************************************/
constexpr static size_t kThreshold = 32;  // 小型区间的大小，在这个大小内采用插入排序

/// @brief 找出 lgk <= n 的 k 的最大值，用于控制分割恶化的情况
template <class Size>
Size slg2(Size n) {
    Size k;
    for (k = 0; n > 1; n >>= 1) ++k;
    return k;
}

// unchecked 表示不经过严格的边界检查，一般在已经知道区间合法的情况下使用
// 但好处是可以减少一些不必要的边界检查，提高效率

/// @brief 将 [first, last) 范围内小于 pivot 的元素放到前面，大于 pivot 的元素放到后面
template <class RandomAccessIterator, class T>
RandomAccessIterator unchecked_partition(RandomAccessIterator first, RandomAccessIterator last, 
                                         const T& pivot) {
    while (true) {
        while (*first < pivot) ++first;
        --last;
        while (pivot < *last) --last;
        if (!(first < last)) return first;
        tinystl::iter_swap(first, last);
        ++first;
    }
}

/// @brief 内省式排序，先进行 quick sort，当分割行为有恶化倾向时，改用 heap sort
template <class RandomAccessIterator, class Size>
void intro_sort(RandomAccessIterator first, RandomAccessIterator last, Size depth_limit) {
    while (last - first > static_cast<Size>(kThreshold)) {
        // 到达最大分割深度限制，使用 heap_sort
        if (depth_limit == 0) {
            tinystl::partial_sort(first, last, last);
            return;
        }
        --depth_limit;
        // 取三点中值作为 pivot
        auto mid = tinystl::median(*(first), *(first + (last - first) / 2), *(last - 1));
        auto cut = tinystl::unchecked_partition(first, last, mid);
        tinystl::intro_sort(cut, last, depth_limit);  // 对后半段递归
        last = cut;  // 更新 last，持续对前半段进行分割
    }
}

/// @brief 插入排序辅助函数，翻转 last 之前的逆转对
template <class RandomAccessIterator, class T>
void unchecked_linear_insert(RandomAccessIterator last, const T& value) {
    auto next = last;
    --next;
    while (value < *next) {
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}

/// @brief 插入排序函数
template <class RandomAccessIterator>
void unchecked_insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
    // 从 first + 1 开始排序
    for (auto i = first; i != last; ++i) {
        tinystl::unchecked_linear_insert(i, *i);
    }
}

/// @brief 插入排序函数
template <class RandomAccessIterator>
void insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
    if (first == last) return;
    for (auto i = first + 1; i != last; ++i) {
        auto value = *i;
        // 如果 value 比 first 还小，就将 first 后移，然后将 value 放到 first 处
        if (value < *first) {
            tinystl::copy_backward(first, i, i + 1);
            *first = value;
        }
        // 这里保证 value 的值一定是小于等于 *first 的，所以可以直接调用 unchecked_linear_insert
        else {
            tinystl::unchecked_linear_insert(i, value);
        }
    }
}

/// @brief 插入排序函数
template <class RandomAccessIterator>
void final_insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
    // 如果排序区间长度大于 kThreshold，就对分别对前后两个区间进行插入排序
    // 之所以分成两个区间是因为要保证最小元素在数组边缘，这样才能减少一个判断
    if (last - first > static_cast<ptrdiff_t>(kThreshold)) {
        // 由于 intro_sort 以 kThreshold 为界限，因此最小元素一定在前 kThreshold 个元素中
        tinystl::insertion_sort(first, first + static_cast<ptrdiff_t>(kThreshold));
        tinystl::unchecked_insertion_sort(first + static_cast<ptrdiff_t>(kThreshold), last);
    }
    else {
        tinystl::insertion_sort(first, last);
    }
}

/// @brief 对 [first, last) 进行排序，先进行 intro_sort，将区间划分为一个个小区间，再对整体使用插入排序
template <class RandomAccessIterator>
void sort(RandomAccessIterator first, RandomAccessIterator last) {
    if (first != last) {
        tinystl::intro_sort(first, last, slg2(last - first) * 2);
        tinystl::final_insertion_sort(first, last);
    }
}

// =========================== 重载版本使用 comp 代替比较操作 ============================= //

/// @brief 将 [first, last) 范围内小于 pivot 的元素放到前面，大于 pivot 的元素放到后面
template <class RandomAccessIterator, class T, class Compare>
RandomAccessIterator unchecked_partition(RandomAccessIterator first, RandomAccessIterator last, 
                                         const T& pivot, Compare comp) {
    while (true) {
        while (comp(*first, pivot)) ++first;
        --last;
        while (comp(pivot, *last)) --last;
        if (!(first < last)) return first;  // 越界了就返回
        tinystl::iter_swap(first, last);
        ++first;
    }
}

/// @brief 内省式排序，先进行 quick sort，当分割行为有恶化倾向时，改用 heap sort
template <class RandomAccessIterator, class Size, class Compare>
void intro_sort(RandomAccessIterator first, RandomAccessIterator last, 
                Size depth_limit, Compare comp) {
    while (last - first > static_cast<ptrdiff_t>(kThreshold)) {
        // 到达最大分割深度限制，使用 heap_sort
        if (depth_limit == 0) {
            tinystl::partial_sort(first, last, last, comp);
            return;
        }
        --depth_limit;
        // 取三点中值作为 pivot
        auto mid = tinystl::median(*(first), *(first + (last - first) / 2), *(last - 1), comp);
        auto cut = tinystl::unchecked_partition(first, last, mid, comp);
        tinystl::intro_sort(cut, last, depth_limit, comp);  // 对后半段递归
        last = cut;  // 更新 last，持续对前半段进行分割
    }
}

/// @brief 插入排序辅助函数，翻转 last 之前的逆转对
template <class RandomAccessIterator, class T, class Compare>
void unchecked_linear_insert(RandomAccessIterator last, const T& value, Compare comp) {
    auto next = last;
    --next;
    // 从尾部开始寻找第一个可插入位置
    while (comp(value, *next)) {
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}

/// @brief 插入排序函数 
template <class RandomAccessIterator, class Compare>
void unchecked_insertion_sort(RandomAccessIterator first, RandomAccessIterator last, 
                                Compare comp) {
    // 从 first + 1 开始排序
    for (auto i = first; i != last; ++i) {
        tinystl::unchecked_linear_insert(i, *i, comp);
    }
}

/// @brief 插入排序函数
template <class RandomAccessIterator, class Compare>
void insertion_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    if (first == last) return;
    for (auto i = first + 1; i != last; ++i) {
        auto value = *i;
        // 如果 value 比 first 还小，就将 first 后移，然后将 value 放到 first 处
        if (comp(value, *first)) {
            tinystl::copy_backward(first, i, i + 1);
            *first = value;
        }
        // 这里保证 value 的值一定是小于等于 *first 的，所以可以直接调用 unchecked_linear_insert
        else {
            tinystl::unchecked_linear_insert(i, value, comp);
        }
    }
}

/// @brief 最终的插入排序函数
template <class RandomAccessIterator, class Compare>
void final_insertion_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    // 如果排序区间长度大于 kThreshold，就对分别对前后两个区间进行插入排序
    // 之所以分成两个区间是因为要保证最小元素在数组边缘，这样才能减少一个判断
    if (last - first > static_cast<ptrdiff_t>(kThreshold)) {
        // 由于 intro_sort 以 kThreshold 为界限，因此最小元素一定在前 kThreshold 个元素中
        tinystl::insertion_sort(first, first + static_cast<ptrdiff_t>(kThreshold), comp);
        tinystl::unchecked_insertion_sort(first + static_cast<ptrdiff_t>(kThreshold), last, comp);
    }
    else {
        tinystl::insertion_sort(first, last, comp);
    }
}

/// @brief 对 [first, last) 进行排序，先进行 intro_sort，将区间划分为一个个小区间，再对整体使用插入排序
template <class RandomAccessIterator, class Compare>
void sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    if (first != last) {
        tinystl::intro_sort(first, last, slg2(last - first) * 2, comp);
        tinystl::final_insertion_sort(first, last, comp);
    }
}

/*****************************************************************************************/
// nth_element
// 对序列重排，使得所有小于第 n 个元素的元素出现在它的前面，大于它的出现在它的后面
// 思路：
// 1. 选取三点中值作为 pivot，对序列进行分割
// 2. 若 nth 元素落在分割点左侧，对左侧序列递归调用 nth_element，否则对右侧序列递归调用 nth_element
// 3. 重复上述过程，直到分割区间长度小于等于 3，然后对这个区间进行插入排序
// TODO: 讲解
/*****************************************************************************************/

/// @brief 对序列重排，使得所有小于第 n 个元素的元素出现在它的前面，大于它的出现在它的后面
template <class RandomAccessIterator>
void nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last) {
    if (nth == last) return;
    while (last - first > 3) {
        // 选取三点中值作为 pivot 进行分割
        auto cut = tinystl::unchecked_partition(first, last, 
            tinystl::median(*first, *(first + (last - first) / 2), *(last - 1)));
        // nth 元素落在分割点右侧，对右侧序列递归调用 nth_element
        if (cut <= nth) first = cut;
        // nth 元素落在分割点左侧，对左侧序列递归调用 nth_element
        else last = cut;
    }
    // 对分割区间进行插入排序
    tinystl::insertion_sort(first, last);
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class RandomAccessIterator, class Compare>
void nth_element(RandomAccessIterator first, RandomAccessIterator nth, 
                 RandomAccessIterator last, Compare comp) {
    if (nth == last) return;
    while (last - first > 3) {
        // 选取三点中值作为 pivot 进行分割
        auto cut = tinystl::unchecked_partition(first, last, 
            tinystl::median(*first, *(first + (last - first) / 2), *(last - 1), comp), comp);
        // nth 元素落在分割点右侧，对右侧序列递归调用 nth_element
        if (cut <= nth) first = cut;
        // nth 元素落在分割点左侧，对左侧序列递归调用 nth_element
        else last = cut;
    }
    // 对分割区间进行插入排序
    tinystl::insertion_sort(first, last, comp);
}

/*****************************************************************************************/
// unique_copy
// 从[first, last)中将元素复制到 result 上，序列必须有序，如果有重复的元素，只会复制一次
/*****************************************************************************************/

/// @brief unique_copy 的 forward_iterator_tag 版本
template <class InputIterator, class ForwardIterator>
ForwardIterator unique_copy_dispatch(InputIterator first, InputIterator last, 
                                     ForwardIterator result, forward_iterator_tag) {
    *result = *first;
    while (++first != last) {
        if (*result != *first) *++result = *first;
    }
    return ++result;
}

/// @brief unique_copy_dispatch 的 output_iterator_tag 版本
// 由于 output iterator 只能进行只读操作，所以不能有 *result != *first 这样的判断
template <class InputIterator, class OutputIterator>
OutputIterator unique_copy_dispatch(InputIterator first, InputIterator last, 
                                    OutputIterator result, output_iterator_tag) {
    auto value = *first;
    *result = value;
    while (++first != last) {
        if (value != *first) {
            value = *first;
            *++result = value;
        }
    }
    return ++result;
}

/// @brief 从[first, last)中将元素复制到 result 上，序列必须有序，如果有重复的元素，只会复制一次 
template <class InputIterator, class OutputIterator>
OutputIterator unique_copy(InputIterator first, InputIterator last, OutputIterator result) {
    if (first == last) return result;
    return tinystl::unique_copy_dispatch(first, last, result, iterator_category(result));
}

// ================================ 重载版本使用函数对象 comp ============================== //

/// @brief unique_copy 的 forward_iterator_tag 版本
template <class InputIterator, class ForwardIterator, class Compare>
ForwardIterator unique_copy_dispatch(InputIterator first, InputIterator last, 
                                     ForwardIterator result, forward_iterator_tag, Compare comp) {
    *result = *first;
    while (++first != last) {
        if (!comp(*result, *first)) *++result = *first;
    }
    return ++result;
}

/// @brief unique_copy_dispatch 的 output_iterator_tag 版本
template <class InputIterator, class OutputIterator, class Compare>
OutputIterator unique_copy_dispatch(InputIterator first, InputIterator last, 
                                    OutputIterator result, output_iterator_tag, Compare comp) {
    auto value = *first;
    *result = value;
    while (++first != last) {
        if (!comp(value, *first)) {
            value = *first;
            *++result = value;
        }
    }
    return ++result;
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class InputIterator, class OutputIterator, class Compare>
OutputIterator unique_copy(InputIterator first, InputIterator last, OutputIterator result, Compare comp) {
    if (first == last) return result;
    return tinystl::unique_copy_dispatch(first, last, result, iterator_category(result), comp);
}

/*****************************************************************************************/
// unique
// 移除[first, last)内重复的元素，序列必须有序，和 remove 类似，它也不能真正的删除重复元素
/*****************************************************************************************/

/// @brief 移除[first, last)内重复的元素，实际只是将重复的元素移动到后面，返回一个迭代器指向不重复序列的尾部
template <class ForwardIterator>
ForwardIterator unique(ForwardIterator first, ForwardIterator last) {
    first = tinystl::adjacent_find(first, last);
    return tinystl::unique_copy(first, last, first);
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class ForwardIterator, class Compare>
ForwardIterator unique(ForwardIterator first, ForwardIterator last, Compare comp) {
    first = tinystl::adjacent_find(first, last, comp);
    return tinystl::unique_copy(first, last, first, comp);
}

}  // namespace tinystl

#endif  // TINYSTL_ALGO_H_