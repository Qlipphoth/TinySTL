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

template <class T, class HashFun, class KeyEqual>
class hashtable;

template <class T, class HashFun, class KeyEqual>
struct ht_iterator;

template <class T, class HashFun, class KeyEqual>
struct ht_const_iterator;

template <class T>
struct ht_const_local_iterator;


// ======================================= hashtable_iterator ====================================== //

template <class T, class Hash, class KeyEqual>
struct ht_iterator_base : public tinystl::iterator<tinystl::forward_iterator_tag, T> {
    typedef tinystl::hashtable<T, Hash, KeyEqual>         hashtable;
    typedef ht_iterator_base<T, Hash, KeyEqual>           base;
    typedef tinystl::ht_iterator<T, Hash, KeyEqual>       iterator;
    typedef tinystl::ht_const_iterator<T, Hash, KeyEqual> const_iterator;
    typedef hashtable_node<T>*                            node_ptr;
    typedef hashtable*                                    contain_ptr;
    typedef const node_ptr                                const_node_ptr;
    typedef const contain_ptr                             const_contain_ptr;

    typedef size_t                                        size_type;
    typedef ptrdiff_t                                     difference_type;

    node_ptr        node;  // 指向当前节点
    contain_ptr     ht;    // 指向哈希表本身

    ht_iterator_base() = default;

    bool operator==(const base& rhs) const { return node == rhs.node; }
    bool operator!=(const base& rhs) const { return node != rhs.node; }
};

template <class T, class Hash, class KeyEqual>
struct ht_iterator : public ht_iterator_base<T, Hash, KeyEqual> {
    typedef ht_iterator_base<T, Hash, KeyEqual> base;
    typedef typename base::hashtable            hashtable;
    typedef typename base::iterator             iterator;
    typedef typename base::const_iterator       const_iterator;
    typedef typename base::node_ptr             node_ptr;
    typedef typename base::contain_ptr          contain_ptr;

    typedef ht_value_traits<T>                  value_traits;
    typedef T                                   value_type;
    typedef value_type*                         pointer;
    typedef value_type&                         reference;

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
    reference operator*()  const { return node->value; }  // TODO: 这样做不会导致循环引用吗？
    pointer   operator->() const { return &(operator*()); }

    iterator& operator++() {
        TINYSTL_DEBUG(node != nullptr);
        const node_ptr old = node;
        node = node->next;  // 移动到下一个节点处
        // 如果下一个位置为空，说明已经到达链表尾部，需要跳到下一个 bucket
        if (node == nullptr) {
            auto index = ht->hash(value_traits::get_key(old_value));
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

template <class T, class Hash, class KeyEqual>
struct ht_const_iterator : public ht_iterator_base<T, Hash, KeyEqual> {
    typedef ht_iterator_base<T, Hash, KeyEqual> base;
    typedef typename base::hashtable            hashtable;
    typedef typename base::iterator             iterator;
    typedef typename base::const_iterator       const_iterator;
    typedef typename base::const_node_ptr       node_ptr;
    typedef typename base::const_contain_ptr    contain_ptr;

    typedef ht_value_traits<T>                  value_traits;
    typedef T                                   value_type;
    typedef const value_type*                   pointer;
    typedef const value_type&                   reference;

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
        MYSTL_DEBUG(node != nullptr);
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
template <class T, class Hash, class KeyEqual>
class hashtable {
    // 友元可以访问 private 成员
    friend struct ht_iterator<T, Hash, KeyEqual>;
    friend struct ht_const_iterator<T, Hash, KeyEqual>;

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

    typedef tinystl::allocator<T>                         allocator_type;
    typedef tinystl::allocator<T>                         data_allocator;
    typedef tinystl::allocator<node_type>                 node_allocator;

    typedef typename allocator_type::pointer              pointer;
    typedef typename allocator_type::const_pointer        const_pointer;
    typedef typename allocator_type::reference            reference;
    typedef typename allocator_type::const_reference      const_reference;
    typedef typename allocator_type::size_type            size_type;
    typedef typename allocator_type::difference_type      difference_type;

    typedef tinystl::ht_iterator<T, Hash, KeyEqual>       iterator;
    typedef tinystl::ht_const_iterator<T, Hash, KeyEqual> const_iterator;
    typedef tinystl::ht_local_iterator<T>                 local_iterator;
    typedef tinystl::ht_const_local_iterator<T>           const_local_iterator;

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
        for (size_type i = 0; i < bucket_size_; ++i) {
            // 找到第一个不为空的 bucket
            if (buckets_[i]) {
                return M_cit(buckets_[n]);
            }
        }
        return M_cit(buckets_[n]);
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

    ~hashtable() { clear(); }
















};



}  // namespace tinystl

#endif  // TINYSTL_HASHTABLE_H_