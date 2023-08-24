#ifndef TINYSTL_ALGORITHM_TEST_H
#define TINYSTL_ALGORITHM_TEST_H

#include <algorithm>
#include <functional>
#include <numeric>

#include "algo.h"
#include "test.h"

namespace tinystl {

namespace test {

#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4389)
#endif // _MSC_VER

namespace algorithm_test {

TEST(equal_range_test) {
    int arr1[] = { 1,2,3,3,3,4,5 };
    auto p1 = tinystl::equal_range(arr1, arr1 + 7, 3);
    auto p2 = std::equal_range(arr1, arr1 + 7, 3);
    auto p3 = tinystl::equal_range(arr1, arr1 + 7, 6, std::equal_to<int>());
    auto p4 = std::equal_range(arr1, arr1 + 7, 6, std::equal_to<int>());
    EXPECT_EQ(p2.first, p1.first);
    EXPECT_EQ(p2.second, p1.second);
    EXPECT_EQ(p4.first, p3.first);
    EXPECT_EQ(p4.second, p3.second);
}

TEST(rotate_test) {
    int arr1[] = { 1,2,3,4,5,6,7,8,9 };
    int arr2[] = { 1,2,3,4,5,6,7,8,9 };
    std::rotate(arr1, arr1 + 3, arr1 + 9);
    tinystl::rotate(arr2, arr2 + 3, arr2 + 9);
    EXPECT_CON_EQ(arr1, arr2);
    std::rotate(arr1 + 3, arr1 + 5, arr1 + 9);
    tinystl::rotate(arr2 + 3, arr2 + 5, arr2 + 9);
    EXPECT_CON_EQ(arr1, arr2);
    std::rotate(arr1, arr1 + 9, arr1 + 9);
    tinystl::rotate(arr2, arr2 + 9, arr2 + 9);
  EXPECT_CON_EQ(arr1, arr2);
}

TEST(inplace_merge_test) {
    int arr1[] = { 1,3,5,7,9,2,4,6,8,10 };
    int arr2[] = { 1,3,5,7,9,2,4,6,8,10 };
    int arr3[] = { 1,2,3,1,2,3,4,5 };
    int arr4[] = { 1,2,3,1,2,3,4,5 };
    std::inplace_merge(arr1, arr1 + 5, arr1 + 10);
    tinystl::inplace_merge(arr2, arr2 + 5, arr2 + 10);
    std::inplace_merge(arr3, arr3 + 3, arr3 + 8, std::less<int>());
    tinystl::inplace_merge(arr4, arr4 + 3, arr4 + 8, std::less<int>());
    EXPECT_CON_EQ(arr1, arr2);
    EXPECT_CON_EQ(arr3, arr4);
}

}  // namespace algorithm_test

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

}  // namespace test

}  // namespace tinystl

#endif //TINYSTL_ALGORITHM_TEST_H