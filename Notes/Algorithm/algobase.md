# Algobase Notes

1. `enable_if`

   > 使用 `std::enable_if` 可以在模板定义中添加条件限制，从而仅在特定条件满足时才实例化模板。如果条件不满足，那么对应的模板定义将被禁用

   ```c++
   template <class Tp, class Up>
   typename std::enable_if<
     std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
     std::is_trivially_copy_assignable<Up>::value,
     Up*>::type
   ```

   1. `std::enable_if<...>::type`：`std::enable_if` 是一个模板类模板参数类型，它根据条件选择是否定义一个 `type` 成员类型。如果条件满足，它定义了一个名为 `type` 的类型别名，否则没有 `type` 成员。
   2. `std::is_same<typename std::remove_const<Tp>::type, Up>::value`：`std::is_same` 是一个类型特性检查模板，用于检查两个类型是否相同。这里使用 `std::remove_const` 来移除 `Tp` 类型的 `const` 限定符，然后检查移除 `const` 后的类型是否与 `Up` 类型相同。如果相同，该条件表达式的值为 `true`，否则为 `false`。
   3. `std::is_trivially_copy_assignable<Up>::value`：`std::is_trivially_copy_assignable` 是一个类型特性检查模板，用于检查类型 `Up` 是否是可平凡复制赋值的。如果 `Up` 类型是可平凡复制赋值的，该条件表达式的值为 `true`，否则为 `false`。
   4. `Up*`：如果条件表达式的值为 `true`，表示满足了特定的条件，那么整个类型表达式的结果类型为 `Up*`，即指向 `Up` 类型的指针类型。

   因此，这个复杂的类型表达式的含义是：如果 `Tp` 类型去除 `const` 限定符后与 `Up` 类型相同，并且 `Up` 类型是可平凡复制赋值的，那么结果类型为 `Up*`，即指向 `Up` 类型的指针类型。否则，没有定义 `type` 成员，即该类型表达式无效。

   ```c++
   template <class U1 = T1, class U2 = T2, 
           typename std::enable_if<std::is_default_constructible<U1>::value &&
           std::is_default_constructible<U2>::value, void>::type>
   ```

   1. `std::enable_if<...>::type`：`std::enable_if` 是一个模板类模板参数类型，它根据条件选择是否定义一个 `type` 成员类型。如果条件满足，它定义了一个名为 `type` 的类型别名，否则没有 `type` 成员。
   2. `std::is_default_constructible<U1>::value` 和 `std::is_default_constructible<U2>::value`：`std::is_default_constructible` 是一个类型特性检查模板，用于检查类型 `U1` 和 `U2` 是否可默认构造。`.value` 是类型特性的静态成员变量，用于获取类型特性的值。如果类型是可默认构造的，则该条件表达式的值为 `true`，否则为 `false`。
   3. `&&`：逻辑与操作符，用于将两个条件连接起来，要求两个条件都满足。
   4. `void`：作为 `std::enable_if` 的第二个模板参数，指定了 `std::enable_if` 未满足条件时的默认返回类型。

   综上所述，这个类型表达式的含义是：如果类型 `U1` 和 `U2` 都是可默认构造的类型，则定义一个名为 `type` 的类型别名，其类型为 `void`。如果其中任何一个类型不可默认构造，则没有定义 `type` 成员。这样，通过使用这个类型表达式，可以根据条件选择是否定义一个特定的类型别名，从而在特定的条件下启用或禁用构造函数。

   ```c++
   template <class U1, class U2, 
           typename std::enable_if<
           std::is_constructible<T1, U1>::value &&
           std::is_constructible<T2, U2>::value &&
           std::is_convertible<U1&&, T1>::value &&
           std::is_convertible<U2&&, T2>::value, int>::type = 0>
           constexpr pair(U1&& a, U2&& b) : 
           first(tinystl::forward<U1>(a)),
           second(tinystl::forward<U2>(b)) {}
   ```

   1. `template <class U1, class U2, typename std::enable_if<...>::type = 0>`：这是一个模板构造函数的声明，使用了模板参数 `U1` 和 `U2`，并通过 `std::enable_if` 进行条件限制。如果条件满足，那么该构造函数可用，否则不可用。

   2. `constexpr pair(U1&& a, U2&& b) : first(tinystl::forward<U1>(a)), second(tinystl::forward<U2>(b)) {}`：这是构造函数的定义。它接受两个参数 `a` 和 `b`，分别用于初始化 `first` 和 `second` 成员。

      - `U1&& a` 和 `U2&& b` 是通用引用，可以绑定到左值或右值。
      - `first(tinystl::forward<U1>(a))` 和 `second(tinystl::forward<U2>(b))` 是成员初始化列表，通过使用 `std::forward` 进行完美转发，将传入的参数 `a` 和 `b` 转发给 `first` 和 `second` 成员进行初始化。
        - `std::forward` 是一个用于完美转发的函数模板，它根据传入的参数类型，将其作为左值引用或右值引用进行转发。这样可以保持参数的值类别（左值或右值）不变。

   3. `std::enable_if<...>::type = 0`：这是用于条件限制的 `std::enable_if` 部分，它确定了构造函数的可用性。具体地，该构造函数满足以下条件：

      - 类型 `T1` 和 `T2` 可以从类型 `U1` 和 `U2` 进行构造。
      - 类型 `U1&&` 和 `U2&&` 可以隐式转换为类型 `T1` 和 `T2`。

      如果满足这些条件，那么构造函数可用。通过 `= 0` 给 `std::enable_if` 的 `type` 成员提供了默认值，以确保构造函数在条件不满足时没有 `type` 成员，从而禁用该构造函数。

   综上所述，这个构造函数的目的是根据传入的参数类型进行构造，并初始化 `pair` 对象的 `first` 和 `second` 成员。它要求类型 `T1` 和 `T2` 可以从类型 `U1` 和 `U2` 进行构造，并且类型 `U1&&` 和 `U2&&` 可以隐式转换为类型 `T1` 和 `T2`。通过使用完美转发和条件限制，可以在满足特定条件时使用该构造函数。

   ```c++
   template <class Tp, class Size, class Up>
       typename std::enable_if<
           std::is_integral<Tp>::value && sizeof(Tp) == 1 &&
           !std::is_same<Tp, bool>::value &&
           std::is_integral<Up>::value && sizeof(Up) == 1,
           Tp*>::type
       unchecked_fill_n(Tp* first, Size n, Up value) {
           if (n > 0) {
               std::memset(first, static_cast<unsigned char>(value), (size_t)n);
           }
           return first + n;
       }
   ```

   1. `typename std::enable_if<...>::type`：这是用于条件限制的 `std::enable_if` 部分，它确定了函数的可用性。具体地，该函数满足以下条件：

      - 类型 `Tp` 是整数类型（`std::is_integral<Tp>::value`）。
      - `Tp` 的大小为 1 字节（`sizeof(Tp) == 1`）。
      - `Tp` 不是布尔类型（`!std::is_same<Tp, bool>::value`）。
      - 类型 `Up` 是整数类型（`std::is_integral<Up>::value`）。
      - `Up` 的大小为 1 字节（`sizeof(Up) == 1`）。

      如果满足这些条件，那么函数可用。`typename std::enable_if<...>::type` 表示返回类型为 `std::enable_if` 的 `type` 成员类型。

   2. 在这里，将 `value` 转换为 `unsigned char` 类型是为了确保转换后的值在传递给 `std::memset` 时具有正确的字节表示。`unsigned char` 类型的取值范围是 0 到 255，可以准确地表示一个字节的所有可能值。而如果直接将其他整数类型（如 `Tp` 或 `Up`）的值传递给 `std::memset`，则可能会导致在复制到内存中时发生意外的截断或溢出。

   3. `std::memset` 是 C 标准库中的函数，用于将指定的值复制到一段内存中。

      

2. `memcmp`

   `std::memcmp` 是 C++ 标准库中的函数，用于比较两段内存的内容是否相等。

   ```c++
   int memcmp(const void* ptr1, const void* ptr2, size_t num);
   ```

   参数说明：

   - `ptr1`：指向第一个内存块的指针。
   - `ptr2`：指向第二个内存块的指针。
   - `num`：要比较的字节数。

   返回值：

   - 如果 `ptr1` 和 `ptr2` 所指向的内存内容**相等**，则**返回值为 0**。
   - 如果 `ptr1` 所指向的内存内容**小于** `ptr2` 所指向的内存内容，则**返回值小于 0**。
   - 如果 `ptr1` 所指向的内存内容**大于** `ptr2` 所指向的内存内容，则**返回值大于 0**。

   `std::memcmp` 会按字节逐个比较 `ptr1` 和 `ptr2` 所指向的内存内容，比较的字节数由 `num` 指定。它是一种按照字典顺序比较内存的方法，逐字节比较直到遇到不同的字节或比较完所有字节。





































