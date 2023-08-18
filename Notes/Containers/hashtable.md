# hashtable Note

1. `enable_if`

   虽然记录过很多 `enable_if`的用法，但还是感觉 `SFINAE`机制很神奇，再记录一下

   ```c++
   template <class Iter, typename std::enable_if<
           tinystl::is_input_iterator<Iter>::value, int>::type = 0>
       hashtable(Iter first, Iter last, size_type bucket_count, 
                 const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
           : size_(tinystl::distance(first, last)), hash_(hash), equal_(equal), mlf_(1.0f) {
           init(tinystl::max(bucket_count, static_cast<size_type>(tinystl::distance(first, last))));
       }
   ```

   这里依然使用了 `SFINAE`技术，只有在 `Iter`为输出迭代器时才会实例化这个函数，其中，`type = 0`被赋予了默认值，如果不给 `std::enable_if` 的 `::type` 成员提供默认值，而且条件不满足时，那么这个 `::type` 成员将不存在。这将导致编译器无法正确解析使用这个类型的代码，可能会产生编译错误。

   给 `::type` 成员提供默认值，即使条件不满足，也会使得代码中的使用更加稳定，因为无论条件是否满足，`::type` 成员都会存在。这在一些模板元编程的情况下非常有用，可以使代码更具通用性和容错性。