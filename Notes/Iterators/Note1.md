# Note1:Iterator 相关知识点

> iterator 是一种 smart pointer   —— 侯捷

1. ## `std::initialized_list`

   `std::initializer_list` 是 C++ 标准库中的一个类模板，用于方便地初始化对象列表。

   `std::initializer_list` 可以用于初始化对象，例如容器、数组、自定义类型等。它提供了一种类似于数组的初始化语法，允许通过大括号 `{}` 中的值列表来初始化对象。

   通过使用 `std::initializer_list`，我们可以以类似于数组的方式初始化对象，不必显式指定容器的大小或元素数量。可以在函数调用或对象初始化时直接使用大括号 `{}` 来提供元素列表。

   `std::initializer_list` 是一个轻量级的包装器，内部维护了一个指向初始化列表的指针以及列表的大小。它不拥有元素内存，只是提供了一个方便的方式来传递和访问初始化列表。 

2. ## `SFINAE ` 技巧

   ```c++
   template <class T>
   struct has_iterator_cat
   {
   private:
     struct two { char a; char b; };
     template <class U> static two test(...);
     template <class U> static char test(typename U::iterator_category* = 0);
   public:
     static const bool value = sizeof(test<T>(0)) == sizeof(char);
   };
   ```

   这段代码定义了一个模板结构体 `has_iterator_cat`，用于检测类型 `T` 是否具有内部类型 `iterator_category`。

   在该结构体内部，定义了一个私有的嵌套结构体 `two`，它包含两个 `char` 类型的成员变量。然后，定义了两个重载的 `test` 函数模板，一个接受任意类型 `U` 的参数（使用可变参数模板），另一个接受参数类型为 `typename U::iterator_category*` 的参数。

   在 `test` 函数模板的实现中，第一个重载的函数使用 `...` 接受任意类型的参数，并返回 `two` 类型。第二个重载的函数使用了 **SFINAE** 技术，它接受一个指向 `U::iterator_category` 类型的指针参数，并返回 `char` 类型。这意味着，当类型 `T` 存在名为 `iterator_category` 的内部类型时，第二个重载的函数会匹配，返回 `char` 类型；否则，第一个重载的函数会匹配，返回 `two` 类型。

   最后，在 `has_iterator_cat` 结构体中定义了一个静态常量 `value`，它通过比较 `sizeof(test<T>(0))` 和 `sizeof(char)` 的大小来确定 `T` 是否具有 `iterator_category` 内部类型。如果两者的大小相等，则表示 `T` 具有 `iterator_category`，`value` 被设为 `true`；否则，表示 `T` 不具有 `iterator_category`，`value` 被设为 `false`。

   通过使用 `has_iterator_cat` 结构体和其静态常量 `value`，可以在编译时检查类型 `T` 是否具有 `iterator_category` 内部类型。

   

   **SFINAE** 是 **"Substitution Failure Is Not An Error"** 的缩写，即 "替换失败不是错误"。

   SFINAE 是 C++ 中的一种模板元编程技术，它允许根据类型推导和模板参数推导的失败来选择重载函数或模板的特定实例。在模板实例化过程中，如果发生了无法匹配的情况，不会引发编译错误，而是将该实例化候选项从候选项集中排除。

   SFINAE 的核心思想是，当在模板实例化过程中遇到了编译错误（例如，找不到某个成员、推导失败等），编译器将忽略该错误，而不会导致整个编译失败。这样，编译器会继续查找其他可行的候选项，并选择可行的候选项进行实例化。

   SFINAE 常常与模板特化、函数重载、模板参数推导和类型推导一起使用，通过在编译时根据条件选择特定的实例，达到编写更加通用和灵活的代码的目的。

   在上述代码中，SFINAE 技术被用于通过函数重载和模板特化来选择不同的 `test` 函数模板实例。当传递的类型 `T` 具有 `iterator_category` 内部类型时，第二个重载的函数将匹配，否则将匹配第一个重载的函数。通过这种方式，可以在编译时检测类型 `T` 是否具有特定的内部类型，并执行相应的操作。

   


