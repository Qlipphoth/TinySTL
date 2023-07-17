# Note1:Iterator 相关知识点

> iterator 是一种 smart pointer   —— 侯捷

1. `std::initialized_list`

   `std::initializer_list` 是 C++ 标准库中的一个类模板，用于方便地初始化对象列表。

   `std::initializer_list` 可以用于初始化对象，例如容器、数组、自定义类型等。它提供了一种类似于数组的初始化语法，允许通过大括号 `{}` 中的值列表来初始化对象。

   通过使用 `std::initializer_list`，我们可以以类似于数组的方式初始化对象，不必显式指定容器的大小或元素数量。可以在函数调用或对象初始化时直接使用大括号 `{}` 来提供元素列表。

   `std::initializer_list` 是一个轻量级的包装器，内部维护了一个指向初始化列表的指针以及列表的大小。它不拥有元素内存，只是提供了一个方便的方式来传递和访问初始化列表。

