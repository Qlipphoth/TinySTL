# TinySTL

[![Build status](https://ci.appveyor.com/api/projects/status/qa386f6e8uakdsff?svg=true)](https://ci.appveyor.com/project/Qlipphoth/tinystl)

简单实现 SGI-STL，配套讲解博客。

TODO

## 文章索引

这里将会列出后续具体实现 `STL` 的博客索引。

### Allocator

- [Allocator-1 相关知识点](https://qlipphoth.github.io/2023/08/19/2023-8-19-2/)
- [Allocator-2 allocator](https://qlipphoth.github.io/2023/08/20/2023-8-20-1/)
- [Allocator-3 alloc](https://qlipphoth.github.io/2023/08/20/2023-8-20-2/)

### Iterator

- [Iterator-1 相关概念引入](https://qlipphoth.github.io/2023/08/22/2023-8-22-1/)
- [Iterator-2 迭代器相应型别](https://qlipphoth.github.io/2023/08/23/2023-8-23-1/)
- [Iterator-3 iterator 实现](https://qlipphoth.github.io/2023/08/24/2023-8-24-1/)

### Container

#### 序列式容器

- vector
  1. [vector](https://qlipphoth.github.io/2023/08/29/2023-8-29-1/)

- list
  1. [list 的结构](https://qlipphoth.github.io/2023/08/31/2023-8-31-1/)
  2. [list 的函数](https://qlipphoth.github.io/2023/09/01/2023-9-1-1/)
  3. [list 的排序](https://qlipphoth.github.io/2023/09/01/2023-9-1-2/)

- deque
  1. [deque 的结构](https://qlipphoth.github.io/2023/09/03/2023-9-3-1/)
  2. [deque 的函数](https://qlipphoth.github.io/2023/09/06/2023-9-6-1/)

- stack & queue
  1. [stack & queue](https://qlipphoth.github.io/2023/09/11/2023-9-11-1/)

- priority queue
  1. [heap](https://qlipphoth.github.io/2023/09/12/2023-9-12-1/)
  2. [priority queue](https://qlipphoth.github.io/2023/09/14/2023-9-14-1/)

#### 关联式容器

- rb-tree
  1. [rb-tree 的预备知识](https://qlipphoth.github.io/2023/09/19/2023-9-19-1/)
  2. [rb-tree 的数据结构](https://qlipphoth.github.io/2023/09/24/2023-9-24-1/)
  3. [rb-tree 的插入与删除](https://qlipphoth.github.io/2023/09/25/2023-9-25-1/)
  4. [rb-tree 的其他函数](https://qlipphoth.github.io/2023/10/06/2023-10-6-1/)

- set/multiset、map/multimap
  1. [set 与 map](https://qlipphoth.github.io/2023/10/07/2023-10-7-1/)

- hashtable
  1. [hashtable 的预备知识](https://qlipphoth.github.io/2023/10/10/2023-10-10-1/)
  2. [hashtable 的数据结构](https://qlipphoth.github.io/2023/10/12/2023-10-12-2/)
  3. [hashtable 的构造与析构](https://qlipphoth.github.io/2023/10/13/2023-10-13-1/)
  4. [hashtable 的增删查](https://qlipphoth.github.io/2023/10/13/2023-10-13-2/)

- unordered_set/multiset、unordered_map/multimap
  1. [unorder_set 与 unordered_map](https://qlipphoth.github.io/2023/10/15/2023-10-15-1/)

### Algorithm

- [STL 中的算法介绍](https://qlipphoth.github.io/2023/10/16/2023-10-16-1/)
- [numeric/set algo](https://qlipphoth.github.io/2023/10/17/2023-10-17-1/)
- [algobase](https://qlipphoth.github.io/2023/10/18/2023-10-18-1/)
- `algo.h` 中的算法
  1. [euqal_range](https://qlipphoth.github.io/2023/10/19/2023-10-19-1/)
  2. [rotate](https://qlipphoth.github.io/2023/10/20/2023-10-20-1/)
  3. [next/prev permutation](https://qlipphoth.github.io/2023/10/21/2023-10-21-1/)
  4. [merge/inplace_merge](https://qlipphoth.github.io/2023/10/22/2023-10-22-1/)
  5. [sort](https://qlipphoth.github.io/2023/10/23/2023-10-23-1/)

### Functor

- [Functor](https://qlipphoth.github.io/2023/10/25/2023-10-25-1/)

### Adaptor

- [Iterator Adapter](https://qlipphoth.github.io/2023/10/29/2023-10-29-1/)
- [Functor Adapter](https://qlipphoth.github.io/2023/10/30/2023-10-30-1/)

### 单元测试

TODO 

### 番外

- [push_back 与 emplace_back](https://qlipphoth.github.io/2023/09/05/2023-9-5-1/)
- [记一个 bug 的解决过程](https://qlipphoth.github.io/2023/09/07/2023-9-7-1/)
- [运算符重载与友元](https://qlipphoth.github.io/2023/09/11/2023-9-11-2/)
- [红黑树寄录](https://qlipphoth.github.io/2023/08/16/2023-8-16-1/)

