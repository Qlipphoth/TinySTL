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

3. `operator->()`

   ```c++
    pointer  operator->() const { return &(operator*()); }
   ```

   1. `operator->()` 是一个重载的成员函数，它返回一个指向 `pointer` 类型的指针。
   2. `operator*()` 是另一个成员函数，它通常用于返回当前迭代器或智能指针所指向的对象的引用或指针。在这个函数中，它返回指针 `pointer` 所指向的对象的引用或指针。
   3. 在返回 `operator*()` 的结果之前，`&(operator*())` 表示取 `operator*()` 的地址，也就是返回一个指向 `operator*()` 所指向对象的指针。

   总之，`operator->()` 函数允许你在类似指针的对象上使用箭头运算符，使得你可以通过类似指针的语法访问该对象的成员。当你用类似指针的对象调用 `->` 运算符时，它将返回所指向对象的指针，使你可以通过该指针访问对象的成员。这种技术通常用于实现智能指针或迭代器，使其可以像指针一样操作。

