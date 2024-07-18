#include <vector> 
#include <iostream> 
#include "../../TinySTL/vector.h"

using namespace std;

class testDemo
{
public:
    testDemo(): num(0) {
    std::cout << "Default constructor called" << endl;
    }
    testDemo(int num): num(num) {
        std::cout << "Parameterized constructor called" << endl;
    }
    testDemo(const testDemo& other): num(other.num) {
        std::cout << "Copy constructor called" << endl;
    }
    // testDemo(testDemo& other): num(other.num) {
    //     std::cout << "Copy constructor called" << endl;
    // }
    // testDemo(testDemo&& other) :num(other.num) {
    //     std::cout << "Move constructor called" << endl;
    // }
    // testDemo(testDemo&& other) = delete;

    // ~testDemo() {}
    
private:
    int num;
};

void stdTest() {
    
    // int a = 2;

    cout << "emplace_back 2:" << endl;
    std::vector<testDemo> demo1;
    // demo1.emplace_back(2);  
    demo1.emplace_back(testDemo(2));

    cout << endl;

    cout << "push_back 2:" << endl;
    std::vector<testDemo> demo2;
    // demo2.push_back(2);
    demo2.push_back(testDemo(2));

    // testDemo a(1);
    // cout << "emplace_back a:" << endl;
    // std::vector<testDemo> demo1;  
    // demo1.emplace_back(testDemo(1));

    // cout << endl;

    // cout << "push_back a:" << endl;
    // std::vector<testDemo> demo2;
    // demo2.push_back(testDemo(1));
}

// void tinystlTest() {
//     // testDemo a(1);

//     cout << "-------------emplace_back 2:-----------" << endl;
//     tinystl::vector<testDemo> demo1;
//     demo1.emplace_back(2);  
//     // demo1.emplace_back(a);

//     cout << endl;

//     cout << "-------------push_back 2:--------------" << endl;
//     tinystl::vector<testDemo> demo2;
//     demo2.push_back(2);
//     // demo2.push_back(a);

//     // cout << "emplace_back a:" << endl;
//     // // std::vector<testDemo> demo1;
//     // // demo1.emplace_back(2);  
//     // demo1.emplace_back(a);

//     // cout << "push_back a:" << endl;
//     // // std::vector<testDemo> demo2;
//     // // demo2.push_back(2);
//     // demo2.push_back(a);

//     // cout << "------------push_back T&&:-------------" << endl;
//     // // testDemo cur(3);
//     // // demo2.push_back(cur);
//     // testDemo demo3(testDemo(3));
// }


int main()
{
    stdTest();
    // tinystlTest();
}
