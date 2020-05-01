#ifndef __ALGORITHM_H_
#define __ALGORITHM_H_

namespace mystl
{
#if 1
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
#endif
} // namespace mystl

#endif //__ALGORITHM_H_