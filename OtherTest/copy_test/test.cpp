#include <iostream> 

#include "../../TinySTL/vector.h"
#include "../../TinySTL/deque.h"
#include "../../TinySTL/algobase.h"

using namespace std;

class A {

public:
    A() : a("A") {}
    A(const A& other) : a(other.a) {}
    friend ostream& operator<<(ostream& os, const A& a) {
        os << a.a;
        return os;
    }

private:
    string a;
};

void copy_test1() {
    tinystl::vector<int> v1{1, 2, 3, 4, 5};
    tinystl::vector<int> v2(5);
    tinystl::copy(v1.begin(), v1.end(), v2.begin());
    for (auto i : v2) {
        cout << i << " ";
    }
    cout << endl;
}

void copy_test2() {
    std::cout << boolalpha << std::is_trivially_copy_assignable<A>::value << endl;
    tinystl::vector<A> v1(5);
    tinystl::vector<A> v2(5);
    tinystl::copy(v1.begin(), v1.end(), v2.begin());
    for (auto i : v2) {
        cout << i << " ";
    }
    cout << endl;
}

void copy_test3() {
    tinystl::deque<int> d1(5);
    tinystl::deque<int> d2(5);
    tinystl::copy(d1.begin(), d1.end(), d2.begin());
    for (auto i : d2) {
        cout << i << " ";
    }
    cout << endl;
}

void copy_test4() {
    const char s1[] = "hello";
    char s2[10];
    tinystl::copy(s1, s1 + 5, s2);
    cout << s2 << endl;
}

int main() {
    // copy_test1();
    copy_test2();
    // copy_test3();
    // copy_test4();
    return 0;
}