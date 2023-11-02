#include <iostream> 

// Test memory leak
void test1()
{
    int* p = new int[10];
    // delete[] p;  // If you comment this line, there will be a memory leak.

// valgrind --leak-check=full ./test
// ...
// ==23515== LEAK SUMMARY:
// ==23515==    definitely lost: 40 bytes in 1 blocks
// ==23515==    indirectly lost: 0 bytes in 0 blocks
// ==23515==      possibly lost: 0 bytes in 0 blocks
// ==23515==    still reachable: 0 bytes in 0 blocks
// ==23515==         suppressed: 0 bytes in 0 blocks
// ...

}

void test2()
{
    int* p = new int[10];
    static int* q = p;

// ==1338== LEAK SUMMARY:
// ==1338==    definitely lost: 0 bytes in 0 blocks
// ==1338==    indirectly lost: 0 bytes in 0 blocks
// ==1338==      possibly lost: 0 bytes in 0 blocks
// ==1338==    still reachable: 40 bytes in 1 blocks
}

void test3()
{
    int* p = new int[10];
    static int* q = p;
    q++;

// ==1521== LEAK SUMMARY:
// ==1521==    definitely lost: 0 bytes in 0 blocks
// ==1521==    indirectly lost: 0 bytes in 0 blocks
// ==1521==      possibly lost: 40 bytes in 1 blocks
// ==1521==    still reachable: 0 bytes in 0 blocks
// ==1521==         suppressed: 0 bytes in 0 blocks
}

int* test4()
{
    int* p = new int[10];

    static int* q = p;
    q++;
    return p;
}

int main()
{
    // test1();
    // test2();
    // test3();
    auto res = test4();
}

