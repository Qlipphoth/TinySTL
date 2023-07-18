# traits编程技巧

> 在算法中使用迭代器时，很可能会用到其相应型别（associated type），如迭代器所指的对象类型。但 C++ 只支持 `sizeof()`，并不支持 `typeof()`，因此需要设计一种能够获取到迭代器相应型别的方法，于是诞生了 traits（萃取）编程技巧

traits 主要包括两个要点：1. 内嵌型别声明 （nested type）；2. 偏特化（partial specialization）

1. 内嵌型别声明

   内嵌型别声明利用模板中的参数推导机制将迭代器的相应型别声明在类中，这样就可以作为函数的返回值类型而传递，如：

   ```c++
   template <class T>
   struct MyIter {
   	typedef T value_type;  // 内嵌型别声明
   	T* ptr;
   	MyIter(T* p = 0): ptr(p) { }
   	T& operator*() const { return *ptr }
   	// ...
   };
   
   template <class I>
   // 利用 typename 声明 I::value_type 为类型，这样就可以利用内嵌型别声明来获取
   // 迭代器的相应型别并作为函数的返回值类型
   typename I::value_type    // 这一行为返回值类型
   func(I ite) { return *ite }
   // ...
   MyIter<int> ite(new int(8));
   cout << func(ite);  // 输出 8
   ```

2. 偏特化

   虽然内嵌型别声明解决了函数返回值类型的问题，但对于原生指针这种没有类结构的类型，则无法在类内定义相应的内嵌型别，这里就要应用到偏特化的做法，为原生指针及原生常量指针各声明一份”特化“的 traits 版本，使得 traits 也可以萃取出它们的相应型别。偏特化相关知识点见同名的另一篇笔记。

   简单来说，偏特化类似于模板类的重载这个概念，针对一些特定的参数类型可以有不同的实现。
   
   - 偏特化版 —— 原生指针迭代器
   
     ```c++
     template <class T>
     struct iterator_traits<T*> {
     	typedef T value_type;  // 若 iterator_traits 接受类型为 int*, 则能萃取出 value_type 为 int
     }
     ```
   
   - 偏特化版 —— 原生常量指针迭代器
   
     ```
     template <class T>
     struct iterator_traits<const T*> {
     	// 若 iterator_traits 接受类型为 const int*, 则能萃取出 value_type 为 int
     	typedef T value_type;  
     }
     ```

STL 中，最常用到的迭代器相应型别有五种，符合标准的 STL 迭代器必须定义了以下五种相应型别：

```c++
struct iterator {
    typedef Category    iterator_category;  // 迭代器类型
    typedef T           value_type;         // 迭代器所指对象的类型
    typedef Distance    difference_type;    // 迭代器之间的距离
    typedef Pointer     pointer;            // 指向迭代器所指对象的指针
    typedef Reference   reference;          // 迭代器所指对象的引用
};
```

