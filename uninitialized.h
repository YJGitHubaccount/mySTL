#ifndef __UNINITIALIZED_H_
#define __UNINITIALIZED_H_
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include <algorithm>
#include <string.h> //memmove

//该文件实现三个低阶函数uninitialized_fill_n,uninitialized_copy,uninitialized_fill
//commit or rollback还未完成
namespace mystl
{
    //前置声明
    template <class ForwardIterator, class Size, class T, class T1>
    inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T &x, T1 *);
    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __true_type);
    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __false_type);

    //Fill block of memory
    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T &x)
    {
        return __uninitialized_fill_n(first, n, x, value_type(first));
    }

    template <class ForwardIterator, class Size, class T, class T1>
    inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T &x, T1 *)
    {
        typedef typename __type_traits<T1>::is_POD_type is_POD;
        return __uninitialized_fill_n_aux(first, n, x, is_POD());
    }

    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __true_type)
    {
        return std::fill_n(first, n, x); //6.4.2节
    }

    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __false_type)
    {
        ForwardIterator cur = first;
        //异常处理未完成
        for (; n > 0; --n, ++cur)
        {
            construct(&*cur, x);
        }
        return cur;
    }

    //前置声明
    template <class InputIterator, class ForwardIterator, class T>
    inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T *);
    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type);
    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type);

    //Copy block of memory
    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
    {
        return __uninitialized_copy(first, last, result, value_type(result));
    }

    template <class InputIterator, class ForwardIterator, class T>
    inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T *)
    {
        typedef typename __type_traits<T>::is_POD_type is_POD;
        return __uninitialized_copy_aux(first, last, result, is_POD());
    }

    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type)
    {
        return std::copy(first, last, result);
    }

    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type)
    {
        ForwardIterator cur = result;
        for (; first != last; ++first, ++cur)
        {
            construct(&*cur, *first);
        }
        return cur;
    }

    //对const char*和const wchar_t*的特化版本
    template <>
    inline char *uninitialized_copy(const char *first, const char *last, char *result)
    {
        memmove(result, first, last - first);
        return result + (last - first);
    }

    template <>
    inline wchar_t *uninitialized_copy(const wchar_t *first, const wchar_t *last, wchar_t *result)
    {
        memmove(result, first, sizeof(wchar_t) * (last - first));
        return result + (last - first);
    }

    //前置声明
    template <class ForwardIterator, class T, class T1>
    inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x, T1 *);
    template <class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __true_type);
    template <class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __false_type);

    //fill block of memory
    template <class ForwardIterator, class T>
    inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x)
    {
        __uninitialized_fill(first, last, x, value_type(first));
    }

    template <class ForwardIterator, class T, class T1>
    inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x, T1 *)
    {
        typedef typename __type_traits<T1>::is_POD_type is_POD;
        return __uninitialized_fill_aux(first, last, x, is_POD());
    }

    template <class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __true_type)
    {
        std::fill(first, last, x);
    }

    template <class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __false_type)
    {
        ForwardIterator cur = first;
        //异常处理未完成
        for (; cur != last; ++cur)
        {
            construct(&*cur, x);
        }
    }
} // namespace mystl

#endif