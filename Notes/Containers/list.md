# List 笔记

1. `remove`

   ```c++
   void remove(const value_type& value) {
           remove_if([&](const value_type& x) { return x == value; });
       }
   
   template <class UnaryPredicate>
   void remove_if(UnaryPredicate pred);
   ```

   `remove` 函数用于删除 list 中值等于 `value` 的节点，逻辑由 `remove_if` 实现。`remove_if` 接受一个一元谓词作为判断条件， `remove` 中使用 `lambda` 表达式做到这一点。

   ```c++
   [&](const value_type& x) { return x == value; }
   ```

   - `[&]` 代表 `lambda` 捕获的所有参数均为所在实体中变量的引用
   - 接收一个 `x` 并判断是否与给定的 `value` 相等。

2. `std::boolalpha`

   `std::boolalpha` 是一个输出流操纵器（ostream manipulator），用于改变 `std::cout` 的输出格式，使得布尔值以文字形式输出而不是以整数形式输出。

   默认情况下，当使用 `std::cout` 输出布尔值时，C++标准库会将 `true` 输出为整数 `1`，将 `false` 输出为整数 `0`。而使用 `std::boolalpha` 后，输出流会将布尔值输出为文字形式的 `"true"` 和 `"false"`，而不是整数形式的 `1` 和 `0`。

