# Note1: Allocator 部分涉及的知识点总结

1. New 干了什么事情

   > 先分配 memory，再调用ctor（构造函数）

   假设有一个 Complex 类型，代表复数，含有两个double 变量分别代表实部和虚部

   ```c++
   class Complex {
   	public: 
   		Complex(...) { ... }
   	private:
   		double m_real;
   		double m_imag;
   }
   ```

   ```c++
   Complex* pc = new Complex(1, 2);
   ```

   new 的操作可分解为以下三步：

   ```c++
   Complex *pc;
   	// 1. 调用 operator new 分配内存，其内部调用 malloc(n)
   	void* mem = operator new( sizeof(Complex) ); 
   	// 2. 将其类型转换为Complex指针
   	pc = static_cast<Complex*>(mem);
   	// 3. 调用 Complex 的构造函数
   	pc->Complex::Complex(1, 2);
   ```

2. Delete

   > 先调用dtor（析构函数），再释放memory

   假设有String类

   ```c++
   class String {
   	public:
   		~String() {
               // array new 一定要搭配 array delete，这样才能保证 array 中的每个元素都
               // 唤起了 dtor，若仅使用 delete，则只会唤起一次 dtor
   			delete[] m_data;  // 释放掉动态分配的内存
   		}
       	...
   	private:
   		char* m_data;
   }
   ```

   ```c++
   String* ps = new String("Hello");
   ...
   delete ps;
   ```

   delete的操作可分为以下两步：

   ```c++
   // 1. 调用析构函数: 释放掉类内部动态分配的的内存
   String::~String(ps);
   // 2. 释放内存：释放掉类本身占的空间（m_data指针），其内部调用 free
   operator delete(ps);
   ```

3. 移动构造函数

   > 为了实现移动构造函数，即使用一个右值引用来构造一个对象，需要利用到一个能够获得绑定到左值上的右值引用的函数，即 std::move()。

   在 C++ 中，移动构造（Move Constructor）是一种特殊的构造函数，用于在对象之间实现资源的高效移动而不进行深拷贝。

   移动构造函数通过接受一个右值引用（R-value reference）作为参数来定义。它用于将一个临时对象或将要销毁的对象的资源有效地“移动”到新创建的对象中，而不是进行资源的复制或拷贝。移动构造函数通常用于提高程序的性能和效率，特别是对于大型对象或需要频繁进行资源管理的情况。

   移动构造函数的语法如下：

   ```c++
   ClassName(ClassName&& other) noexcept
   {
     // 在此进行资源的移动操作
   }
   ```

   > `noexcept` 是 C++ 中的关键字，用于指定函数是否可能引发异常。
   >
   > 当一个函数被声明为 `noexcept`，表示该函数不会抛出任何异常。
   >
   > `noexcept` 在函数声明或定义中的使用对编译器是一种承诺，即函数不会引发异常。它有助于编译器进行优化，以提高程序的性能和效率。当调用一个被声明为 `noexcept` 的函数时，编译器可以进行一些优化假设，例如避免生成额外的异常处理代码。

   其中，`ClassName` 是类的名称，`other` 是一个右值引用参数，用于接受被移动的对象。

   在移动构造函数中，可以执行资源的移动操作，例如交换指针、转移所有权或移动资源的状态。通常情况下，移动构造函数会将被移动对象的资源指针或状态转移到新创建的对象中，并将被移动对象置于有效但未定义的状态，以便销毁时不会释放资源。

   移动构造函数通常由编译器自动生成，但如果类中存在显式定义的复制构造函数或析构函数时，编译器不会自动生成移动构造函数。在这种情况下，需要显式定义移动构造函数来确保正确的移动语义。

4. 完美转发

   C++ 中的完美转发（Perfect Forwarding）是一种技术，用于在函数模板中将参数以原始的值类别转发给其他函数，保持参数的值类别不变。它是泛型编程中非常有用的概念。

   完美转发的目标是在转发参数时避免不必要的拷贝或移动操作。当一个函数接收到一个参数，然后将其传递给另一个函数时，完美转发可以确保参数以原始的左值引用或右值引用形式传递，而不会进行额外的拷贝或移动操作。

   完美转发的关键在于使用**通用引用（Universal Reference）**和**引用折叠（Reference Collapsing）**的特性。通用引用是指以 `T&&` 形式声明的参数，其中 `T` 是一个类型模板参数。通过使用通用引用，参数可以根据传入的实参的值类别（左值还是右值）推导出适当的引用类型。

   其次，解释一下折叠的含义。所谓的折叠，就是多个的意思。上面介绍引用分为左值引用和右值引用两种，那么将这两种类型进行排列组合，就有四种情况：

   ```c++
   - 左值-左值 T& &
   - 左值-右值 T& &&
   - 右值-左值 T&& &
   - 右值-右值 T&& &&
   ```

   所有的引用折叠最终都代表一个引用，要么是左值引用，要么是右值引用。

   **规则是：如果任一引用为左值引用，则结果为左值引用。否则（即两个都是右值引用），结果为右值引用。**

   在std中，实现完美转发的函数为 `std::forward`，`std::forward` 必须配合通用引用即 `T&&`使用，例如：

   1. 接收 `T` 为 int，为右值，返回 `T&&` 亦为右值
   2. 接收 `T` 为 int &，为左值，返回 `T& &&`，按照引用折叠规则，等价于 `T&` ，为左值
   3. 接收 `T` 为 int &&，为右值，返回 `T&& && = T&&` 为右值

   这样就利用通用引用与引用折叠的特性完成了完美转发。

   































