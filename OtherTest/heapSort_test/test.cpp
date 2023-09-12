
#include <iostream>
#include <bits/stl_heap.h>
#include <vector>

#include "../../TinySTL/vector.h"
#include "../../TinySTL/heap.h"

using namespace std;

void test1() {
    tinystl::vector<int> a{3, 2, 1};
    tinystl::make_heap(a.begin(), a.end());

    for (auto i : a) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

void test2() {
    std::vector<int> a{3, 2, 1};
    std::make_heap(a.begin(), a.end());

    for (auto i : a) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

int main() {
    test1();
    test2();
}

