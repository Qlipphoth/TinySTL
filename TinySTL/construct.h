#ifndef TINYSTL_CONSTRUCT_H_
#define TINYSTL_CONSTRUCT_H_

// 这个头文件包含两个函数 construct，destroy
// construct : 负责对象的构造
// destroy   : 负责对象的析构

#include <new>          // placement new
#include "type_traits.h"
#include "iterator.h"
#include "util.h"

namespace tinystl {

// ==================================== construct ==================================== //
// 负责对象的构造

template <class Ty>
void construct(Ty* ptr) {
    // 将 ptr 强转为 void*，然后在 ptr 上调用默认构造函数
    ::new ((void*)ptr) Ty();  
}

template <class Ty1, class Ty2>
void construct(Ty1* ptr, const Ty2& value) {
    // 将 ptr 强转为 void*，然后在 ptr 上调用拷贝构造函数
    ::new ((void*)ptr) Ty1(value);  
}

// ... 的作用是将参数包展开，详见 note2.md
template <class Ty, class... Args>
void construct(Ty* ptr, Args&&... args) {
    // 带参数构造与移动构造均使用这个函数
    ::new ((void*) ptr) Ty(tinystl::forward<Args>(args)...);
}

// =================================== destroy =================================== //
// 负责对象的析构

// 平凡析构，不做任何处理
template <class Ty>
void destroy_one(Ty*, std::true_type) {}

// 非平凡析构，调用对象的析构函数
template <class Ty>
void destroy_one(Ty* pointer, std::false_type) {
    if (pointer != nullptr) {
        pointer->~Ty();
    }
}

template <class Ty>
void destroy(Ty* pointer);

template <class ForwardIterator>
void destroy(ForwardIterator first, ForwardIterator last);

// 平凡析构，不做任何处理
template <class ForwardIterator>
void destroy_cat(ForwardIterator, ForwardIterator, std::true_type) {}

// 非平凡析构，调用对象的析构函数
template <class ForwardIterator>
void destroy_cat(ForwardIterator first, ForwardIterator last, std::false_type) {
    for (; first != last; ++first) {
        destroy(&*first);
    }
}

template <class Ty>
void destroy(Ty* pointer) {
    // 判断是否是平凡析构
    destroy_one(pointer, std::is_trivially_destructible<Ty>{});
}

template <class ForwardIterator>
void destroy(ForwardIterator first, ForwardIterator last) {
    // 通过 iterator_traits 获取迭代器的类型，然后判断是否是平凡析构
    destroy_cat(first, last, std::is_trivially_destructible<
    typename iterator_traits<ForwardIterator>::value_type>{});
}

}  // namespace tinystl

#endif  // TINYSTL_CONSTRUCT_H_