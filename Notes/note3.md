# Note3：uninitialized.h 涉及知识点

1. typename

   ```c++
   template <class InputIterator, class ForwardIteraotr>
   ForwardIteraotr uninitialized_copy(InputIterator first, InputIterator last, 
       ForwardIteraotr result) {
           // 判断是否为 POD 类型，这里使用了 std::is_trivially_copy_assignable
           return tinystl::uninitialized_copy(first, last, result, 
               std::is_trivially_copy_assignable<typename iterator_traits<InputIterator>::value_type>{});
       }
   ```

   在C++中，`typename`关键字用于标识一个依赖类型(dependent type)。依赖类型是指依赖于模板参数的类型，例如在模板中使用的类型，而这些类型在编译时才能确定。

   在这段代码中，`typename`关键字用于指示`iterator_traits<ForwardIterator>::value_type`是一个类型，而不是一个静态成员变量或静态函数。`iterator_traits`是一个模板，它接受一个迭代器类型作为模板参数，并提供关于该迭代器的类型信息。`value_type`是`iterator_traits`的一个成员类型，用于表示迭代器指向的元素类型。

   通过在`typename iterator_traits<ForwardIterator>::value_type`之前使用`typename`关键字，编译器将知道`value_type`是一个类型，并且可以正确地解析该表达式。如果没有使用`typename`关键字，编译器可能会将`value_type`解释为一个静态成员变量或静态函数，导致编译错误。

   

2. std::is_trivially_copy_assignable

   `std::is_trivially_copy_assignable`是C++标准库中的类型特性(trait)，用于检查给定类型是否满足可平凡复制赋值的要求。

   可平凡复制赋值是指一个类型的赋值操作符（`operator=`）是否可以通过简单地按位复制（bitwise copy）来完成，而无需执行任何自定义的赋值操作。如果一个类型是可平凡复制赋值的，那么其赋值操作符可以按照内存块的复制方式来进行，通常是通过 `memcpy` 或类似的底层复制机制来实现。

   `std::is_trivially_copy_assignable`提供了一个编译时的布尔值，用于指示给定类型是否满足可平凡复制赋值的要求。如果给定类型满足可平凡复制赋值，`std::is_trivially_copy_assignable`将返回`true`；否则，返回`false`。

3. std::is_trivially_move_assignable

   与 2 类似，是否满足可平凡移动赋值。

   

   

   