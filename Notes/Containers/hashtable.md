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

2. `ml != ml`

   ```c++
   void max_load_factor(float ml) noexcept {
       THROW_OUT_OF_RANGE_IF(ml != ml || ml < 0, "invalid hash load factor");
       mlf_ = ml;
   }
   ```

   这种判断 `ml != ml` 的用法通常用来检测浮点数是否为 NaN（Not-a-Number）。NaN 是一种特殊的浮点数值，表示无法表示的或者未定义的数值。在 C++ 中，可以通过这种方式来判断一个浮点数是否是 NaN，因为 NaN 与自身比较永远返回 false。

   所以，`ml != ml` 这个判断就是在检测传入的 `ml` 是否为 NaN。如果 `ml` 是 NaN，那么条件为 true，会抛出一个异常，表示传入的加载因子无效。如果 `ml` 不是 NaN，那么条件为 false，代码会继续执行。这是一种防御性的编程技巧，用来确保函数参数的有效性。

3. `+ 0.5f`

   ```c++
   void reserve(size_type count) {
       rehash(static_cast<size_type>(static_cast<float>(count) / max_load_factor() + 0.5f));
   }
   ```

   这段代码实现了 `reserve` 函数，用于设置哈希表的桶数，以容纳指定数量的元素。它的实现基于 `rehash` 函数，通过计算当前元素数量和加载因子来确定新的桶数。

   具体解释如下：

   1. `count` 是预期的元素数量，表示要为多少个元素做预留空间。
   2. `max_load_factor()` 返回当前哈希表的最大加载因子。
   3. `(count / max_load_factor())` 计算预期的桶数，为了确保不会浪费过多的空间，这里使用了当前元素数量除以最大加载因子。
   4. `(count / max_load_factor() + 0.5f)` 相当于进行了一次四舍五入，保证结果是一个整数。
   5. `static_cast<size_type>(static_cast<float>(count) / max_load_factor() + 0.5f)` 将浮点数计算结果转换为整数，作为新的预期桶数。
   6. `rehash()` 函数会根据预期桶数重新分配哈希表的内部结构。

   总之，`reserve` 函数的目的是为了提前预留足够的桶数，以便在插入元素时可以更好地分配元素到桶中，从而减少哈希冲突，提高哈希表的性能。

4.  `worth rehash` 

   ```c++
   if (static_cast<float>(size_) / static_cast<float>(n) < max_load_factor() - 0.25f
       && static_cast<float>(n) < static_cast<float>(bucket_size_) * 0.75f) {
       replace_bucket(n);
   }
   ```

   这段代码是在哈希表的 `rehash` 操作中使用的条件，用于判断是否值得进行重新哈希（rehash）。重新哈希是在哈希表容量扩展或收缩时进行的一种操作，目的是为了保持哈希表的负载因子在一个合适的范围内，以维持较好的性能。

   条件判断分为两个部分：

   1. `(float)size_ / (float)n < max_load_factor() - 0.25f`：这个部分检查当前哈希表中的元素数量与新的桶数量 `n` 之间的比值是否小于当前的最大负载因子减去0.25。如果成立，说明当前负载因子偏小，不足以利用哈希表的存储能力，值得进行重新哈希。
   2. `(float)n < (float)bucket_size_ * 0.75`：这个部分检查新的桶数量 `n` 是否小于当前桶数组的容量的0.75倍。如果成立，说明新的桶数量相对于当前桶数组的容量较小，可以减小空间浪费，值得进行重新哈希。

   综合两个条件，如果当前负载因子较小，并且新的桶数量适中，就认为值得进行重新哈希，即执行 `replace_bucket(n)` 操作，将哈希表重新组织为新的桶数量。这样可以在保持较低负载因子的同时，提高哈希表的性能。































