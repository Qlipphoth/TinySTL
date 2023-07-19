#ifndef TINYSTL_EXCEPTDEF_H_
#define TINYSTL_EXCEPTDEF_H_

// 这个头文件定义了一些异常处理的宏

#include <stdexcept>
#include <cassert>

namespace tinystl {

    #define TINYSTL_DEBUG(expr) assert(expr)               // 断言
    
    #define THROW_LENGTH_ERROR_IF(expr, what) if ((expr)) throw std::length_error(what)          // 抛出长度错误
    
    #define THROW_OUT_OF_RANGE_IF(expr, what) if ((expr)) throw std::out_of_range(what)          // 抛出越界错误
    
    #define THROW_RUNTIME_ERROR_IF(expr, what) if ((expr)) throw std::runtime_error(what)         // 抛出运行时错误

}

#endif