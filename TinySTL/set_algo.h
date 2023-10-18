#ifndef TINYSTL_SET_ALGO_H_
#define TINYSTL_SET_ALGO_H_

// 这个头文件包含 set 的四种算法: union, intersection, difference, symmetric_difference
// 所有函数都要求序列有序

#include "algobase.h"   // copy, copy_n, copy_backward
#include "iterator.h"

namespace tinystl {

/*****************************************************************************************/
// set_union
// 计算 S1 ∪ S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
/*****************************************************************************************/

/// @brief 计算 S1 ∪ S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1, 
                         InputIterator2 first2, InputIterator2 last2, 
                         OutputIterator result) {
    // 过程类似于合并两个有序链表的做法
    // 两个序列必须有序
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
        } 
        else if (*first2 < *first1) {
            *result = *first2;
            ++first2;
        }
        else {
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }
    // 将剩余元素拷贝到 result 中
    return tinystl::copy(first2, last2, tinystl::copy(first1, last1, result));
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1, 
                         InputIterator2 first2, InputIterator2 last2, 
                         OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
        } 
        else if (comp(*first2, *first1)) {
            *result = *first2;
            ++first2;
        }
        else {
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }
    // 将剩余元素拷贝到 result 中
    return tinystl::copy(first2, last2, tinystl::copy(first1, last1, result));
}

/*****************************************************************************************/
// set_intersection
// 计算 S1 ∩ S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
/*****************************************************************************************/

/// @brief 计算 S1 ∩ S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1, 
                                InputIterator2 first2, InputIterator2 last2, 
                                OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            ++first1;
        } 
        else if (*first2 < *first1) {
            ++first2;
        }
        else {
            *result = *first1;
            ++first1;
            ++first2;
            ++result;
        }
    }
    return result;
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1, 
                                InputIterator2 first2, InputIterator2 last2, 
                                OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            ++first1;
        } 
        else if (comp(*first2, *first1)) {
            ++first2;
        }
        else {
            *result = *first1;
            ++first1;
            ++first2;
            ++result;
        }
    }
    return result;
}

/*****************************************************************************************/
// set_difference
// 计算 S1-S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
/*****************************************************************************************/

/// @brief 计算 S1-S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1, 
                              InputIterator2 first2, InputIterator2 last2, 
                              OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        // 只有 *first1 < *first2 才能确保这个元素在 S1 中有而 S2 中没有
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
            ++result;
        } 
        // 否则与 S2 的下一个元素继续比较
        else if (*first2 < *first1) {
            ++first2;
        }
        // 相等则说明这个元素在 S1 和 S2 中都有，不是 S1-S2 的元素
        else {
            ++first1;
            ++first2;
        }
    }
    // 剩下的部分都是 S1 中有而 S2 中没有的元素
    return tinystl::copy(first1, last1, result);
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1, 
                              InputIterator2 first2, InputIterator2 last2, 
                              OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
            ++result;
        } 
        else if (comp(*first2, *first1)) {
            ++first2;
        }
        else {
            ++first1;
            ++first2;
        }
    }
    return tinystl::copy(first1, last1, result);
}

/*****************************************************************************************/
// set_symmetric_difference
// 等同于并集减去交集
// 计算 (S1-S2)∪(S2-S1) 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
/*****************************************************************************************/

/// @brief 计算 (S1-S2)∪(S2-S1) 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1, 
                                        InputIterator2 first2, InputIterator2 last2, 
                                        OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (*first2 < *first1) {
            *result = *first2;
            ++first2;
            ++result;
        }
        else {
            ++first1;
            ++first2;
        }
    }
    // 剩下的部分是集合中独特的元素
    return tinystl::copy(first2, last2, tinystl::copy(first1, last1, result));
}

/// @brief 重载版本使用函数对象 comp 代替比较操作
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1, 
                                        InputIterator2 first2, InputIterator2 last2, 
                                        OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (comp(*first2, *first1)) {
            *result = *first2;
            ++first2;
            ++result;
        }
        else {
            ++first1;
            ++first2;
        }
    }
    return tinystl::copy(first2, last2, tinystl::copy(first1, last1, result));
}

}  // namespace tinystl

#endif // !TINYSTL_SET_ALGO_H_