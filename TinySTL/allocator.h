#ifndef TINYSTL_ALLOCATOR_H_
#define TINYSTL_ALLOCATOR_H_

// 标准空间配置器，只是对 new 和 delete 做了简单封装
// 简单起见，这里没有实现 set_new_handler 等异常处理机制

#include <cstddef>      // ptrdiff_t, size_t
#include <new>          // placement new
#include "construct.h"  // 构造和销毁对象
#include "util.h"       // forward, move

namespace tinystl {

template <class T>
class allocator {

public:
    // 根据 STL 规范，allocator 必须有以下的型别定义，详见 traits 部分的博客内容
    typedef T           value_type;       // 元素类型
    typedef T*          pointer;          // 指针
    typedef const T*    const_pointer;    // 常量指针
    typedef T&          reference;        // 引用
    typedef const T&    const_reference;  // 常量引用
    typedef size_t      size_type;        // 元素数量
    typedef ptrdiff_t   difference_type;  // 两个指针之间的距离

public:
    static T* allocate();  
    static T* allocate(size_type n);

    static void deallocate(T* ptr);
    static void deallocate(T* ptr, size_type n);
    
    static void construct(T* ptr);  // 默认构造
    static void construct(T* ptr, const T& value);  // 拷贝构造
    static void construct(T* ptr, T&& value);  // 移动构造
    
    template <class... Args>
    static void construct(T* ptr, Args&&... args);  // 带参数的构造

    static void destroy(T* ptr);
    static void destroy(T* first, T* last);    
};

/// @brief 分配一个 T 大小的内存
template <class T>
T* allocator<T>::allocate() {
    // 调用全局的 operator new, 分配一个 T 大小的内存并进行类型转换
    return static_cast<T*>(::operator new (sizeof(T)));
}

/// @brief 分配 n 个 T 大小的内存
template <class T>
T* allocator<T>::allocate(size_type n) {
    // 调用全局的 operator new, 分配 n 个 T 大小的内存并进行类型转换
    if (n == 0) return nullptr;
    return static_cast<T*>(::operator new (n * sizeof(T)));
}

/// @brief 释放 ptr 指向的内存
template <class T>
void allocator<T>::deallocate(T* ptr) {
    // 调用全局的 operator delete, 释放 ptr 指向的内存
    if (ptr == nullptr) return;
    ::operator delete(ptr);
}

/// @brief 释放 ptr 指向的内存
/// 实际上这里的 size 参数没有用到，只是为了与函数签名一致
template <class T>
void allocator<T>::deallocate(T* ptr, size_type /*size*/) {
    // 调用全局的 operator delete, 释放 ptr 指向的内存
    if (ptr == nullptr) return;
    ::operator delete(ptr);
}

/// @brief 默认构造一个对象
template <class T>
void allocator<T>::construct(T* ptr) {
    // 调用 construct.h 中的 construct 函数
    tinystl::construct(ptr);  // 默认构造
}

/// @brief 拷贝构造一个对象 
template <class T>
void allocator<T>::construct(T* ptr, const T& value) {
    tinystl::construct(ptr, value);  // 拷贝构造
}

/// @brief 移动构造一个对象
template <class T>
void allocator<T>::construct(T* ptr, T&& value) {
    // tinystl::move 将左值转换为右值引用, 详见 util.h
    tinystl::construct(ptr, tinystl::move(value));  // 移动构造
}

/// @brief 带参构造一个对象
template <class T>
template <class... Args>
void allocator<T>::construct(T* ptr, Args&&... args) {
    // tinystl::forward 用于完美转发，详见 util.h
    tinystl::construct(ptr, tinystl::forward<Args>(args)...);  // 带参数的构造
}

/// @brief 析构一个对象
template <class T>
void allocator<T>::destroy(T* ptr) {
    // 调用 construct.h 中的 destroy 函数
    tinystl::destroy(ptr);
}

/// @brief 析构两个指针之间的所有对象
template <class T>
void allocator<T>::destroy(T* first, T* last) {
    tinystl::destroy(first, last);
}

}  // namespace tinystl

#endif  // TINYSTL_ALLOCATOR_H_