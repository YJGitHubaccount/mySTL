#ifndef __VECTOR_H_
#define __VECTOR_H_

#include "algorithm.h"
#include "allocator.h"
#include "construct.h"
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
    vector(long n, const reference value)
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
            inset_aux(end(), x);
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
            inset_aux(end(), x);
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
            mystl::copy(position + 1, end(), position);
        pop_back();
        return position;
    }
    void resize(size_type new_size, const reference x)
    {
        if (new_size < size())
            erase(begin() + new_size, end());
        else
            insert(end(), new_size - size(), x);
    }
    void resize(size_type new_size)
    {
        resize(new_size, T());
    }
    void clear()
    {
        erase(begin(), end());
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
    void insert_aux(iterator position, const reference value)
    {
    }
    iterator allocate_and_fill(size_type n, const reference x)
    {
        iterator result = data_allocator::allocate(n);
        uninitialize_fill_n(result, n, x); //2.3½Ú
        return result;
    }
};

#endif // __vector_H_
