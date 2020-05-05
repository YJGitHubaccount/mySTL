#ifndef __ALGORITHM_H_
#define __ALGORITHM_H_

#include "functor.h"
#include "iterator.h"
namespace mystl
{
#if 0
    template <class InputIterator, class OutputIterator>
    OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
    {
        while (first != last)
        {
            *result = *first;
            ++result;
            ++first;
        }
        return result;
    }

    template <class BidirectionalItterator1, class BidirectionalItterator2>
    BidirectionalItterator2 copy_backward(BidirectionalItterator1 first, BidirectionalItterator1 last, BidirectionalItterator2 result)
    {
        BidirectionalItterator2 cur = result;
        while (last != first)
        {
            *(--cur) = *(--last);
        }
        return cur;
    }
#endif

    template <class InputIterator, class Predicate>
    typename iterator_traits<InputIterator>::difference_type count_if(InputIterator first, InputIterator last, Predicate pred)
    {
        typename iterator_traits<InputIterator>::difference_type count = 0;
        for (; first != last; first++)
        {
            if (pred(*first))
                count++;
        }
        return count;
    }

} // namespace mystl

#endif //__ALGORITHM_H_