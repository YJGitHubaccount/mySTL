/**
 * 该文件实现heap的演算法，没有实际的heap类
 * 注：这些算法不允许指定比较标准，使用需重载>,<操作符
*/

#include "iterator.h"

namespace mystl
{
    /**
     * @param holeIndex = 最后一个元素（插入）索引
     * @param topIndex = 0
    */
    template <class RandomAccessIterator, class Distance, class T>
    void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value)
    {
        Distance parent = (holeIndex - 1) / 2; //找出父节点
        while (holeIndex > topIndex && *(first + parent) < value)
        {
            //若未到达顶点，且父节点值小于新值,进行回溯
            *(first + holeIndex) = *(first + parent);
            holeIndex = parent;
            parent = (holeIndex - 1) / 2;
        }
        *(first + holeIndex) = value;
    }

    template <class RandomAccessIterator, class Distance, class T>
    void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance *, T *)
    {
        __push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
    }

    /**
     * 注意：该函数使用时，新元素已经置于容器的最尾端
    */
    template <class RandomAccessIterator>
    inline void push_heap(RandomAccessIterator first, RandomAccessIterator last)
    {
        __push_heap_aux(first, last, distance_type(first), value_type(first));
    }

    /**
     * 重排以holeIndex为根节点的子树
    */
    template <class RandomAccessIterator, class Distance, class T>
    void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value)
    {
        Distance topIndex = holeIndex;
        Distance secondChild = 2 * holeIndex + 2; //洞节点的右子节点 (2*(holdIndex+1)+1)-1
        while (secondChild < len)
        {
            //令secondChild为洞节点较大的子节点
            if (*(first + secondChild) < *(first + (secondChild - 1)))
            {
                secondChild--;
            }

            *(first + holeIndex) = *(first + secondChild);
            holeIndex = secondChild;
            secondChild = 2 * holeIndex + 2;
        }
        //没有右子节点，只有左子节点
        if (secondChild == len)
        {
            *(first + holeIndex) = *(first + (secondChild - 1));
            holeIndex = secondChild - 1;
        }
        //将调整值填入洞节点
        __push_heap(first, holeIndex, topIndex, value);
    }

    /**
     * @param result 存放结果的位置，为容器最低端
     * @param value  欲调整值,为容器最低端元素
    */
    template <class RandomAccessIterator, class Distance, class T>
    void __pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value, Distance *)
    {
        *result = *first;
        __adjust_heap(first, Distance(0), Distance(last - first), value);
    }

    template <class RandomAccessIterator, class T>
    void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T *)
    {
        __pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
    }

    template <class RandomAccessIterator>
    inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last)
    {
        __pop_heap_aux(first, last, value_type(first));
    }

    template <class RandomAccessIterator>
    inline void sort_heap(RandomAccessIterator first, RandomAccessIterator last)
    {
        while (last - first > 1)
        {
            pop_heap(first, last--);
        }
    }

    template <class RandomAccessIterator, class T, class Distance>
    void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T *, Distance *)
    {
        Distance len = last - first;
        if (len < 2)
            return;

        Distance holeIndex = (len - 2) / 2; //最后一个分支节点,index+1=len/2

        //初始化堆
        while (true)
        {
            __adjust_heap(first, holeIndex, len, T(*(first + holeIndex)));
            if (holeIndex == 0)
            {
                return;
            }
            holeIndex--;
        }
    }

    /**
     * 将[first,last)排列为一个heap
    */
    template <class RandomAccessIterator>
    inline void make_heap(RandomAccessIterator first, RandomAccessIterator last)
    {
        __make_heap(first, last, value_type(first), distance_type(first));
    }

} // namespace mystl
