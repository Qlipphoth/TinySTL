#include <iostream> 
#include <stack>
#include <deque>
#include <vector>
#include <list>

using namespace std;

class addFloat
{
public:
    addFloat(float f) : m_f(f) {
        cout << "---construct addFloat(float f)---" << endl;
    }

    friend addFloat operator+(const addFloat& a, const addFloat& b)
    {
        cout << "---operator+(const addFloat& a, const addFloat& b)---" << endl;
        return addFloat(a.m_f + b.m_f);
    }

    void show()
    {
        cout << m_f << endl;
    }

private:
    float m_f;

};

void test1() {
    addFloat a(1.1);
    addFloat b(2.2);
    addFloat c = a + b;
    c.show();
}

void test2() {
    addFloat a(1.1);
    addFloat c = a + 2.2;
    addFloat d = 2.2 + a;
    c.show();
    d.show();
}

int main() {
    // test1();
    test2();
    return 0;
}
