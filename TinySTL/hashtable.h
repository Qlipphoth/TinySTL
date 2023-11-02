#ifndef TINYSTL_HASHTABLE_H_
#define TINYSTL_HASHTABLE_H_

// 这个头文件包含了一个模板类 hashtable
// hashtable : 哈希表，使用开链法处理冲突

#include <initializer_list>

#include "algo.h"
#include "functional.h"
#include "memory.h"
#include "vector.h"
#include "util.h"
#include "exceptdef.h"

namespace tinystl {

// ========================================== hashtable node ========================================== //

template <class T>
struct hashtable_node {
    hashtable_node* next;   // 指向下一个节点
    T               value;  // 节点的值

    hashtable_node() = default;
    hashtable_node(const T& v) : next(nullptr), value(v) {}

    hashtable_node(const hashtable_node& rhs) : next(rhs.next), value(rhs.value) {}
    hashtable_node(hashtable_node&& rhs) noexcept : next(rhs.next), value(tinystl::move(rhs.value)) {
        rhs.next = nullptr;
    }
};

// ============================================ value traits ============================================ //

/// @brief hashtable value traits
template <class T, bool>
struct ht_value_traits_imp {
    typedef T key_type;
    typedef T mapped_type;
    typedef T value_type;

    template <class Ty>
    static const key_type& get_key(const Ty& value) {
        return value;
    }

    template <class Ty>
    static const value_type& get_value(const Ty& value) {
        return value;
    }
};

/// @brief 针对 pair 的特化
/// @tparam T pair
template <class T>
struct ht_value_traits_imp<T, true> {
    typedef typename std::remove_cv<typename T::first_type>::type key_type;
    typedef typename T::second_type                               mapped_type;
    typedef T                                                     value_type;

    template <class Ty>
    static const key_type& get_key(const Ty& value) {
        return value.first;
    }

    template <class Ty>
    static const value_type& get_value(const Ty& value) {
        return value;
    }
};

template <class T>
struct ht_value_traits {
    static constexpr bool is_map = tinystl::is_pair<T>::value;

    typedef ht_value_traits_imp<T, is_map> value_traits_type;

    typedef typename value_traits_type::key_type    key_type;
    typedef typename value_traits_type::mapped_type mapped_type;
    typedef typename value_traits_type::value_type  value_type;

    template <class Ty>
    static const key_type& get_key(const Ty& value) {
        return value_traits_type::get_key(value);
    }

    template <class Ty>
    static const value_type& get_value(const Ty& value) {
        return value_traits_type::get_value(value);
    }

};

// forward declaration

template <class T, class HashFun, class KeyEqual, class Alloc>
class hashtable;

template <class T, class HashFun, class KeyEqual, class Alloc>
struct ht_iterator;

template <class T, class HashFun, class KeyEqual, class Alloc>
struct ht_const_iterator;

template <class T>
struct ht_const_local_iterator;


// ======================================= hashtable_iterator ====================================== //

template <class T, class Hash, class KeyEqual, class Alloc>
struct ht_iterator_base : 
    public tinystl::iterator<tinystl::forward_iterator_tag, T> 
{
    typedef tinystl::hashtable<T, Hash, KeyEqual, Alloc>                hashtable;
    typedef ht_iterator_base<T, Hash, KeyEqual, Alloc>                  base;
    typedef tinystl::ht_iterator<T, Hash, KeyEqual, Alloc>              iterator;
    typedef tinystl::ht_const_iterator<T, Hash, KeyEqual, Alloc>        const_iterator;
    typedef hashtable_node<T>*                                          node_ptr;
    typedef hashtable*                                                  contain_ptr;
    typedef const node_ptr                                              const_node_ptr;
    typedef const contain_ptr                                           const_contain_ptr;

    typedef size_t                                                      size_type;
    typedef ptrdiff_t                                                   difference_type;

    node_ptr        node;  // 指向当前节点
    contain_ptr     ht;    // 指向哈希表本身

    ht_iterator_base() = default;

    bool operator==(const base& rhs) const { return node == rhs.node; }
    bool operator!=(const base& rhs) const { return node != rhs.node; }
};

template <class T, class Hash, class KeyEqual, class Alloc>
struct ht_iterator : 
    public ht_iterator_base<T, Hash, KeyEqual, Alloc> 
{
    typedef ht_iterator_base<T, Hash, KeyEqual, Alloc>  base;
    typedef typename base::hashtable                    hashtable;
    typedef typename base::iterator                     iterator;
    typedef typename base::const_iterator               const_iterator;
    typedef typename base::node_ptr                     node_ptr;
    typedef typename base::contain_ptr                  contain_ptr;

    typedef ht_value_traits<T>                          value_traits;
    typedef T                                           value_type;
    typedef value_type*                                 pointer;
    typedef value_type&                                 reference;

    // 使用 base 的 node 及 ht 指针
    using base::node;
    using base::ht;

    ht_iterator() = default;
    ht_iterator(node_ptr n, contain_ptr t) {
        node = n;
        ht = t;
    } 
    ht_iterator(const iterator& rhs) {
        node = rhs.node;
        ht = rhs.ht;
    }
    ht_iterator(const const_iterator& rhs) {
        node = rhs.node;
        ht = rhs.ht;
    }

    iterator& operator=(const iterator& rhs) {
        if (this != &rhs) {
            node = rhs.node;
            ht = rhs.ht;
        }
        return *this;
    }
    
    iterator& operator=(const const_iterator& rhs) {
        if (this != &rhs) {
            node = rhs.node;
            ht = rhs.ht;
        }
        return *this;
    }

    // 重载操作符
    reference operator*()  const { return node->value; }
    pointer   operator->() const { return &(operator*()); }

    iterator& operator++() {
        TINYSTL_DEBUG(node != nullptr);
        const node_ptr old = node;
        node = node->next;  // 移动到下一个节点处
        // 如果下一个位置为空，说明已经到达链表尾部，需要跳到下一个 bucket
        if (node == nullptr) {
            auto index = ht->hash(value_traits::get_key(old->value));
            while (!node && ++index < ht->bucket_size_) {
                node = ht->buckets_[index];
            } 
        }
        return *this;
    }

    iterator operator++(int) {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }
};

template <class T, class Hash, class KeyEqual, class Alloc>
struct ht_const_iterator : 
    public ht_iterator_base<T, Hash, KeyEqual, Alloc> {
    typedef ht_iterator_base<T, Hash, KeyEqual, Alloc>      base;
    typedef typename base::hashtable                        hashtable;
    typedef typename base::iterator                         iterator;
    typedef typename base::const_iterator                   const_iterator;
    typedef typename base::const_node_ptr                   node_ptr;
    typedef typename base::const_contain_ptr                contain_ptr;

    typedef ht_value_traits<T>                              value_traits;
    typedef T                                               value_type;
    typedef const value_type*                               pointer;
    typedef const value_type&                               reference;

    // 使用 base 的 node 及 ht 指针
    using base::node;
    using base::ht;

    ht_const_iterator() = default;
    ht_const_iterator(node_ptr n, contain_ptr t) {
        node = n;
        ht = t;
    }
    ht_const_iterator(const iterator& rhs) {
        node = rhs.node;
        ht = rhs.ht;
    }
    ht_const_iterator(const const_iterator& rhs) {
        node = rhs.node;
        ht = rhs.ht;
    }

    const_iterator& operator=(const iterator& rhs) {
        if (this != &rhs) {
            node = rhs.node;
            ht = rhs.ht;
        }
        return *this;
    }

    const_iterator& operator=(const const_iterator& rhs) {
        if (this != &rhs) {
            node = rhs.node;
            ht = rhs.ht;
        }
        return *this;
    }

    // 重载操作符
    reference operator*()  const { return node->value; }
    pointer   operator->() const { return &(operator*()); }

    const_iterator& operator++() {
        TINYSTL_DEBUG(node != nullptr);
        const node_ptr old = node;
        node = node->next;
        // 如果下一个位置为空，说明已经到达链表尾部，需要跳到下一个 bucket
        if (node == nullptr) {
            auto index = ht->hash(value_traits::get_key(old->value));
            while (!node && ++index < ht->bucket_size_) {
                node = ht->buckets_[index];
            }
        }
        return *this;
    }

    const_iterator operator++(int) {
        const_iterator tmp = *this;
        ++*this;
        return tmp;
    }
};

// ======================================= local_iterator ====================================== //
// 用于遍历一个特定 bucket 内的元素
// 2023.10.12: 已弃用

template <class T>
struct ht_local_iterator :public tinystl::iterator<tinystl::forward_iterator_tag, T> {
    typedef T                          value_type;
    typedef value_type*                pointer;
    typedef value_type&                reference;
    typedef size_t                     size_type;
    typedef ptrdiff_t                  difference_type;
    typedef hashtable_node<T>*         node_ptr;

    typedef ht_local_iterator<T>       self;
    typedef ht_local_iterator<T>       local_iterator;
    typedef ht_const_local_iterator<T> const_local_iterator;
    node_ptr node;

    ht_local_iterator(node_ptr n) : node(n) {}
    ht_local_iterator(const local_iterator& rhs) : node(rhs.node) {}
    ht_local_iterator(const const_local_iterator& rhs) : node(rhs.node) {}

    reference operator*()  const { return node->value; }
    pointer   operator->() const { return &(operator*()); }

    self& operator++() {
        MYSTL_DEBUG(node != nullptr);
        node = node->next;
        return *this;
    }
    
    self operator++(int) {
        self tmp(*this);
        ++*this;
        return tmp;
    }

    bool operator==(const self& other) const { return node == other.node; }
    bool operator!=(const self& other) const { return node != other.node; }
};

template <class T>
struct ht_const_local_iterator :public tinystl::iterator<tinystl::forward_iterator_tag, T> {
    typedef T                          value_type;
    typedef const value_type*          pointer;
    typedef const value_type&          reference;
    typedef size_t                     size_type;
    typedef ptrdiff_t                  difference_type;
    typedef const hashtable_node<T>*   node_ptr;

    typedef ht_const_local_iterator<T> self;
    typedef ht_local_iterator<T>       local_iterator;
    typedef ht_const_local_iterator<T> const_local_iterator;

    node_ptr node;

    ht_const_local_iterator(node_ptr n) : node(n) {}
    ht_const_local_iterator(const local_iterator& rhs) : node(rhs.node) {}
    ht_const_local_iterator(const const_local_iterator& rhs) : node(rhs.node) {}

    reference operator*()  const { return node->value; }
    pointer   operator->() const { return &(operator*()); }

    self& operator++() {
        MYSTL_DEBUG(node != nullptr);
        node = node->next;
        return *this;
    }

    self operator++(int) {
        self tmp(*this);
        ++*this;
        return tmp;
    }

    bool operator==(const self& other) const { return node == other.node; }
    bool operator!=(const self& other) const { return node != other.node; }
};

// ======================================== bucket ======================================== //

// bucket 的大小，SGI-STL 中以质数来设计 bucket 的大小

#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)
#define SYSTEM_64 1
#else
#define SYSTEM_32 1
#endif

#ifdef SYSTEM_64

#define PRIME_NUM 99

// 1. start with p = 101
// 2. p = next_prime(p * 1.7)
// 3. if p < (2 << 63), go to step 2, otherwise, go to step 4
// 4. end with p = prev_prime(2 << 63 - 1)
static constexpr size_t ht_prime_list[] = {
  101ull, 173ull, 263ull, 397ull, 599ull, 907ull, 1361ull, 2053ull, 3083ull,
  4637ull, 6959ull, 10453ull, 15683ull, 23531ull, 35311ull, 52967ull, 79451ull,
  119179ull, 178781ull, 268189ull, 402299ull, 603457ull, 905189ull, 1357787ull,
  2036687ull, 3055043ull, 4582577ull, 6873871ull, 10310819ull, 15466229ull,
  23199347ull, 34799021ull, 52198537ull, 78297827ull, 117446801ull, 176170229ull,
  264255353ull, 396383041ull, 594574583ull, 891861923ull, 1337792887ull,
  2006689337ull, 3010034021ull, 4515051137ull, 6772576709ull, 10158865069ull,
  15238297621ull, 22857446471ull, 34286169707ull, 51429254599ull, 77143881917ull,
  115715822899ull, 173573734363ull, 260360601547ull, 390540902329ull, 
  585811353559ull, 878717030339ull, 1318075545511ull, 1977113318311ull, 
  2965669977497ull, 4448504966249ull, 6672757449409ull, 10009136174239ull,
  15013704261371ull, 22520556392057ull, 33780834588157ull, 50671251882247ull,
  76006877823377ull, 114010316735089ull, 171015475102649ull, 256523212653977ull,
  384784818980971ull, 577177228471507ull, 865765842707309ull, 1298648764060979ull,
  1947973146091477ull, 2921959719137273ull, 4382939578705967ull, 6574409368058969ull,
  9861614052088471ull, 14792421078132871ull, 22188631617199337ull, 33282947425799017ull,
  49924421138698549ull, 74886631708047827ull, 112329947562071807ull, 168494921343107851ull,
  252742382014661767ull, 379113573021992729ull, 568670359532989111ull, 853005539299483657ull,
  1279508308949225477ull, 1919262463423838231ull, 2878893695135757317ull, 4318340542703636011ull,
  6477510814055453699ull, 9716266221083181299ull, 14574399331624771603ull, 18446744073709551557ull
};

#else

#define PRIME_NUM 44

// 1. start with p = 101
// 2. p = next_prime(p * 1.7)
// 3. if p < (2 << 31), go to step 2, otherwise, go to step 4
// 4. end with p = prev_prime(2 << 31 - 1)
static constexpr size_t ht_prime_list[] = {
  101u, 173u, 263u, 397u, 599u, 907u, 1361u, 2053u, 3083u, 4637u, 6959u, 
  10453u, 15683u, 23531u, 35311u, 52967u, 79451u, 119179u, 178781u, 268189u,
  402299u, 603457u, 905189u, 1357787u, 2036687u, 3055043u, 4582577u, 6873871u,
  10310819u, 15466229u, 23199347u, 34799021u, 52198537u, 78297827u, 117446801u,
  176170229u, 264255353u, 396383041u, 594574583u, 891861923u, 1337792887u,
  2006689337u, 3010034021u, 4294967291u,
};

#endif

/// @brief 返回大于等于 n 的最小质数
inline size_t ht_next_prime(size_t n) {
    const size_t* first = ht_prime_list;
    const size_t* last = ht_prime_list + PRIME_NUM;
    const size_t* pos = tinystl::lower_bound(first, last, n);
    // 如果 n 比所有的质数都大，返回最大的质数
    return pos == last ? *(last - 1) : *pos;
}

// =========================================== hashtable =========================================== //

/// @brief 模板类 hashtable
/// @tparam T  数据类型
/// @tparam Hash  哈希函数
/// @tparam KeyEqual  判断键值是否相等的函数
template <class T, class Hash, class KeyEqual, class Alloc = alloc>
class hashtable {
    // 友元可以访问 private 成员
    friend struct ht_iterator<T, Hash, KeyEqual, Alloc>;
    friend struct ht_const_iterator<T, Hash, KeyEqual, Alloc>;

public:  // hashtable 的型别定义
    typedef ht_value_traits<T>                            value_traits;
    typedef typename value_traits::key_type               key_type;
    typedef typename value_traits::mapped_type            mapped_type;
    typedef typename value_traits::value_type             value_type;
    typedef Hash                                          hasher;
    typedef KeyEqual                                      key_equal;

    typedef hashtable_node<T>                             node_type;
    typedef node_type*                                    node_ptr;
    // 使用 vector 实现 bucket，利用 vector 的动态扩容能力
    typedef tinystl::vector<node_ptr>                     bucket_type;

    // typedef tinystl::allocator<T>                         allocator_type;
    // typedef tinystl::allocator<T>                         data_allocator;
    // typedef tinystl::allocator<node_type>                 node_allocator;

    typedef simple_alloc<T, Alloc>                        allocator_type;
    typedef simple_alloc<T, Alloc>                        data_allocator;
    typedef simple_alloc<node_type, Alloc>                node_allocator;

    // typedef typename allocator_type::pointer              pointer;
    // typedef typename allocator_type::const_pointer        const_pointer;
    // typedef typename allocator_type::reference            reference;
    // typedef typename allocator_type::const_reference      const_reference;
    // typedef typename allocator_type::size_type            size_type;
    // typedef typename allocator_type::difference_type      difference_type;

    typedef value_type*                                   pointer;
    typedef const value_type*                             const_pointer;
    typedef value_type&                                   reference;
    typedef const value_type&                             const_reference;
    typedef size_t                                        size_type;
    typedef ptrdiff_t                                     difference_type;

    typedef tinystl::ht_iterator<T, Hash, KeyEqual, Alloc>          iterator;
    typedef tinystl::ht_const_iterator<T, Hash, KeyEqual, Alloc>    const_iterator;
    typedef tinystl::ht_local_iterator<T>                           local_iterator;
    typedef tinystl::ht_const_local_iterator<T>                     const_local_iterator;

    allocator_type get_allocator() const { return allocator_type(); }

private:  // 成员变量，表现一个 hashtable
    bucket_type buckets_;       // bucket
    size_type   bucket_size_;   // bucket 的大小
    size_type   size_;          // 元素的个数
    hasher      hash_;          // 哈希函数
    key_equal   equal_;         // 判断键值是否相等的函数
    float       mlf_;           // 最大负载因子

private:  // 辅助函数
    bool is_equal(const key_type& key1, const key_type& key2) {
        return equal_(key1, key2);
    }

    bool is_equal(const key_type& key1, const key_type& key2) const {
        return equal_(key1, key2);
    }

    /// @brief 将 node 转换为 const_iterator 指针
    const_iterator M_cit(node_ptr node) const noexcept {
        return const_iterator(node, const_cast<hashtable*>(this));
    }

    iterator M_begin() noexcept {
        for (size_type i = 0; i < bucket_size_; ++i) {
            // 找到第一个不为空的 bucket
            if (buckets_[i]) {
                return iterator(buckets_[i], this);
            }
        }
        return iterator(nullptr, this);
    }

    const_iterator M_begin() const noexcept {
        for (size_type n = 0; n < bucket_size_; ++n) {
            // 找到第一个不为空的 bucket
            if (buckets_[n]) {
                return M_cit(buckets_[n]);
            }
        }
        return M_cit(nullptr);
    }

public:  // 构造、复制、移动、析构函数
    // 这里使将构造函数声明为 explicit，因为后两个参数都缺省了，可以通过 size_type 
    // 直接进行隐式转换，会产生一些不必要的 bug
    explicit hashtable(size_type bucket_count, 
                       const Hash& hash = Hash(), 
                       const KeyEqual& equal = KeyEqual())
        : size_(0), hash_(hash), equal_(equal), mlf_(1.0f) {
        init(bucket_count);
    }


    template <class Iter, typename std::enable_if<
        tinystl::is_input_iterator<Iter>::value, int>::type = 0>
    hashtable(Iter first, Iter last, size_type bucket_count, 
              const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
        : size_(tinystl::distance(first, last)), hash_(hash), equal_(equal), mlf_(1.0f) {
        init(tinystl::max(bucket_count, static_cast<size_type>(tinystl::distance(first, last))));
    }

    hashtable(const hashtable& rhs) : hash_(rhs.hash_), equal_(rhs.equal_) {
        copy_init(rhs);
    }

    hashtable(hashtable&& rhs) noexcept :
        bucket_size_(rhs.bucket_size_), 
        size_(rhs.size_),
        hash_(rhs.hash_),
        equal_(rhs.equal_),
        mlf_(rhs.mlf_) {
        buckets_ = tinystl::move(rhs.buckets_);
        rhs.bucket_size_ = 0;
        rhs.size_ = 0;
        rhs.mlf_ = 1.0f;
    }

    hashtable& operator=(const hashtable& rhs);
    hashtable& operator=(hashtable&& rhs) noexcept;

    ~hashtable() { 
        clear(); 

    }

public:  // 迭代器相关
    iterator begin() noexcept { return M_begin(); }
    const_iterator begin() const noexcept { return M_begin(); }
    const_iterator cbegin() const noexcept { return M_begin(); }

    iterator end() noexcept { return iterator(nullptr, this); }
    const_iterator end() const noexcept { return M_cit(nullptr); }
    const_iterator cend() const noexcept { return M_cit(nullptr); }

public:  // 容量相关
    bool      empty()    const noexcept { return size_ == 0; }
    size_type size()     const noexcept { return size_; }
    size_type max_size() const noexcept { return static_cast<size_type>(-1); }

public:  // 修改容器相关操作
    template <class ...Args>
    iterator emplace_multi(Args&&... args);

    template <class ...Args>
    tinystl::pair<iterator, bool> emplace_unique(Args&&... args);

    // [note]: hint 对于 hash_table 其实没有意义，因为即使提供了 hint，也要做一次 hash，
    // 来确保 hash_table 的性质，所以选择忽略它
    template <class ...Args>
    iterator emplace_multi_use_hint(const_iterator /*hint*/, Args&&... args) {
        return emplace_multi(tinystl::forward<Args>(args)...);
    }

    template <class ...Args>
    iterator emplace_unique_use_hint(const_iterator /*hint*/, Args&&... args) {
        return emplace_unique(tinystl::forward<Args>(args)...).first;
    }

    iterator insert_multi_noresize(const value_type& value);
    pair<iterator, bool> insert_unique_noresize(const value_type& value);

    iterator insert_multi(const value_type& value) {
        rehash_if_need(1);
        return insert_multi_noresize(value);
    }

    iterator insert_multi(value_type&& value) {
        return emplace_multi(tinystl::move(value));
    }

    pair<iterator, bool> insert_unique(const value_type& value) {
        rehash_if_need(1);
        return insert_unique_noresize(value);
    }

    pair<iterator, bool> insert_unique(value_type&& value) {
        return emplace_unique(tinystl::move(value));
    }

    iterator insert_multi_use_hint(const_iterator /*hint*/, const value_type& value) {
        return insert_multi(value);
    }

    iterator insert_multi_use_hint(const_iterator /*hint*/, value_type&& value) {
        return emplace_multi(tinystl::move(value));
    }

    iterator insert_unique_use_hint(const_iterator /*hint*/, const value_type& value) {
        return insert_unique(value).first;
    }

    iterator insert_unique_use_hint(const_iterator /*hint*/, value_type&& value) {
        return emplace_unique(tinystl::move(value)).first;
    }

    template <class InputIterator>
    void insert_multi(InputIterator first, InputIterator last) {
        copy_insert_multi(first, last, tinystl::iterator_category(first));
    }

    template <class InputIterator>
    void insert_unique(InputIterator first, InputIterator last) {
        copy_insert_unique(first, last, tinystl::iterator_category(first));
    }

    void erase(const_iterator pos);
    void erase(const_iterator first, const_iterator last);

    size_type erase_multi(const key_type& key);
    size_type erase_unique(const key_type& key);

    void clear();

    void swap(hashtable& rhs) noexcept;

public:  // 查找相关操作
    size_type                                     count(const key_type& key) const;

    iterator                                      find(const key_type& key);
    const_iterator                                find(const key_type& key) const;

    tinystl::pair<iterator, iterator>             equal_range_multi(const key_type& key);
    tinystl::pair<const_iterator, const_iterator> equal_range_multi(const key_type& key) const;

    tinystl::pair<iterator, iterator>             equal_range_unique(const key_type& key);
    tinystl::pair<const_iterator, const_iterator> equal_range_unique(const key_type& key) const;

public:  // bucket 操作
    // 同名函数均用 size_type 作为参数，实现重载

    // local_iterator begin(size_type n) noexcept {
    //     TINYSTL_DEBUG(n < size_);
    //     return local_iterator(buckets_[n]);
    // }

    // const_local_iterator begin(size_type n) const noexcept {
    //     TINYSTL_DEBUG(n < size_);
    //     return const_local_iterator(buckets_[n]);
    // }

    // const_local_iterator cbegin(size_type n) const noexcept {
    //     TINYSTL_DEBUG(n < size_);
    //     return const_local_iterator(buckets_[n]);
    // }

    // local_iterator end(size_type n) noexcept {
    //     TINYSTL_DEBUG(n < size_);
    //     return nullptr;
    // }

    // const_local_iterator end(size_type n) const noexcept {
    //     TINYSTL_DEBUG(n < size_);
    //     return nullptr;
    // }

    // const_local_iterator cend(size_type n) const noexcept {
    //     TINYSTL_DEBUG(n < size_);
    //     return nullptr;
    // }

    
    size_type bucket_count() const noexcept { return bucket_size_; }
    // hash 桶能装下的最大元素个数
    size_type max_bucket_count() const noexcept { return ht_prime_list[PRIME_NUM - 1]; }

    size_type bucket_size(size_type n) const noexcept;

    /// @brief 返回键值为 key 的元素所在的 bucket 的编号
    size_type bucket(const key_type& key) const { return hash(key); }

public:  // hash 相关操作
    float load_factor() const noexcept {
        return bucket_size_ != 0 ? static_cast<float>(size_) / bucket_size_ : 0.0f;
    }

    float max_load_factor() const noexcept { return mlf_; }

    void max_load_factor(float ml) noexcept {
        // ml != ml 用于判断 ml 是否为 NaN
        // THROW_OUT_OF_RANGE_IF(ml != ml || ml < 0, "invalid hash load factor");
        mlf_ = ml;
    }

    void rehash(size_type count);

    /// @brief 重新分配桶的个数
    /// @param count 元素个数
    void reserve(size_type count) {
        rehash(static_cast<size_type>(static_cast<float>(count) / max_load_factor() + 0.5f));
    }

    hasher hash_function()  const { return hash_; }
    key_equal key_eq()      const { return equal_; }

private:  // hashtable 成员函数
    void init(size_type n);
    void copy_init(const hashtable& rhs);

    template <class ...Args>
    node_ptr create_node(Args&&... args);

    void destroy_node(node_ptr node);

    size_type next_size(size_type n) const;
    size_type hash(const key_type& key, size_type n) const;
    size_type hash(const key_type& key) const;
    void      rehash_if_need(size_type n);

    template <class InputIterator>
    void copy_insert_multi(InputIterator first, InputIterator last, tinystl::input_iterator_tag);

    template <class ForwardIterator>
    void copy_insert_multi(ForwardIterator first, ForwardIterator last, tinystl::forward_iterator_tag);

    template <class InputIterator>
    void copy_insert_unique(InputIterator first, InputIterator last, tinystl::input_iterator_tag);

    template <class ForwardIterator>
    void copy_insert_unique(ForwardIterator first, ForwardIterator last, tinystl::forward_iterator_tag);

    pair<iterator, bool> insert_node_unique(node_ptr node);
    iterator             insert_node_multi(node_ptr node);

    void replace_bucket(size_type bucket_count);
    void erase_bucket(size_type n, node_ptr first, node_ptr last);
    void erase_bucket(size_type n, node_ptr last);

    bool equal_to_multi(const hashtable& other);
    bool equal_to_unique(const hashtable& other);

};

// ========================================= 函数实现 ========================================= //

/// @brief 复制赋值运算符
template <class T, class Hash, class KeyEqual, class Alloc>
hashtable<T, Hash, KeyEqual, Alloc>&
hashtable<T, Hash, KeyEqual, Alloc>::operator=(const hashtable& rhs) {
    if (this != &rhs) {
        hashtable tmp(rhs);
        swap(tmp);
    }
    return *this;
}

/// @brief 移动赋值运算符
template <class T, class Hash, class KeyEqual, class Alloc>
hashtable<T, Hash, KeyEqual, Alloc>&
hashtable<T, Hash, KeyEqual, Alloc>::operator=(hashtable&& rhs) noexcept {
    hashtable tmp(tinystl::move(rhs));
    swap(tmp);
    return *this;
}

/// @brief 就地构造元素，键值允许重复，强异常安全保证
template <class T, class Hash, class KeyEqual, class Alloc>
template <class ...Args>
typename hashtable<T, Hash, KeyEqual, Alloc>::iterator
hashtable<T, Hash, KeyEqual, Alloc>::emplace_multi(Args&&... args) {
    auto np = create_node(tinystl::forward<Args>(args)...);
    try {
        if (static_cast<float>(size_ + 1) > static_cast<float>(bucket_size_) * max_load_factor()) {
            rehash(size_ + 1);
        }
    }
    catch (...) {
        destroy_node(np); 
        throw;
    }
    return insert_node_multi(np);
}

/// @brief 就地构造元素，键值不允许重复，强异常安全保证
template <class T, class Hash, class KeyEqual, class Alloc>
template <class ...Args>
tinystl::pair<typename hashtable<T, Hash, KeyEqual, Alloc>::iterator, bool>
hashtable<T, Hash, KeyEqual, Alloc>::emplace_unique(Args&&... args) {
    auto np = create_node(tinystl::forward<Args>(args)...);
    try {
        if (static_cast<float>(size_ + 1) > static_cast<float>(bucket_size_) * max_load_factor()) {
            rehash(size_ + 1);
        }
    }
    catch (...) {
        destroy_node(np);
        throw;
    }
    return insert_node_unique(np);
}

/// @brief 在不需要重新分配桶的情况下插入新节点，键值允许重复
template <class T, class Hash, class KeyEqual, class Alloc>
typename hashtable<T, Hash, KeyEqual, Alloc>::iterator
hashtable<T, Hash, KeyEqual, Alloc>::insert_multi_noresize(const value_type& value) {
    const auto n = hash(value_traits::get_key(value));
    auto first = buckets_[n];
    auto tmp = create_node(value);
    for (auto cur = first; cur; cur = cur->next) {
        // 如果链表中存在相同键值的节点就马上插入，然后返回
        if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(value))) {
            tmp->next = cur->next;
            cur->next = tmp;
            ++size_;
            return iterator(tmp, this);
        }
    }
    // 否则插入在链表头部
    tmp->next = first;  // 将新节点插入到链表头部
    buckets_[n] = tmp;  // 更新 bucket[n] 的头部
    ++size_;
    return iterator(tmp, this);
}

template <class T, class Hash, class KeyEqual, class Alloc>
tinystl::pair<typename hashtable<T, Hash, KeyEqual, Alloc>::iterator, bool>
hashtable<T, Hash, KeyEqual, Alloc>::insert_unique_noresize(const value_type& value) {
    const auto n = hash(value_traits::get_key(value));
    auto first = buckets_[n];
    for (auto cur = first; cur; cur = cur->next) {
        // 如果链表中存在相同键值的节点就马上返回
        if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(value))) {
            return tinystl::make_pair(iterator(cur, this), false);
        }
    }
    // 否则插入在链表头部
    auto tmp = create_node(value);
    tmp->next = first;  // 将新节点插入到链表头部
    buckets_[n] = tmp;  // 更新 bucket[n] 的头部
    ++size_;
    return tinystl::make_pair(iterator(tmp, this), true);
}

/// @brief 删除迭代器所指向的节点
template <class T, class Hash, class KeyEqual, class Alloc>
void hashtable<T, Hash, KeyEqual, Alloc>::erase(const_iterator pos) {
    auto p = pos.node;
    if (p) {
        const auto n = hash(value_traits::get_key(p->value));  // 计算 bucket 的位置
        auto cur = buckets_[n];
        // p 位于链表的头部
        if (cur == p) {
            buckets_[n] = cur->next;
            destroy_node(cur);
            --size_;
        }
        // 否则在链表中查找 p
        else {
            auto next = cur->next;
            while (next) {
                if (next == p) {
                    cur->next = next->next;
                    destroy_node(next);
                    --size_;
                    break;
                }
                else {
                    cur = next;
                    next = cur->next;
                }
            }
        }
    }
}

/// @brief 删除 [first, last) 内的节点
template <class T, class Hash, class KeyEqual, class Alloc>
void hashtable<T, Hash, KeyEqual, Alloc>::erase(const_iterator first, const_iterator last) {
    if (first.node == last.node) return;
    auto first_bucket = first.node
        ? hash(value_traits::get_key(first.node->value))
        : bucket_size_;
    auto last_bucket = last.node
        ? hash(value_traits::get_key(last.node->value))
        : bucket_size_;
    if (first_bucket == last_bucket) {
        erase_bucket(first_bucket, first.node, last.node);
    }
    else {
        erase_bucket(first_bucket, first.node, nullptr);
        for (size_type n = first_bucket + 1; n < last_bucket; ++n) {
            if (buckets_[n] != nullptr) erase_bucket(n, nullptr);
        }
        if (last_bucket != bucket_size_) {
            erase_bucket(last_bucket, last.node);
        }
    }
}

/// @brief 删除键值为 key 的节点，返回删除的节点个数
template <class T, class Hash, class KeyEqual, class Alloc>
typename hashtable<T, Hash, KeyEqual, Alloc>::size_type
hashtable<T, Hash, KeyEqual, Alloc>::erase_multi(const key_type& key) {
    auto p = equal_range_multi(key);
    if (p.first.node != nullptr) {
        erase(p.first, p.second);
        return tinystl::distance(p.first, p.second);
    }
    return 0;
}

template <class T, class Hash, class KeyEqual, class Alloc>
typename hashtable<T, Hash, KeyEqual, Alloc>::size_type
hashtable<T, Hash, KeyEqual, Alloc>::erase_unique(const key_type& key) {
    const auto n = hash(key);
    auto first = buckets_[n];
    if (first) {
        if (is_equal(value_traits::get_key(first->value), key)) {
            buckets_[n] = first->next;
            destroy_node(first);
            --size_;
            return 1;
        }
        else {
            auto next = first->next;
            while (next) {
                if (is_equal(value_traits::get_key(next->value), key)) {
                    first->next = next->next;
                    destroy_node(next);
                    --size_;
                    return 1;
                }
                first = next;
                next = first->next;
            }
        }
    }
    return 0;
}

/// @brief 清空 hashtable
template <class T, class Hash, class KeyEqual, class Alloc>
void hashtable<T, Hash, KeyEqual, Alloc>::clear() {
    if (size_ != 0) {
        for (size_type i = 0; i < bucket_size_; ++i) {
            auto cur = buckets_[i];
            while (cur) {
                auto next = cur->next;
                destroy_node(cur);
                cur = next;
            }
            buckets_[i] = nullptr;
        }
        size_ = 0;
    }
}

/// @brief 得到某个 bucket 中节点的个数
template <class T, class Hash, class KeyEqual, class Alloc>
typename hashtable<T, Hash, KeyEqual, Alloc>::size_type
hashtable<T, Hash, KeyEqual, Alloc>::bucket_size(size_type n) const noexcept {
    size_type result = 0;
    // 遍历 bucket[n] 链表
    for (auto cur = buckets_[n]; cur; cur = cur->next) ++result;
    return result;
}

/// @brief 重新对元素进行一遍哈希，插入到新的位置
template <class T, class Hash, class KeyEqual, class Alloc>
void hashtable<T, Hash, KeyEqual, Alloc>::rehash(size_type count) {
    auto n = ht_next_prime(count);  // 获取 bucket 的大小
    if (n > bucket_size_) {
        replace_bucket(n);
    }
    else {
        // 判断是否值得重新哈希
        if (static_cast<float>(size_) / static_cast<float>(n) < max_load_factor() - 0.25f
            && static_cast<float>(n) < static_cast<float>(bucket_size_) * 0.75f) {
            replace_bucket(n);
        }
    }
}

/// @brief 查找键值为 key 的节点，返回迭代器
template <class T, class Hash, class KeyEqual, class Alloc>
typename hashtable<T, Hash, KeyEqual, Alloc>::iterator
hashtable<T, Hash, KeyEqual, Alloc>::find(const key_type& key) {
    const auto n = hash(key);
    node_ptr first = buckets_[n];
    for (; first && !is_equal(value_traits::get_key(first->value), key); first = first->next) {}
    return iterator(first, this);
}

template <class T, class Hash, class KeyEqual, class Alloc>
typename hashtable<T, Hash, KeyEqual, Alloc>::const_iterator
hashtable<T, Hash, KeyEqual, Alloc>::find(const key_type& key) const {
    const auto n = hash(key);
    node_ptr first = buckets_[n];
    for (; first && !is_equal(value_traits::get_key(first->value), key); first = first->next) {}
    return M_cit(first);
}

template <class T, class Hash, class KeyEqual, class Alloc>
typename hashtable<T, Hash, KeyEqual, Alloc>::size_type
hashtable<T, Hash, KeyEqual, Alloc>::count(const key_type& key) const {
    const auto n = hash(key);
    size_type result = 0;
    // 相同的值一定在同一个哈希桶里，所以只需要遍历 bucket[n] 即可
    for (node_ptr cur = buckets_[n]; cur; cur = cur->next) {
        if (is_equal(value_traits::get_key(cur->value), key)) ++result;
    }
    return result;
}

/// @brief 查找与键值 key 相等的区间，返回一个 pair，指向区间的首尾
template <class T, class Hash, class KeyEqual, class Alloc>
tinystl::pair<typename hashtable<T, Hash, KeyEqual, Alloc>::iterator, 
    typename hashtable<T, Hash, KeyEqual, Alloc>::iterator>
hashtable<T, Hash, KeyEqual, Alloc>::equal_range_multi(const key_type& key) {
    const auto n = hash(key);
    for (node_ptr first = buckets_[n]; first; first = first->next) {
        if (is_equal(value_traits::get_key(first->value), key)) {
            for (node_ptr cur = first->next; cur; cur = cur->next) {
                // 相等的区间在当前的桶范围之内
                if (!is_equal(value_traits::get_key(cur->value), key)) {
                    return tinystl::make_pair(iterator(first, this), iterator(cur, this));
                }
            }
            // 当前桶直到最后一个元素都相等，返回的区间的尾部指向下一个不为空的桶
            for (size_type m = n + 1; m < bucket_size_; ++m) {
                if (buckets_[m]) {
                    return tinystl::make_pair(iterator(first, this), iterator(buckets_[m], this));
                }
            }
            return tinystl::make_pair(iterator(first, this), end());
        }
    }
    return tinystl::make_pair(end(), end());
}

template <class T, class Hash, class KeyEqual, class Alloc>
tinystl::pair<typename hashtable<T, Hash, KeyEqual, Alloc>::const_iterator, 
    typename hashtable<T, Hash, KeyEqual, Alloc>::const_iterator>
hashtable<T, Hash, KeyEqual, Alloc>::equal_range_multi(const key_type& key) const {
    const auto n = hash(key);
    for (node_ptr first = buckets_[n]; first; first = first->next) {
        if (is_equal(value_traits::get_key(first->value), key)) {
            for (node_ptr cur = first->next; cur; cur = cur->next) {
                // 相等的区间在当前的桶范围之内
                if (!is_equal(value_traits::get_key(cur->value), key)) {
                    return tinystl::make_pair(M_cit(first), M_cit(cur));
                }
            }
            // 当前桶直到最后一个元素都相等，查找下一个不为空的桶
            for (size_type m = n + 1; m < bucket_size_; ++m) {
                if (buckets_[m]) {
                    return tinystl::make_pair(M_cit(first), M_cit(buckets_[m]));
                }
            }
            return tinystl::make_pair(M_cit(first), end());
        }
    }
    return tinystl::make_pair(cend(), cend());
}

/// @brief 查找与键值 key 相等的区间，返回一个 pair，指向区间的首尾
template <class T, class Hash, class KeyEqual, class Alloc>
tinystl::pair<typename hashtable<T, Hash, KeyEqual, Alloc>::iterator, 
    typename hashtable<T, Hash, KeyEqual, Alloc>::iterator>
hashtable<T, Hash, KeyEqual, Alloc>::equal_range_unique(const key_type& key) {
    const auto n = hash(key);
    for (node_ptr first = buckets_[n]; first; first = first->next) {
        if (is_equal(value_traits::get_key(first->value), key)) {
            if (first->next) {
                return tinystl::make_pair(iterator(first, this), iterator(first->next, this));
            }
            for (auto m = n + 1; m < bucket_size_; ++m) {
                if (buckets_[m]) {
                    return tinystl::make_pair(iterator(first, this), iterator(buckets_[m], this));
                }
            }
            return tinystl::make_pair(iterator(first, this), end());
        }
    }
    return tinystl::make_pair(end(), end());
}

template <class T, class Hash, class KeyEqual, class Alloc>
tinystl::pair<typename hashtable<T, Hash, KeyEqual, Alloc>::const_iterator, 
    typename hashtable<T, Hash, KeyEqual, Alloc>::const_iterator>
hashtable<T, Hash, KeyEqual, Alloc>::equal_range_unique(const key_type& key) const {
    const auto n = hash(key);
    for (node_ptr first = buckets_[n]; first; first = first->next) {
        if (is_equal(value_traits::get_key(first->value), key)) {
            if (first->next) {
                return tinystl::make_pair(M_cit(first), M_cit(first->next));
            }
            for (auto m = n + 1; m < bucket_size_; ++m) {
                if (buckets_[m]) {
                    return tinystl::make_pair(M_cit(first), M_cit(buckets_[m]));
                }
            }
            return tinystl::make_pair(M_cit(first), cend());
        }
    }
    return tinystl::make_pair(cend(), cend());
}

/// @brief 交换两个 hashtable
template <class T, class Hash, class KeyEqual, class Alloc>
void hashtable<T, Hash, KeyEqual, Alloc>::swap(hashtable& rhs) noexcept {
    if (this != &rhs) {
        tinystl::swap(buckets_, rhs.buckets_);
        tinystl::swap(bucket_size_, rhs.bucket_size_);
        tinystl::swap(size_, rhs.size_);
        tinystl::swap(hash_, rhs.hash_);
        tinystl::swap(equal_, rhs.equal_);
        tinystl::swap(mlf_, rhs.mlf_);
    }
}

// ======================================= 辅助函数实现 ======================================= //

/// @brief 初始化能容纳 n 个元素的 hashtable
template <class T, class Hash, class KeyEqual, class Alloc>
void hashtable<T, Hash, KeyEqual, Alloc>::init(size_type n) {
    const auto bucket_count = next_size(n);
    try {
        buckets_.reserve(bucket_count);
        // 初始化 buckets_，将每个 bucket 置为空
        // buckets_ 为 vector，可以使用 vector 的 assign 方法
        buckets_.assign(bucket_count, nullptr);
    }
    catch (...) {
        bucket_size_ = 0;
        size_ = 0;
        throw;
    }
    bucket_size_ = buckets_.size();
}

/// @brief 用一个 hashtable 初始化当前 hashtable
template <class T, class Hash, class KeyEqual, class Alloc>
void hashtable<T, Hash, KeyEqual, Alloc>::copy_init(const hashtable& rhs) {
    bucket_size_ = 0;
    buckets_.reserve(rhs.bucket_size_);
    buckets_.assign(rhs.bucket_size_, nullptr);
    try {
        for (size_type i = 0; i < rhs.bucket_size_; ++i) {
            auto cur = rhs.buckets_[i];
            // 如果 rhs 的某个 bucket 处存在链表
            if (cur) {
                auto copy = create_node(cur->value);
                buckets_[i] = copy;
                // 复制链表
                for (auto next = cur->next; next; cur = next, next = cur->next) {
                    copy->next = create_node(next->value);
                    copy = copy->next;
                }
                copy->next = nullptr;
            }
        }
        bucket_size_ = rhs.bucket_size_;
        size_ = rhs.size_;
        mlf_ = rhs.mlf_;
    }
    catch (...) {
        clear();
        // throw;
    }
}

/// @brief 创建一个节点
template <class T, class Hash, class KeyEqual, class Alloc>
template <class ...Args>
typename hashtable<T, Hash, KeyEqual, Alloc>::node_ptr
hashtable<T, Hash, KeyEqual, Alloc>::create_node(Args&& ...args) {
    node_ptr np = node_allocator::allocate(1);
    try {
        tinystl::construct(tinystl::address_of(np->value), tinystl::forward<Args>(args)...);
        np->next = nullptr;
    }
    catch (...) {
        node_allocator::deallocate(np);
        throw;        
    }
    return np;
}

/// @brief 销毁一个节点
template <class T, class Hash, class KeyEqual, class Alloc>
void hashtable<T, Hash, KeyEqual, Alloc>::destroy_node(node_ptr node) {
    tinystl::destroy(tinystl::address_of(node->value));
    node_allocator::deallocate(node);
    node = nullptr;
}

/// @brief 根据 n 计算 bucket 的大小
template <class T, class Hash, class KeyEqual, class Alloc>
typename hashtable<T, Hash, KeyEqual, Alloc>::size_type
hashtable<T, Hash, KeyEqual, Alloc>::next_size(size_type n) const {
    return ht_next_prime(n);
}

/// @brief 根据 key 计算 hash 值，对 n 取模
template <class T, class Hash, class KeyEqual, class Alloc>
typename hashtable<T, Hash, KeyEqual, Alloc>::size_type
hashtable<T, Hash, KeyEqual, Alloc>::hash(const key_type& key, size_type n) const {
    return hash_(key) % n;
}

/// @brief 根据 key 计算 hash 值，对 bucket_size_ 取模
template <class T, class Hash, class KeyEqual, class Alloc>
typename hashtable<T, Hash, KeyEqual, Alloc>::size_type
hashtable<T, Hash, KeyEqual, Alloc>::hash(const key_type& key) const {
    return hash_(key) % bucket_size_;
}

/// @brief 如果插入 n 个元素后，负载因子大于最大负载因子，就重新分配桶的个数
template <class T, class Hash, class KeyEqual, class Alloc>
void hashtable<T, Hash, KeyEqual, Alloc>::rehash_if_need(size_type n) {
    if (static_cast<float>(size_ + n) > static_cast<float>(bucket_size_) * max_load_factor()) {
        rehash(next_size(size_ + n));
    }
}

/// @brief 将 [first, last) 内的元素插入到 hashtable 中，键值允许重复
template <class T, class Hash, class KeyEqual, class Alloc>
template <class InputIterator>
void hashtable<T, Hash, KeyEqual, Alloc>::copy_insert_multi(InputIterator first, InputIterator last, 
    tinystl::input_iterator_tag) {
    rehash_if_need(tinystl::distance(first, last));
    for (; first != last; ++first) {
        insert_multi_noresize(*first);
    }
}

template <class T, class Hash, class KeyEqual, class Alloc>
template <class ForwardIterator>
void hashtable<T, Hash, KeyEqual, Alloc>::copy_insert_multi(ForwardIterator first, ForwardIterator last, 
    tinystl::forward_iterator_tag) {
    auto n = tinystl::distance(first, last);
    rehash_if_need(n);
    for (; n > 0; --n, ++first) {
        insert_multi_noresize(*first);
    }
}

/// @brief 将 [first, last) 内的元素插入到 hashtable 中，键值不允许重复
template <class T, class Hash, class KeyEqual, class Alloc>
template <class InputIterator>
void hashtable<T, Hash, KeyEqual, Alloc>::copy_insert_unique(InputIterator first, InputIterator last, 
    tinystl::input_iterator_tag) {
    rehash_if_need(tinystl::distance(first, last));
    for (; first != last; ++first) {
        insert_unique_noresize(*first);
    }
}

template <class T, class Hash, class KeyEqual, class Alloc>
template <class ForwardIterator>
void hashtable<T, Hash, KeyEqual, Alloc>::copy_insert_unique(ForwardIterator first, ForwardIterator last, 
    tinystl::forward_iterator_tag) {
    auto n = tinystl::distance(first, last);
    rehash_if_need(n);
    for (; n > 0; --n, ++first) {
        insert_unique_noresize(*first);
    }
}

/// @brief 在 hashtable 中插入一个节点，键值允许重复
template <class T, class Hash, class KeyEqual, class Alloc>
typename hashtable<T, Hash, KeyEqual, Alloc>::iterator
hashtable<T, Hash, KeyEqual, Alloc>::insert_node_multi(node_ptr node) {
    const auto n = hash(value_traits::get_key(node->value));
    auto cur = buckets_[n];
    if (cur == nullptr) {
        buckets_[n] = node;
        ++size_;
        return iterator(node, this);
    }
    for (; cur->next; cur = cur->next) {
        // 如果链表中存在相同键值的节点就马上插入，然后返回
        // 相同键值的节点放在一起，方便查找
        if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(node->value))) {
            node->next = cur->next;
            cur->next = node;
            ++size_;
            return iterator(node, this);
        }
    }
    // 否则插入在链表头部
    node->next = buckets_[n];  // 将新节点插入到链表头部
    buckets_[n] = node;        // 更新 bucket[n] 的头部
    ++size_;
    return iterator(node, this);
}

/// @brief 在 hashtable 中插入一个节点，键值不允许重复
template <class T, class Hash, class KeyEqual, class Alloc>
pair<typename hashtable<T, Hash, KeyEqual, Alloc>::iterator, bool>
hashtable<T, Hash, KeyEqual, Alloc>::insert_node_unique(node_ptr node) {
    const auto n = hash(value_traits::get_key(node->value));
    auto cur = buckets_[n];
    if (cur == nullptr) {
        buckets_[n] = node;
        ++size_;
        return tinystl::make_pair(iterator(node, this), true);
    }
    for (; cur; cur = cur->next) {
        // 如果链表中存在相同键值的节点就马上返回
        // 相同键值的节点放在一起，方便查找
        if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(node->value))) {
            return tinystl::make_pair(iterator(cur, this), false);
        }
    }
    // 否则插入在链表头部
    node->next = buckets_[n];  // 将新节点插入到链表头部
    buckets_[n] = node;        // 更新 bucket[n] 的头部
    ++size_;
    return tinystl::make_pair(iterator(node, this), true);
}

/// @brief 用新的桶替换旧的桶
template <class T, class Hash, class KeyEqual, class Alloc>
void hashtable<T, Hash, KeyEqual, Alloc>::replace_bucket(size_type bucket_count) {
    bucket_type bucket(bucket_count);
    if (size_ != 0) {
        // 遍历每一个 bucket
        for (size_type i = 0; i < bucket_size_; ++i) {
            // 遍历 bucket 中的每一个节点
            for (auto first = buckets_[i]; first; first = first->next) {
                auto tmp = create_node(first->value);
                // 计算新的哈希值，即新的 bucket 的位置
                const auto n = hash(value_traits::get_key(first->value), bucket_count);
                auto f = bucket[n];
                bool is_inserted = false;
                for (auto cur = f; cur; cur = cur->next) {
                    // 找到与当前节点相同键值的节点，插入到该节点之后
                    if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(tmp->value))) {
                        tmp->next = cur->next;
                        cur->next = tmp;
                        is_inserted = true;
                        break;
                    }
                }
                // 如果没有找到相同键值的节点，就插入到链表头部
                if (!is_inserted) {
                    tmp->next = f;
                    bucket[n] = tmp;
                }
            }
        }
    }
    buckets_.swap(bucket);
    bucket_size_ = buckets_.size();
}

/// @brief 在第 n 个 bucket 内，删除 [first, last) 内的节点
template <class T, class Hash, class KeyEqual, class Alloc>
void hashtable<T, Hash, KeyEqual, Alloc>::erase_bucket(size_type n, node_ptr first, node_ptr last) {
    auto cur = buckets_[n];
    if (cur == first) {
        erase_bucket(n, last);
    }
    else {
        auto next = cur->next;
        for (; next != first; cur = next, next = cur->next) {}
        while (next != last) {
            cur->next = next->next;
            destroy_node(next);
            next = cur->next;
            --size_;
        }
    }
}

/// @brief 在第 n 个 bucket 内，删除 [buckets_[n], last) 内的节点
template <class T, class Hash, class KeyEqual, class Alloc>
void hashtable<T, Hash, KeyEqual, Alloc>::erase_bucket(size_type n, node_ptr last) {
    auto cur = buckets_[n];
    while (cur != last) {
        auto next = cur->next;
        destroy_node(cur);
        cur = next;
        --size_;
    }
    buckets_[n] = last;
}

/// @brief 判断两个 hashtable 是否相等，键值允许重复
template <class T, class Hash, class KeyEqual, class Alloc>
bool hashtable<T, Hash, KeyEqual, Alloc>::equal_to_multi(const hashtable& rhs) {
    if (size_ != rhs.size_) return false;
    for (auto f = begin(), l = end(); f != l;) {
        auto p1 = equal_range_multi(value_traits::get_key(*f));
        auto p2 = rhs.equal_range_multi(value_traits::get_key(*f));
        if (tinystl::distance(p1.first, p1.second) != tinystl::distance(p2.first, p2.second) ||
            !tinystl::is_permutation(p1.first, p2.second, p2.first, p2.second)) {
            return false;
        }
        f = p1.second;
    }
    return true;
}

/// @brief 判断两个 hashtable 是否相等，键值不允许重复
template <class T, class Hash, class KeyEqual, class Alloc>
bool hashtable<T, Hash, KeyEqual, Alloc>::equal_to_unique(const hashtable& rhs) {
    if (size_ != rhs.size_) return false;
    for (auto f = begin(), l = end(); f != l; ++f) {
        auto res = rhs.find(value_traits::get_key(*f));
        if (res.node == nullptr || *res != *f) {
            return false;
        }
    }
    return true;
}

/// @brief 交换两个 hashtable
template <class T, class Hash, class KeyEqual, class Alloc>
void swap(hashtable<T, Hash, KeyEqual, Alloc>& lhs, hashtable<T, Hash, KeyEqual, Alloc>& rhs) noexcept {
    lhs.swap(rhs);
}

}  // namespace tinystl

#endif  // TINYSTL_HASHTABLE_H_