#ifndef TINYSTL_FUNCTOR_TEST_H_
#define TINYSTL_FUNCTOR_TEST_H_

// funtor_test : 用于测试仿函数的功能

#include <iostream>
#include "../TinySTL/functional.h"
#include "../TinySTL/vector.h"
#include "../TinySTL/algorithm.h"
#include "test.h"

namespace tinystl 
{

namespace test
{

namespace functor_test
{

void functor_test()
{
    std::cout << "[===============================================================]" << std::endl;
    std::cout << "[------------------------ Run functor test ---------------------]" << std::endl;

    tinystl::vector<int> v(10);
    tinystl::vector<int> v1{ 1,2,3,4,5,6,7,8,9,10 };
    tinystl::vector<int> v2{ 10,9,8,7,6,5,4,3,2,1 };
    tinystl::vector<int> v3{ 1,0,1,0,1,0,1,0,1,0 };
    tinystl::vector<pair<int, int>> v4{ {1,2}, {2,3}, {3,4}, {4,5}, {5,6} };

    // plus
    FUN_AFTER(v, tinystl::transform(
        v1.begin(), v1.end(), v.begin(), 
        tinystl::bind2nd(tinystl::plus<int>(), 1))
    );  // 2 3 4 5 6 7 8 9 10 11

    // minus
    FUN_AFTER(v, tinystl::transform(
        v1.begin(), v1.end(), v.begin(), 
        tinystl::bind2nd(tinystl::minus<int>(), 1))
    );  // 0 1 2 3 4 5 6 7 8 9

    // multiplies
    FUN_AFTER(v, tinystl::transform(
        v1.begin(), v1.end(), v.begin(), 
        tinystl::bind2nd(tinystl::multiplies<int>(), 2))
    );  // 2 4 6 8 10 12 14 16 18 20

    // divides
    FUN_AFTER(v, tinystl::transform(
        v1.begin(), v1.end(), v.begin(), 
        tinystl::bind2nd(tinystl::divides<int>(), 2))
    );  // 0 0 1 1 2 2 3 3 4 4

    // modulus
    FUN_AFTER(v, tinystl::transform(
        v1.begin(), v1.end(), v.begin(), 
        tinystl::bind2nd(tinystl::modulus<int>(), 2))
    );  // 1 0 1 0 1 0 1 0 1 0

    // negateate
    FUN_AFTER(v, tinystl::transform(
        v1.begin(), v1.end(), v.begin(), 
        tinystl::negate<int>())
    );  // -1 -2 -3 -4 -5 -6 -7 -8 -9 -10

    // equal_to
    v.clear();
    FUN_AFTER(v, tinystl::copy_if(
        v1.begin(), v1.end(), back_inserter(v),
        tinystl::bind2nd(tinystl::equal_to<int>(), 1))
    );  // 1

    // not_equal_to
    v.clear();
    FUN_AFTER(v, tinystl::copy_if(
        v1.begin(), v1.end(), back_inserter(v),
        tinystl::bind2nd(tinystl::not_equal_to<int>(), 1))
    );  // 2 3 4 5 6 7 8 9 10

    // greater
    v.clear();
    FUN_AFTER(v, tinystl::copy_if(
        v1.begin(), v1.end(), back_inserter(v),
        tinystl::bind2nd(tinystl::greater<int>(), 5))
    );  // 6 7 8 9 10

    // less
    v.clear();
    FUN_AFTER(v, tinystl::copy_if(
        v1.begin(), v1.end(), back_inserter(v),
        tinystl::bind2nd(tinystl::less<int>(), 5))
    );  // 1 2 3 4

    // greater_equal
    v.clear();
    FUN_AFTER(v, tinystl::copy_if(
        v1.begin(), v1.end(), back_inserter(v),
        tinystl::bind2nd(tinystl::greater_equal<int>(), 5))
    );  // 5 6 7 8 9 10

    // less_equal
    v.clear();
    FUN_AFTER(v, tinystl::copy_if(
        v1.begin(), v1.end(), back_inserter(v),
        tinystl::bind2nd(tinystl::less_equal<int>(), 5))
    );  // 1 2 3 4 5

    // logical_and
    v.clear();
    FUN_AFTER(v, tinystl::transform(
        v3.begin(), v3.end(), back_inserter(v), 
        tinystl::bind2nd(tinystl::logical_and<int>(), 1))
    );  // 1 0 1 0 1 0 1 0 1 0

    // logical_or
    v.clear();
    FUN_AFTER(v, tinystl::transform(
        v3.begin(), v3.end(), back_inserter(v),
        tinystl::bind2nd(tinystl::logical_or<int>(), 1))
    );  // 1 1 1 1 1 1 1 1 1 1 

    // logical_not
    FUN_AFTER(v, tinystl::transform(
        v3.begin(), v3.end(), v.begin(),
        tinystl::logical_not<int>())
    );  // 0 1 0 1 0 1 0 1 0 1

    // identity
    FUN_AFTER(v, tinystl::transform(
        v1.begin(), v1.end(), v.begin(),
        tinystl::identity<int>())
    );  // 1 2 3 4 5 6 7 8 9 10

    // select1st
    v.clear();
    FUN_AFTER(v, tinystl::transform(
        v4.begin(), v4.end(), back_inserter(v),
        tinystl::select1st<pair<int, int>>())
    );  // 1 2 3 4 5

    // select2nd
    v.clear();
    FUN_AFTER(v, tinystl::transform(
        v4.begin(), v4.end(), back_inserter(v),
        tinystl::select2nd<pair<int, int>>())
    );  // 2 3 4 5 6

    // project1st
    v.clear();
    FUN_AFTER(v, tinystl::transform(
        v1.begin(), v1.end(), back_inserter(v),
        tinystl::bind2nd(tinystl::project1st<int, int>(), 1))
    );  // 1 2 3 4 5 6 7 8 9 10

    // project2nd
    v.clear();
    FUN_AFTER(v, tinystl::transform(
        v1.begin(), v1.end(), back_inserter(v),
        tinystl::bind1st(tinystl::project2nd<int, int>(), 1))
    );  // 1 2 3 4 5 6 7 8 9 10

    std::cout << "[----------------------- End functor test ----------------------]" << std::endl;

}

}  // namespace functor_test

}  // namespace test

}  // namespace tinystl

#endif // !TINYSTL_FUNCTOR_TEST_H_