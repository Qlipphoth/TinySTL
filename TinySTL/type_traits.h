#ifndef MYTINYSTL_TYPE_TRAITS_H_
#define MYTINYSTL_TYPE_TRAITS_H_

// 这个头文件用于提取类型信息

// use standard header for type_traits
#include <type_traits>

// SGI-STL 中， 所有的基本类型都是设置的为`__true_type`型别, 
// 而所有的对象都会被特化为`__false_type`型别, 这是为了保守而不得这样做. 
// 也因为`__true_type`型别让我们在执行普通类型时能够以最大效率进行对其的copy, 析构等操作.

namespace tinystl {

    struct __true_type {};
    struct __false_type {};

    



}


#endif