# Test.h 笔记

1. define1

   ```c++
   #define TESTCASE_NAME(testcase_name) testcase_name##_TEST
   ```

   这句代码在预处理阶段将 `TESTCASE_NAME(testcase_name)` 替换为 `testcase_name##_TEST`。`##` 是预处理指令中的连接符，用于将宏参数 `testcase_name` 和 `_TEST` 连接在一起形成新的标识符。

   换句话说，当你使用 `TESTCASE_NAME(testcase_name)` 这个宏时，预处理器会将其替换为 `testcase_name##_TEST`。

2. define2

   ```c++
   #define MYTINYSTL_TEST_(testcase_name)                        \
   class TESTCASE_NAME(testcase_name) : public TestCase {        \
   public:                                                       \
       TESTCASE_NAME(testcase_name)(const char* case_name)       \
           : TestCase(case_name) {};                             \
       virtual void Run();                                       \
   private:                                                      \
       static TestCase* const testcase_;                         \
   };                                                            \
                                                                 \
   TestCase* const TESTCASE_NAME(testcase_name)                  \
       ::testcase_ = UnitTest::GetInstance()->RegisterTestCase(  \
           new TESTCASE_NAME(testcase_name)(#testcase_name));    \
   void TESTCASE_NAME(testcase_name)::Run()
   ```

   ```c++
   #define TEST(testcase_name) MYTINYSTL_TEST_(testcase_name)
   ```

   当使用这个宏时，会展开为如下代码：

   ```c++
   class TESTCASE_NAME(testcase_name) : public TestCase {
   public:
       TESTCASE_NAME(testcase_name)(const char* case_name)
           : TestCase(case_name) {};
       virtual void Run();
   private:
       static TestCase* const testcase_;
   };
   
   TestCase* const TESTCASE_NAME(testcase_name)
       ::testcase_ = UnitTest::GetInstance()->RegisterTestCase(
           new TESTCASE_NAME(testcase_name)(#testcase_name));
   
   void TESTCASE_NAME(testcase_name)::Run()
   ```

   1. 定义了一个名为 `testcase_name_TEST` 的类，该类继承自 `TestCase`，该类接收一个常量字符串以初始化，并定义了一个虚函数 `void` 以及一个指针常量 `testcase_` 。

   2. 以 `testcase_name` 为参数实例化一个 `testcase_name_TEST` 对象，其中 `#testcase_name` 用于将 `testcase_name` 转化为字符串，并注册进 UnitTest 中，这里 UnitTest 是一个单例，用于有序地保存所有的 `TestCase`。

   3. 最后实现 `testcase_name` 中的 `Run` 函数，注意此处并没有写完，目的是在后续的代码中使用下述的简单方式为 `Run` 中添加语句。

      ```c++
      TEST(testcase_name) {
      	...   // 添加到 testcase_name_Test::Run() 中的功能
      }
      ```

3. define3

   ```c++
   do { ... } while(0)
   ```

   用于将 do 中的语句只执行一次。通常，使用宏定义时，为了确保宏在语法上的正确性和一致性，可以使用 do-while 循环来将多个语句组合成一个单一的语句。

   这种技巧的目的之一是为了处理宏的嵌套和语法扩展的问题。使用 do-while 循环可以确保宏定义的语法正确，即使在宏展开时会生成多个语句。

4. C字符串和C++的 `std::string`

   C 字符串和 C++ 的 std::string 对象是两种表示和处理字符串的不同方式。

   1. C 字符串：
      - C 字符串是由字符数组构成的，以空字符 `\0` 结尾作为字符串的结束标志。
      - C 字符串使用字符数组来存储和操作字符串数据，需要手动管理内存和长度。
      - C 字符串使用字符指针来引用字符串的起始地址，并使用一系列的 C 标准库函数（如 strcpy、strlen、strcat 等）来进行字符串操作。
      - C 字符串的长度是根据空字符 `\0` 来确定的，可以通过遍历字符数组来获取字符串的长度。
      - C 字符串的操作相对较低级，需要开发者手动处理内存分配和释放，并且容易出现缓冲区溢出和内存错误的问题。
   2. std::string 对象：
      - std::string 是 C++ 标准库提供的字符串类，封装了字符串的存储和操作功能。
      - std::string 使用**动态内存分配**来管理字符串的存储空间，可以根据需要自动调整字符串的大小。
      - std::string 提供了一系列成员函数（如 append、length、substr 等）来进行字符串操作，使用起来更方便和安全。
      - std::string 对象会自动跟踪字符串的长度，**不需要使用特殊的结束标志**。
      - std::string 提供了丰富的操作符重载和成员函数，使得字符串操作更加方便和易于理解。

   总结而言，C 字符串是较低级别的字符串表示方式，需要手动管理内存和长度，并使用 C 标准库函数进行操作。而 std::string 对象是 C++ 标准库提供的高级字符串类，封装了字符串的存储和操作，提供了更方便、安全和易用的接口。在 C++ 中，推荐使用 std::string 来处理字符串，尽量避免直接使用 C 字符串。

   > C中字符串用数组存储，因而没有重载 `==` 运算符，不支持字符串之间的比较，若想判断两字符串是否相等，需要使用 `strcmp` 函数，而 `std::string` 重载了 `==` 运算符，可以直接比较。

5. `std::stringstream`

   ```c++
   void test_len(size_t len1, size_t len2, size_t len3, size_t wide)
   {
     std::string str1, str2, str3;
     std::stringstream ss;
     ss << len1 << " " << len2 << " " << len3;
     ss >> str1 >> str2 >> str3;
     str1 += "   |";
     std::cout << std::setw(wide) << str1;
     str2 += "   |";
     std::cout << std::setw(wide) << str2;
     str3 += "   |";
     std::cout << std::setw(wide) << str3 << "\n";
   }
   ```

   这段代码定义了一个名为 `test_len` 的函数，该函数接受四个参数 `len1`、`len2`、`len3` 和 `wide`。

   在函数内部，创建了三个空的 `std::string` 对象 `str1`、`str2` 和 `str3`。然后，使用 `std::stringstream` 对象 `ss` 将参数 `len1`、`len2` 和 `len3` 转换为字符串，并将它们分别存储到 `str1`、`str2` 和 `str3` 中。

   接下来，每个字符串都添加了一些额外的内容（如空格和竖线），以增加字符串的长度。然后，使用 `std::cout` 和 `std::setw()` 函数打印每个字符串，其中 `std::setw(wide)` 用于设置输出的宽度，确保每个字符串占据相同的宽度。

   最后，打印一个换行符 `\n`，以结束当前行的输出。

6. `srand(int)`

   ```c++
   srand((int)time(0));
   ```

   这行代码用于**设置随机数生成器的种子**。在 C 和 C++ 中，使用 `srand` 函数设置随机数种子，可以使后续调用 `rand` 函数生成不同的随机数序列。

   具体而言，`srand` 函数的参数是一个整数值，用于设置随机数生成器的种子。通常使用当前时间的值作为种子，以确保每次运行程序时都能得到不同的随机数序列。`time(0)` 函数返回当前时间的值，类型为 `time_t`，通过将其转换为 `int` 类型作为 `srand` 函数的参数，将时间值用作随机数种子。

   通过设置不同的种子值，可以在每次运行程序时生成不同的随机数序列。如果不调用 `srand` 函数设置种子，则默认使用一个固定的种子值，从而导致每次运行程序时生成相同的随机数序列。







































