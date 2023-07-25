#ifndef TINYSTL_DEQUE_TEST_H
#define TINYSTL_DEQUE_TEST_H

#include <deque>

#include "../TinySTL/deque.h"
#include "test.h"

namespace tinystl {

namespace test {

namespace deque_test {

    void deque_test() {
        std::cout << "[===============================================================]" << std::endl;
        std::cout << "[----------------- Run container test : deque ------------------]" << std::endl;
        std::cout << "[-------------------------- API test ---------------------------]" << std::endl;
        int a[] = { 1,2,3,4,5 };
        tinystl::deque<int> d1;
        tinystl::deque<int> d2(5);
        tinystl::deque<int> d3(5, 1);
        tinystl::deque<int> d4(a, a + 5);
        tinystl::deque<int> d5(d2);
        tinystl::deque<int> d6(std::move(d2));
        tinystl::deque<int> d7;
        d7 = d3;
        tinystl::deque<int> d8;
        d8 = std::move(d3);
        tinystl::deque<int> d9{ 1,2,3,4,5,6,7,8,9 };
        tinystl::deque<int> d10;
        d10 = { 1,2,3,4,5,6,7,8,9 };

        FUN_AFTER(d1, d1.assign(5, 1));                            // 1 1 1 1 1
        FUN_AFTER(d1, d1.assign(8, 9));                            // 9 9 9 9 9 9 9 9
        FUN_AFTER(d1, d1.assign(a, a + 5));                        // 1 2 3 4 5
        FUN_AFTER(d1, d1.assign({ 1,2,3,4,5 }));                   // 1 2 3 4 5
        FUN_AFTER(d1, d1.insert(d1.end(), 6));                     // 1 2 3 4 5 6
        FUN_AFTER(d1, d1.insert(d1.end() - 1, 2, 7));              // 1 2 3 4 5 7 7 6  
                                                                   // 出错变为 1 2 3 4 5 7 7 9
                                                                   // 见 deque.md

        FUN_AFTER(d1, d1.insert(d1.begin(), a, a + 5));            // 1 2 3 4 5 1 2 3 4 5 7 7 6
        FUN_AFTER(d1, d1.erase(d1.begin()));                       // 2 3 4 5 1 2 3 4 5 7 7 6
        FUN_AFTER(d1, d1.erase(d1.begin(), d1.begin() + 4));       // 1 2 3 4 5 7 7 6
        FUN_AFTER(d1, d1.emplace_back(8));                         // 1 2 3 4 5 7 7 6 8
        FUN_AFTER(d1, d1.emplace_front(8));                        // 8 1 2 3 4 5 7 7 6 8
        FUN_AFTER(d1, d1.emplace(d1.begin() + 1, 9));              // 8 9 1 2 3 4 5 7 7 6 8
        FUN_AFTER(d1, d1.push_front(1));                           // 1 8 9 1 2 3 4 5 7 7 6 8
        FUN_AFTER(d1, d1.push_back(2));                            // 1 8 9 1 2 3 4 5 7 7 6 8 2
        FUN_AFTER(d1, d1.pop_back());                              // 1 8 9 1 2 3 4 5 7 7 6 8
        FUN_AFTER(d1, d1.pop_front());                             // 8 9 1 2 3 4 5 7 7 6 8
        FUN_AFTER(d1, d1.shrink_to_fit());                         // 8 9 1 2 3 4 5 7 7 6 8
        FUN_AFTER(d1, d1.resize(5));                               // 8 9 1 2 3
        FUN_AFTER(d1, d1.resize(8, 8));                            // 8 9 1 2 3 8 8 8
        FUN_AFTER(d1, d1.clear());                                 //
        FUN_AFTER(d1, d1.shrink_to_fit());                         // 
        FUN_AFTER(d1, d1.swap(d4));                                // 1 2 3 4 5
        FUN_VALUE(*(d1.begin()));                                  // 1
        FUN_VALUE(*(d1.end() - 1));                                // 5
        FUN_VALUE(*(d1.rbegin()));                                 // 5
        FUN_VALUE(*(d1.rend() - 1));                               // 1
        FUN_VALUE(d1.front());                                     // 1
        FUN_VALUE(d1.back());                                      // 5
        FUN_VALUE(d1.at(1));                                       // 2
        FUN_VALUE(d1[2]);                                          // 3
        std::cout << std::boolalpha;
        FUN_VALUE(d1.empty());                                     // false
        std::cout << std::noboolalpha;
        FUN_VALUE(d1.size());                                      // 5
        FUN_VALUE(d1.max_size());                                  // 18446744073709551615
        PASSED;
        #if PERFORMANCE_TEST_ON
        std::cout << "[--------------------- Performance Testing ---------------------]" << std::endl;
        std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
        std::cout << "|     push_front      |";
        #if LARGER_TEST_DATA_ON
        CON_TEST_P1(deque<int>, push_front, rand(), SCALE_LL(LEN1), SCALE_LL(LEN2), SCALE_LL(LEN3));
        #else
        CON_TEST_P1(deque<int>, push_front, rand(), SCALE_L(LEN1), SCALE_L(LEN2), SCALE_L(LEN3));
        #endif
        std::cout << std::endl;
        std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
        std::cout << "|     push_back       |";
        #if LARGER_TEST_DATA_ON
        CON_TEST_P1(deque<int>, push_back, rand(), SCALE_LL(LEN1), SCALE_LL(LEN2), SCALE_LL(LEN3));
        #else
        CON_TEST_P1(deque<int>, push_back, rand(), SCALE_L(LEN1), SCALE_L(LEN2), SCALE_L(LEN3));
        #endif
        std::cout << std::endl;
        std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
        PASSED;
        #endif
        std::cout << "[----------------- End container test : deque ------------------]" << std::endl;
    }

}  // namespace deque_test

}  // namespace test

}  // namespace tinystl

#endif