#ifndef TINYSTL_ALGO_H_
#define TINYSTL_ALGO_H_

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif

// 这个头文件包含了 mystl 的一系列算法

#include <cstddef>
#include <ctime>

#include "algobase.h"
#include "memory.h"
// #include "heap_algo.h"
// #include "functional.h"

namespace tinystl {

/*****************************************************************************************/
// reverse
// 将[first, last)区间内的元素反转
/*****************************************************************************************/

template <class BidirectionalIterator>
void reverse_dispatch(BidirectionalIterator first, BidirectionalIterator last, 
    bidirectional_iterator_tag) {
    while (true) {
        // TODO: 分析这里的巧妙实现
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

}  // namespace tinystl

#endif  // TINYSTL_ALGO_H_