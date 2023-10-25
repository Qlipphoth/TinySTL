#ifndef TINYSTL_FUNCTIONAL_H
#define TINYSTL_FUNCTIONAL_H

// 这个头文件包含了 TINYSTL 的仿函数与哈希函数

#include <cstddef>

namespace tinystl {

// 定义一元函数的参数型别和返回值型别
template <class Arg, class Result>
struct unary_function {
    typedef Arg       argument_type;
    typedef Result    result_type;
};

// 定义二元函数的参数型别的返回值型别
template <class Arg1, class Arg2, class Result>
struct binary_function {
    typedef Arg1      first_argument_type;
    typedef Arg2      second_argument_type;
    typedef Result    result_type;
};


// ====================================== Arithmetic Functor ====================================== //

/// @brief 加法仿函数
/// @tparam T 
template <class T>
struct plus: public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x + y; }
};

/// @brief 减法仿函数
/// @tparam T
template <class T>
struct minus: public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x - y; }
};

/// @brief 乘法仿函数
/// @tparam T
template <class T>
struct multiplies: public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x * y; }
};

/// @brief 除法仿函数
/// @tparam T
template <class T>
struct divides: public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x / y; }
};

/// @brief 取模仿函数
/// @tparam T
template <class T>
struct modulus: public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x % y; }
};

/// @brief 取反仿函数
/// @tparam T
template <class T>
struct negate: public unary_function<T, T> {
    T operator()(const T& x) const { return -x; }
};

/// @brief 证同元素，对于加法，返回 0
template <class T>
T identity_element(plus<T>) { return T(0); }

/// @brief 证同元素，对于乘法，返回 1
template <class T>
T identity_element(multiplies<T>) { return T(1); }


// ====================================== Relational Functor ====================================== //

/// @brief 等于仿函数
template <class T>
struct equal_to: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x == y; }
};

/// @brief 不等于仿函数
template <class T>
struct not_equal_to: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x != y; }
};

/// @brief 大于仿函数
template <class T>
struct greater: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x > y; }
};

/// @brief 大于等于仿函数
template <class T>
struct greater_equal: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x >= y; }
};

/// @brief 小于仿函数
template <class T>
struct less: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x < y; }
};

/// @brief 小于等于仿函数
template <class T>
struct less_equal: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x <= y; }
};


// ====================================== Logical Functor ====================================== //

/// @brief 逻辑与仿函数
template <class T>
struct logical_and: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x && y; }
};

/// @brief 逻辑或仿函数
template <class T>
struct logical_or: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x || y; }
};

/// @brief 逻辑非仿函数
template <class T>
struct logical_not: public unary_function<T, bool> {
    bool operator()(const T& x) const { return !x; }
};


// ================================ identity、select、project =============================== //

/// @brief 证同仿函数，返回原值
template <class T>
struct identity: public unary_function<T, T> {
    const T& operator()(const T& x) const { return x; }
};

/// @brief 选择仿函数，返回第一个参数
template <class Pair>
struct select1st: public unary_function<Pair, typename Pair::first_type> {
    const typename Pair::first_type& operator()(const Pair& x) const { return x.first; }
};

/// @brief 选择仿函数，返回第二个参数
template <class Pair>
struct select2nd: public unary_function<Pair, typename Pair::second_type> {
    const typename Pair::second_type& operator()(const Pair& x) const { return x.second; }
};

/// @brief 投射仿函数，返回第一个参数
template <class Arg1, class Arg2>
struct project1st: public binary_function<Arg1, Arg2, Arg1> {
    Arg1 operator()(const Arg1& x, const Arg2&) const { return x; }
};

/// @brief 投射仿函数，返回第二个参数
template <class Arg1, class Arg2>
struct project2nd: public binary_function<Arg1, Arg2, Arg2> {
    Arg2 operator()(const Arg1&, const Arg2& y) const { return y; }
};


// ==================================== Function Adapters ==================================== //

/// @brief 仿函数配接器，用于将一个一元谓词取反
/// @tparam Predicate 
template <class Predicate>
class unary_negate: public unary_function<typename Predicate::argument_type, bool> {
protected:
    Predicate pred;  // 将谓词作为成员变量，就能对其运算结果进行操作了
public:
    explicit unary_negate(const Predicate& x): pred(x) {}
    bool operator()(const typename Predicate::argument_type& x) const { return !pred(x); }
};

/// @brief 辅助函数，用于生成一个 unary_negate 对象
template <class Predicate>
inline unary_negate<Predicate> not1(const Predicate& pred) {
    return unary_negate<Predicate>(pred);
}


/// @brief 仿函数配接器，用于将一个二元谓词取反
/// @tparam Predicate
template <class Predicate>
class binary_negate: public binary_function<typename Predicate::first_argument_type,
                                            typename Predicate::second_argument_type,
                                            bool> {
protected:
    Predicate pred;
public:
    explicit binary_negate(const Predicate& x): pred(x) {}
    bool operator()(const typename Predicate::first_argument_type& x,
                    const typename Predicate::second_argument_type& y) const {
        return !pred(x, y);
    }
};


/// @brief 辅助函数，用于生成一个 binary_negate 对象
template <class Predicate>
inline binary_negate<Predicate> not2(const Predicate& pred) {
    return binary_negate<Predicate>(pred);
}

/// @brief 仿函数配接器，用于将一个二元仿函数的第一个参数绑定为一个固定值，使其变成一元仿函数
/// @tparam Operation 
template <class Operation>
class binder1st: public unary_function<typename Operation::second_argument_type,
                                       typename Operation::result_type> {
protected:
    Operation op;  // 将二元仿函数作为成员变量
    typename Operation::first_argument_type value;  // 绑定的值
public:
    binder1st(const Operation& x, const typename Operation::first_argument_type& y)
        : op(x), value(y) {}
    typename Operation::result_type operator()(const typename Operation::second_argument_type& x) const {
        return op(value, x);
    }
};

/// @brief 辅助函数，将二元仿函数的第一个参数绑定为一个固定值，使其变成一元仿函数
/// @param op 二元仿函数
/// @param x  绑定的值
/// @return  返回一个一元仿函数
template <class Operation, class T>
inline binder1st<Operation> bind1st(const Operation& op, const T& x) {
    typedef typename Operation::first_argument_type arg1_type;
    return binder1st<Operation>(op, arg1_type(x));
}


/// @brief 仿函数配接器，用于将一个二元仿函数的第二个参数绑定为一个固定值，使其变成一元仿函数
/// @tparam Operation 
template <class Operation>
class binder2nd: public unary_function<typename Operation::first_argument_type,
                                       typename Operation::result_type> {
protected:
    Operation op;
    typename Operation::second_argument_type value;
public:
    binder2nd(const Operation& x, const typename Operation::second_argument_type& y)
        : op(x), value(y) {}
    typename Operation::result_type operator()(const typename Operation::first_argument_type& x) const {
        return op(x, value);
    }
};

/// @brief 辅助函数，将二元仿函数的第二个参数绑定为一个固定值，使其变成一元仿函数
/// @param op 二元仿函数
/// @param x  绑定的值
template <class Operation, class T>
inline binder2nd<Operation> bind2nd(const Operation& op, const T& x) {
    typedef typename Operation::second_argument_type arg2_type;
    return binder2nd<Operation>(op, arg2_type(x));
}


/// @brief 仿函数配接器，产生一个仿函数 f(x) = op1(op2(x))
/// @tparam Operation1
/// @tparam Operation2
template <class Operation1, class Operation2>
class unary_compose: public unary_function<typename Operation2::argument_type,
                                           typename Operation1::result_type> {
protected:
    Operation1 op1;
    Operation2 op2;
public:
    unary_compose(const Operation1& x, const Operation2& y): op1(x), op2(y) {}
    typename Operation1::result_type operator()(const typename Operation2::argument_type& x) const {
        return op1(op2(x));
    }
};

/// @brief 辅助函数，产生一个仿函数 f(x) = op1(op2(x))
template <class Operation1, class Operation2>
inline unary_compose<Operation1, Operation2> compose1(const Operation1& op1, const Operation2& op2) {
    return unary_compose<Operation1, Operation2>(op1, op2);
}


/// @brief 仿函数配接器，产生一个仿函数 f(x) = op1(op2(x), op3(x))
/// @tparam Operation1
/// @tparam Operation2
/// @tparam Operation3
template <class Operation1, class Operation2, class Operation3>
class binary_compose: public unary_function<typename Operation2::argument_type,
                                            typename Operation1::result_type> {
protected:
    Operation1 op1;
    Operation2 op2;
    Operation3 op3;
public:
    binary_compose(const Operation1& x, const Operation2& y, const Operation3& z)
        : op1(x), op2(y), op3(z) {}
    typename Operation1::result_type operator()(const typename Operation2::argument_type& x) const {
        return op1(op2(x), op3(x));
    }
};

/// @brief 辅助函数，产生一个仿函数 f(x) = op1(op2(x), op3(x))
template <class Operation1, class Operation2, class Operation3>
inline binary_compose<Operation1, Operation2, Operation3> 
compose2(const Operation1& op1, const Operation2& op2, const Operation3& op3) {
    return binary_compose<Operation1, Operation2, Operation3>(op1, op2, op3);
}


// ======================================= ptr_fun ======================================= //

/// @brief 函数配接器，将一个一元函数包装为仿函数
/// @tparam Arg 一元函数的参数类型
/// @tparam Result 一元函数的返回值类型
template <class Arg, class Result>
class pointer_to_unary_function: public unary_function<Arg, Result> {
protected:
    Result (*ptr)(Arg);  // 函数指针
public:
    pointer_to_unary_function() {}
    explicit pointer_to_unary_function(Result (*x)(Arg)): ptr(x) {}
    Result operator()(Arg x) const { return ptr(x); }
};

/// @brief 将一个一元函数包装为仿函数
/// @param x  一元函数
/// @return  仿函数
template <class Arg, class Result>
inline pointer_to_unary_function<Arg, Result> ptr_fun(Result (*x)(Arg)) {
    return pointer_to_unary_function<Arg, Result>(x);
}


/// @brief 函数配接器，将一个二元函数包装为仿函数
/// @tparam Arg1 二元函数的第一个参数类型
/// @tparam Arg2 二元函数的第二个参数类型
/// @tparam Result 二元函数的返回值类型
template <class Arg1, class Arg2, class Result>
class pointer_to_binary_function: public binary_function<Arg1, Arg2, Result> {
protected:
    Result (*ptr)(Arg1, Arg2);
public:
    pointer_to_binary_function() {}
    explicit pointer_to_binary_function(Result (*x)(Arg1, Arg2)): ptr(x) {}
    Result operator()(Arg1 x, Arg2 y) const { return ptr(x, y); }
};

/// @brief 将一个二元函数包装为仿函数
/// @param x  二元函数
/// @return  仿函数
template <class Arg1, class Arg2, class Result>
inline pointer_to_binary_function<Arg1, Arg2, Result> ptr_fun(Result (*x)(Arg1, Arg2)) {
    return pointer_to_binary_function<Arg1, Arg2, Result>(x);
}


// ======================================= mem_fun ===================================== //

/// @brief 无任何参数，通过指针调用，non-const 成员函数
/// @tparam S 返回值类型
/// @tparam T 成员函数所属的类
template <class S, class T>
class mem_fun_t: public unary_function<T*, S> {
public:
    explicit mem_fun_t(S (T::*pf)()): f(pf) {}
    S operator()(T* p) const { return (p->*f)(); }  // 转调用
private:
    S (T::*f)();  // 内部成员，指向成员函数
};


/// @brief 无任何参数，通过指针调用，const 成员函数
/// @tparam S 返回值类型
/// @tparam T 成员函数所属的类
template <class S, class T>
class const_mem_fun_t: public unary_function<const T*, S> {
public:
    explicit const_mem_fun_t(S (T::*pf)() const): f(pf) {}
    S operator()(const T* p) const { return (p->*f)(); }
private:
    S (T::*f)() const;
};


/// @brief 无任何参数，通过引用调用，non-const 成员函数
/// @tparam S 返回值类型
/// @tparam T 成员函数所属的类
template <class S, class T>
class mem_fun_ref_t: public unary_function<T, S> {
public:
    explicit mem_fun_ref_t(S (T::*pf)()): f(pf) {}
    S operator()(T& r) const { return (r.*f)(); }
private:
    S (T::*f)();
};


/// @brief 无任何参数，通过引用调用，const 成员函数
/// @tparam S 返回值类型
/// @tparam T 成员函数所属的类
template <class S, class T>
class const_mem_fun_ref_t: public unary_function<T, S> {
public:
    explicit const_mem_fun_ref_t(S (T::*pf)() const): f(pf) {}
    S operator()(const T& r) const { return (r.*f)(); }
private:
    S (T::*f)() const;
};


/// @brief 一个参数，通过指针调用，non-const 成员函数
/// @tparam S 返回值类型
/// @tparam T 成员函数所属的类
/// @tparam A 参数类型
template <class S, class T, class A>
class mem_fun1_t: public binary_function<T*, A, S> {
public:
    explicit mem_fun1_t(S (T::*pf)(A)): f(pf) {}
    S operator()(T* p, A x) const { return (p->*f)(x); }
private:
    S (T::*f)(A);
};


/// @brief 一个参数，通过指针调用，const 成员函数
/// @tparam S 返回值类型
/// @tparam T 成员函数所属的类
/// @tparam A 参数类型
template <class S, class T, class A>
class const_mem_fun1_t: public binary_function<const T*, A, S> {
public:
    explicit const_mem_fun1_t(S (T::*pf)(A) const): f(pf) {}
    S operator()(const T* p, A x) const { return (p->*f)(x); }
private:
    S (T::*f)(A) const;
};


/// @brief 一个参数，通过引用调用，non-const 成员函数
/// @tparam S 返回值类型
/// @tparam T 成员函数所属的类
/// @tparam A 参数类型
template <class S, class T, class A>
class mem_fun1_ref_t: public binary_function<T, A, S> {
public:
    explicit mem_fun1_ref_t(S (T::*pf)(A)): f(pf) {}
    S operator()(T& r, A x) const { return (r.*f)(x); }
private:
    S (T::*f)(A);
};


/// @brief 一个参数，通过引用调用，const 成员函数
/// @tparam S 返回值类型
/// @tparam T 成员函数所属的类
/// @tparam A 参数类型
template <class S, class T, class A>
class const_mem_fun1_ref_t: public binary_function<T, A, S> {
public:
    explicit const_mem_fun1_ref_t(S (T::*pf)(A) const): f(pf) {}
    S operator()(const T& r, A x) const { return (r.*f)(x); }
private:
    S (T::*f)(A) const;
};


// 辅助函数

template <class S, class T>
inline mem_fun_t<S, T> mem_fun(S (T::*f)()) {
    return mem_fun_t<S, T>(f);
}

template <class S, class T>
inline const_mem_fun_t<S, T> mem_fun(S (T::*f)() const) {
    return const_mem_fun_t<S, T>(f);
}

template <class S, class T>
inline mem_fun_ref_t<S, T> mem_fun_ref(S (T::*f)()) {
    return mem_fun_ref_t<S, T>(f);
}

template <class S, class T>
inline const_mem_fun_ref_t<S, T> mem_fun_ref(S (T::*f)() const) {
    return const_mem_fun_ref_t<S, T>(f);
}

template <class S, class T, class A>
inline mem_fun1_t<S, T, A> mem_fun(S (T::*f)(A)) {
    return mem_fun1_t<S, T, A>(f);
}

template <class S, class T, class A>
inline const_mem_fun1_t<S, T, A> mem_fun(S (T::*f)(A) const) {
    return const_mem_fun1_t<S, T, A>(f);
}

template <class S, class T, class A>
inline mem_fun1_ref_t<S, T, A> mem_fun_ref(S (T::*f)(A)) {
    return mem_fun1_ref_t<S, T, A>(f);
}

template <class S, class T, class A>
inline const_mem_fun1_ref_t<S, T, A> mem_fun_ref(S (T::*f)(A) const) {
    return const_mem_fun1_ref_t<S, T, A>(f);
}


// ======================================= hash ======================================= //

// 对于大部分类型，hash function 什么都不做
template <class Key>
struct hash {};

// 针对指针的偏特化版本
template <class T>
struct hash<T*> {
    size_t operator()(T* p) const noexcept
    { return reinterpret_cast<size_t>(p); }
};

// 对于整型类型，只是返回原值
#define TINYSTL_TRIVIAL_HASH_FCN(Type)         \
template <> struct hash<Type> {                \
    size_t operator()(Type val) const noexcept \
    { return static_cast<size_t>(val); }       \
};

TINYSTL_TRIVIAL_HASH_FCN(bool)

TINYSTL_TRIVIAL_HASH_FCN(char)

TINYSTL_TRIVIAL_HASH_FCN(signed char)

TINYSTL_TRIVIAL_HASH_FCN(unsigned char)

TINYSTL_TRIVIAL_HASH_FCN(wchar_t)

TINYSTL_TRIVIAL_HASH_FCN(char16_t)

TINYSTL_TRIVIAL_HASH_FCN(char32_t)

TINYSTL_TRIVIAL_HASH_FCN(short)

TINYSTL_TRIVIAL_HASH_FCN(unsigned short)

TINYSTL_TRIVIAL_HASH_FCN(int)

TINYSTL_TRIVIAL_HASH_FCN(unsigned int)

TINYSTL_TRIVIAL_HASH_FCN(long)

TINYSTL_TRIVIAL_HASH_FCN(unsigned long)

TINYSTL_TRIVIAL_HASH_FCN(long long)

TINYSTL_TRIVIAL_HASH_FCN(unsigned long long)

#undef TINYSTL_TRIVIAL_HASH_FCN

// 对于浮点数，逐位哈希
inline size_t bitwise_hash(const unsigned char* first, size_t count) {
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)
    const size_t fnv_offset = 14695981039346656037ull;
    const size_t fnv_prime = 1099511628211ull;
#else
    const size_t fnv_offset = 2166136261u;
    const size_t fnv_prime = 16777619u;
#endif
    size_t result = fnv_offset;
    for (size_t i = 0; i < count; ++i) {
        result ^= (size_t)first[i];
        result *= fnv_prime;
    }
    return result;
}

template <>
struct hash<float> {
    size_t operator()(const float& val) { 
        return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(float));
    }
};

template <>
struct hash<double> {
    size_t operator()(const double& val) {
        return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(double));
    }
};

template <>
struct hash<long double> {
    size_t operator()(const long double& val) {
        return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(long double));
    }
};

}  // namespace tinystl

#endif //TINYSTL_FUNCTIONAL_H