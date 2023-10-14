#ifndef TINYSTL_UNORDERED_SET_TEST_H_
#define TINYSTL_UNORDERED_SET_TEST_H_

// unordered_set test : 测试 unordered_set, unordered_multiset 的接口与它们 insert 的性能

#include <unordered_set>

#include "../TinySTL/unordered_set.h"
#include "set_test.h"
#include "test.h"

namespace tinystl {

namespace test {

namespace unordered_set_test {

void unordered_set_test() {
    std::cout << "[===============================================================]" << std::endl;
    std::cout << "[-------------- Run container test : unordered_set -------------]" << std::endl;
    std::cout << "[-------------------------- API test ---------------------------]" << std::endl;
    int a[] = { 5,4,3,2,1 };
    tinystl::unordered_set<int> us1;
    tinystl::unordered_set<int> us2(520);
    tinystl::unordered_set<int> us3(520, tinystl::hash<int>());
    tinystl::unordered_set<int> us4(520, tinystl::hash<int>(), tinystl::equal_to<int>());
    tinystl::unordered_set<int> us5(a, a + 5);
    tinystl::unordered_set<int> us6(a, a + 5, 100);
    tinystl::unordered_set<int> us7(a, a + 5, 100, tinystl::hash<int>());
    tinystl::unordered_set<int> us8(a, a + 5, 100, tinystl::hash<int>(), tinystl::equal_to<int>());
    tinystl::unordered_set<int> us9(us5);
    tinystl::unordered_set<int> us10(std::move(us5));
    tinystl::unordered_set<int> us11;
    us11 = us6;
    tinystl::unordered_set<int> us12;
    us12 = std::move(us6);
    tinystl::unordered_set<int> us13{ 1,2,3,4,5 };
    tinystl::unordered_set<int> us14;
    us13 = { 1,2,3,4,5 };

    FUN_AFTER(us1, us1.emplace(1));
    FUN_AFTER(us1, us1.emplace_hint(us1.end(), 2));
    FUN_AFTER(us1, us1.insert(5));
    FUN_AFTER(us1, us1.insert(us1.begin(), 5));
    FUN_AFTER(us1, us1.insert(a, a + 5));
    FUN_AFTER(us1, us1.erase(us1.begin()));
    FUN_AFTER(us1, us1.erase(us1.begin(), us1.find(3)));
    FUN_AFTER(us1, us1.erase(1));
    std::cout << std::boolalpha;
    FUN_VALUE(us1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(us1.size());
    FUN_VALUE(us1.bucket_count());
    FUN_VALUE(us1.max_bucket_count());
    FUN_VALUE(us1.bucket(1));
    FUN_VALUE(us1.bucket_size(us1.bucket(5)));
    FUN_AFTER(us1, us1.clear());
    FUN_AFTER(us1, us1.swap(us7));
    FUN_VALUE(*us1.begin());
    std::cout << std::boolalpha;
    FUN_VALUE(us1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(us1.size());
    FUN_VALUE(us1.max_size());
    FUN_VALUE(us1.bucket_count());
    FUN_AFTER(us1, us1.reserve(1000));
    // FUN_VALUE(*us1.begin(us1.bucket(1)));
    FUN_VALUE(us1.size());
    FUN_VALUE(us1.bucket_count());
    FUN_VALUE(us1.bucket_size(1));
    FUN_VALUE(us1.bucket_size(2));
    FUN_VALUE(us1.bucket_size(3));
    FUN_AFTER(us1, us1.rehash(150));
    FUN_VALUE(us1.bucket_count());
    FUN_VALUE(us1.count(1));
    FUN_VALUE(*us1.find(3));
    auto first = *us1.equal_range(3).first;
    auto second = *us1.equal_range(3).second;
    std::cout << " us1.equal_range(3) : from " << first << " to " << second << std::endl;
    FUN_VALUE(us1.load_factor());
    FUN_VALUE(us1.max_load_factor());
    FUN_AFTER(us1, us1.max_load_factor(1.5f));
    FUN_VALUE(us1.max_load_factor());
    PASSED;
#if PERFORMANCE_TEST_ON
    std::cout << "[--------------------- Performance Testing ---------------------]" << std::endl;
    std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
    std::cout << "|       emplace       |";
#if LARGER_TEST_DATA_ON
    CON_TEST_P1(unordered_set<int>, emplace, rand(), SCALE_L(LEN1), SCALE_L(LEN2), SCALE_L(LEN3));
#else
    CON_TEST_P1(unordered_set<int>, emplace, rand(), SCALE_M(LEN1), SCALE_M(LEN2), SCALE_M(LEN3));
#endif
    std::cout << std::endl;
    std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
    PASSED;
#endif
    std::cout << "[-------------- End container test : unordered_set -------------]" << std::endl;
}

void unordered_multiset_test() {
    std::cout << "[===============================================================]" << std::endl;
    std::cout << "[------------ Run container test : unordered_multiset ----------]" << std::endl;
    std::cout << "[-------------------------- API test ---------------------------]" << std::endl;
    int a[] = { 5,4,3,2,1 };
    tinystl::unordered_multiset<int> us1;
    tinystl::unordered_multiset<int> us2(520);
    tinystl::unordered_multiset<int> us3(520, tinystl::hash<int>());
    tinystl::unordered_multiset<int> us4(520, tinystl::hash<int>(), tinystl::equal_to<int>());
    tinystl::unordered_multiset<int> us5(a, a + 5);
    tinystl::unordered_multiset<int> us6(a, a + 5, 100);
    tinystl::unordered_multiset<int> us7(a, a + 5, 100, tinystl::hash<int>());
    tinystl::unordered_multiset<int> us8(a, a + 5, 100, tinystl::hash<int>(), tinystl::equal_to<int>());
    tinystl::unordered_multiset<int> us9(us5);
    tinystl::unordered_multiset<int> us10(std::move(us5));
    tinystl::unordered_multiset<int> us11;
    us11 = us6;
    tinystl::unordered_multiset<int> us12;
    us12 = std::move(us6);
    tinystl::unordered_multiset<int> us13{ 1,2,3,4,5 };
    tinystl::unordered_multiset<int> us14;
    us14 = { 1,2,3,4,5 };

    FUN_AFTER(us1, us1.emplace(1));
    FUN_AFTER(us1, us1.emplace_hint(us1.end(), 2));
    FUN_AFTER(us1, us1.insert(5));
    FUN_AFTER(us1, us1.insert(us1.begin(), 5));
    FUN_AFTER(us1, us1.insert(a, a + 5));
    FUN_AFTER(us1, us1.erase(us1.begin()));
    FUN_AFTER(us1, us1.erase(us1.begin(), us1.find(3)));
    FUN_AFTER(us1, us1.erase(1));
    std::cout << std::boolalpha;
    FUN_VALUE(us1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(us1.size());
    FUN_VALUE(us1.bucket_count());
    FUN_VALUE(us1.max_bucket_count());
    FUN_VALUE(us1.bucket(1));
    FUN_VALUE(us1.bucket_size(us1.bucket(5)));
    FUN_AFTER(us1, us1.clear());
    FUN_AFTER(us1, us1.swap(us7));
    FUN_VALUE(*us1.begin());
    std::cout << std::boolalpha;
    FUN_VALUE(us1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(us1.size());
    FUN_VALUE(us1.max_size());
    FUN_VALUE(us1.bucket_count());
    FUN_AFTER(us1, us1.reserve(1000));
    // FUN_VALUE(*us1.begin(us1.bucket(1)));
    FUN_VALUE(us1.size());
    FUN_VALUE(us1.bucket_count());
    FUN_VALUE(us1.bucket_size(1));
    FUN_VALUE(us1.bucket_size(2));
    FUN_VALUE(us1.bucket_size(3));
    FUN_AFTER(us1, us1.rehash(150));
    FUN_VALUE(us1.bucket_count());
    FUN_VALUE(us1.count(1));
    FUN_VALUE(*us1.find(3));
    auto first = *us1.equal_range(3).first;
    auto second = *us1.equal_range(3).second;
    std::cout << " us1.equal_range(3) : from " << first << " to " << second << std::endl;
    FUN_VALUE(us1.load_factor());
    FUN_VALUE(us1.max_load_factor());
    FUN_AFTER(us1, us1.max_load_factor(1.5f));
    FUN_VALUE(us1.max_load_factor());
    PASSED;
#if PERFORMANCE_TEST_ON
    std::cout << "[--------------------- Performance Testing ---------------------]" << std::endl;
    std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
    std::cout << "|       emplace       |";
#if LARGER_TEST_DATA_ON
    CON_TEST_P1(unordered_multiset<int>, emplace, rand(), SCALE_M(LEN1), SCALE_M(LEN2), SCALE_M(LEN3));
#else
    CON_TEST_P1(unordered_multiset<int>, emplace, rand(), SCALE_S(LEN1), SCALE_S(LEN2), SCALE_S(LEN3));
#endif
    std::cout << std::endl;
    std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
    PASSED;
#endif
    std::cout << "[------------ End container test : unordered_multiset ----------]" << std::endl;
}

} // namespace unordered_set_test

} // namespace test

} // namespace tinystl

#endif // !TINYSTL_UNORDERED_SET_TEST_H_

