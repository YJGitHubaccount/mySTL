//author:James_Y
//该文件主要实现优先队列priority_queue

#ifndef __PRIORITY_QUEUE_H_
#define __PRIORITY_QUEUE_H_

#include "heap_algo_copy.h"
#include "vector.h"

namespace mystl
{
    template <class T, class Sequence = vector<T>, class Compare = less<typename Sequence::value_type>>
    class priority_queue
    {
    public:
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::size_type size_type;
        typedef typename Sequence::reference reference;
        typedef typename Sequence::const_reference const_reference;

    protected:
        Sequence c;
        Compare comp;

    public:
        priority_queue() = default;
        template <class InputIterator>
        priority_queue(InputIterator first, InputIterator last, const Compare &x) : c(first, last), comp(x)
        {
            make_heap(c.begin(), c.end(), comp);
        }
        template <class InputIterator>
        priority_queue(InputIterator first, InputIterator last) : c(first, last)
        {
            make_heap(c.begin(), c.end(), comp);
        }
        ~priority_queue() = default;

        bool empty() { return c.empty(); }
        size_type size() { return c.size(); }
        const_reference top() const { return c.front(); }
        void push(const value_type &x)
        {
            c.push_back(x);
            push_heap(c.begin(), c.end(), comp);
        }

        void pop()
        {
            pop_heap(c.begin(), c.end(), comp);
            c.pop_back();
        }
    };

} // namespace mystl

#endif // __PRIORITY_QUEUE_H_