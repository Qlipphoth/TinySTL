#ifndef TINYSTL_VECTOR_TEST_H_
#define TINYSTL_VECTOR_TEST_H_

// vector test : 测试 vector 的接口与 push_back 的性能

#include <vector>

#include "../TinySTL/vector.h"
#include "test.h"

namespace tinystl
{
namespace test
{
namespace vector_test
{

void vector_test()
{
  std::cout << "[===============================================================]\n";
  std::cout << "[----------------- Run container test : vector -----------------]\n";
  std::cout << "[-------------------------- API test ---------------------------]\n";
  int a[] = { 1,2,3,4,5 };
  tinystl::vector<int> v1;
  tinystl::vector<int> v2(10);
  tinystl::vector<int> v3(10, 1);
  tinystl::vector<int> v4(a, a + 5);
  tinystl::vector<int> v5(v2);
  tinystl::vector<int> v6(std::move(v2));
  tinystl::vector<int> v7{ 1,2,3,4,5,6,7,8,9 };
  tinystl::vector<int> v8, v9, v10;
  v8 = v3;
  v9 = std::move(v3);
  v10 = { 1,2,3,4,5,6,7,8,9 };

  FUN_AFTER(v1, v1.assign(8, 8));                             // 8 8 8 8 8 8 8 8
  FUN_AFTER(v1, v1.assign(a, a + 5));                         // 1 2 3 4 5
  FUN_AFTER(v1, v1.emplace(v1.begin(), 0));                   // 0 1 2 3 4 5
  FUN_AFTER(v1, v1.emplace_back(6));                          // 0 1 2 3 4 5 6
  FUN_AFTER(v1, v1.push_back(6));                             // 0 1 2 3 4 5 6 6
  FUN_AFTER(v1, v1.insert(v1.end(), 7));                      // 0 1 2 3 4 5 6 6 7
  FUN_AFTER(v1, v1.insert(v1.begin() + 3, 2, 3));             // 0 1 2 3 3 3 4 5 6 6 7
  FUN_AFTER(v1, v1.insert(v1.begin(), a, a + 5));             // 1 2 3 4 5 0 1 2 3 3 3 4 5 6 6 7
  FUN_AFTER(v1, v1.pop_back());                               // 1 2 3 4 5 0 1 2 3 3 3 4 5 6 6
  FUN_AFTER(v1, v1.erase(v1.begin()));                        // 2 3 4 5 0 1 2 3 3 3 4 5 6 6
  FUN_AFTER(v1, v1.erase(v1.begin(), v1.begin() + 2));        // 4 5 0 1 2 3 3 3 4 5 6 6
  FUN_AFTER(v1, v1.reverse());                                // 6 6 5 4 3 3 3 2 1 0 5 4
  FUN_AFTER(v1, v1.swap(v4));                                 // 1 2 3 4 5
  FUN_VALUE(*v1.begin());                                     // 1
  FUN_VALUE(*(v1.end() - 1));                                 // 5 
  FUN_VALUE(*v1.rbegin());                                    // 5
  FUN_VALUE(*(v1.rend() - 1));                                // 1
  FUN_VALUE(v1.front());                                      // 1
  FUN_VALUE(v1.back());                                       // 5
  FUN_VALUE(v1[0]);                                           // 1
  FUN_VALUE(v1.at(1));                                        // 2

  // v1: 1 2 3 4 5
  int* p = v1.data();
  *p = 10;
  *++p = 20;
  p[1] = 30;
  std::cout << " After change v1.data() :" << "\n";          // 10 20 30 4 5
  COUT(v1);
  std::cout << std::boolalpha;
  FUN_VALUE(v1.empty());                                     // false
  std::cout << std::noboolalpha;
  FUN_VALUE(v1.size());                                      // 5
  FUN_VALUE(v1.max_size());                                  // 4611686018427387903  (2^62 - 1) 64 位系统 4 字节
  FUN_VALUE(v1.capacity());                                  // 16, 最小分配 16 个空间
  FUN_AFTER(v1, v1.resize(10));                              // 10 20 30 4 5 0 0 0 0 0
  FUN_VALUE(v1.size());                                      // 10
  FUN_VALUE(v1.capacity());                                  // 16
  FUN_AFTER(v1, v1.shrink_to_fit());                         // 10 20 30 4 5 0 0 0 0 0
  FUN_VALUE(v1.size());                                      // 10
  FUN_VALUE(v1.capacity());                                  // 10
  FUN_AFTER(v1, v1.resize(6, 6));                            // 10 20 30 4 5 0
  FUN_VALUE(v1.size());                                      // 6
  FUN_VALUE(v1.capacity());                                  // 10
  FUN_AFTER(v1, v1.shrink_to_fit());                         // 10 20 30 4 5 0
  FUN_VALUE(v1.size());                                      // 6
  FUN_VALUE(v1.capacity());                                  // 6
  FUN_AFTER(v1, v1.clear());                                 //
  FUN_VALUE(v1.size());                                      // 0
  FUN_VALUE(v1.capacity());                                  // 6   clear 只改变 size, 不改变 capacity
  FUN_AFTER(v1, v1.reserve(5));                              //
  FUN_VALUE(v1.size());                                      // 0
  FUN_VALUE(v1.capacity());                                  // 6
  FUN_AFTER(v1, v1.reserve(20));                             //
  FUN_VALUE(v1.size());                                      // 0
  FUN_VALUE(v1.capacity());                                  // 20
  FUN_AFTER(v1, v1.shrink_to_fit());                         //
  FUN_VALUE(v1.size());                                      // 0
  FUN_VALUE(v1.capacity());                                  // 0
  PASSED;

#if PERFORMANCE_TEST_ON
  std::cout << "[--------------------- Performance Testing ---------------------]\n";
  std::cout << "|---------------------|-------------|-------------|-------------|\n";
  std::cout << "|      push_back      |";
#if LARGER_TEST_DATA_ON
  CON_TEST_P1(vector<int>, push_back, rand(), SCALE_LL(LEN1), SCALE_LL(LEN2), SCALE_LL(LEN3));
#else
  CON_TEST_P1(vector<int>, push_back, rand(), SCALE_L(LEN1), SCALE_L(LEN2), SCALE_L(LEN3));
#endif
  std::cout << "\n";
  std::cout << "|---------------------|-------------|-------------|-------------|\n";
  PASSED;
#endif
  std::cout << "[----------------- End container test : vector -----------------]\n";

}

} // namespace vector_test
} // namespace test
} // namespace tinystl
#endif // !MYTINYSTL_VECTOR_TEST_H_

