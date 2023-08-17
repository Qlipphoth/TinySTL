#ifndef TINYSTL_MAP_H_
#define TINYSTL_MAP_H_

// 这个头文件包含了两个模板类 map 和 multimap
// map      : 映射，元素具有键值和实值，会根据键值大小自动排序，键值不允许重复
// multimap : 映射，元素具有键值和实值，会根据键值大小自动排序，键值允许重复

// notes:
//
// 异常保证：
// mystl::map<Key, T> / mystl::multimap<Key, T> 满足基本异常保证，对以下等函数做强异常安全保证：
//   * emplace
//   * emplace_hint
//   * insert

#include "rb_tree.h"

namespace tinystl {

// ============================================= map ============================================= //

/// @brief 模板类 map，键值不允许重复
/// @tparam Key  键值类型
/// @tparam T  实值类型
/// @tparam Compare  键值比较方式，缺省使用 tinystl::less
template <class Key, class T, class Compare = tinystl::less<Key>>
class map {

public:  // map 的嵌套型别定义
    typedef Key                             key_type;
    typedef T                               mapped_type;
    typedef tinystl::pair<const Key, T>     value_type;
    typedef Compare                         key_compare;

public:  // 用于比较两个元素的仿函数
    class value_compare : public tinystl::binary_function<value_type, value_type, bool> {
        friend class map<Key, T, Compare>;
    private:
        Compare comp;
        value_compare(Compare c) : comp(c) {}
    public:
        bool operator()(const value_type& lhs, const value_type& rhs) const {
            return comp(lhs.first, rhs.first);  // 比较键值的大小
        }
    };

private:  // 以 tinystl::rb_tree 作为底层机制
    typedef tinystl::rb_tree<value_type, key_compare> base_type;
    base_type tree_;  // 底层红黑树

public:  // 使用 rb_tree 定义的型别
    typedef typename base_type::node_type              node_type;
    
    // map 不允许修改键值，但允许修改实值
    typedef typename base_type::pointer                pointer;
    typedef typename base_type::const_pointer          const_pointer;
    typedef typename base_type::reference              reference;
    typedef typename base_type::const_reference        const_reference;
    typedef typename base_type::iterator               iterator;
    typedef typename base_type::const_iterator         const_iterator;
    typedef typename base_type::reverse_iterator       reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    typedef typename base_type::size_type              size_type;
    typedef typename base_type::difference_type        difference_type;
    typedef typename base_type::allocator_type         allocator_type;

public:  // 构造、复制、移动、赋值函数
    map() = default;

    template <class InputIterator>
    map(InputIterator first, InputIterator last) : tree_() {
        tree_.insert_unique(first, last);
    }

    map(std::initializer_list<value_type> ilist) : tree_() {
        tree_.insert_unique(ilist.begin(), ilist.end());
    }

    map(const map& rhs) : tree_(rhs.tree_) {}

    map(map&& rhs) noexcept : tree_(tinystl::move(rhs.tree_)) {}

    map& operator=(const map& rhs) {
        tree_ = rhs.tree_;
        return *this;
    }

    map& operator=(map&& rhs) noexcept {
        tree_ = tinystl::move(rhs.tree_);
        return *this;
    }

    map& operator=(std::initializer_list<value_type> ilist) {
        tree_.clear();
        tree_.insert_unique(ilist.begin(), ilist.end());
        return *this;
    }

public:  // 相关接口
    key_compare    key_comp()      const { return tree_.key_comp(); }
    value_compare  value_comp()    const { return value_compare(tree_.key_comp()); }
    allocator_type get_allocator() const { return tree_.get_allocator(); }

public:  // 迭代器相关操作
    iterator               begin()        noexcept { return tree_.begin(); }
    const_iterator         begin()  const noexcept { return tree_.begin(); }
    iterator               end()          noexcept { return tree_.end(); }
    const_iterator         end()    const noexcept { return tree_.end(); }
    reverse_iterator       rbegin()       noexcept { return tree_.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return tree_.rbegin(); }
    reverse_iterator       rend()         noexcept { return tree_.rend(); }
    const_reverse_iterator rend()   const noexcept { return tree_.rend(); }

    const_iterator         cbegin()  const noexcept { return tree_.cbegin(); }
    const_iterator         cend()    const noexcept { return tree_.cend(); }
    const_reverse_iterator crbegin() const noexcept { return tree_.crbegin(); }
    const_reverse_iterator crend()   const noexcept { return tree_.crend(); }

public:  // 容量相关操作
    bool                   empty()    const noexcept { return tree_.empty(); }
    size_type              size()     const noexcept { return tree_.size(); }
    size_type              max_size() const noexcept { return tree_.max_size(); }

public:  // 访问元素相关
    
    /// @brief 访问以 key 为键值的实值，若不存在则抛出异常
    /// @param key  键值
    /// @return 实值
    mapped_type& at(const key_type& key) {
        // it 指向大于等于 key 的第一个节点的位置，若 map 中存在 key，则这个节点的键值一定等于 key
        // 否则，map 中不存在 key，抛出异常
        iterator it = lower_bound(key);
        THROW_OUT_OF_RANGE_IF(it == end() || key_comp()(it->first, key),
            "map<Key, T> no such element exists");
        return it->second;
    }

    const mapped_type& at(const key_type& key) const {
        iterator it = lower_bound(key);
        THROW_OUT_OF_RANGE_IF(it == end() || key_comp()(it->first, key),
            "map<Key, T> no such element exists");
        return it->second;
    }

    /// @brief 访问以 key 为键值的实值，若不存在则插入一个新节点
    /// @param key 键值 
    /// @return  实值
    mapped_type& operator[](const key_type& key) {
        iterator it = lower_bound(key);
        // 若不存在 key，则插入一个新节点
        if (it == end() || key_comp()(key, it->first)) {
            // 默认构造一个 mapped_type 类型的右值，作为新节点的实值
            it = emplace_hint(it, key, mapped_type());
        }
        return it->second;
    }

    mapped_type& operator[](key_type&& key) {
        iterator it = lower_bound(key);
        if (it == end() || key_comp()(key, it->first)) {
            it = emplace_hint(it, tinystl::move(key), mapped_type());
        }
        return it->second;
    }

public:  // 插入删除相关，调用 rb_tree 的接口

    template <class ...Args>
    tinystl::pair<iterator, bool> emplace(Args&& ...args) {
        return tree_.emplace_unique(tinystl::forward<Args>(args)...);
    }

    template <class ...Args>
    iterator emplace_hint(const_iterator hint, Args&& ...args) {
        return tree_.emplace_unique_use_hint(hint, tinystl::forward<Args>(args)...);
    }

    tinystl::pair<iterator, bool> insert(const value_type& value) {
        return tree_.insert_unique(value);
    }

    tinystl::pair<iterator, bool> insert(value_type&& value) {
        return tree_.insert_unique(tinystl::move(value));
    }

    iterator insert(iterator hint, const value_type& value) {
        return tree_.insert_unique(hint, value);
    }

    iterator insert(iterator hint, value_type&& value) {
        return tree_.insert_unique(hint, tinystl::move(value));
    }

    template <class InputIterator>
    void insert (InputIterator first, InputIterator last) {
        tree_.insert_unique(first, last);
    }

    void erase(iterator pos) { tree_.erase(pos); }
    size_type erase(const key_type& key) { return tree_.erase_unique(key); }
    void erase(iterator first, iterator last) { tree_.erase(first, last); }

    void clear() { tree_.clear(); }

public:  // map 相关操作
    iterator        find(const key_type& key)                { return tree_.find(key); }
    const_iterator  find(const key_type& key)          const { return tree_.find(key); }
    
    size_type       count(const key_type& key)         const { return tree_.count_unique(key); }
    
    iterator        lower_bound(const key_type& key)         { return tree_.lower_bound(key); }
    const_iterator  lower_bound(const key_type& key)   const { return tree_.lower_bound(key); }

    iterator        upper_bound(const key_type& key)         { return tree_.upper_bound(key); }
    const_iterator  upper_bound(const key_type& key)   const { return tree_.upper_bound(key); }

    tinystl::pair<iterator, iterator> equal_range(const key_type& key) {
        return tree_.equal_range_unique(key);
    }

    tinystl::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return tree_.equal_range_unique(key);
    }

    void swap(map& rhs) noexcept { tree_.swap(rhs.tree_); }

public:
    friend bool operator==(const map& lhs, const map& rhs) { return lhs.tree_ == rhs.tree_; }
    friend bool operator< (const map& lhs, const map& rhs) { return lhs.tree_ <  rhs.tree_; }
};

// 重载比较操作符

template <class Key, class T, class Compare>
bool operator==(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs) {
    return lhs == rhs;
}

template <class Key, class T, class Compare>
bool operator<(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs) {
    return lhs < rhs;
}

template <class Key, class T, class Compare>
bool operator!=(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs) {
    return !(lhs == rhs);
}

template <class Key, class T, class Compare>
bool operator>(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs) {
    return rhs < lhs;
}

template <class Key, class T, class Compare>
bool operator<=(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs) {
    return !(rhs < lhs);
}

template <class Key, class T, class Compare>
bool operator>=(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs) {
    return !(lhs < rhs);
}

// 重载 swap
template <class Key, class T, class Compare>
void swap(map<Key, T, Compare>& lhs, map<Key, T, Compare>& rhs) noexcept {
    lhs.swap(rhs);
}


// ========================================== multimap ========================================== //

/// @brief 模板类 multimap，键值允许重复
/// @tparam Key  键值类型
/// @tparam T  实值类型
/// @tparam Compare  键值比较方式，缺省使用 tinystl::less
template <class Key, class T, class Compare = tinystl::less<Key>>
class multimap {

public:  // multimap 的嵌套型别定义
    typedef Key                             key_type;
    typedef T                               mapped_type;
    typedef tinystl::pair<const Key, T>     value_type;
    typedef Compare                         key_compare;

public:  // 用于比较两个元素的仿函数
    class value_compare : public tinystl::binary_function<value_type, value_type, bool> {
        friend class multimap<Key, T, Compare>;
    private:
        Compare comp;
        value_compare(Compare c) : comp(c) {}
    public:
        bool operator()(const value_type& lhs, const value_type& rhs) const {
            return comp(lhs.first, rhs.first);  // 比较键值的大小
        }
    };

private:  // 以 tinystl::rb_tree 作为底层机制
    typedef tinystl::rb_tree<value_type, key_compare> base_type;
    base_type tree_;  // 底层红黑树

public:  // 使用 rb_tree 定义的型别
    typedef typename base_type::node_type              node_type;
    
    // multimap 不允许修改键值，但允许修改实值
    typedef typename base_type::pointer                pointer;
    typedef typename base_type::const_pointer          const_pointer;
    typedef typename base_type::reference              reference;
    typedef typename base_type::const_reference        const_reference;
    typedef typename base_type::iterator               iterator;
    typedef typename base_type::const_iterator         const_iterator;
    typedef typename base_type::reverse_iterator       reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    typedef typename base_type::size_type              size_type;
    typedef typename base_type::difference_type        difference_type;
    typedef typename base_type::allocator_type         allocator_type;

public:  // 构造、复制、移动、赋值函数
    multimap() = default;

    template <class InputIterator>
    multimap(InputIterator first, InputIterator last) : tree_() {
        tree_.insert_multi(first, last);
    }

    multimap(std::initializer_list<value_type> ilist) : tree_() {
        tree_.insert_unique(ilist.begin(), ilist.end());
    }

    multimap(const multimap& rhs) : tree_(rhs.tree_) {}

    multimap(multimap&& rhs) noexcept : tree_(tinystl::move(rhs.tree_)) {}

    multimap& operator=(const multimap& rhs) {
        tree_ = rhs.tree_;
        return *this;
    }

    multimap& operator=(multimap&& rhs) noexcept {
        tree_ = tinystl::move(rhs.tree_);
        return *this;
    }

    multimap& operator=(std::initializer_list<value_type> ilist) {
        tree_.clear();
        tree_.insert_unique(ilist.begin(), ilist.end());
        return *this;
    }

public:  // 相关接口
    key_compare    key_comp()      const { return tree_.key_comp(); }
    value_compare  value_comp()    const { return value_compare(tree_.key_comp()); }
    allocator_type get_allocator() const { return tree_.get_allocator(); }

public:  // 迭代器相关操作
    iterator               begin()        noexcept { return tree_.begin(); }
    const_iterator         begin()  const noexcept { return tree_.begin(); }
    iterator               end()          noexcept { return tree_.end(); }
    const_iterator         end()    const noexcept { return tree_.end(); }
    reverse_iterator       rbegin()       noexcept { return tree_.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return tree_.rbegin(); }
    reverse_iterator       rend()         noexcept { return tree_.rend(); }
    const_reverse_iterator rend()   const noexcept { return tree_.rend(); }

    const_iterator         cbegin()  const noexcept { return tree_.cbegin(); }
    const_iterator         cend()    const noexcept { return tree_.cend(); }
    const_reverse_iterator crbegin() const noexcept { return tree_.crbegin(); }
    const_reverse_iterator crend()   const noexcept { return tree_.crend(); }

public:  // 容量相关操作
    bool                   empty()    const noexcept { return tree_.empty(); }
    size_type              size()     const noexcept { return tree_.size(); }
    size_type              max_size() const noexcept { return tree_.max_size(); }

// 由于允许多个键值相同，所以不提供访问元素相关的接口

public:  // 插入删除相关，调用 rb_tree 的接口

    template <class ...Args>
    iterator emplace(Args&& ...args) {
        return tree_.emplace_multi(tinystl::forward<Args>(args)...);
    }

    template <class ...Args>
    iterator emplace_hint(const_iterator hint, Args&& ...args) {
        return tree_.emplace_multi_use_hint(hint, tinystl::forward<Args>(args)...);
    }

    iterator insert(const value_type& value) {
        return tree_.insert_multi(value);
    }

    iterator insert(value_type&& value) {
        return tree_.insert_multi(tinystl::move(value));
    }

    iterator insert(iterator hint, const value_type& value) {
        return tree_.insert_multi(hint, value);
    }

    iterator insert(iterator hint, value_type&& value) {
        return tree_.insert_multi(hint, tinystl::move(value));
    }

    template <class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        tree_.insert_multi(first, last);
    }

    void erase(iterator pos) { tree_.erase(pos); }
    size_type erase(const key_type& key) { return tree_.erase_unique(key); }
    void erase(iterator first, iterator last) { tree_.erase(first, last); }

    void clear() { tree_.clear(); }

public:  // multimap 相关操作
    iterator        find(const key_type& key)                { return tree_.find(key); }
    const_iterator  find(const key_type& key)          const { return tree_.find(key); }
    
    size_type       count(const key_type& key)         const { return tree_.count_multi(key); }
    
    iterator        lower_bound(const key_type& key)         { return tree_.lower_bound(key); }
    const_iterator  lower_bound(const key_type& key)   const { return tree_.lower_bound(key); }

    iterator        upper_bound(const key_type& key)         { return tree_.upper_bound(key); }
    const_iterator  upper_bound(const key_type& key)   const { return tree_.upper_bound(key); }

    tinystl::pair<iterator, iterator> equal_range(const key_type& key) {
        return tree_.equal_range_multi(key);
    }

    tinystl::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return tree_.equal_range_multi(key);
    }

    void swap(multimap& rhs) noexcept { tree_.swap(rhs.tree_); }

public:
    friend bool operator==(const multimap& lhs, const multimap& rhs) { return lhs.tree_ == rhs.tree_; }
    friend bool operator< (const multimap& lhs, const multimap& rhs) { return lhs.tree_ <  rhs.tree_; }
};

// 重载比较操作符

template <class Key, class T, class Compare>
bool operator==(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs) {
    return lhs == rhs;
}

template <class Key, class T, class Compare>
bool operator<(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs) {
    return lhs < rhs;
}

template <class Key, class T, class Compare>
bool operator!=(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs) {
    return !(lhs == rhs);
}

template <class Key, class T, class Compare>
bool operator>(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs) {
    return rhs < lhs;
}

template <class Key, class T, class Compare>
bool operator<=(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs) {
    return !(rhs < lhs);
}

template <class Key, class T, class Compare>
bool operator>=(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs) {
    return !(lhs < rhs);
}

// 重载 swap
template <class Key, class T, class Compare>
void swap(multimap<Key, T, Compare>& lhs, multimap<Key, T, Compare>& rhs) noexcept {
    lhs.swap(rhs);
}

}  // namespace tinystl

#endif // !TINYSTL_MAP_H_