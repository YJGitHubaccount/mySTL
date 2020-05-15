#ifndef __DEQUE_H_
#define __DEQUE_H_

#include "allocator.h"
#include <algorithm>

namespace mystl
{
    template <class T, size_t BufSiz>
    struct __deque_iterator
    {
        typedef __deque_iterator<T, BufSiz> iterator;
        typedef const iterator const_iterator;

        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T **map_pointer;

        typedef __deque_iterator self;

        pointer cur;      //指向缓冲区当前元素
        pointer first;    //指向缓冲区头元素
        pointer last;     //指向缓冲区尾元素
        map_pointer node; //指向管控中心

        inline size_t __deque_buf_size(size_t n, size_t sz)
        {
            return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
        }

        static size_t buffer_size()
        {
            return __deque_buf_size(BufSiz, sizeof(T));
        }

        void set_node(map_pointer new_node)
        {
            node = new_node;
            first = *new_node;
            last = first + difference_type(buffer_size());
        }

        reference operator*()
        {
            return *cur;
        }
        pointer operator->()
        {
            return &(operator*());
        }
        difference_type operator-(const self &x) const
        {
            return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
        }
        self &operator++()
        {
            ++cur;
            if (cur == last)
            {
                set_node(node + 1);
                cur = first;
            }
            return *this;
        }
        self operator++(int)
        {
            self temp = *this;
            ++*this;
            return temp;
        }
        self &operator--()
        {
            if (cur == first)
            {
                set_node(node - 1);
                cur = last;
            }
            --cur;
            return *this;
        }
        self operator--(int)
        {
            self temp = *this;
            --*this;
            return temp;
        }
        self &operator+=(difference_type n)
        {
            difference_type offset = n + (cur - first);
            if (offset >= 0 && offset < difference_type(buffer_size()))
            {
                cur += n;
            }
            else
            {
                difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size()) : -difference_type((-offset - 1) / buffer_size()) - 1;
                set_node(node + node_offset);
                cur = first + (offset - node_offset * difference_type(buffer_size()));
            }
            return *this;
        }
        self operator+(difference_type n) const
        {
            self temp = *this;
            return temp += n;
        }
        self &operator-=(difference_type n)
        {
            return (*this) += -n;
        }
        self operator-(difference_type n) const
        {
            self temp = *this;
            return temp -= n;
        }
        reference operator[](difference_type n) const
        {
            return *(*this + n);
        }
        bool operator==(const self &x) const
        {
            return cur == x.cur;
        }
        bool operator!=(const self &x) const
        {
            return !(*this == x);
        }
        bool operator<(const self &x) const
        {
            return (node == x.node) ? (cur < x.cur) : (node < x.node);
        }
    };

    template <class T, class Alloc = allocator<T>, size_t BufSiz = 0>
    class deque
    {
    public:
        typedef T value_type;
        typedef value_type *pointer;
        typedef value_type &reference;
        typedef size_t size_type;
        typedef __deque_iterator<T, BufSiz> iterator;

    protected:
        typedef pointer *map_pointer;
        typedef simple_alloc<value_type, Alloc> data_allocator;
        typedef simple_alloc<pointer, Alloc> map_allocator;

    protected:
        iterator start;
        iterator finish;
        map_pointer map;
        size_type map_size;

    protected:
        void fill_initialize(size_type n, const value_type &value)
        {
            create_map_and_nodes(n);
            map_pointer cur;
            try
            {
                for (cur = start.node; cur < finish.node; ++cur)
                    uninitialized_fill(*cur, *cur + buffer_size(), value);
                uninitialized_fill(finish.first, finish.cur, value);
            }
            catch (...)
            {
                for (cur = start.node; cur <= finish.node; ++cur)
                {
                    destroy(cur, cur + buffer_size());
                    deallocate_node(cur);
                }
                destroy(map, map + map_size);
                deallocate_map(map);
            }
        }
        pointer allocate_node()
        {
            return data_allocator::allocate(buffer_size());
        }
        void deallocate_node(pointer x)
        {
            data_allocator::deallocate(x);
        }
        void dellocate_map(map_pointer x)
        {
            map_allocator::deallocate(x);
        }
        void create_map_and_nodes(size_type num_elements)
        {
            //已经分配的buffer
            size_type num_nodes = num_elements / buffer_size() + 1;
            //总buffer
            map_size = std::max(initial_map_size(), num_nodes + 2);
            map = map_allocator::allocate(map_size);
            //    map_size - num_nodes == 已经分配的内存中 不需要初始化的部分
            map_pointer nstart = map + (map_size - num_nodes) / 2;
            map_pointer nfinish = nstart + num_nodes - 1;

            map_pointer cur;
            try
            {
                for (cur = nstart; cur <= nfinish; ++cur)
                {
                    *cur = allocate_node();
                }
            }
            catch (...)
            {
                for (cur = nstart; cur <= nfinish; ++cur)
                {
                    destroy(*cur, *cur + buffer_size());
                    deallocate_node(*cur);
                }
                destroy(map, map + map_size);
                deallocate_map(map);
                throw;
            }
            start.set_node(nstart);
            finish.set_node(finish);
            start.cur = start.first;
            finish.cur = finish.first + num_elements % buffer_size();
        }

    public:
        deque(size_type n, const value_type &value)
            : start(), finish(), map(nullptr), map_size(0)
        {
            fill_initialize(n, value);
        }

        void push_back(const value_type &t)
        {
            if (finish.cur != finish.last - 1)
            {
                construct(finish.cur, t);
                ++finish.cur;
            }
            else
            {
                push_back_aux(t);
            }
        }
        void push_front(const value_type &t)
        {
        }
        void push_back_aux(const value_type &t)
        {
            value_type t_copy = t;
            reserve_map_at_back()
        }
        void push_front_aux(const value_type &t)
        {
        }
        void reverse_map_at_back(size_type nodes_to_add = 1)
        {
            //尾端空间不足
            if (nodes_to_add + 1 > map_size - (finish.node - map))
            {
                reallocate_map(nodes_to_add, false);
            }
        }
        void reverse_map_at_front(size_type nodes_to_add = 1)
        {
            //前端空间不足
            if (nodes_to_add > start.node - map)
            {
                reallocate_map(nodes_to_add, true);
            }
        }
        void reallocate_map(size_type nodes_to_add, bool add_at_front)
        {
        }
        iterator begin() const
        {
            return start;
        }
        iterator end() const
        {
            return finish;
        }

        reference operator[](size_type n) const
        {
            return start[difference_type(n)];
        }

        reference front() const
        {
            return *start;
        }
        reference back() const {
            return *(finish - 1)}

        size_type size() const
        {
            return size_type(finish - start);
        }
        size_type max_size() const
        {
            return size_type(-1);
        }
        bool empty() const
        {
            return finish == start;
        }
    };
} // namespace mystl
#endif //__DEQUE_H_