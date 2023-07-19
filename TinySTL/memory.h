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


}

#endif