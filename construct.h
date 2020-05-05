#ifndef __CONSTRUCT_H_
#define __CONSTRUCT_H_

#include "iterator.h"
#include "type_traits.h"
#include <new>
namespace mystl
{

    template <class T1, class T2>
    inline void construct(T1 *p, const T2 &value)
    {
        new (p) T1(value); //唤起T1::T1(value)
    }

    //版本一，解构一个对象
    template <class T>
    inline void destroy(T *pointer)
    {
        pointer->~T(); //唤起destructor
    }

    //前置声明
    template <class ForwardIterator, class T>
    inline void __destroy(ForwardIterator first, ForwardIterator last, T *);
    template <class ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type);
    template <class ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type);

    //版本二,value_type与__type_trait<>在3.7节
    template <class ForwardIterator>
    inline void destroy(ForwardIterator first, ForwardIterator last)
    {
        __destroy(first, last, value_type(first));
    }

    //判断元素的数值类型value_type是否有trivial_destructor
    template <class ForwardIterator, class T>
    inline void __destroy(ForwardIterator first, ForwardIterator last, T *)
    {
        typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
        __destroy_aux(first, last, trivial_destructor());
    }

    //若元素的value_type有trivial destructor(默认析构器)
    template <class ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type) {}

    //若元素的value_type有non-trivial destructor
    template <class ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type)
    {
        for (; first < last; first++)
        {
            destroy(&*first);
        }
    }

    //destroy版本二针对char*和wchar_t*的特化版
    //Q:为什么要特化以下两种版本?
    //A:对于char*和wchar_t*类型的迭代器，由于char和wchar_t其都是内置类型，他们没有析构函数，也可以认为其析构是trivial的，
    //那么就不用调用析构函数了。由于，如果采用上面的版本，还需要进行__destroy()和__destroy_aux()，这样的调用其实是无用的，
    //因为最后什么都没有执行，为了效率的考虑，直接对第二版的destroy()函数模版进行特化。
    template <>
    inline void destroy(char *, char *) {}

    template <>
    inline void destroy(wchar_t *, wchar_t *) {}

    /*
注：destroy第二版本准备将[first,last)范围的所有物件解构，先利用value_type()判断迭代器所指物件的类型
，再利用__type_traits<T>判别该类型的结构式是否无关痛痒。若是__true_type,什么也不做,若是__false_type，
则遍历调用destroy版本一。
*/

} // namespace mystl
#endif //__CONSTRUCT_H_
