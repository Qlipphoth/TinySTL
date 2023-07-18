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