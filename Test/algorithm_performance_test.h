#ifndef TINYSTL_ALGORITHM_PERFORMANCE_TEST_H_
#define TINYSTL_ALGORITHM_PERFORMANCE_TEST_H_

// 仅仅针对 sort, binary_search 做了性能测试

#include <algorithm>

#include "../TinySTL/algorithm.h"
#include "test.h"

namespace tinystl
{
namespace test
{
namespace algorithm_performance_test
{

// 函数性能测试宏定义
#define TEST_SORT(mode, count) do {                            \
    std::string fun_name = "sort";                             \
    srand((int)time(0));                                       \
    char buf[10];                                              \
    clock_t start, end;                                        \
    int *arr = new int[count];                                 \
    for(size_t i = 0; i < count; ++i)  *(arr + i) = rand();    \
    start = clock();                                           \
    mode::sort(arr, arr + count);                              \
    end = clock();                                             \
    int n = static_cast<int>(static_cast<double>(end - start)  \
        / CLOCKS_PER_SEC * 1000);                              \
    std::snprintf(buf, sizeof(buf), "%d", n);                  \
    std::string t = buf;                                       \
    t += "ms   |";                                             \
    std::cout << std::setw(WIDE) << t;                         \
    delete []arr;                                              \
} while(0)

#define TEST_BI_SEARCH(mode, count) do {                       \
    std::string fun_name = "bi_Search";                        \
    srand((int)time(0));                                       \
    char buf[10];                                              \
    clock_t start, end;                                        \
    int *arr = new int[count];                                 \
    for(size_t i = 0; i < count; ++i)  *(arr + i) = rand();    \
    start = clock();                                           \
    for(size_t i = 0; i < count; ++i)                          \
        mode::binary_search(arr, arr + count, rand());         \
    end = clock();                                             \
    int n = static_cast<int>(static_cast<double>(end - start)  \
        / CLOCKS_PER_SEC * 1000);                              \
    std::snprintf(buf, sizeof(buf), "%d", n);                  \
    std::string t = buf;                                       \
    t += "ms   |";                                             \
    std::cout << std::setw(WIDE) << t;                         \
    delete []arr;                                              \
} while(0)

#define TEST_EQUAL_RANGE(mode, len, count) do {                 \
  srand((int)time(0));                                          \
  clock_t start, end;                                           \
  char buf[10];                                                 \
  mode::vector<int> v(len);                                     \
  mode::iota(v.begin(), v.end(), 1);                            \
  start = clock();                                              \
  for (int i = 0; i < count; ++i) {                             \
    int target = rand() % len + 1;                              \
    auto range = mode::equal_range(v.begin(), v.end(), target); \
  }                                                             \
  end = clock();                                                \
  int n = static_cast<int>(static_cast<double>(end - start)     \
      / CLOCKS_PER_SEC * 1000);                                 \
  std::snprintf(buf, sizeof(buf), "%d", n);                     \
  std::string t = buf;                                          \
  t += "ms    |";                                               \
  std::cout << std::setw(WIDE) << t;                            \
} while (0)

#define TEST_ROTATE(mode, len, count) do {                      \
  srand((int)time(0));                                          \
  clock_t start, end;                                           \
  char buf[10];                                                 \
  mode::vector<int> v(len);                                     \
  mode::iota(v.begin(), v.end(), 1);                            \
  start = clock();                                              \
  for (int i = 0; i < count; ++i) {                             \
    int middle = rand() % len + 1;                              \
    mode::rotate(v.begin(), v.begin() + middle, v.end());       \
  }                                                             \
  end = clock();                                                \
  int n = static_cast<int>(static_cast<double>(end - start)     \
      / CLOCKS_PER_SEC * 1000);                                 \
  std::snprintf(buf, sizeof(buf), "%d", n);                     \
  std::string t = buf;                                          \
  t += "ms    |";                                               \
  std::cout << std::setw(WIDE) << t;                            \
} while (0)

void sort_test()
{
  std::cout << "[----------------------- function : sort -----------------------]" << std::endl;
  std::cout << "| orders of magnitude |";
  TEST_LEN(LEN1, LEN2, LEN3, WIDE);
  std::cout << "|         std         |";
  TEST_SORT(std, LEN1);
  TEST_SORT(std, LEN2);
  TEST_SORT(std, LEN3);
  std::cout << std::endl << "|       tinystl       |";
  TEST_SORT(tinystl, LEN1);
  TEST_SORT(tinystl, LEN2);
  TEST_SORT(tinystl, LEN3);
  std::cout << std::endl;
}

void binary_search_test()
{
  std::cout << "[------------------- function : binary_search ------------------]" << std::endl;
  std::cout << "| orders of magnitude |";
  TEST_LEN(LEN1, LEN2, LEN3, WIDE);
  std::cout << "|         std         |";
  TEST_BI_SEARCH(std, LEN1);
  TEST_BI_SEARCH(std, LEN2);
  TEST_BI_SEARCH(std, LEN3);
  std::cout << std::endl << "|       tinystl       |";
  TEST_BI_SEARCH(tinystl, LEN1);
  TEST_BI_SEARCH(tinystl, LEN2);
  TEST_BI_SEARCH(tinystl, LEN3);
  std::cout << std::endl;
}

void equal_range_test()
{
  std::cout << "[------------------- function : equal_range --------------------]" << std::endl;
  std::cout << "| orders of magnitude |";
  TEST_LEN(LEN1, LEN2, LEN3, WIDE);
  std::cout << "|         std         |";
  TEST_EQUAL_RANGE(std, LEN1, LEN1);
  TEST_EQUAL_RANGE(std, LEN2, LEN2);
  TEST_EQUAL_RANGE(std, LEN3, LEN3);
  std::cout << std::endl << "|       tinystl       |";
  TEST_EQUAL_RANGE(tinystl, LEN1, LEN1);
  TEST_EQUAL_RANGE(tinystl, LEN2, LEN2);
  TEST_EQUAL_RANGE(tinystl, LEN3, LEN3);
  std::cout << std::endl;
}

void rotate_test()
{
  std::cout << "[---------------------- function : rotate ----------------------]" << std::endl;
  std::cout << "| orders of magnitude |";
  TEST_LEN(LEN1, LEN2, LEN3, WIDE);
  std::cout << "|         std         |";
  TEST_ROTATE(std, SCALE_SSS(LEN1), SCALE_SSS(LEN1));
  TEST_ROTATE(std, SCALE_SSS(LEN2), SCALE_SSS(LEN1));
  TEST_ROTATE(std, SCALE_SSS(LEN3), SCALE_SSS(LEN1));
  std::cout << std::endl << "|       tinystl       |";
  TEST_ROTATE(tinystl, SCALE_SSS(LEN1), SCALE_SSS(LEN1));
  TEST_ROTATE(tinystl, SCALE_SSS(LEN2), SCALE_SSS(LEN1));
  TEST_ROTATE(tinystl, SCALE_SSS(LEN3), SCALE_SSS(LEN1));
  std::cout << std::endl;
}

void algorithm_performance_test()
{

#if PERFORMANCE_TEST_ON
  std::cout << "[===============================================================]" << std::endl;
  std::cout << "[--------------- Run algorithm performance test ----------------]" << std::endl;
  sort_test();
  binary_search_test();
  equal_range_test();
  rotate_test();
  std::cout << "[--------------- End algorithm performance test ----------------]" << std::endl;
  std::cout << "[===============================================================]" << std::endl;
#endif // PERFORMANCE_TEST_ON

}

} // namespace algorithm_performance_test

} // namespace test

} // namespace tinystl

#endif // !TINYSTL_ALGORITHM_PERFORMANCE_TEST_H_

