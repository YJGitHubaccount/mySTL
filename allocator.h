#ifndef __ALLOCATOR_H_
#define __ALLOCATOR_H_

//该头文件包含一个模板类allocator

#include "construct.h"
#include "util.h"

namespace mystl
{

template <class T, class Alloc>
class simple_alloc
{
public:
    static T *allocate(size_t n)
    {
        return 0 == n ? 0 : (T *)Alloc::allocate(n * sizeof(T));
    }
    static T *allocate()
    {
        return (T *)Alloc::allocate(sizeof(T));
    }
    static void deallocate(T *p, size_t n)
    {
        if (0 != n)
            Alloc::deallocate(p, n * sizeof(T));
    }
    static void deallocate(T *p)
    {
        Alloc::deallocate(p);
    }
};

#if 1
template <class T>
class allocator
{
public:
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

public:
    static T *allocate(size_type n);

    static void deallocate(T *ptr);
    static void deallocate(T *ptr, size_type n);

    static void construct(T *ptr);
    static void construct(T *ptr, const_reference value);
    static void construct(T *ptr, T &&value);

    static void destroy(T *ptr);
    static void destroy(T *first, T *last);
};

template <class T>
T *allocator<T>::allocate(size_type n)
{
    if (0 == n)
        return nullptr;
    return static_cast<T *>(::operator new(n));
}

template <class T>
void allocator<T>::deallocate(T *ptr)
{
    if (ptr == nullptr)
        return;
    ::operator delete(ptr);
}

template <class T>
void allocator<T>::deallocate(T *ptr, size_type n)
{
    if (ptr == nullptr)
        return;
    ::operator delete(ptr);
}

template <class T>
void allocator<T>::construct(T *ptr)
{
    mystl::construct(ptr);
}

template <class T>
void allocator<T>::construct(T *ptr, const_reference value)
{
    mystl::construct(ptr, value);
}

template <class T>
void allocator<T>::construct(T *ptr, T &&value)
{
    mystl::construct(ptr, std::move(value));
}

template <class T>
void allocator<T>::destroy(T *ptr)
{
    mystl::destroy(ptr);
}

template <class T>
void allocator<T>::destroy(T *first, T *last)
{
    mystl::destroy(first, last);
}

#endif //class allocator

} //namespace mystl

#endif
