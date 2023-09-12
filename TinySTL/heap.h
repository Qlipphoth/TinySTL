#ifndef TINYSTL_HEAP_H_
#define TINYSTL_HEAP_H_

// 这个头文件包含了 heap 相关的算法
// 严格意义上来讲heap并不是一个容器, 所以他没有实现自己的迭代器, 
// 也就没有遍历操作, 它只是一种算法.

#include "iterator.h"

namespace tinystl {

/*****************************************************************************************/
// push_heap
// 该函数接受两个迭代器，表示一个 heap 容器的首尾，并且新元素已经插入到底部容器的最尾端，调整 heap
/*****************************************************************************************/

/// @brief 在堆中新加入元素后，执行 percolate up 操作
/// @param first      堆的首元素迭代器
/// @param holeIndex  新加入元素的索引
/// @param topIndex   堆的首元素索引
/// @param value      新加入的元素
template <class RandomIterator, class Distance, class T>
void push_heap_aux(RandomIterator first, Distance holeIndex, Distance topIndex, T value) {
    auto parent = (holeIndex - 1) / 2;
    while (holeIndex > topIndex && *(first + parent) < value) {
        // 如果不是根节点, 并且父节点的值小于新值
        // 那么就把父节点的值赋给洞节点
        *(first + holeIndex) = *(first + parent);
        // 更新洞节点的索引
        holeIndex = parent;
        // 更新父节点的索引
        parent = (holeIndex - 1) / 2;
    }
    // 最后把新值赋给洞节点
    *(first + holeIndex) = value;
}

template <class RandomIterator, class Distance>
void push_heap_d(RandomIterator first, RandomIterator last, Distance*) {
    tinystl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1));
}

template <class RandomIterator>
void push_heap(RandomIterator first, RandomIterator last) {
    // 新元素应置于底部容器的最尾端
    tinystl::push_heap_d(first, last, distance_type(first));
}

// ========================== 利用 comp 的重载版本 ========================== //

template <class RandomIterator, class Distance, class T, class Compared>
void push_heap_aux(RandomIterator first, Distance holeIndex, Distance topIndex, T value, Compared comp) {
    auto parent = (holeIndex - 1) / 2;
    while (holeIndex > topIndex && comp(*(first + parent), value)) {
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;
}

template <class RandomIterator, class Compared, class Distance>
void push_heap_d(RandomIterator first, RandomIterator last, Distance*, Compared comp) {
    tinystl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1), comp);
}

template <class RandomIterator, class Compared>
void push_heap(RandomIterator first, RandomIterator last, Compared comp) {
    tinystl::push_heap_d(first, last, distance_type(first), comp);
}


/*****************************************************************************************/
// pop_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，将 heap 的根节点取出放到容器尾部，调整 heap
/*****************************************************************************************/

/// @brief 从洞节点开始调整堆，使之重新成为一个 max-heap
/// @param first      堆的首元素迭代器
/// @param holeIndex  洞节点的索引
/// @param len        堆的长度
/// @param value      洞节点的值
template <class RandomIterator, class Distance, class T>
void adjust_heap(RandomIterator first, Distance holeIndex, Distance len, T value) {
    // percolate down
    auto topIndex = holeIndex;
    auto rchild = 2 * holeIndex + 2;
    // 调整范围为 [first, first + len)
    while (rchild < len) {
        // 找到左右子节点中较大的那个
        if (*(first + rchild) < *(first + rchild - 1)) --rchild;
        // 将较大的子节点的值赋给洞节点
        *(first + holeIndex) = *(first + rchild);
        // 更新洞节点的索引
        holeIndex = rchild;
        // 更新右子节点的索引
        rchild = 2 * (rchild + 1);
    }

    // 此时右子节点中已经是换下来的根节点值，无需调整，所以只需考虑左子节点
    if (rchild == len) {
        // 将左子节点的值赋给洞节点
        *(first + holeIndex) = *(first + rchild - 1);
        // 更新洞节点的索引为左子节点的索引
        holeIndex = rchild - 1;
    }

    // 注意：上述的下放过程并没有考虑洞节点的值是否已经大于两个子节点，而是直接下放到叶子节点
    // 所以这里需要再次进行 percolate up 操作，将洞节点的值上放到合适的位置
    // 侯捷老师的《STL源码剖析》中这部分说的是将这一步直接改为
    // *(first + holeIndex) = value;  // 这样肯定是有问题的。
    tinystl::push_heap_aux(first, holeIndex, topIndex, value);
}

template <class RandomIterator, class T, class Distance>
void pop_heap_aux(RandomIterator first, RandomIterator last, RandomIterator result, T value, Distance*) {
    // 先将首值调至尾节点，然后调整[first, last - 1)使之重新成为一个 max-heap
    *result = *first;
    tinystl::adjust_heap(first, static_cast<Distance>(0), last - first, value);
}

template <class RandomIterator>
void pop_heap(RandomIterator first, RandomIterator last) {
    tinystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first));
}

// ========================== 利用 comp 的重载版本 ========================== //

template <class RandomIterator, class Distance, class T, class Compared>
void adjust_heap(RandomIterator first, Distance holeIndex, Distance len, T value, Compared comp) {
    // percolate down
    auto topIndex = holeIndex;
    auto rchild = 2 * holeIndex + 2;
    while (rchild < len) {
        if (comp(*(first + rchild), *(first + rchild - 1))) --rchild;
        *(first + holeIndex) = *(first + rchild);
        holeIndex = rchild;
        rchild = 2 * (rchild + 1);
    }
    // 如果没有右子节点
    if (rchild == len) {
        *(first + holeIndex) = *(first + rchild - 1);
        holeIndex = rchild - 1;
    }
    tinystl::push_heap_aux(first, holeIndex, topIndex, value, comp);
}

template <class RandomIterator, class T, class Distance, class Compared>
void pop_heap_aux(RandomIterator first, RandomIterator last, RandomIterator result, 
    T value, Distance*, Compared comp) {
    // 先将首值调至尾节点，然后调整[first, last - 1)使之重新成为一个 max-heap
    *result = *first;
    tinystl::adjust_heap(first, static_cast<Distance>(0), last - first, value, comp);
}

template <class RandomIterator, class Compared>
void pop_heap(RandomIterator first, RandomIterator last, Compared comp) {
    // 这里之所以将 last - 1 传入，是因为 pop_heap 之后，尾部的元素就是最大的元素，只需调整 [first, last - 1)
    tinystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first), comp);
}


/*****************************************************************************************/
// sort_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，不断执行 pop_heap 操作，直到首尾最多相差1
/*****************************************************************************************/

/// @brief 堆排序，每次将堆顶元素放到尾部，然后调整堆
/// @param first  堆的首元素迭代器
/// @param last   堆的尾元素迭代器
template <class RandomIterator>
void sort_heap(RandomIterator first, RandomIterator last) {
    // 从尾部开始，每次 pop_heap 之后，尾部的元素就是最大的元素
    while (last - first > 1) {
        tinystl::pop_heap(first, last--);
    }
}

// ========================== 利用 comp 的重载版本 ========================== //

template <class RandomIterator, class Compared>
void sort_heap(RandomIterator first, RandomIterator last, Compared comp) {
    while (last - first > 1) {
        tinystl::pop_heap(first, last--, comp);
    }
}


/*****************************************************************************************/
// make_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，把容器内的数据变为一个 heap
/*****************************************************************************************/

template <class RandomIterator, class Distance>
void make_heap_aux(RandomIterator first, RandomIterator last, Distance*) {
    if (last - first < 2) return;
    auto len = last - first;
    // 由于任何一个叶节点都可以看作只包含一个元素的堆，无需调整，所以从最后一个非叶节点开始调整
    auto holeIndex = (len - 2) / 2;
    while (true) {
        // 重排以 holeIndex 为首的子树
        tinystl::adjust_heap(first, holeIndex, len, *(first + holeIndex));
        if (holeIndex == 0) return;
        holeIndex--;
    }
}

/// @brief 将容器的 [first, last) 区间变为一个 heap
/// @param first  堆的首元素迭代器
/// @param last   堆的尾元素迭代器
template <class RandomIterator>
void make_heap(RandomIterator first, RandomIterator last) {
    tinystl::make_heap_aux(first, last, distance_type(first));
}

// ========================== 利用 comp 的重载版本 ========================== //

template <class RandomIterator, class Distance, class Compared>
void make_heap_aux(RandomIterator first, RandomIterator last, Distance*, Compared comp) {
    if (last - first < 2) return;
    auto len = last - first;
    auto holeIndex = (len - 2) / 2;
    while (true) {
        // 重排以 holeIndex 为首的子树
        tinystl::adjust_heap(first, holeIndex, len, *(first + holeIndex), comp);
        if (holeIndex == 0) return;
        holeIndex--;
    }
}

template <class RandomIterator, class Compared>
void make_heap(RandomIterator first, RandomIterator last, Compared comp) {
    tinystl::make_heap_aux(first, last, distance_type(first), comp);
}

}  // namespace tinystl

#endif  // TINYSTL_HEAP_H_