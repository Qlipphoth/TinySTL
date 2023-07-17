# Note2

1.  ... 的作用

   ```c++
   template <class Ty, class... Args>
   void construct(Ty* ptr, Args&&... args) {
     ::new ((void*)ptr) Ty(mystl::forward<Args>(args)...);
   }
   
   ```

   在这段代码中，`...` 是 C++ 中的可变参数模板（Variadic Template）语法的一部分，用于表示模板参数包（parameter pack）。

   1. `class... Args`：
      - 这里使用 `...` 表示模板参数 `Args` 是一个模板参数包，它可以接受零个或多个参数类型。
      - `Args` 是一个用于接受模板参数的占位符，表示可能的多个类型参数。
   2. `Args&& ...args`：
      - 这里使用 `...` 表示模板参数 `Args` 是一个模板参数包（parameter pack），它可以接受零个或多个参数类型。
      - `Args&&` 是通用引用类型的模板参数包，即用于接受传入的可变参数 `args` 的类型。
      - 通过 `Args&& ...args`，函数模板 `construct` 可以接受任意数量的参数，并以右值引用或左值引用形式进行转发。
   3. `tinystl::forward<Args>(args)...`：
      - 在这里，`tinystl::forward<Args>` 是一个完美转发函数模板，用于将参数以原始的值类别转发。
      - `tinystl::forward<Args>(args)` 是将参数 `args` 以原始的值类别转发给模板函数 `tinystl::forward`。
      - `...` 在这里是展开语法，用于展开模板参数包 `args`，将其作为一系列参数进行传递给 `tinystl::forward`。
      - 通过 `tinystl::forward<Args>(args)...`，函数模板 `construct` 将接受的参数进行完美转发，保持参数的原始值类别，并将其传递给 `Ty` 类型的构造函数。

2. volatile 

   > https://zhuanlan.zhihu.com/p/62060524

   volatile 关键字是一种类型修饰符，用它声明的类型变量表示可以被某些编译器未知的因素更改。比如：操作系统、硬件或者其它线程等。遇到这个关键字声明的变量，编译器对访问该变量的代码就不再进行优化，从而可以提供对特殊地址的稳定访问。

   一般说来，volatile用在如下的几个地方：

   **1) 中断服务程序中修改的供其它程序检测的变量需要加volatile；**

   **2) 多任务环境下各任务间共享的标志应该加volatile；**

   **3) 存储器映射的硬件寄存器通常也要加volatile说明，因为每次对它的读写都可能由不同意义；**

   多线程下的 volatile：

   有些变量是用volatile关键字声明的。当两个线程都要用到某一个变量且该变量的值会被改变时，应该用volatile声明，该关键字的作用是**防止优化编译器把变量从内存装入CPU寄存器中。**如果变量被装入寄存器，那么两个线程有可能一个使用内存中的变量，一个使用寄存器中的变量，这会造成程序的错误执行。volatile的意思是**让编译器每次操作该变量时一定要从内存中真正取出，而不是使用已经存在寄存器中的值，**

3. ROUND_UP

   ```c++
   size_t alloc::ROUND_UP(size_t bytes) {
           return ((bytes + __ALIGN - 1) & ~(__ALIGN - 1));
   }
   ```

   ROUND_UP的作用是将指定的字节数 `bytes` 向上取整为一个对齐的值。

   具体解释如下：

   1. `__ALIGN` 是一个常量，表示内存对齐的边界值。它通常是 8 或者更大的 2 的幂次值。内存对齐是指要求分配的内存块的起始地址是特定字节数的倍数，以提高内存访问效率。
   2. 函数将 `bytes` 加上 `__ALIGN - 1`，相当于向上取整。这是因为要满足对齐的要求，如果 `bytes` 不是 `__ALIGN` 的倍数，就需要向上调整到下一个 `__ALIGN` 的倍数。
   3. 使用位运算 `& ~(__ALIGN - 1)`，实际上是将结果与 `__ALIGN - 1` 的二进制补码进行按位取反，再进行按位与操作。这个操作会将低位的非零位都置为 0，将高位保持不变。这样可以将结果向下取整到最接近的 `__ALIGN` 的倍数。
   4. 最终返回的结果就是向上取整到对齐边界的值。