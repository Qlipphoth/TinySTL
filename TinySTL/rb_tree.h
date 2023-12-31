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

// 已弃用
// template <class T>
// struct rb_tree_node_traits {
//     typedef rb_tree_color_type                  color_type;

//     typedef rb_tree_value_traits<T>             value_traits;
//     typedef typename value_traits::key_type     key_type;
//     typedef typename value_traits::mapped_type  mapped_type;
//     typedef typename value_traits::value_type   value_type;

//     typedef rb_tree_node_base<T>*               base_ptr;
//     typedef rb_tree_node<T>*                    node_ptr;
// };

// =====================================  rb-tree node base ===================================== //

template <class T>
struct rb_tree_node_base {
    typedef rb_tree_color_type                  color_type;
    typedef rb_tree_node_base<T>*               base_ptr;
    typedef rb_tree_node<T>*                    node_ptr;

    base_ptr   parent;      // 父节点
    base_ptr   left;        // 左子节点
    base_ptr   right;       // 右子节点
    color_type color;       // 节点颜色

    // base_ptr get_base_ptr() {
    //     return &*this;
    // }

    // 已弃用
    // node_ptr get_node_ptr() {
    //     // return static_cast<node_ptr>(get_base_ptr());
    //     return reinterpret_cast<node_ptr>(&*this);
    // }

    // 已弃用
    // node_ptr& get_node_ref() {
    //     // return static_cast<node_ptr&>(*this);
    //     return reinterpret_cast<node_ptr&>(*this);
    // }
};

// =====================================  rb-tree node ===================================== //

template <class T>
struct rb_tree_node : public rb_tree_node_base<T> {
    typedef rb_tree_node_base<T>*              base_ptr;                
    typedef rb_tree_node<T>*                   node_ptr;

    T value;  // 节点的值

    // base_ptr get_base_ptr() {
    //     return static_cast<base_ptr>(&*this);
    // }

    // 已弃用
    // node_ptr get_node_ptr() {
    //     return &*this;
    // }
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
            node = node->right;  // 指向整棵树的 max 节点
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
    typedef typename tree_traits::node_ptr   node_ptr;  // link_type

    typedef rb_tree_iterator<T>              iterator;
    typedef rb_tree_const_iterator<T>        const_iterator;
    typedef iterator                         self;

    using rb_tree_iterator_base<T>::node;

    // 构造函数
    rb_tree_iterator() {}
    rb_tree_iterator(base_ptr x) { node = x; }
    rb_tree_iterator(node_ptr x) { node = x; }
    rb_tree_iterator(const iterator& rhs) { node = rhs.node; }
    rb_tree_iterator(const const_iterator& rhs) { node = rhs.node; }

    // 重载操作符
    // reference operator*() const { return node->get_node_ptr()->value; }  // 获得节点的值
    reference operator*()  const { return static_cast<node_ptr>(node)->value; }  // 获得节点的值
    pointer   operator->() const { return &(operator*()); }                       // 获得节点的值的指针

    rb_tree_iterator& operator=(const iterator& rhs) {
        if (this != &rhs) {
            node = rhs.node;
        }
        return *this;
    }

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
    typedef typename tree_traits::node_ptr        node_ptr;  // link_type

    typedef rb_tree_iterator<T>                   iterator;
    typedef rb_tree_const_iterator<T>             const_iterator;
    typedef const_iterator                        self;

    using rb_tree_iterator_base<T>::node;  // 见 note

    // 构造函数
    rb_tree_const_iterator() {}
    rb_tree_const_iterator(base_ptr x) { node = x; }
    rb_tree_const_iterator(node_ptr x) { node = x; }
    rb_tree_const_iterator(const iterator& rhs) { node = rhs.node; }
    rb_tree_const_iterator(const const_iterator& rhs) { node = rhs.node; }

    // 重载操作符
    // reference operator*()  const { return node->get_node_ptr()->value; }
    reference operator*()  const { return static_cast<node_ptr>(node)->value; }
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

// =================================== tree-algorithm =================================== //

/// @brief 查找最小节点
template <class NodePtr>
NodePtr rb_tree_min(NodePtr x) noexcept {
    while (x->left != nullptr) x = x->left;
    return x;
}

/// @brief 查找最大节点
template <class NodePtr>
NodePtr rb_tree_max(NodePtr x) noexcept {
    while (x->right != nullptr) x = x->right;
    return x;
}

/// @brief 判断该节点是否为左儿子
template <class NodePtr>
bool rb_tree_is_lchild(NodePtr x) noexcept {
    return x == x->parent->left;
}

/// @brief 判断该节点的颜色是否为红色
template <class NodePtr>
bool rb_tree_is_red(NodePtr x) noexcept {
    return x->color == rb_tree_red;
}

/// @brief 将节点染成黑色
template <class NodePtr>
void rb_tree_set_black(NodePtr x) noexcept {
    x->color = rb_tree_black;
}

/// @brief 将节点染成红色
template <class NodePtr>
void rb_tree_set_red(NodePtr x) noexcept {
    x->color = rb_tree_red;
}

/// @brief 获得 rb-tree 的下一个节点
template <class NodePtr>
NodePtr rb_tree_next(NodePtr x) noexcept {
    if (x->right != nullptr) return rb_tree_min(x->right);
    while (!rb_tree_is_lchild(x)) x = x->parent;
    return x->parent;
}

// ============== rb-tree rotate ============== //
// 根节点为引用传参！！！根节点为引用传参！！！根节点为引用传参！！！
// 要修改根节点的值，必须传引用！！！

/*---------------------------------------*\
|       p                         p       |
|      / \                       / \      |
|     x   d    rotate left      y   d     |
|    / \       ===========>    / \        |
|   a   y                     x   c       |
|      / \                   / \          |
|     b   c                 a   b         |
\*---------------------------------------*/
// 左旋，参数一为左旋点，参数二为根节点
template <class NodePtr>
void rb_tree_rotate_left(NodePtr x, NodePtr& root) noexcept {
    auto y = x->right;
    x->right = y->left;
    if (y->left != nullptr) y->left->parent = x;
    y->parent = x->parent;

    // 如果 x 为根节点，让 y 顶替 x 成为根节点
    if (x == root) root = y;
    else if (rb_tree_is_lchild(x)) x->parent->left = y;
    else x->parent->right = y;

    // 调整 x 与 y 的关系
    y->left = x;
    x->parent = y;
}

/*----------------------------------------*\
|     p                         p          |
|    / \                       / \         |
|   d   x      rotate right   d   y        |
|      / \     ===========>      / \       |
|     y   a                     b   x      |
|    / \                           / \     |
|   b   c                         c   a    |
\*----------------------------------------*/
// 右旋，参数一为右旋点，参数二为根节点
template <class NodePtr>
void rb_tree_rotate_right(NodePtr x, NodePtr& root) noexcept {
    auto y = x->left;
    x->left = y->right;
    if (y->right != nullptr) y->right->parent = x;
    y->parent = x->parent;

    // 如果 x 为根节点，让 y 顶替 x 成为根节点
    if (x == root) root = y;
    else if (rb_tree_is_lchild(x)) x->parent->left = y;
    else x->parent->right = y;

    // 调整 x 与 y 的关系
    y->right = x;
    x->parent = y;
}

// ================== rb-tree insert ================== //
// 插入节点后使 rb tree 重新平衡，参数一为新增节点，参数二为根节点
// 插入节点分为五种情况：

// 1. 插入节点为根节点，直接将节点颜色置为黑色。
// 2. 插入节点的父节点为黑色，不需要做任何操作。
// 3. 插入节点的父节点为红色，需要进行调整。
//     1. 插入节点的叔叔节点为红色，将父节点和叔叔节点置为黑色，祖父节点置为红色，将祖父节点作为新的插入节点，继续调整（因为此时祖父节点可能也不满足性质 4）。
//     2. 插入节点的叔叔节点为黑色，且插入节点与父节点同侧（LL/RR），以祖父节点为支点进行反向旋转（R/L），将父节点置为黑色，祖父节点置为红色。
//     3. 插入节点的叔叔节点为黑色，且插入节点与父节点异侧（LR/RL），以父节点为支点进行反向旋转（L/R），转化为 3.2。

template <class NodePtr>
void rb_tree_insert_rebalance(NodePtr x, NodePtr& root) noexcept {
    rb_tree_set_red(x);  // 新增节点为红色
    // 父节点为黑色不用处理，case2
    while (x != root && rb_tree_is_red(x->parent)) {
        // 如果父节点为左子节点
        if (rb_tree_is_lchild(x->parent)) {
            auto uncle = x->parent->parent->right;  // 叔叔节点
            // uncle 红色，case3.1
            if (uncle != nullptr && rb_tree_is_red(uncle)) {
                rb_tree_set_black(x->parent);  // 父节点变为黑色
                rb_tree_set_black(uncle);      // 叔叔节点变为黑色
                rb_tree_set_red(x);            // 祖父节点变为红色
                x = x->parent->parent;         // 祖父节点变为当前节点
            }
            // 无叔叔节点或叔叔节点为黑
            else {
                // LR, case3.3 转化为 case3.2
                if (!rb_tree_is_lchild(x)) {
                    x = x->parent;                 // 父节点变为当前节点
                    rb_tree_rotate_left(x, root);  // 左旋
                }
                // case 3.2
                rb_tree_set_black(x->parent);        // 父节点变为黑色
                rb_tree_set_red(x->parent->parent);  // 祖父节点变为红色
                rb_tree_rotate_right(x->parent->parent, root);  // 右旋
                break;
            }
        }
        // 对称处理
        else {
            auto uncle = x->parent->parent->left;    // 叔叔节点
            // uncle 红色，case3.1
            if (uncle != nullptr && rb_tree_is_red(uncle)) {
                rb_tree_set_black(x->parent);  // 父节点变为黑色
                rb_tree_set_black(uncle);      // 叔叔节点变为黑色
                x = x->parent->parent;         // 祖父节点变为当前节点
                rb_tree_set_red(x);            // 祖父节点变为红色
            }
            // 无叔叔节点或叔叔节点为黑
            else {
                // RL, case3.3 转化为 case3.2
                if (rb_tree_is_lchild(x)) {
                    x = x->parent;                  // 父节点变为当前节点
                    rb_tree_rotate_right(x, root);  // 右旋
                }
                // case 3.2
                rb_tree_set_black(x->parent);        // 父节点变为黑色
                rb_tree_set_red(x->parent->parent);  // 祖父节点变为红色
                rb_tree_rotate_left(x->parent->parent, root);  // 左旋
                break;
            }
        }
    }
    rb_tree_set_black(root);  // 根节点为黑色
}

// ================== rb-tree erase ================== //
// 删除节点后使 rb tree 重新平衡，参数一为要删除的节点，参数二为根节点，参数三为最小节点，参数四为最大节点
template <class NodePtr>
NodePtr rb_tree_erase_rebalance(NodePtr z, NodePtr& root, NodePtr& leftmost, NodePtr& rightmost) {
    
    // =============================== 1. 找到要删除的节点 =============================== //
    // 1. 删除只有一个子树的节点：删除原节点，用单独的子节点代替被删除节点。
    // 2. 删除没有子树的节点：直接删除原节点，相当于用一个空节点 (NIL) 代替了被删除节点。
    // 3. 删除有两个子树的节点：寻找后继或前继节点（这里选择后继），将待删除节点的值变为后继节点的值，
    // 再删除后继节点，删除后继节点一定会变成 1 或 2 的情况。

    // y 是可能的替换节点，指向最终要删除的节点
    auto y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
    // x 是 y 的一个独子节点或 NIL 节点
    auto x = y->left != nullptr ? y->left : y->right;
    // xp 是 x 的父节点
    NodePtr xp = nullptr;

    // case3
    // y != z 说明 z 有两个非空子节点，此时 y 是 z 的后继节点，x 指向 y 的非空子节点。
    // 用 y 顶替 z 的位置，用 x 顶替 y 的位置，最后用 y 指向 z
    if (y != z) {
        z->left->parent = y;
        y->left = z->left;

        // 如果 y 不是 z 的右子节点，那么 z 的右子节点一定有左孩子
        // x 替换 y 的位置
        if (y != z->right) {
            xp = y->parent;
            if (x != nullptr) x->parent = y->parent;
            y->parent->left = x;
            y->right = z->right;
            z->right->parent = y;
        }
        else xp = y;

        // 连接 y 与 z 的父节点
        if (root == z) root = y;
        else if (z->parent->left == z) z->parent->left = y;
        else z->parent->right = y;

        y->parent = z->parent;
        tinystl::swap(y->color, z->color);
        y = z;
    }

    // case1、case2
    // y == z 说明 z 至多只有一个孩子
    else {
        xp = y->parent;
        if (x) x->parent = y->parent;

        // 连接 x 与 z 的父节点
        if (root == z) root = x;
        else if (z->parent->left == z) z->parent->left = x;
        else z->parent->right = x;

        // 此时 z 有可能是最左节点或最右节点，更新数据
        if (leftmost == z) leftmost = x == nullptr ? xp : rb_tree_min(x);
        if (rightmost == z) rightmost = x == nullptr ? xp : rb_tree_max(x);
    }

    // =============================== 2. 调整红黑树 =============================== //
    // 此时，y 指向要删除的节点，x 为替代节点，从 x 节点开始调整。
    // 如果删除的节点为红色，树的性质没有被破坏，否则按照以下情况调整（x 为左子节点为例）：
    // 1. x 为新的根节点，直接将 x 染成黑色，调整结束。
    // 2. 兄弟节点为红色，令父节点为红，兄弟节点为黑，进行左（右）旋，继续处理。
    // 3. 父节点为黑色，兄弟节点为黑色，且两个子节点都为黑色或 NIL，令兄弟节点为红，父节点成为当前节点，继续处理。
    // 4. 父节点为红色，兄弟节点为黑色，且两个子节点都为黑色或 NIL，令兄弟节点为红，算法结束。
    // 5. 兄弟节点为黑色，右子节点为红色，令兄弟节点为父节点的颜色，父节点为黑色，兄弟节点的右子节点
    //    为黑色，以父节点为支点左（右）旋，树的性质调整完成，算法结束。
    // 6. 兄弟节点为黑色，左子节点为红色或 NIL，右子节点为黑色或 NIL，
    //    令兄弟节点为红，兄弟节点的左子节点为黑，以兄弟节点为支点右（左）旋，继续处理。

    // 仅在删除黑色节点时需要调整
    if (!rb_tree_is_red(y)) {
        // 注意这里的循环条件: !rb_tree_is_red(x)，当遇到 case3 的情况时会转而处理 P 节点
        // 此时 p == x，会再进行一次条件判断，当 p 为红色时，便为 case4 的情况
        // 直接跳出循环，最后将 x 设为黑色，结束调整。
        while (x != root && (x == nullptr || !rb_tree_is_red(x))) {
            if (x == xp->left) {
                auto brother = xp->right;
                // case2：兄弟节点为红色节点，旋转父节点
                if (rb_tree_is_red(brother)) {
                    rb_tree_set_red(xp);               // 父节点变为红色
                    rb_tree_set_black(brother);        // 兄弟节点变为黑色
                    rb_tree_rotate_left(xp, root);     // 左旋
                    brother = xp->right;               // 更新兄弟节点
                }
                // case3: 兄弟节点及两个子节点均为黑色
                if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
                    (brother->right == nullptr || !rb_tree_is_red(brother->right))) {
                    rb_tree_set_red(brother);          // 兄弟节点变为红色
                    x = xp;                            // 父节点成为当前节点
                    xp = xp->parent;                   // 父节点的父节点成为父节点
                    // 此时如果 p 为红色会直接跳出循环
                }
                else {
                    // case5: 兄弟节点同侧的节点为黑色
                    if (brother->right == nullptr || !rb_tree_is_red(brother->right)) {
                        if (brother->left != nullptr) 
                            rb_tree_set_black(brother->left);  // 兄弟节点的左子节点变为黑色
                        rb_tree_set_red(brother);              // 兄弟节点变为红色
                        rb_tree_rotate_right(brother, root);   // 右旋
                        brother = xp->right;                   // 更新兄弟节点 
                    }
                    // 处理之后变为 case6
                    // case6: 兄弟节点的同侧节点为红色
                    brother->color = xp->color;                // 兄弟节点变为父节点的颜色
                    rb_tree_set_black(xp);                     // 父节点变为黑色
                    if (brother->right != nullptr) 
                        rb_tree_set_black(brother->right);     // 兄弟节点的右子节点变为黑色
                    rb_tree_rotate_left(xp, root);             // 左旋
                    break;
                }
            }
            // x 为右子节点，对称处理
            else {
                auto brother = xp->left;
                // case 1
                if (rb_tree_is_red(brother)) {
                    rb_tree_set_black(brother);
                    rb_tree_set_red(xp);
                    rb_tree_rotate_right(xp, root);
                    brother = xp->left;
                }
                // case 2
                if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
                    (brother->right == nullptr || !rb_tree_is_red(brother->right))) {
                    rb_tree_set_red(brother);
                    x = xp;
                    xp = xp->parent;
                }
                else {
                    // case 3
                    if (brother->left == nullptr || !rb_tree_is_red(brother->left)) {
                        if (brother->right != nullptr) 
                            rb_tree_set_black(brother->right);
                        rb_tree_set_red(brother);
                        rb_tree_rotate_left(brother, root);
                        brother = xp->left;
                    }
                    // 转为 case 4
                    brother->color = xp->color;
                    rb_tree_set_black(xp);
                    if (brother->left != nullptr) 
                        rb_tree_set_black(brother->left);
                    rb_tree_rotate_right(xp, root);
                    break;
                }
            }
        }
        // 将 x 的颜色改为黑色，有可能是 root 节点也有可能是 case4 跳出了循环
        if (x != nullptr) rb_tree_set_black(x);
    }
    return y;  // 这里返回的值实际上没有用到
    // 注意：执行完 rb_tree_erase_rebalance 保证了传入的 z，及要被删除的节点，此时一定不在树的结构中了
    // 这意味着对其进行析构是安全的，不会破坏树的性质。
}

// ========================================== rb_tree ========================================== //

/// @brief 模板类 rb_tree
/// @tparam T  节点的值类型
/// @tparam Compare  节点键值比较准则
template <class T, class Compare, class Alloc = alloc>
class rb_tree {

public:  // rb_tree 的嵌套型别定义
    typedef rb_tree_traits<T>                               tree_traits;
    typedef rb_tree_value_traits<T>                         value_traits;

    typedef typename tree_traits::base_type                 base_type;
    typedef typename tree_traits::base_ptr                  base_ptr;
    typedef typename tree_traits::node_type                 node_type;
    typedef typename tree_traits::node_ptr                  node_ptr;  // link_type
    typedef typename tree_traits::key_type                  key_type;
    typedef typename tree_traits::mapped_type               mapped_type;
    typedef typename tree_traits::value_type                value_type;
    typedef Compare                                         key_compare;

    // typedef tinystl::allocator<T>                           allocator_type;
    // typedef tinystl::allocator<T>                           data_allocator;
    // typedef tinystl::allocator<base_type>                   base_allocator;
    // typedef tinystl::allocator<node_type>                   node_allocator;

    typedef simple_alloc<T, Alloc>                          allocator_type;
    typedef simple_alloc<T, Alloc>                          data_allocator;
    typedef simple_alloc<base_type, Alloc>                  base_allocator;
    typedef simple_alloc<node_type, Alloc>                  node_allocator;

    // typedef typename allocator_type::pointer                pointer;
    // typedef typename allocator_type::const_pointer          const_pointer;
    // typedef typename allocator_type::reference              reference;
    // typedef typename allocator_type::const_reference        const_reference;
    // typedef typename allocator_type::size_type              size_type;
    // typedef typename allocator_type::difference_type        difference_type;

    typedef value_type*                                     pointer;
    typedef const value_type*                               const_pointer;
    typedef value_type&                                     reference;
    typedef const value_type&                               const_reference;
    typedef size_t                                          size_type;
    typedef ptrdiff_t                                       difference_type;

    typedef rb_tree_iterator<T>                             iterator;
    typedef rb_tree_const_iterator<T>                       const_iterator;
    typedef tinystl::reverse_iterator<iterator>             reverse_iterator;
    typedef tinystl::reverse_iterator<const_iterator>       const_reverse_iterator;

    allocator_type get_allocator() const { return node_allocator(); }
    key_compare    key_comp()      const { return key_comp_; }

private:  // rb_tree 的数据成员
    base_ptr    header_;      // 特殊节点，与根节点互为父节点
    size_type   node_count_;  // 节点数量
    key_compare key_comp_;    // 节点键值比较准则

private:
    /// @brief 获取根节点
    base_ptr& root()      const { return header_->parent; }
    /// @brief 获取最小节点
    base_ptr& leftmost()  const { return header_->left; }
    /// @brief 获取最大节点
    base_ptr& rightmost() const { return header_->right; }

public:  // 构造、复制、析构函数
    rb_tree() { rb_tree_init(); }

    rb_tree(const rb_tree& rhs);
    rb_tree(rb_tree&& rhs) noexcept;

    rb_tree& operator=(const rb_tree& rhs);
    rb_tree& operator=(rb_tree&& rhs) noexcept;

    ~rb_tree() { clear(); }

public:  // 迭代器相关操作
    iterator                begin()     noexcept        { return leftmost(); }
    const_iterator          begin()     const noexcept  { return leftmost(); }
    const_iterator          cbegin()    const noexcept  { return begin(); }

    iterator                end()       noexcept        { return header_; }
    const_iterator          end()       const noexcept  { return header_; }
    const_iterator          cend()      const noexcept  { return end(); }

    reverse_iterator        rbegin()    noexcept        { return reverse_iterator(end()); }
    const_reverse_iterator  rbegin()    const noexcept  { return const_reverse_iterator(end()); }
    const_reverse_iterator  crbegin()   const noexcept  { return rbegin(); }

    reverse_iterator        rend()      noexcept        { return reverse_iterator(begin()); }
    const_reverse_iterator  rend()      const noexcept  { return const_reverse_iterator(begin()); }
    const_reverse_iterator  crend()     const noexcept  { return rend(); }

public:  // 容量相关操作
    bool        empty()     const noexcept  { return node_count_ == 0; }
    size_type   size()      const noexcept  { return node_count_; }
    size_type   max_size()  const noexcept  { return static_cast<size_type>(-1); }

public:  // 元素相关操作

    // ====================== emplace ====================== //
    template <class ...Args>
    iterator  emplace_multi(Args&& ...args);

    template <class ...Args>
    tinystl::pair<iterator, bool> emplace_unique(Args&& ...args);

    template <class ...Args>
    iterator  emplace_multi_use_hint(iterator hint, Args&& ...args);

    template <class ...Args>
    iterator  emplace_unique_use_hint(iterator hint, Args&& ...args);

    // ====================== insert ====================== //
    iterator  insert_multi(const value_type& value);
    
    iterator  insert_multi(value_type&& value) {
        return emplace_multi(tinystl::move(value));
    }

    iterator  insert_multi(iterator hint, const value_type& value) {
        return emplace_multi_use_hint(hint, value);
    }

    iterator  insert_multi(iterator hint, value_type&& value) {
        return emplace_multi_use_hint(hint, tinystl::move(value));
    }

    template <class InputIterator>
    void      insert_multi(InputIterator first, InputIterator last) {
        size_type n = tinystl::distance(first, last);
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "rb_tree<T, Comp>'s size too big");
        for (; n > 0; --n, ++first)
        insert_multi(end(), *first);
    }

    
    tinystl::pair<iterator, bool> insert_unique(const value_type& value);
    
    tinystl::pair<iterator, bool> insert_unique(value_type&& value) {
        return emplace_unique(tinystl::move(value));
    }

    iterator  insert_unique(iterator hint, const value_type& value) {
        return emplace_unique_use_hint(hint, value);
    }

    iterator  insert_unique(iterator hint, value_type&& value) {
        return emplace_unique_use_hint(hint, tinystl::move(value));
    }

    template <class InputIterator>
    void      insert_unique(InputIterator first, InputIterator last) {
        size_type n = tinystl::distance(first, last);
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "rb_tree<T, Comp>'s size too big");
        for (; n > 0; --n, ++first)
            insert_unique(end(), *first);
    }

    // ====================== erase ====================== //

    iterator  erase(iterator hint);

    size_type erase_multi(const key_type& key);
    size_type erase_unique(const key_type& key);

    void erase(iterator first, iterator last);

    void clear();

public:  // 查找相关操作
    iterator              find(const key_type& key);
    const_iterator        find(const key_type& key) const;

    size_type             count_multi(const key_type& key) const {
        auto p = equal_range_multi(key);
        return static_cast<size_type>(tinystl::distance(p.first, p.second));
    }

    size_type             count_unique(const key_type& key) const {
        return find(key) == end() ? 0 : 1;
    }

    iterator              lower_bound(const key_type& key);
    const_iterator        lower_bound(const key_type& key) const;

    iterator              upper_bound(const key_type& key);
    const_iterator        upper_bound(const key_type& key) const;

    tinystl::pair<iterator, iterator>
    equal_range_multi(const key_type& key) {
        return tinystl::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
    }

    tinystl::pair<const_iterator, const_iterator>
    equal_range_multi(const key_type& key) const {
        return tinystl::pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
    }

    tinystl::pair<iterator, iterator>
    equal_range_unique(const key_type& key) {
        iterator it = find(key);
        auto next = it;
        return it == end() ? tinystl::make_pair(it, it) : tinystl::make_pair(it, ++next);
    }

    tinystl::pair<const_iterator, const_iterator>
    equal_range_unique(const key_type& key) const {
        const_iterator it = find(key);
        auto next = it;
        return it == end() ? tinystl::make_pair(it, it) : tinystl::make_pair(it, ++next);
    }

    void swap(rb_tree& rhs) noexcept;

private:  // 辅助函数
    
    // node related
    template <class ...Args>
    node_ptr create_node(Args&& ...args);
    node_ptr clone_node(base_ptr x);
    void     destroy_node(node_ptr x);

    // init / reset
    void     rb_tree_init();
    void     reset();

    // get_insert_pos
    tinystl::pair<base_ptr, bool> get_insert_multi_pos(const key_type& key);
    tinystl::pair<tinystl::pair<base_ptr, bool>, bool> get_insert_unique_pos(const key_type& key);

    // insert
    iterator insert_value_at(base_ptr x, const value_type& value, bool add_to_left);
    iterator insert_node_at(base_ptr x, node_ptr node, bool add_to_left);

    // insert use hint
    iterator insert_multi_use_hint(iterator hint, key_type key, node_ptr node);
    iterator insert_unique_use_hint(iterator hint, key_type key, node_ptr node);

    // copy / erase
    base_ptr copy_from(base_ptr x, base_ptr p);
    void     erase_since(base_ptr x);
};


// ============================================ 函数实现 ================================================ //

/// @brief 复制构造函数
template <class T, class Compare, class Alloc>
rb_tree<T, Compare, Alloc>::rb_tree(const rb_tree& rhs) {
    rb_tree_init();
    if (rhs.node_count_ != 0) {
        root() = copy_from(rhs.root(), header_);
        leftmost() = rb_tree_min(root());
        rightmost() = rb_tree_max(root());
    }
    node_count_ = rhs.node_count_;
    key_comp_ = rhs.key_comp_;
}

/// @brief 移动构造函数
template <class T, class Compare, class Alloc>
rb_tree<T, Compare, Alloc>::rb_tree(rb_tree&& rhs) noexcept 
    : header_(tinystl::move(rhs.header_)), 
    node_count_(rhs.node_count_), 
    key_comp_(rhs.key_comp_) {
    rhs.reset();
} 

/// @brief 复制赋值运算符
template <class T, class Compare, class Alloc>
rb_tree<T, Compare, Alloc>& rb_tree<T, Compare, Alloc>::operator=(const rb_tree& rhs) {
    if (this != &rhs) {
        clear();
        if (rhs.node_count_ != 0) {
            root() = copy_from(rhs.root(), header_);
            leftmost() = rb_tree_min(root());
            rightmost() = rb_tree_max(root());
        }
        node_count_ = rhs.node_count_;
        key_comp_ = rhs.key_comp_;
    }
    return *this;
}

/// @brief 移动赋值运算符
template <class T, class Compare, class Alloc>
rb_tree<T, Compare, Alloc>& rb_tree<T, Compare, Alloc>::operator=(rb_tree&& rhs) noexcept {
    clear();
    header_ = tinystl::move(rhs.header_);
    node_count_ = rhs.node_count_;
    key_comp_ = rhs.key_comp_;
    rhs.reset();
    return *this;
}

/// @brief 就地插入元素，键值允许重复
template <class T, class Compare, class Alloc>
template <class ...Args>
typename rb_tree<T, Compare, Alloc>::iterator
rb_tree<T, Compare, Alloc>::emplace_multi(Args&& ...args) {
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Compare, Alloc>'s size too big");
    node_ptr node = create_node(tinystl::forward<Args>(args)...);
    auto res = get_insert_multi_pos(value_traits::get_key(node->value));
    return insert_node_at(res.first, node, res.second);
}

/// @brief 就地插入元素，键值不允许重复
/// @return 返回一个 pair，其中 first 为插入位置，second 表示是否插入成功
template <class T, class Compare, class Alloc>
template <class ...Args>
tinystl::pair<typename rb_tree<T, Compare, Alloc>::iterator, bool>
rb_tree<T, Compare, Alloc>::emplace_unique(Args&& ...args) {
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Compare, Alloc>'s size too big");
    node_ptr node = create_node(tinystl::forward<Args>(args)...);
    auto res = get_insert_unique_pos(value_traits::get_key(node->value));

    // 插入成功
    if (res.second) {
        return tinystl::make_pair(insert_node_at(res.first.first, node, res.first.second), true);
    }
    destroy_node(node);
    return tinystl::make_pair(iterator(res.first.first), false);
}

/// @brief 就地插入元素，键值允许重复，当 hint 位置与插入位置接近时，插入操作的时间复杂度可以降低
template <class T, class Compare, class Alloc>
template <class ...Args>
typename rb_tree<T, Compare, Alloc>::iterator
rb_tree<T, Compare, Alloc>::emplace_multi_use_hint(iterator hint, Args&& ...args) {
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Compare, Alloc>'s size too big");
    node_ptr node = create_node(tinystl::forward<Args>(args)...);
    key_type key  = value_traits::get_key(node->value);

    if (node_count_ == 0) {
        return insert_node_at(header_, node, true);
    }

    if (hint == begin()) {
        if (key_comp_(key, value_traits::get_key(*hint))) {
            return insert_node_at(hint.node, node, true);
        }
        else {
            auto res = get_insert_multi_pos(key);
            return insert_node_at(res.first, node, res.second);  // TODO: why?
        }
    }
    else if (hint == end()) {
        // previous : rightmost()->get_node_ptr()->value
        // now      : static_cast<link_type>(rightmost())->value
        if (!key_comp_(key, value_traits::get_key(static_cast<node_ptr>(rightmost())->value))) {
            return insert_node_at(rightmost(), node, false);
        }
        else {
            auto pos = get_insert_multi_pos(key);
            return insert_node_at(pos.first, node, pos.second);
        }
    }
    return insert_multi_use_hint(hint, key, node);
}

/// @brief 就地插入元素，键值不允许重复，当 hint 位置与插入位置接近时，插入操作的时间复杂度可以降低
template<class T, class Compare, class Alloc>
template<class ...Args>
typename rb_tree<T, Compare, Alloc>::iterator
rb_tree<T, Compare, Alloc>::emplace_unique_use_hint(iterator hint, Args&& ...args) {
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Compare, Alloc>'s size too big");
    node_ptr node = create_node(tinystl::forward<Args>(args)...);
    key_type key  = value_traits::get_key(node->value);

    if (node_count_ == 0) {
        return insert_node_at(header_, node, true);
    }
    if (hint == begin()) {
        if (key_comp_(key, value_traits::get_key(*hint))) {
            return insert_node_at(hint.node, node, true);
        }
        else {
            auto res = get_insert_unique_pos(key);
            if (!res.second) {
                destroy_node(node);
                return res.first.first;
            }
            return insert_node_at(res.first.first, node, res.first.second);
        }
    }
    else if (hint == end()) {
        // previous : rightmost()->get_node_ptr()->value
        // now      : static_cast<link_type>(rightmost())->value
        if (key_comp_(value_traits::get_key(static_cast<node_ptr>(rightmost())->value), key)) {
            return insert_node_at(rightmost(), node, false);
        }
        else {
            auto res = get_insert_unique_pos(key);
            if (!res.second) {
                destroy_node(node);
                return res.first.first;
            }
            return insert_node_at(res.first.first, node, res.first.second);
        }
    }
    return insert_unique_use_hint(hint, key, node);
}

/// @brief 插入元素，节点键值允许重复
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::iterator
rb_tree<T, Compare, Alloc>::insert_multi(const value_type& value) {
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Compare, Alloc>'s size too big");
    auto res = get_insert_multi_pos(value_traits::get_key(value));
    return insert_value_at(res.first, value, res.second);
}

/// @brief 插入元素，节点键值不允许重复，返回一个 pair，若插入成功，pair 的第二参数为 true，否则为 false
template <class T, class Compare, class Alloc>
tinystl::pair<typename rb_tree<T, Compare, Alloc>::iterator, bool>
rb_tree<T, Compare, Alloc>::insert_unique(const value_type& value) {
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Compare, Alloc>'s size too big");
    auto res = get_insert_unique_pos(value_traits::get_key(value));
    // 插入成功
    if (res.second) {
        return tinystl::make_pair(insert_value_at(res.first.first, value, res.first.second), true);
    }
    return tinystl::make_pair(res.first.first, false);
}

/// @brief 删除 hint 位置的节点
/// @return 返回删除节点的后继节点
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::iterator
rb_tree<T, Compare, Alloc>::erase(iterator hint) {
    // auto node = hint.node->get_node_ptr();
    auto node = static_cast<node_ptr>(hint.node);
    iterator next(node);
    ++next;
    // 重新平衡
    rb_tree_erase_rebalance(hint.node, root(), leftmost(), rightmost());
    destroy_node(node);
    --node_count_;
    return next;
}

/// @brief 删除键值等于 key 的元素，返回删除的个数
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::size_type
rb_tree<T, Compare, Alloc>::erase_multi(const key_type& key) {
    auto res = equal_range_multi(key);
    auto count = tinystl::distance(res.first, res.second);
    erase(res.first, res.second);
    return count;
}

/// @brief 删除键值等于 key 的元素，返回删除的个数
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::size_type
rb_tree<T, Compare, Alloc>::erase_unique(const key_type& key) {
    auto it = find(key);
    if (it == end()) return 0;
    erase(it);
    return 1;
}

/// @brief 删除 [first, last) 范围内的元素
template <class T, class Compare, class Alloc>
void rb_tree<T, Compare, Alloc>::erase(iterator first, iterator last) {
    if (first == begin() && last == end()) clear();
    else {
        while (first != last) erase(first++);
    }
}

/// @brief 清空 rb-tree
template <class T, class Compare, class Alloc>
void rb_tree<T, Compare, Alloc>::clear() {
    if (node_count_ != 0) {
        erase_since(root());
        leftmost() = header_;
        root() = nullptr;
        rightmost() = header_;
        node_count_ = 0;
    }
}

/// @brief 查找键值为 key 的元素，若找到，返回指向该元素的迭代器，否则返回 end()
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::iterator
rb_tree<T, Compare, Alloc>::find(const key_type& key) {
    auto y = header_;  // 最后一个不小于 key 的节点
    auto x = root();   // 当前节点
    while (x != nullptr) {
        // key 小于等于 x 键值，向左走

        // previous : x->get_node_ptr()->value
        // now      : static_cast<link_type>(x)->value
        if (!key_comp_(value_traits::get_key(static_cast<node_ptr>(x)->value), key)) {
            y = x;
            x = x->left;
        }
        else x = x->right;
    }
    iterator res(y);
    // 若 res 不等于 end()，且 key 小于等于 res 键值，返回 res
    if (res != end() && !key_comp_(key, value_traits::get_key(*res))) return res;
    return end();
}

/// @brief 查找键值为 key 的元素，若找到，返回指向该元素的迭代器，否则返回 end()
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::const_iterator
rb_tree<T, Compare, Alloc>::find(const key_type& key) const {
    auto y = header_;  // 最后一个不小于 key 的节点
    auto x = root();   // 当前节点
    while (x != nullptr) {
        // key 小于等于 x 键值，向左走

        // previous : x->get_node_ptr()->value
        // now      : static_cast<link_type>(x)->value
        if (!key_comp_(value_traits::get_key(static_cast<node_ptr>(x)->value), key)) {
            y = x;
            x = x->left;
        }
        else x = x->right;
    }
    const_iterator res(y);
    // 若 res 不等于 end()，且 key 小于等于 res 键值，返回 res
    if (res != end() && !key_comp_(key, value_traits::get_key(*res))) return res;
    return end();
}

/// @brief 键值不小于 key 的第一个位置
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::iterator
rb_tree<T, Compare, Alloc>::lower_bound(const key_type& key) {
    auto y = header_;  // 最后一个不小于 key 的节点
    auto x = root();   // 当前节点
    while (x != nullptr) {
        // key 小于等于 x 键值，向左走

        // previous : x->get_node_ptr()->value
        // now      : static_cast<link_type>(x)->value
        if (!key_comp_(value_traits::get_key(static_cast<node_ptr>(x)->value), key)) {
            y = x;
            x = x->left;
        }
        else x = x->right;
    }
    return iterator(y);
}

/// @brief 键值不小于 key 的第一个位置
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::const_iterator
rb_tree<T, Compare, Alloc>::lower_bound(const key_type& key) const {
    auto y = header_;  // 最后一个不小于 key 的节点
    auto x = root();   // 当前节点
    while (x != nullptr) {
        // key 小于等于 x 键值，向左走

        // previous : x->get_node_ptr()->value
        // now      : static_cast<link_type>(x)->value
        if (!key_comp_(value_traits::get_key(static_cast<node_ptr>(x)->value), key)) {
            y = x;
            x = x->left;
        }
        else x = x->right;
    }
    return const_iterator(y);
}

/// @brief 键值不小于 key 的最后一个位置
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::iterator
rb_tree<T, Compare, Alloc>::upper_bound(const key_type& key) {
    auto y = header_;  // 最后一个不小于 key 的节点
    auto x = root();   // 当前节点
    while (x != nullptr) {
        // key 小于 x 键值，向左走

        // previous : x->get_node_ptr()->value
        // now      : static_cast<link_type>(x)->value
        if (key_comp_(key, value_traits::get_key(static_cast<node_ptr>(x)->value))) {
            y = x;
            x = x->left;
        }
        else x = x->right;
    }
    return iterator(y);
}

/// @brief 键值不小于 key 的最后一个位置
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::const_iterator
rb_tree<T, Compare, Alloc>::upper_bound(const key_type& key) const {
    auto y = header_;  // 最后一个不小于 key 的节点
    auto x = root();   // 当前节点
    while (x != nullptr) {
        // key 小于 x 键值，向左走

        // previous : x->get_node_ptr()->value
        // now      : static_cast<link_type>(x)->value
        if (key_comp_(key, value_traits::get_key(static_cast<node_ptr>(x)->value))) {
            y = x;
            x = x->left;
        }
        else x = x->right;
    }
    return const_iterator(y);
}

/// @brief 交换 rb-tree
template <class T, class Compare, class Alloc>
void rb_tree<T, Compare, Alloc>::swap(rb_tree& rhs) noexcept {
    if (this != &rhs) {
        tinystl::swap(header_, rhs.header_);
        tinystl::swap(node_count_, rhs.node_count_);
        tinystl::swap(key_comp_, rhs.key_comp_);
    }
}

// ======================================= 辅助函数 ======================================= // 

/// @brief 创建节点
template <class T, class Compare, class Alloc>
template <class ...Args>
typename rb_tree<T, Compare, Alloc>::node_ptr
rb_tree<T, Compare, Alloc>::create_node(Args&& ...args) {
    auto tmp = node_allocator::allocate(1);
    try {
        // 在节点位置构造元素
        tinystl::construct(tinystl::address_of(tmp->value), tinystl::forward<Args>(args)...);
        tmp->parent = nullptr;
        tmp->left = nullptr;
        tmp->right = nullptr;
    }
    catch (...) {
        node_allocator::deallocate(tmp);
        throw;
    }
    return tmp;
}

/// @brief 复制节点
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::node_ptr
rb_tree<T, Compare, Alloc>::clone_node(base_ptr x) {
    // auto tmp = create_node(x->get_node_ptr()->value);
    auto tmp = create_node(static_cast<node_ptr>(x)->value);
    tmp->color = x->color;
    tmp->left = nullptr;
    tmp->right = nullptr;
    return tmp;
}


/// @brief 销毁节点
template <class T, class Compare, class Alloc>
void rb_tree<T, Compare, Alloc>::destroy_node(node_ptr x) {
    tinystl::destroy(tinystl::address_of(x->value));
    node_allocator::deallocate(x);
}

/// @brief 初始化 rb-tree
template <class T, class Compare, class Alloc>
void rb_tree<T, Compare, Alloc>::rb_tree_init() {
    header_ = base_allocator::allocate(1);
    header_->color = rb_tree_red;  // header_ 为红色，与 root 区分
    root() = nullptr;
    leftmost() = header_;
    rightmost() = header_;
    node_count_ = 0;
}

/// @brief 重置 rb-tree
template <class T, class Compare, class Alloc>
void rb_tree<T, Compare, Alloc>::reset() {
    header_ = nullptr;
    node_count_ = 0;
}

/// @brief 获取插入位置，键值允许重复，返回一个 pair，其中 first 为插入位置（父节点），second 表示是否在左侧插入
template <class T, class Compare, class Alloc>
tinystl::pair<typename rb_tree<T, Compare, Alloc>::base_ptr, bool>
rb_tree<T, Compare, Alloc>::get_insert_multi_pos(const key_type& key) {
    auto x = root();
    auto y = header_;
    bool add_to_left = true;
    while (x != nullptr) {
        y = x;

        // previous : x->get_node_ptr()->value
        // now      : static_cast<link_type>(x)->value
        add_to_left = key_comp_(key, value_traits::get_key(static_cast<node_ptr>(x)->value));
        x = add_to_left ? x->left : x->right;
    }
    return tinystl::make_pair(y, add_to_left);
}

/// @brief 获取插入位置，键值不允许重复，返回一个 pair<pair, bool>，其中 first 为一个 pair，
///       first.first 为插入位置，first.second 表示是否在左侧插入，second 表示是否插入成功
template <class T, class Compare, class Alloc>
tinystl::pair<tinystl::pair<typename rb_tree<T, Compare, Alloc>::base_ptr, bool>, bool>
rb_tree<T, Compare, Alloc>::get_insert_unique_pos(const key_type& key) {
    auto x = root();
    auto y = header_;
    bool add_to_left = true;  // 树为空时也在 header_ 左边插入
    while (x != nullptr) {
        y = x;

        // previous : x->get_node_ptr()->value
        // now      : static_cast<link_type>(x)->value
        add_to_left = key_comp_(key, value_traits::get_key(static_cast<node_ptr>(x)->value));
        x = add_to_left ? x->left : x->right;
    }
    iterator it(y);  // y 为插入点的父节点
    // iterator it = iterator(y);
    if (add_to_left) {
        // 如果树为空树或插入点在最左节点处，肯定可以插入新的节点 
        if (it == begin()) {
            return tinystl::make_pair(tinystl::make_pair(y, true), true);
        }
        // 否则（插入节点的父节点不是最左节点）
        else --it;
    }

    // std::cout << "addleft: " << add_to_left << " " << "it: " << value_traits::get_key(*it) << " " 
    // << "key: " << key << std::endl;

    // 表明新节点没有重复
    if (key_comp_(value_traits::get_key(*it), key)) {
        return tinystl::make_pair(tinystl::make_pair(y, add_to_left), true);
    }
    // 进行至此，表示新节点与现有节点键值重复
    return tinystl::make_pair(tinystl::make_pair(y, add_to_left), false);
}

/// @brief 在 x 位置插入节点，节点值为 value，add_to_left 表示是否在左侧插入
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::iterator
rb_tree<T, Compare, Alloc>::insert_value_at(base_ptr x, const value_type& value, bool add_to_left) {
    node_ptr node = create_node(value);
    node->parent = x;
    // auto base_node = node->get_base_ptr();
    auto base_node = static_cast<base_ptr>(node);
    if (x == header_) {
        root() = base_node;
        leftmost() = base_node;
        rightmost() = base_node;
    }
    else if (add_to_left) {
        x->left = base_node;
        if (x == leftmost()) leftmost() = base_node;
    }
    else {
        x->right = base_node;
        if (x == rightmost()) rightmost() = base_node;
    }
    rb_tree_insert_rebalance(base_node, root());
    ++node_count_;
    return iterator(node);
}

/// @brief 在 x 位置插入 node 节点，add_to_left 表示是否在左侧插入
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::iterator
rb_tree<T, Compare, Alloc>::insert_node_at(base_ptr x, node_ptr node, bool add_to_left) {
    node->parent = x;
    // auto base_node = node->get_base_ptr();
    auto base_node = static_cast<base_ptr>(node);
    if (x == header_) {
        root() = base_node;
        leftmost() = base_node;
        rightmost() = base_node;
    }
    else if (add_to_left) {
        x->left = base_node;
        if (x == leftmost()) leftmost() = base_node;
    }
    else {
        x->right = base_node;
        if (x == rightmost()) rightmost() = base_node;
    }
    rb_tree_insert_rebalance(base_node, root());
    ++node_count_;
    return iterator(node);
}

/// @brief 插入元素，键值允许重复，使用 hint
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::iterator 
rb_tree<T, Compare, Alloc>::insert_multi_use_hint(iterator hint, key_type key, node_ptr node) {
    auto np = hint.node;
    auto before = hint;
    -- before;
    auto bnp = before.node;
    // before <= node <= hint
    if (!key_comp_(key, value_traits::get_key(*before)) && 
        !key_comp_(value_traits::get_key(*hint), key)) {
        // 插入 bnp 的右侧
        if (bnp->right == nullptr) {
            return insert_node_at(bnp, node, false);
        }
        // 插入 np 的左侧
        else if (np->left == nullptr) {
            return insert_node_at(np, node, true);
        }
    }
    auto pos = get_insert_multi_pos(key);
    return insert_node_at(pos.first, node, pos.second);
}

/// @brief 插入元素，键值不允许重复，使用 hint 
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::iterator
rb_tree<T, Compare, Alloc>::insert_unique_use_hint(iterator hint, key_type key, node_ptr node) {
    auto np = hint.node;
    auto before = hint;
    --before;
    auto bnp = before.node;

    // before < node < hint
    if (key_comp_(value_traits::get_key(*before), key) &&
        key_comp_(key, value_traits::get_key(*hint))) {
        // 插入 bnp 的右侧
        if (bnp->right == nullptr) {
            return insert_node_at(bnp, node, false);
        }
        // 插入 np 的左侧
        else if (np->left == nullptr) {
            return insert_node_at(np, node, true);
        }
    }
    auto res = get_insert_unique_pos(key);
    if (!res.second) {
        destroy_node(node);
        return res.first.first;
    }
    return insert_node_at(res.first.first, node, res.first.second);
}

/// @brief 复制一棵树，节点从 x 开始，p 为 x 的父节点
template <class T, class Compare, class Alloc>
typename rb_tree<T, Compare, Alloc>::base_ptr
rb_tree<T, Compare, Alloc>::copy_from(base_ptr x, base_ptr p) {
    auto top = clone_node(x);
    top->parent = p;
    try {
        // 若有右子树，则递归复制
        if (x->right) top->right = copy_from(x->right, top);
        p = top;
        x = x->left;
        while (x != nullptr) {
            auto y = clone_node(x);
            p->left = y;
            y->parent = p;
            if (x->right) y->right = copy_from(x->right, y);
            p = y;
            x = x->left;
        }
    }
    catch (...) {
        erase_since(top);
        throw;
    }
    return top;
}

/// @brief 从 x 开始递归删除树
template <class T, class Compare, class Alloc>
void rb_tree<T, Compare, Alloc>::erase_since(base_ptr x) {
    while (x != nullptr) {
        erase_since(x->right);
        auto y = x->left;
        // destroy_node(x->get_node_ptr());
        destroy_node(static_cast<node_ptr>(x));
        x = y;
    }
}

// ========================================= 重载比较操作符 ========================================= //

template <class T, class Compare, class Alloc>
bool operator==(const rb_tree<T, Compare, Alloc>& lhs, const rb_tree<T, Compare, Alloc>& rhs) {
    return lhs.size() == rhs.size() && tinystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, class Compare, class Alloc>
bool operator<(const rb_tree<T, Compare, Alloc>& lhs, const rb_tree<T, Compare, Alloc>& rhs) {
    return tinystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T, class Compare, class Alloc>
bool operator!=(const rb_tree<T, Compare, Alloc>& lhs, const rb_tree<T, Compare, Alloc>& rhs) {
    return !(lhs == rhs);
}

template <class T, class Compare, class Alloc>
bool operator>(const rb_tree<T, Compare, Alloc>& lhs, const rb_tree<T, Compare, Alloc>& rhs) {
    return rhs < lhs;
}

template <class T, class Compare, class Alloc>
bool operator<=(const rb_tree<T, Compare, Alloc>& lhs, const rb_tree<T, Compare, Alloc>& rhs) {
    return !(rhs < lhs);
}

template <class T, class Compare, class Alloc>
bool operator>=(const rb_tree<T, Compare, Alloc>& lhs, const rb_tree<T, Compare, Alloc>& rhs) {
    return !(lhs < rhs);
}

// ========================================= 重载 swap ========================================= //

template <class T, class Compare, class Alloc>
void swap(rb_tree<T, Compare, Alloc>& lhs, rb_tree<T, Compare, Alloc>& rhs) noexcept {
    lhs.swap(rhs);
}

}  // namespace tinystl
 
#endif  // TINYSTL_RB_TREE_H_