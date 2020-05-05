#ifndef __VECTOR_H_
#define __VECTOR_H_
#include "algorithm.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"
#include <algorithm>

namespace mystl
{
    template <class T, class Alloc = mystl::allocator<T>>
    class vector
    {
    public:
        typedef T value_type;
        typedef value_type *iterator;
        typedef T *pointer;
        typedef T &reference;
        typedef size_t size_type;

    protected:
        typedef mystl::simple_alloc<value_type, Alloc> data_allocator;

        iterator start;
        iterator finish;
        iterator end_of_storage;

    public:
        //ctor
        vector() : start(0), finish(0), end_of_storage(0) {}
        vector(size_type n, const reference value)
        {
            fill_initialize(n, value);
        }
        vector(int n, const reference value)
        {
            fill_initialize(n, value);
        }
        vector(int n, T &&value)
        {
            fill_initialize(n, value);
        }
        explicit vector(size_type n)
        {
            fill_initialize(n, T());
        }
        ~vector()
        {
            mystl::destroy(start, finish);
            deallocate();
        }

        reference operator[](size_type n)
        {
            return *(begin() + n);
        }

        iterator begin() const
        {
            return start;
        }
        iterator end() const
        {
            return finish;
        }
        bool empty()
        {
            return start == finish;
        }
        bool full()
        {
            return finish == end_of_storage;
        }
        size_type size() const
        {
            return size_type(end() - begin());
        }
        size_type capacity() const
        {
            return size_type(end_of_storage - begin());
        }
        value_type front() const
        {
            return *begin();
        }
        value_type back() const
        {
            return *(end() - 1);
        }

        void push_back(const reference x)
        {
            if (!full())
            {
                mystl::construct(finish, x);
                ++finish;
            }
            else
            {
                insert_aux(end(), x);
            }
        }
        void push_back(T &&x)
        {
            if (!full())
            {
                mystl::construct(finish, x);
                ++finish;
            }
            else
            {
                insert_aux(end(), x);
            }
        }
        void pop_back()
        {
            --finish;
            mystl::destroy(finish);
        }
        iterator erase(iterator position)
        {
            if (position + 1 != end())
                std::copy(position + 1, end(), position);
            pop_back();
            return position;
        }
        iterator erase(iterator first, iterator last)
        {
            iterator i = std::copy(last, finish, first);
            destroy(i, finish);
            finish = finish - (last - first);
            return first;
        }
        void resize(size_type new_size, const reference x)
        {
            if (new_size < size())
                erase(begin() + new_size, end());
            else
                insert(end(), new_size - size(), x);
        }
        void resize(size_type new_size, T &&x)
        {
            resize(new_size, x);
        }
        void resize(size_type new_size)
        {
            resize(new_size, T());
        }
        void clear()
        {
            erase(begin(), end());
        }
        //安插n个元素
        void insert(iterator position, size_type n, const reference x)
        {
            if (n != 0)
            {
                if (size_type(end_of_storage - finish) >= n)
                {
                    //备用空间足够的情况
                    value_type x_copy = x;
                    const size_type elems_after = finish - position;
                    iterator old_finish = finish;
                    if (elems_after > n)
                    {
                        uninitialized_copy(finish - n, finish, finish);
                        finish += n;
                        std::copy_backward(position, old_finish - n, old_finish);
                        std::fill(position, position + n, x_copy);
                    }
                    else
                    {
                        uninitialized_fill_n(finish, n - elems_after, x_copy);
                        finish += n - elems_after;
                        uninitialized_copy(position, old_finish, finish);
                        finish += elems_after;
                        std::fill(position, old_finish, x_copy);
                    }
                }
                else
                {
                    //备用空间不够
                    const size_type old_size = size();
                    const size_type len = old_size + std::max(old_size, n);

                    iterator new_start = data_allocator::allocate(len);
                    iterator new_finish = new_start;
                    try
                    {
                        new_finish = uninitialized_copy(start, position, new_start);
                        new_finish = uninitialized_fill_n(new_finish, n, x);
                        new_finish = uninitialized_copy(position, finish, new_finish);
                    }
                    catch (...)
                    {
                        destroy(new_start, new_finish);
                        data_allocator::deallocate(new_start, len);
                        throw;
                    }
                    destroy(start, finish);
                    deallocate();

                    start = new_start;
                    finish = new_finish;
                    end_of_storage = new_start + len;
                }
            }
        }
        void insert(iterator position, size_type n, T &&x)
        {
            insert(position, n, x);
        }
        void insert(iterator position, T &&x)
        {
            insert(position, 1, x);
        }
        void insert(iterator first, iterator last, T &&x)
        {
            insert(first, size_type(last - first), x);
        }

    protected:
        void deallocate()
        {
            if (start)
            {
                data_allocator::deallocate(start, capacity());
            }
        }
        void fill_initialize(size_type n, const reference value)
        {
            start = allocate_and_fill(n, value);
            finish = start + n;
            end_of_storage = finish;
        }
        void fill_initialize(size_type n, T &&value)
        {
            fill_initialize(n, value);
        }
        iterator allocate_and_fill(size_type n, const reference x)
        {
            iterator result = data_allocator::allocate(n);
            uninitialized_fill_n(result, n, x);
            return result;
        }
        //安插一个元素
        void insert_aux(iterator position, const reference x)
        {
            if (!full())
            {
                construct(finish, *(finish - 1));
                ++finish;
                //Q:为什么创建一个对象x_copy，直接使用x不就可以了吗
                value_type x_copy = x;
                std::copy_backward(position, finish - 2, finish - 1);
                *position = x_copy;
            }
            else
            {
                const size_type old_size = size();
                const size_type len = old_size != 0 ? 2 * old_size : 1;

                iterator new_start = data_allocator::allocate(len);
                iterator new_finish = new_start;
                try
                {
                    //将[start,position)复制到新区域
                    new_finish = uninitialized_copy(start, position, new_start);
                    //插入新元素
                    construct(new_finish, x);
                    ++new_finish;
                    //将[position,finish)复制到新区域
                    new_finish = uninitialized_copy(position, finish, new_finish);
                }
                catch (...)
                {
                    //rollback
                    destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, len);
                    throw;
                }
                //释放原来的vector
                destroy(begin(), end());
                deallocate();

                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + len;
            }
        }
    };
} // namespace mystl

#endif // __vector_H_
