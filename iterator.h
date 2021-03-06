#ifndef __ITERATOR_H_
#define __ITERATOR_H_

//该文件主要实现了iterator_traits
#include <stddef.h> //ptrdiff_t,size_t
#
namespace mystl
{
    //iterator_catetory_tag
    struct input_iterator_tag
    {
    };
    struct output_iterator_tag
    {
    };
    struct forward_iterator_tag : public input_iterator_tag
    {
    };
    struct bidirectional_iterator_tag : public forward_iterator_tag
    {
    };
    struct random_access_iterator_tag : public bidirectional_iterator_tag
    {
    };

    template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T *, class Reference = T &>
    struct iterator
    {
        typedef Category iterator_category;
        typedef T value_type;
        typedef Distance difference_type;
        typedef Pointer pointer;
        typedef Reference reference;
    };

    //前置声明
    template <class Iterator>
    struct iterator_traits;

    template <class Iterator>
    class reverse_iterator
    {
    protected:
        Iterator current; //正向iterator
        //逆向iterator取值=正向iterator退一位取值
    public:
        typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
        typedef typename iterator_traits<Iterator>::value_type value_type;
        typedef typename iterator_traits<Iterator>::difference_type difference_type;
        typedef typename iterator_traits<Iterator>::pointer pointer;
        typedef typename iterator_traits<Iterator>::reference reference;
        typedef Iterator iterator_type;
        typedef reverse_iterator<Iterator> self;

    public:
        explicit reverse_iterator(iterator_type x) : current(x) {}
        reverse_iterator(const self &x) : current(x.current) {}
        //取出对应的正向iterator
        iterator_type base() const
        {
            return current;
        }
        reference operator*() const
        {
            Iterator temp = current;
            return *(--temp);
        }
        pointer operator->() const
        {
            return &(operator*());
        }
        self &operator++()
        {
            --current;
            return *this;
        }
        self &operator--()
        {
            ++current;
            return *this;
        }
        self operator+(difference_type n) const
        {
            return self(current - n);
        }
        self operator-(difference_type n) const
        {
            return self(current + n);
        }
        bool operator==(iterator_type iter)
        {
            return (current == iter);
        }
        bool operator!=(iterator_type iter)
        {
            return !(current == iter);
        }
        bool operator==(self iter)
        {
            return (current == iter);
        }
        bool operator!=(self iter)
        {
            return !(current == iter);
        }
    };

    template <class Container>
    class insert_iterator
    {
    protected:
        Container *container;
        typename Container::iterator iter;

    public:
        typedef output_iterator_tag iterator_category; //注意类型
        typedef typename iterator_traits<typename Container::iterator>::value_type value_type;
        typedef typename iterator_traits<typename Container::iterator>::difference_type difference_type;

        insert_iterator(Container &x, typename Container::iterator i) : container(&x), iter(i) {}

        insert_iterator<Container> &operator=(typename Container::value_type &value)
        {
            iter = container->insert(iter, value);
            ++iter;
            return *this;
        }
        //忽略++操作
        insert_iterator<Container> &operator++()
        {
            return *this;
        }
        //忽略*操作
        insert_iterator<Container> &operator*()
        {
            return *this;
        }
    };

    template <class Container, class Iterator>
    inline insert_iterator<Container> inserter(Container &x, Iterator i)
    {
        return insert_iterator<Container>(x, typename Container::iterator(i));
    }

    template <class Iterator>
    struct iterator_traits
    {
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type value_type;
        typedef typename Iterator::difference_type difference_type;
        typedef typename Iterator::pointer pointer;
        typedef typename Iterator::reference reference;
    };

    //针对native pointer而设计的traits
    template <class T>
    struct iterator_traits<T *>
    {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef T *pointer;
        typedef T &reference;
    };

    //针对native const pointer而设计的traits
    template <class T>
    struct iterator_traits<const T *>
    {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef const T *pointer;
        typedef const T &reference;
    };

    //这个函数用于确定iterator的category
    template <class Iterator>
    inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator &)
    {
        typedef typename iterator_traits<Iterator>::iterator_category category;
        return category();
    }

    //这个函数用于确定iterator的distance_type
    template <class Iterator>
    inline typename iterator_traits<Iterator>::difference_type *distance_type(const Iterator &)
    {
        return static_cast<typename iterator_traits<Iterator>::difference_type *>(0);
    }

    //这个函数用于确定iterator的value_type
    template <class Iterator>
    inline typename iterator_traits<Iterator>::value_type *value_type(const Iterator &)
    {
        return static_cast<typename iterator_traits<Iterator>::value_type *>(0);
    }

    //以下是distance函数，用于计算两个iterator之间的距离
    template <class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type __distance(InputIterator first, InputIterator last, input_iterator_tag)
    {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last)
        {
            ++first;
            ++n;
        }
        return n;
    }
    //Q：为什么没有forward_iterator_tag,bidirectional_iterator_tag的特化版本
    //A:因为bidirectional_iterator_tag继承于forward_iterator_tag，forward_iterator_tag继承于input_iterator_tag
    template <class RandomAccessIterator>
    inline typename iterator_traits<RandomAccessIterator>::difference_type __distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
    {
        return first - last;
    }

    template <class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last)
    {
        typedef typename iterator_traits<InputIterator>::iterator_category category;
        return __distance(first, last, category());
    }

    //以下advance函数,可以使iterator前进
    template <class InputIterator, class Distance>
    inline void __advance(InputIterator &i, Distance n, input_iterator_tag)
    {
        while (n--)
            ++i;
    }

    template <class BidirectionalItterator, class Distance>
    inline void __advance(BidirectionalItterator &i, Distance n, bidirectional_iterator_tag)
    {
        if (n >= 0)
            while (n--)
                ++i;
        else
            while (n++)
                --i;
    }

    template <class RandomAccessIterator, class Distance>
    inline void __advance(RandomAccessIterator &i, Distance n, random_access_iterator_tag)
    {
        i += n;
    }

    template <class InputIterator, class Distance>
    inline void advance(InputIterator &i, Distance n)
    {
        __advance(i, n, iterator_category(i));
    }

} // namespace mystl
#endif //__ITERATOR_H_
