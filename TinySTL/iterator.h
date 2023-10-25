#ifndef TINYSTL_ITERATOR_H_
#define TINYSTL_ITERATOR_H_

// 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数，

#include <cstddef>  // ptrdiff_t, size_t
#include "type_traits.h"

namespace tinystl {

// 五种迭代器类型
struct input_iterator_tag {};  // 只读
struct output_iterator_tag {};  // 只写
struct forward_iterator_tag : public input_iterator_tag {};  // 可读写
struct bidirectional_iterator_tag : public forward_iterator_tag {};  // 双向
struct random_access_iterator_tag : public bidirectional_iterator_tag {};  // 随机访问

/// @brief iterator 模板结构体，如果每个新设计的迭代器都继承自它，就可保证符合 STL 规范
/// @tparam Category  迭代器类型（必须提供）
/// @tparam T         迭代器所指对象的类型（必须提供）
/// @tparam Distance  ptrdiff_t（默认）
/// @tparam Pointer   T*（默认）
/// @tparam Reference T&（默认）
template <class Category, class T, class Distance = ptrdiff_t, 
    class Pointer = T*, class Reference = T&>
struct iterator {
    typedef Category    iterator_category;  // 迭代器类型
    typedef T           value_type;         // 迭代器所指对象的类型
    typedef Distance    difference_type;    // 迭代器之间的距离类型
    typedef Pointer     pointer;            // 指向迭代器所指对象的指针类型
    typedef Reference   reference;          // 迭代器所指对象的引用类型
};

// iterator traits

// =========================================== traits =========================================== //
// 这部分的实现太精彩了
// 用于判断某个类型是否为迭代器类型，见 iterator 部分的笔记: Note1-SFINAE技巧

template <class T>
struct has_iterator_cat {
    private:
        struct two { char a; char b; };
        template <class U> static two test(...);
        template <class U> static char test(typename U::iterator_category* = 0);
    public:
        static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

template <class Iterator, bool>
struct iterator_traits_impl {};

// `iterator_traits`结构体就是使用`typename`对参数类型的提取(萃取), 
// 并且对参数类型在进行一次命名, 看上去对参数类型的使用有了一层间接性.
template <class Iterator>
struct iterator_traits_impl<Iterator, true> {
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type        value_type;
    typedef typename Iterator::pointer           pointer;
    typedef typename Iterator::reference         reference;
    typedef typename Iterator::difference_type   difference_type;
};

template <class Iterator, bool>
struct iterator_traits_helper {};

// 判断某个类型的迭代器是否可以转换为输入迭代器或输出迭代器
template <class Iterator>
struct iterator_traits_helper<Iterator, true>
    : public iterator_traits_impl<Iterator,
    std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
    std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value> {};


// 萃取迭代器的特性
// 1. 先利用`has_iterator_cat`判断是否有`iterator_category`这个类型
// 2. 再利用`iterator_traits_helper`判断是否可以转换为输入迭代器或输出迭代器
template <class Iterator>
struct iterator_traits 
  : public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};



// 针对原生指针(native pointer)而设计的 traits 偏特化版本
template <class T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag  iterator_category;  // 原生指针是随机访问迭代器
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef T*                          pointer;
    typedef T&                          reference;
};

// 针对原生指针(const native pointer)而设计的 traits 偏特化版本
template <class T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag  iterator_category;  // 原生指针是随机访问迭代器
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef const T*                    pointer;
    typedef const T&                    reference;
};

// 如果 T 和 U 能够转换为相同的类型，则继承自 m_true_type
template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
struct has_iterator_cat_of
  : public m_bool_constant<std::is_convertible<
  typename iterator_traits<T>::iterator_category, U>::value> {};

// 否则继承自 m_false_type
template <class T, class U>
struct has_iterator_cat_of<T, U, false> : public m_false_type {};

// 萃取某种迭代器
template <class Iter>
struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {};

template <class Iter>
struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {};

template <class Iter>
struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

template <class Iter>
struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

template <class Iter>
struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

template <class Iterator>
struct is_iterator : public m_bool_constant<is_input_iterator<Iterator>::value ||
    is_output_iterator<Iterator>::value> {};

// ============================================================================================ //


/// @brief 用于获取某个迭代器的 category
template <class Iterator>
typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&) {
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();  // 返回默认构造的 category 对象，即迭代器的类别
}

/// @brief 用于获取某个迭代器的 disstance_type
template <class Iterator>
typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator&) {
    // **这里用到了0可以转换成指针的性质, 相当于返回一个空指针, 但是可以通过它们确定不同的参数类型.** 
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

/// @brief 用于获取某个迭代器的 value_type
template <class Iterator>
typename iterator_traits<Iterator>::value_type* value_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

/*****************************************************************************************/
// distance
// 计算两个迭代器之间的距离
/*****************************************************************************************/

/// @brief distance 的重载版本，用于 input_iterator_tag 类型的迭代器
template <class InputIterator>  // 以算法所能接受的最低阶迭代器类型，来为其迭代器类别参数命名
typename iterator_traits<InputIterator>::difference_type __distance(InputIterator first, 
    InputIterator last, input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    // input_iterator_tag 类型的迭代器只能使用 ++ 操作符
    while (first != last) {
        ++first; ++n;
    }
    return n;
}

/// @brief distance 的重载版本，用于 random_access_iterator_tag 类型的迭代器
template <class RandomAccessIterator>  // 以算法所能接受的最低阶迭代器类型，来为其迭代器类别参数命名
typename iterator_traits<RandomAccessIterator>::difference_type __distance(RandomAccessIterator first, 
    RandomAccessIterator last, random_access_iterator_tag) {
    return last - first;  // 随机访问迭代器可以直接相减
}

/// @brief 计算两个迭代器之间的距离，根据迭代器的类型调用不同的函数
template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, 
    InputIterator last) {
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    return __distance(first, last, category());  // 通过迭代器的类别来调用不同的函数
}


/*****************************************************************************************/
// advance
// 使迭代器前进 n 个距离
/*****************************************************************************************/

/// @brief 使迭代器前进 n 个距离
/// @tparam InputIterator  迭代器类型
/// @tparam Distance     距离类型
/// @param i  迭代器
/// @param n  距离
/// @param    迭代器的类型
template <class InputIterator, class Distance>
void __advance(InputIterator& i, Distance n, input_iterator_tag) {
    while (n--) ++i;
}

template <class BidirectionalIterator, class Distance>
void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
    if (n >= 0) while (n--) ++i;
    else while (n++) --i;
}

template <class RandomAccessIterator, class Distance>
void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag) {
    i += n;
}

template <class InputIterator, class Distance>
void advance(InputIterator& i, Distance n) {
    __advance(i, n, iterator_category(i));
}


// ====================================== reverse_iterator ====================================== //
// 模板类：reverse_iterator

template <class Iterator>
class reverse_iterator {

    private:
        Iterator current;  // 记录对应的正向迭代器

    public:
        // 反向迭代器的五种相应型别
        typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
        typedef typename iterator_traits<Iterator>::value_type        value_type;
        typedef typename iterator_traits<Iterator>::difference_type   difference_type;
        typedef typename iterator_traits<Iterator>::pointer           pointer;
        typedef typename iterator_traits<Iterator>::reference         reference;

        typedef Iterator                                              iterator_type;
        typedef reverse_iterator<Iterator>                            self;

    public:
        
        // 构造函数
        reverse_iterator() {}
        explicit reverse_iterator(iterator_type i) :current(i) {}
        reverse_iterator(const self& rhs) :current(rhs.current) {}

    public:

        // 取出对应的正向迭代器
        iterator_type base() const { return current; }

        // 重载操作符
        reference operator*() const { 
            // 实际对应正向迭代器的前一个位置
            auto tmp = current;
            return *--tmp;
        }

        pointer operator->() const {
            return &(operator*());
        }

        // 前进(++)变为后退(--)
        self& operator++() {
            --current;
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            --current;
            return tmp;
        }

        // 后退(--)变为前进(++)
        self& operator--() {
            ++current;
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            ++current;
            return tmp;
        }

        self& operator+=(difference_type n) {
            current -= n;
            return *this;
        }

        self operator+(difference_type n) const {
            return self(current - n);
        }

        self& operator-=(difference_type n) {
            current += n;
            return *this;
        }

        self operator-(difference_type n) const {
            return self(current + n);
        }

        reference operator[](difference_type n) const {
            return *(*this + n);
        }

};

// 重载 operator-
template <class Iterator>
typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& lhs,
          const reverse_iterator<Iterator>& rhs) {
    return rhs.base() - lhs.base();
}

// 重载比较操作符
template <class Iterator>
bool operator==(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
    return lhs.base() == rhs.base();
}

template <class Iterator>
bool operator<(const reverse_iterator<Iterator>& lhs,
  const reverse_iterator<Iterator>& rhs) {
    return rhs.base() < lhs.base();
}

template <class Iterator>
bool operator!=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
    return !(lhs == rhs);
}

template <class Iterator>
bool operator>(const reverse_iterator<Iterator>& lhs,
               const reverse_iterator<Iterator>& rhs) {
    return rhs < lhs;
}

template <class Iterator>
bool operator<=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
    return !(rhs < lhs);
}

template <class Iterator>
bool operator>=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
    return !(lhs < rhs);
}


// ====================================== back_insert_iterator ====================================== //

/// @brief 配接器 back_insert_iterator，将赋值操作转换为 push_back 操作
/// @tparam Container 
template <class Container>
class back_insert_iterator {
protected:
    Container* container;  // 底层容器

public:  // 类型定义
    // 由于需要进行写入操作，因此迭代器类型为 output_iterator_tag
    typedef output_iterator_tag     iterator_category;
    typedef void                    value_type;
    typedef void                    difference_type;
    typedef void                    pointer;
    typedef void                    reference;
    typedef Container               container_type;

    // 构造函数
    explicit back_insert_iterator(Container& x) :container(&x) {}

    /// @brief back_insert_iterator 功能的关键所在，将赋值操作转换为 push_back 操作
    back_insert_iterator& operator=(const typename Container::value_type& value) {
        container->push_back(value);
        return *this;
    }

    back_insert_iterator& operator=(typename Container::value_type&& value) {
        container->push_back(std::move(value));
        return *this;
    }

    // 重载操作符
    // 这三个操作符什么都不做，返回自身
    back_insert_iterator& operator*() { return *this; }
    back_insert_iterator& operator++() { return *this; }
    back_insert_iterator& operator++(int) { return *this; }
};


/// @brief 辅助函数，返回一个容器的 back_insert_iterator
template <class Container>
inline back_insert_iterator<Container> back_inserter(Container& x) {
    return back_insert_iterator<Container>(x);
}


// ====================================== front_insert_iterator ====================================== //

/// @brief 配接器 front_insert_iterator，将赋值操作转换为 push_front 操作
/// @tparam Container 
template <class Container>
class front_insert_iterator {
protected:
    Container* container;  // 底层容器

public:  // 类型定义
    typedef output_iterator_tag     iterator_category;
    typedef void                    value_type;
    typedef void                    difference_type;
    typedef void                    pointer;
    typedef void                    reference;
    typedef Container               container_type;

    // 构造函数
    explicit front_insert_iterator(Container& x) :container(&x) {}

    // NOTE: 由于调用的是 push_front，因此不适用于 vector

    /// @brief front_insert_iterator 功能的关键所在，将赋值操作转换为 push_front 操作
    front_insert_iterator& operator=(const typename Container::value_type& value) {
        container->push_front(value);
        return *this;
    }

    front_insert_iterator& operator=(typename Container::value_type&& value) {
        container->push_front(std::move(value));
        return *this;
    }

    // 重载操作符
    // 这三个操作符什么都不做，返回自身
    front_insert_iterator& operator*() { return *this; }
    front_insert_iterator& operator++() { return *this; }
    front_insert_iterator& operator++(int) { return *this; }
};

/// @brief 辅助函数，返回一个容器的 front_insert_iterator
template <class Container>
inline front_insert_iterator<Container> front_inserter(Container& x) {
    return front_insert_iterator<Container>(x);
}

}  // namespace tinystl

#endif  // TINYSTL_ITERATOR_H_