//author:James_Y
//该文件主要实现双向链表list

#ifndef __LIST_H_
#define __LIST_H_

#include "allocator.h"
#include "iterator.h"

namespace mystl
{
    template <class T>
    struct __list_node
    {
        typedef __list_node<T> *pointer;
        pointer prev;
        pointer next;
        T data;
    };

    template <class T>
    struct __list_iterator
    {
        typedef __list_iterator<T> iterator;

        typedef bidirectional_iterator_tag iterator_category;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef __list_node<T> *link_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        link_type node; //指向list节点

        //constructor
        __list_iterator(link_type x) : node(x) {}
        __list_iterator() {}
        __list_iterator(const iterator &x) : node(x.node) {}

        bool operator==(const iterator &rhs) const
        {
            return node == rhs.node;
        }
        bool operator!=(const iterator &rhs) const
        {
            return !(*this == rhs);
        }
        reference operator*() const
        {
            return (*node).data;
        }
        pointer operator->() const
        {
            return &(operator*());
        }
        iterator &operator++()
        {
            node = (link_type)((*node).next);
            return *this;
        }
        iterator operator++(int)
        {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }
        iterator &operator--()
        {
            node = (link_type)((*node).prev);
            return *this;
        }
        iterator operator--(int)
        {
            iterator tmp = *this;
            --*this;
            return tmp;
        }
    };

    template <class T, class Alloc = mystl::allocator<T>>
    class list
    {
    protected:
        typedef __list_node<T> list_node;
        typedef mystl::simple_alloc<list_node, allocator<list_node>> list_node_allocator;

    public:
        typedef list_node *link_type;
        typedef size_t size_type;
        typedef __list_iterator<T> iterator;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef mystl::simple_alloc<value_type, Alloc> data_allocator;

    protected:
        link_type node;

    public:
        list()
        {
            empty_initialize();
        }
        list(pointer begin, pointer end)
        {
            while (begin != end)
            {
                push_back(*begin);
                begin++;
            }
        }
        ~list()
        {
            clear();
            put_node(node);
        }
        iterator begin() const
        {
            return node->next;
        }
        iterator end() const
        {
            return node;
        }
        bool empty() const
        {
            return begin() == end();
        }
        size_type size() const
        {
            size_t result = 0;
            result = distance(begin(), end());
            return result;
        }
        reference front() const
        {
            return *begin();
        }
        reference back() const
        {
            return *(--end());
        }
        //在position所指位置安插节点,并返回安插节点的iterator
        iterator insert(iterator position, const reference x)
        {
            link_type temp = create_node(x);
            temp->next = position.node;
            temp->prev = position.node->prev;
            (position.node->prev)->next = temp;
            position.node->prev = temp;
            return temp;
        }
        void push_back(const reference x)
        {
            insert(end(), x);
        }
        void push_back(value_type &&x)
        {
            insert(end(), x);
        }
        void push_front(const reference x)
        {
            insert(begin(), x);
        }
        void push_front(value_type &&x)
        {
            insert(begin(), x);
        }
        //删除指定节点,并返回删除节点的下一个节点的iterator
        iterator erase(iterator position)
        {
            link_type next_node = position.node->next;
            link_type prev_node = position.node->prev;
            prev_node->next = next_node;
            next_node->prev = prev_node;
            destroy_node(position.node);
            return next_node;
        }
        //删除[first,last)范围内元素
        iterator erase(iterator first, iterator last)
        {
            link_type prev_node = first.node->prev;
            for (; first != last; first++)
            {
                destroy(first.node);
            }
            prev_node->next = last.node;
            last.node->prev = prev_node;
            return last;
        }
        void pop_front()
        {
            erase(begin());
        }
        void pop_back()
        {
            erase(--end());
        }
        //清除所有节点
        void clear()
        {
            link_type cur = node->next;
            while (cur != node)
            {
                link_type temp = cur;
                cur = cur->next;
                destroy_node(temp);
            }
            node->next = node;
            node->prev = node;
        }
        //删除数值为value的所有元素
        void remove(const reference value)
        {
            for (iterator iter = begin(); iter != end();)
            {
                if (*iter == value)
                {
                    iter = erase(iter);
                }
                else
                {
                    iter++;
                }
            }
        }
        //移除数值相同的连续元素最终只保留一个
        void unique()
        {
            iterator first = begin();
            iterator last = end();
            if (first == last)
                return;
            iterator next = first;
            while (++next != last)
            {
                if (*first == *next)
                    erase(next);
                else
                {
                    first = next;
                    next = first;
                }
            }
        }

    protected:
        //配置一个节点并返回
        link_type get_node()
        {
            return list_node_allocator::allocate();
        }
        //释放一个节点
        void put_node(link_type p)
        {
            list_node_allocator::deallocate(p);
        }
        //产生一个节点并带有元素值
        link_type create_node(const reference x)
        {
            link_type p = get_node();
            mystl::construct(&p->data, x);
            return p;
        }
        //解构并释放一个节点
        void destroy_node(link_type p)
        {
            destroy(&p->data);
            put_node(p);
        }
        //产生一个空list
        void empty_initialize()
        {
            node = get_node();
            node->prev = node;
            node->next = node;
        }
#if 0
    //将[first,last)内的所有元素搬移到position之前,p139
    void transfer(iterator position, iterator first, iterator last)
    {

    }
    void spice()
#endif
    };

} // namespace mystl
#endif //__LIST_H_
