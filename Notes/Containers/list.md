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