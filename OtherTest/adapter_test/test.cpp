#include <iostream> 

#include "../../TinySTL/vector.h"
#include "../../TinySTL/deque.h"
#include "../../TinySTL/iterator.h"
#include "../../TinySTL/functional.h"
#include "../../TinySTL/algorithm.h"


class Shape
{
public:
    virtual void draw() {
        std::cout << "draw shape" << std::endl;
    };
};

class Circle : public Shape
{
public:
    void draw() override
    {
        std::cout << "draw circle" << std::endl;
    }
};

class Rectangle : public Shape
{
public:
    void draw() override
    {
        std::cout << "draw rectangle" << std::endl;
    }
};

class Square : public Shape
{
public:
    void draw() override
    {
        std::cout << "draw square" << std::endl;
    }
};


void test1()
{
    tinystl::vector<int> v1 = tinystl::vector<int>(10);
    tinystl::iota(v1.begin(), v1.end(), 0);
    tinystl::vector<int> v2;

    tinystl::copy(v1.begin(), v1.end(), tinystl::back_inserter(v2));

    for (auto i : v2)
        std::cout << i << " ";
    std::cout << std::endl;

    tinystl::copy(v2.begin(), v2.end(), tinystl::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
}

void test2()
{
    tinystl::deque<int> d1 = tinystl::deque<int>(10);
    tinystl::iota(d1.begin(), d1.end(), 0);
    tinystl::deque<int> d2;

    tinystl::copy(d1.begin(), d1.end(), tinystl::front_inserter(d2));

    for (auto i : d2)
        std::cout << i << " ";
    std::cout << std::endl;

    tinystl::copy(d2.begin(), d2.end(), tinystl::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
}

void reverse_iterator_test()
{
    tinystl::vector<int> v1 = tinystl::vector<int>(10);
    tinystl::iota(v1.begin(), v1.end(), 0);
    
    auto it3 = v1.begin() + 3;
    auto reverse_it3 = tinystl::reverse_iterator<tinystl::vector<int>::iterator>(it3);
    std::cout << *it3 << " " << *reverse_it3 << std::endl;

    tinystl::for_each(v1.begin(), v1.end(), [](int i) { std::cout << i << " "; });
    std::cout << std::endl;

    tinystl::for_each(v1.rbegin(), v1.rend(), [](int i) { std::cout << i << " "; });
    std::cout << std::endl;
}


void istream_iterator_test()
{
    tinystl::istream_iterator<int> in(std::cin);
    tinystl::istream_iterator<int> eof;

    tinystl::vector<int> v;
    tinystl::copy(in, eof, tinystl::back_inserter(v));

    tinystl::copy(v.begin(), v.end(), tinystl::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
}

bool in2to7(int i)
{
    return i > 2 && i < 7;
}

void functor_adapter_test()
{
    tinystl::vector<int> v1 = tinystl::vector<int>(10);
    tinystl::iota(v1.begin(), v1.end(), 0);

    // 打印出大于5的元素
    tinystl::copy_if(
        v1.begin(), 
        v1.end(), 
        tinystl::ostream_iterator<int>(std::cout, " "), 
        tinystl::bind2nd(tinystl::greater<int>(), 5)
    );
    std::cout << std::endl;

    // 打印出大于2小于7的元素
    tinystl::copy_if(
        v1.begin(), 
        v1.end(), 
        tinystl::ostream_iterator<int>(std::cout, " "), 
        tinystl::compose2(
            tinystl::logical_and<bool>(),
            tinystl::bind2nd(tinystl::greater<int>(), 2), 
            tinystl::bind2nd(tinystl::less<int>(), 7)
        )
    );
    std::cout << std::endl;

    // 打印出小于等于2或者大于等于7的元素
    tinystl::copy_if(
        v1.begin(), 
        v1.end(), 
        tinystl::ostream_iterator<int>(std::cout, " "), 
        tinystl::not1(
            tinystl::compose2(
                tinystl::logical_and<bool>(),
                tinystl::bind2nd(tinystl::greater<int>(), 2), 
                tinystl::bind2nd(tinystl::less<int>(), 7)
            )
        )
    );
    std::cout << std::endl;

    // 打印出大于2小于7的元素
    tinystl::copy_if(
        v1.begin(), 
        v1.end(), 
        tinystl::ostream_iterator<int>(std::cout, " "), 
        tinystl::ptr_fun(in2to7)
    );
    std::cout << std::endl;

    // 打印出小于等于2或者大于等于7的元素
    tinystl::copy_if(
        v1.begin(), 
        v1.end(), 
        tinystl::ostream_iterator<int>(std::cout, " "), 
        tinystl::not1(tinystl::ptr_fun(in2to7))
    );
    std::cout << std::endl;

    // 打印出小于等于2或者大于等于7的元素
    // tinystl::copy_if(
    //     v1.begin(), 
    //     v1.end(), 
    //     tinystl::ostream_iterator<int>(std::cout, " "), 
    //     tinystl::not1(in2to7)
    // );
    // std::cout << std::endl;
    // 报错
}

void mem_fun_test()
{
    tinystl::vector<Shape*> v;
    v.push_back(new Circle());
    v.push_back(new Rectangle());
    v.push_back(new Square());

    tinystl::for_each(v.begin(), v.end(), tinystl::mem_fun(&Shape::draw));

    tinystl::for_each(v.begin(), v.end(), [](Shape *s) { s->draw(); });

    tinystl::vector<Shape> v1;
    v1.push_back(Circle());
    v1.push_back(Rectangle());
    v1.push_back(Square());

    tinystl::for_each(v1.begin(), v1.end(), tinystl::mem_fun_ref(&Shape::draw));

    tinystl::for_each(v1.begin(), v1.end(), [](Shape &s) { s.draw(); });
}

int main()
{
    // test1();
    // test2();
    // reverse_iterator_test();
    // istream_iterator_test();
    // functor_adapter_test();
    mem_fun_test();
    return 0;
}
