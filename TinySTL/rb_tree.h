#ifndef TINYSTL_RB_TREE_H_
#define TINYSTL_RB_TREE_H_

// 这个头文件包含一个模板类 rb_tree
// rb_tree : 红黑树

#include <initializer_list>

#include <cassert>

#include "functional.h"
#include "iterator.h"
#include "memory.h"
#include "type_traits.h"
#include "exceptdef.h"

namespace tinystl {

// 红黑树的节点颜色

typedef bool rb_tree_color_type;

static constexpr rb_tree_color_type rb_tree_red = false;   // 红色为 0
static constexpr rb_tree_color_type rb_tree_black = true;  // 黑色为 1

template <class T> struct rb_tree_node_base;  // 定义了 rb-tree 的指针
template <class T> struct rb_tree_node;       // 继承了前者，增加了 value，代表一个完整节点

template <class T> struct rb_tree_iterator;
template <class T> struct rb_tree_const_iterator;

// =====================================  rb-tree value traits ===================================== //

template <class T, bool>
struct rb_tree_value_traits_imp {
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

template <class T>
struct rb_tree_value_traits_imp<T, true> {
    typedef typename std::remove_cv<typename T::first_type>::type  key_type;
    typedef typename T::second_type                                mapped_type;
    typedef T                                                      value_type;

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
struct rb_tree_value_traits {
    static constexpr bool is_map = tinystl::is_pair<T>::value;

    typedef rb_tree_value_traits_imp<T, is_map>     value_traits_type;
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

// =====================================  rb-tree node traits ===================================== //

template <class T>
struct rb_tree_node_traits {
    typedef rb_tree_color_type                  color_type;

    typedef rb_tree_value_traits<T>             value_traits;
    typedef typename value_traits::key_type     key_type;
    typedef typename value_traits::mapped_type  mapped_type;
    typedef typename value_traits::value_type   value_type;

    typedef rb_tree_node_base<T>*               base_ptr;
    typedef rb_tree_node<T>*                    node_ptr;
};

// =====================================  rb-tree node base ===================================== //

template <class T>
struct rb_tree_node_base {
    typedef rb_tree_color_type                  color_type;
    typedef rb_tree_node_base<T>*               base_ptr;
    typedef rb_tree_node<T>*                    node_ptr;

    base_ptr parent;      // 父节点
    base_ptr left;        // 左子节点
    base_ptr right;       // 右子节点
    color_type color;     // 节点颜色

    base_ptr get_base_ptr() {
        return &*this;
    }

    node_ptr get_node_ptr() {
        return static_cast<node_ptr>(get_base_ptr());  // TODO: 试试 static_cast 行不行
    }

    node_ptr& get_node_ref() {
        return static_cast<node_ptr&>(*this);
    }
};

// =====================================  rb-tree node ===================================== //

template <class T>
struct rb_tree_node : public rb_tree_node_base<T> {
    typedef rb_tree_node_base<T>*              base_ptr;                
    typedef rb_tree_node<T>*                   node_ptr;

    T value;  // 节点的值

    base_ptr get_base_ptr() {
        return static_cast<base_ptr>(&*this);
    }

    node_ptr get_node_ptr() {
        return &*this;
    }
};

// =====================================  rb-tree traits ===================================== //

template <class T>
struct rb_tree_traits {
    typedef rb_tree_value_traits<T>            value_traits;

    typedef typename value_traits::key_type    key_type;
    typedef typename value_traits::mapped_type mapped_type;
    typedef typename value_traits::value_type  value_type;

    typedef value_type*                        pointer;
    typedef const value_type*                  const_pointer;
    typedef value_type&                        reference;
    typedef const value_type&                  const_reference;

    typedef rb_tree_node_base<T>               base_type;
    typedef rb_tree_node<T>                    node_type;

    typedef base_type*                         base_ptr;
    typedef node_type*                         node_ptr;
};

// =================================  rb-tree iterator base =================================== //

template <class T>
struct rb_tree_iterator_base : public tinystl::iterator<tinystl::bidirectional_iterator_tag, T> {
    typedef typename rb_tree_traits<T>::base_ptr base_ptr;

    base_ptr node;  // 指向 rb-tree 的节点

    rb_tree_iterator_base() : node(nullptr) {}  // 默认构造函数

    // 使迭代器前进
    void inc() {
        if (node->right != nullptr) {
            node = rb_tree_min(node->right);
        }
        // 如果没有右子节点
        else {
            auto p = node->parent;
            while (node == p->right) {
                node = p;
                p = p->parent;
            }
            // // 应对“寻找根节点的下一节点，而根节点没有右子节点”的特殊情况
            if (node->right != p) {
                node = p;
            }
        }
    }

    // 使迭代器后退
    void dec() {
        // 如果 node 为 header，则指向整棵树的 max 节点
        if (node->parent->parent == node && rb_tree_is_red(node)) {
            node = node->right;
        }
        else if (node->left != nullptr) {
            node = rb_tree_max(node->left);
        }
        // 非 header 节点，也无左子节点
        else {
            auto p = node->parent;
            while (node == p->left) {
                node = p;
                p = p->parent;
            }
            node = p;
        }
    }

    bool operator==(const rb_tree_iterator_base& rhs) { return node == rhs.node; }
    bool operator!=(const rb_tree_iterator_base& rhs) { return node != rhs.node; }
};

// =================================  rb-tree iterator =================================== //

template <class T>
struct rb_tree_iterator : public rb_tree_iterator_base<T> {
    typedef rb_tree_traits<T>                tree_traits;

    typedef typename tree_traits::value_type value_type;
    typedef typename tree_traits::pointer    pointer;
    typedef typename tree_traits::reference  reference;
    typedef typename tree_traits::base_ptr   base_ptr;
    typedef typename tree_traits::node_ptr   node_ptr;

    typedef rb_tree_iterator<T>              iterator;
    typedef rb_tree_const_iterator<T>        const_iterator;
    typedef iterator                         self;

    using rb_tree_iterator_base<T>::node = node;  // TODO: 试试这样行不行

    // 构造函数
    rb_tree_iterator() {}
    rb_tree_iterator(base_ptr x) { node = x; }
    rb_tree_iterator(node_ptr x) { node = x; }
    rb_tree_iterator(const iterator& rhs) { node = rhs.node; }
    rb_tree_iterator(const const_iterator& rhs) { node = rhs.node; }

    // 重载操作符
    reference operator*() const { return node->get_node_ptr()->value; }  // 获得节点的值
    pointer operator->() const { return &(operator*()); }                // 获得节点的值的指针

    self& operator++() {
        this->inc();
        return *this;
    }

    self operator++(int) {
        self tmp(*this);
        this->inc();
        return tmp;
    }

    self& operator--() {
        this->dec();
        return *this;
    }

    self operator--(int) {
        self tmp(*this);
        this->dec();
        return tmp;
    }
};

template <class T>
struct rb_tree_const_iterator : public rb_tree_iterator_base<T> {
    typedef rb_tree_traits<T>                     tree_traits;

    typedef typename tree_traits::value_type      value_type;
    typedef typename tree_traits::const_pointer   pointer;
    typedef typename tree_traits::const_reference reference;
    typedef typename tree_traits::base_ptr        base_ptr;
    typedef typename tree_traits::node_ptr        node_ptr;

    typedef rb_tree_iterator<T>                   iterator;
    typedef rb_tree_const_iterator<T>             const_iterator;
    typedef const_iterator                        self;

    using rb_tree_iterator_base<T>::node = node;

    // 构造函数
    rb_tree_const_iterator() {}
    rb_tree_const_iterator(base_ptr x) { node = x; }
    rb_tree_const_iterator(node_ptr x) { node = x; }
    rb_tree_const_iterator(const iterator& rhs) { node = rhs.node; }
    rb_tree_const_iterator(const const_iterator& rhs) { node = rhs.node; }

    // 重载操作符
    reference operator*()  const { return node->get_node_ptr()->value; }
    pointer   operator->() const { return &(operator*()); }

    self& operator++() {
        this->inc();
        return *this;
    }

    self operator++(int) {
        self tmp(*this);
        this->inc();
        return tmp;
    }

    self& operator--() {
        this->dec();
        return *this;
    }

    self operator--(int) {
        self tmp(*this);
        this->dec();
        return tmp;
    }
};

}

#endif