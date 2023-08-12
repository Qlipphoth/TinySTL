# Stack 笔记

1. `noexcept()`

   ```c++
   stack(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value) : c_(std::move(c)) {}
   ```

   `noexcept(std::is_nothrow_move_constructible<Container>::value)` 是**构造函数的异常规范**（exception specification）部分。在 C++ 中，**异常规范用于指定函数是否可能引发异常**。具体来说，`noexcept` 关键字用于声明函数是否应该是不会引发异常的。

   在这个上下文中，`noexcept(std::is_nothrow_move_constructible<Container>::value)` 的意思是：

   - `std::is_nothrow_move_constructible<Container>` 是一个类型特性（type trait），用于检查类型 `Container` 是否支持无异常移动构造。它返回一个布尔值，表示该类型是否支持无异常移动构造。
   - `std::is_nothrow_move_constructible<Container>::value` 获取上述类型特性返回的布尔值。
   - `noexcept(...)` 是 C++ 中的关键字，后面可以跟一个表达式，表示函数是否应该不会引发异常。

   因此，`noexcept(std::is_nothrow_move_constructible<Container>::value)` 的含义是，**构造函数是否被声明为不会引发异常，取决于类型 `Container` 是否支持无异常移动构造。**如果 `Container` 类型支持无异常移动构造（即 `std::is_nothrow_move_constructible<Container>::value` 为 `true`），那么构造函数会被声明为不会引发异常。如果 `Container` 类型不支持无异常移动构造（即 `std::is_nothrow_move_constructible<Container>::value` 为 `false`），那么构造函数不会被声明为不会引发异常。