#ifndef TINYSTL_MEMORY_H_
#define TINYSTL_MEMORY_H_

// 这个头文件负责更高级的动态内存管理
// 包含一些基本函数、空间配置器、未初始化的储存空间管理，以及一个模板类 auto_ptr

#include <cstddef>
#include <cstdlib>
#include <climits>

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"


namespace tinystl {

/// @brief 获取对象地址
/// @tparam Tp  一个类型
/// @param value  一个对象
/// @return  返回对象的地址
template <class Tp>
constexpr Tp* address_of(Tp& value) noexcept {
    return &value;
}


// ======================================= temporary_buffer ======================================= //
// 类模板 temporary_buffer 
// 进行临时缓冲区的申请与释放

template <class ForwardIterator, class T>
class temporary_buffer {

private:
    ptrdiff_t original_len;  // 缓冲区大小
    ptrdiff_t len;           // 缓冲区大小
    T*        buffer;        // 缓冲区首地址

public:  // 构造、析构函数
    temporary_buffer(ForwardIterator first, ForwardIterator last);

    ~temporary_buffer() {
        tinystl::destroy(buffer, buffer + len);
        free(buffer);
    }

public:  // 访问元素相关操作
    ptrdiff_t size()           const noexcept { return len; }
    ptrdiff_t requested_size() const noexcept { return original_len; }
    T*        begin()          const noexcept { return buffer; }
    T*        end()            const noexcept { return buffer + len; }

private:  // 辅助函数
    void allocate_buffer();
    void initialize_buffer(const T&, std::true_type) {}
    void initialize_buffer(const T& value, std::false_type) {
        tinystl::uninitialized_fill_n(buffer, len, value);
    }

private:
    temporary_buffer(const temporary_buffer&) = delete;  // 禁止拷贝构造
    void operator=(const temporary_buffer&) = delete;    // 禁止赋值

};

// 构造函数

template <class ForwardIterator, class T>
temporary_buffer<ForwardIterator, T>::
temporary_buffer(ForwardIterator first, ForwardIterator last) {
    try {
        len = tinystl::distance(first, last);
        allocate_buffer();
        if (len > 0) {
            initialize_buffer(*first, std::is_trivially_default_constructible<T>());
        }
    } catch (...) {
        free(buffer);
        buffer = nullptr;
        len = 0;
    }
}

template <class ForwardIterator, class T>
void temporary_buffer<ForwardIterator, T>::allocate_buffer() {
    original_len = len;
    if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T))) {
        len = INT_MAX / sizeof(T);
    }
    while (len > 0) {
        buffer = static_cast<T*>(malloc(len * sizeof(T)));
        if (buffer) break;
        len /= 2;
    }
}


// ======================================= get_temporary_buffer ======================================= //

/// @brief 申请临时缓冲区
template <class T>
pair<T*, ptrdiff_t> get_buffer_helper(ptrdiff_t len, T*) {
    if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T))) {
        len = INT_MAX / sizeof(T);
    }
    while (len > 0) {
        T* tmp = static_cast<T*>(malloc(len * sizeof(T)));
        if (tmp) return pair<T*, ptrdiff_t>(tmp, len);
        len /= 2;  // 申请失败，减小申请空间
    }
    return pair<T*, ptrdiff_t>(nullptr, 0);
}

/// @brief 申请大小为 len，类型为 T 的临时缓冲区
template <class T>
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len) {
    return get_buffer_helper(len, static_cast<T*>(0));
}

/// @brief 针对原生指针的重载版本
template <class T>
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len, T*) {
    return get_buffer_helper(len, static_cast<T*>(0));
}

/// @brief 释放临时缓冲区
template <class T>
void release_temporary_buffer(T* ptr) {
    free(ptr);
}


}  // namespace tinystl

#endif  // TINYSTL_MEMORY_H_