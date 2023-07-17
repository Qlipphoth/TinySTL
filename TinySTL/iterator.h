#ifndef TINYSTL_ITERATOR_H_
#define TINYSTL_ITERATOR_H_

// 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数，

#include <cstddef>  // ptrdiff_t, size_t
#include "type_traits.h"

namespace tinystl {

// 五种迭代器类型
struct input_iterator_tag {};  // 只读
struct output_iterator_tag {};  // 只写
struct forward_iterator_tag : public input_iterator_tag {};  // 可读写
struct bidirectional_iterator_tag : public forward_iterator_tag {};  // 双向
struct random_access_iterator_tag : public bidirectional_iterator_tag {};  // 随机访问

// iterator 模板结构体，如果每个新设计的迭代器都继承自它，就可保证符合 STL 规范
template <class Category, class T, class Distance = ptrdiff_t, 
    class Pointer = T*, class Reference = T&>
struct iterator {
    typedef Category    iterator_category;  // 迭代器类型
    typedef T           value_type;         // 迭代器所指对象的类型
    typedef Distance    difference_type;    // 迭代器之间的距离
    typedef Pointer     pointer;            // 指向迭代器所指对象的指针
    typedef Reference   reference;          // 迭代器所指对象的引用
};

// traits

// `iterator_traits`结构体就是使用`typename`对参数类型的提取(萃取), 
// 并且对参数类型在进行一次命名, 看上去对参数类型的使用有了一层间接性.
template <class Iterator>
struct iterator_traits {
    typedef typename Iterator::iterator_category    iterator_category;
    typedef typename Iterator::value_type           value_type;
    typedef typename Iterator::difference_type      difference_type;
    typedef typename Iterator::pointer              pointer;
    typedef typename Iterator::reference            reference;
};

// 针对原生指针(native pointer)而设计的 traits 偏特化版本
template <class T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag  iterator_category;  // 原生指针是随机访问迭代器
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef T*                          pointer;
    typedef T&                          reference;
};

// 针对原生指针(const native pointer)而设计的 traits 偏特化版本
template <class T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag  iterator_category;  // 原生指针是随机访问迭代器
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef const T*                    pointer;
    typedef const T&                    reference;
};

/// @brief 用于获取某个迭代器的 category
/// @tparam Iterator  迭代器类型 
/// @param   迭代器对象
/// @return  迭代器的类型
template <class Iterator>
typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&) {
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();  // 返回默认构造的 category 对象，即迭代器的类别
}

/// @brief 用于获取某个迭代器的 disstance_type
/// @tparam Iterator  迭代器类型
/// @param   迭代器对象
/// @return  迭代器的 distance_type
template <class Iterator>
typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator&) {
    // **这里用到了0可以转换成指针的性质, 相当于返回一个空指针, 但是可以通过它们确定不同的参数类型.** 
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

/// @brief 用于获取某个迭代器的 value_type
/// @tparam Iterator  迭代器类型
/// @param   迭代器对象
/// @return  迭代器的 value_type
template <class Iterator>
typename iterator_traits<Iterator>::value_type* value_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// distance

/// @brief 计算两个迭代器之间的距离
/// @tparam InputIterator  迭代器类型
/// @param first  第一个迭代器
/// @param last   第二个迭代器
/// @param   迭代器的类型
/// @return  两个迭代器之间的距离
template <class InputIterator>  // 以算法所能接受的最低阶迭代器类型，来为其迭代器类别参数命名
typename iterator_traits<InputIterator>::difference_type __distance(InputIterator first, 
    InputIterator last, input_iterator_tag) {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) {
            ++first; ++n;
        }
        return n;
    }

template <class RandomAccessIterator>  // 以算法所能接受的最低阶迭代器类型，来为其迭代器类别参数命名
typename iterator_traits<RandomAccessIterator>::difference_type __distance(RandomAccessIterator first, 
    RandomAccessIterator last, random_access_iterator_tag) {
        return last - first;  // 随机访问迭代器可以直接相减
    }

template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, 
    InputIterator last) {
        typedef typename iterator_traits<InputIterator>::iterator_category category;
        return __distance(first, last, category());  // 通过迭代器的类别来调用不同的函数
    }


// advance

/// @brief 使迭代器前进 n 个距离
/// @tparam InputIterator  迭代器类型
/// @tparam Distance     距离类型
/// @param i  迭代器
/// @param n  距离
/// @param    迭代器的类型
template <class InputIterator, class Distance>
void __advance(InputIterator& i, Distance n, input_iterator_tag) {
    while (n--) ++i;
}

template <class BidirectionalIterator, class Distance>
void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
    if (n >= 0) while (n--) ++i;
    else while (n++) --i;
}

template <class RandomAccessIterator, class Distance>
void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag) {
    i += n;
}

template <class InputIterator, class Distance>
void advance(InputIterator& i, Distance n) {
    __advance(i, n, iterator_category(i));
}


// TODO: reverse_iterator








}



#endif