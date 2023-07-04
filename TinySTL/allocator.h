#ifndef TINYSTL_ALLOCATOR_H_
#define TINYSTL_ALLOCATOR_H_

// 这个头文件包含一个模板类 allocator，用于管理内存的分配、释放，对象的构造、析构

// 涉及到的较重要知识点：(详见 note1.md)
// 1. new 与 delete 的具体操作
// 2. 移动复制与 std::move
// 3. 完美转发、通用引用、引用折叠与 std::forward

#include <cstddef>      // ptrdiff_t, size_t
#include <new>          // placement new
#include "construct.h"
#include "util.h"

namespace tinystl {

    template <class T>
    class allocator {
        public:
            typedef T           value_type;
            typedef T*          pointer;
            typedef const T*    const_pointer;
            typedef T&          reference;
            typedef const T&    const_reference;
            typedef size_t      size_type; 
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

    template <class T>
    T* allocator<T>::allocate() {
        // 调用全局的 operator new, 分配一个 T 大小的内存并进行类型转换
        return static_cast<T*>(::operator new (sizeof(T)));
    }

    template <class T>
    T* allocator<T>::allocate(size_type n) {
        // 调用全局的 operator new, 分配 n 个 T 大小的内存并进行类型转换
        if (n == 0) return nullptr;
        return static_cast<T*>(::operator new (n * sizeof(T)));
    }

    template <class T>
    void allocator<T>::deallocate(T* ptr) {
        // 调用全局的 operator delete, 释放 ptr 指向的内存
        if (ptr == nullptr) return;
        ::operator delete(ptr);
    }

    template <class T>
    void allocator<T>::deallocate(T* ptr, size_type /*size*/) {
        // 调用全局的 operator delete, 释放 ptr 指向的内存
        if (ptr == nullptr) return;
        ::operator delete(ptr);
    }

    template <class T>
    void allocator<T>::construct(T* ptr) {
        // 调用 construct.h 中的 construct 函数
        tinystl::construct(ptr);  // 默认构造
    }

    template <class T>
    void allocator<T>::construct(T* ptr, const T& value) {
        tinystl::construct(ptr, value);  // 拷贝构造
    }

    template <class T>
    void allocator<T>::construct(T* ptr, T&& value) {
        // tinystl::move 将左值转换为右值引用, 详见 util.h
        tinystl::construct(ptr, tinystl::move(value));  // 移动构造
    }

    template <class T>
    template <class... Args>
    void allocator<T>::construct(T* ptr, Args&&... args) {
        // tinystl::forward 用于完美转发，详见 util.h
        tinystl::construct(ptr, tinystl::forward<Args>(args)...);  // 带参数的构造
    }

    template <class T>
    /// @brief 析构一个对象
    void allocator<T>::destroy(T* ptr) {
        // 调用 construct.h 中的 destroy 函数
        tinystl::destroy(ptr);
    }

    template <class T>
    /// @brief 析构两个指针之间的所有对象
    void allocator<T>::destroy(T* first, T* last) {
        tinystl::destroy(first, last);
    }

}

#endif