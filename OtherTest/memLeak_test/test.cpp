#include <iostream> 

// Test memory leak
void test()
{
    int* p = new int[10];
    // delete[] p;  // If you comment this line, there will be a memory leak.
}

int main()
{
    test();
    return 0;
}

// valgrind --leak-check=full ./test
// ...
// ==23515== LEAK SUMMARY:
// ==23515==    definitely lost: 40 bytes in 1 blocks
// ==23515==    indirectly lost: 0 bytes in 0 blocks
// ==23515==      possibly lost: 0 bytes in 0 blocks
// ==23515==    still reachable: 0 bytes in 0 blocks
// ==23515==         suppressed: 0 bytes in 0 blocks
// ...