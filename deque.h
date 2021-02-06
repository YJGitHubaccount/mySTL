#ifndef __DEQUE_H_
#define __DEQUE_H_

/**
 * 该文件实现deque，未完成。。
*/

#include "algorithm.h"
#include "allocator.h"
#include "uninitialized.h"

namespace mystl
{
    //由于buffer_size static，不能放到类中
    inline size_t __deque_buf_size(size_t n, size_t sz)
    {
        return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
    }

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

        //一个缓冲区存放多少个T元素
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

        reference operator*() const
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
        typedef ptrdiff_t difference_type;

    protected:
        typedef pointer *map_pointer;
        typedef simple_alloc<value_type, Alloc> data_allocator;
        typedef simple_alloc<pointer, allocator<pointer>> map_allocator; //这里似乎会出问题

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
                    uninitialized_fill(*cur, *cur + iterator::buffer_size(), value);
                uninitialized_fill(finish.first, finish.cur, value);
            }
            catch (...)
            {
                for (cur = start.node; cur <= finish.node; ++cur)
                {
                    destroy(cur, cur + iterator::buffer_size());
                    dellocate_node(*cur);
                }
                destroy(map, map + map_size);
                dellocate_map(map);
            }
        }
        pointer allocate_node()
        {
            return data_allocator::allocate(iterator::buffer_size());
        }
        void dellocate_node(pointer x)
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
            size_type num_nodes = num_elements / iterator::buffer_size() + 1;
            //总buffer
            map_size = std::max(8, static_cast<int>(num_nodes + 2));
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
                    destroy(*cur, *cur + iterator::buffer_size());
                    dellocate_node(*cur);
                }
                destroy(map, map + map_size);
                dellocate_map(map);
                throw;
            }
            start.set_node(nstart);
            finish.set_node(nfinish);
            start.cur = start.first;
            finish.cur = finish.first + num_elements % iterator::buffer_size();
        }
        void push_back_aux(const value_type &t)
        {
            value_type t_copy = t;
            reserve_map_at_back();
            *(finish.node + 1) = allocate_node();
            try
            {
                construct(finish.cur, t_copy);
                finish.set_node(finish.node + 1);
                finish.cur = finish.first;
            }
            catch (...)
            {
                dellocate_node(*(finish.node + 1));
                throw;
            }
        }
        void push_front_aux(const value_type &t)
        {
            value_type t_copy = t;
            reserve_map_at_front();
            *(start.node - 1) = allocate_node();
            try
            {
                start.set_node(start.node - 1);
                start.cur = start.last - 1;
                construct(start.cur, t_copy);
            }
            catch (...)
            {
                start.set_node(start.node + 1);
                start.cur = start.first;
                dellocate_node(*(start.node - 1));
                throw;
            }
        }
        void reserve_map_at_back(size_type nodes_to_add = 1)
        {
            //尾端空间不足
            if (nodes_to_add + 1 > map_size - (finish.node - map))
            {
                reallocate_map(nodes_to_add, false);
            }
        }
        void reserve_map_at_front(size_type nodes_to_add = 1)
        {
            //前端空间不足
            if (nodes_to_add > start.node - map)
            {
                reallocate_map(nodes_to_add, true);
            }
        }
        void reallocate_map(size_type nodes_to_add, bool add_at_front)
        {
            size_type old_num_nodes = finish.node - start.node + 1;
            size_type new_num_nodes = old_num_nodes + nodes_to_add;
            map_pointer new_nstart;
            if (map_size > 2 * new_num_nodes)
            {
                new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
                if (new_nstart < start.node)
                    copy(start.node, finish.node + 1, new_nstart);
                else
                    copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
            }
            else
            {
                size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
                // 配置㆒塊空間，準備給新 map 使用。
                map_pointer new_map = map_allocator::allocate(new_map_size);
                new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
                // 把原 map 內容拷貝過來。
                copy(start.node, finish.node + 1, new_nstart);
                // 釋放原 map
                map_allocator::deallocate(map, map_size);
                // 設定新 map 的起始位址與大小
                map = new_map;
                map_size = new_map_size;
            }
            // 重新設定迭代器 start 和 finish
            start.set_node(new_nstart);
            finish.set_node(new_nstart + old_num_nodes - 1);
        }
        // 只有當 finish.cur == finish.first 時才會被呼叫。
        void pop_back_aux()
        {
            dellocate_node(finish.first);     // 釋放最後㆒個緩衝區
            finish.set_node(finish.node - 1); // 調整 finish 的狀態，使指向
            finish.cur = finish.last - 1;     // ㆖㆒個緩衝區的最後㆒個元素
            destroy(finish.cur);              // 將該元素解構。
        }
        // 只有當 start.cur == start.last - 1 時才會被呼叫。
        void pop_front_aux()
        {
            destroy(start.cur);             // 將第㆒緩衝區的第㆒個元素解構。
            dellocate_node(start.first);    // 釋放第㆒緩衝區。
            start.set_node(start.node + 1); // 調整 start 的狀態，使指向
            start.cur = start.first;        // ㆘㆒個緩衝區的第㆒個元素。
        }
        iterator insert_aux(iterator pos, const value_type &x)
        {
            difference_type index = pos - start; // 安插點之前的元素個數
            value_type x_copy = x;
            if (index < size() / 2)
            {                            // 如果安插點之前的元素個數比較少
                push_front(front());     // 在最前端加入與第㆒元素同值的元素。
                iterator front1 = start; // 以㆘標示記號，然後進行元素搬移...
                ++front1;
                iterator front2 = front1;
                ++front2;
                pos = start + index;
                iterator pos1 = pos;
                ++pos1;
                copy(front2, pos1, front1); // 元素搬移
            }
            else
            {                            // 安插點之後的元素個數比較少
                push_back(back());       // 在最尾端加入與最後元素同值的元素。
                iterator back1 = finish; // 以㆘標示記號，然後進行元素搬移...
                --back1;
                iterator back2 = back1;
                --back2;
                pos = start + index;
                copy_backward(pos, back2, back1); // 元素搬移
            }
            *pos = x_copy; // 在安插點㆖設定新值
            return pos;
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
            if (start.cur != start.first)
            {
                construct(start.cur - 1, t);
                --start.cur;
            }
            else
                push_front_aux(t);
        }

        void pop_back()
        {
            if (finish.cur != finish.first)
            {
                // 最後緩衝區有㆒個（或更多）元素
                --finish.cur;        // 調整指標，相當於排除了最後元素
                destroy(finish.cur); // 將最後元素解構
            }
            else
                // 最後緩衝區沒有任何元素
                pop_back_aux(); // 這裡將進行緩衝區的釋放工作
        }

        void pop_front()
        {
            if (start.cur != start.last - 1)
            {
                // 第㆒緩衝區有㆒個（或更多）元素
                destroy(start.cur); // 將第㆒元素解構
                ++start.cur;        // 調整指標，相當於排除了第㆒元素
            }
            else
                // 第㆒緩衝區僅有㆒個元素
                pop_front_aux(); // 這裡將進行緩衝區的釋放工作
        }

        // 注意，最終需要保留㆒個緩衝區。這是 deque 的策略，也是 deque 的初始狀態。
        void clear()
        {
            // 以㆘針對頭尾以外的每㆒個緩衝區（它們㆒定都是飽滿的）
            for (map_pointer node = start.node + 1; node < finish.node; ++node)
            {
                // 將緩衝區內的所有元素解構。注意，呼叫的是 destroy() 第㆓版本，見 2.2.3 節
                destroy(*node, *node + iterator::buffer_size());
                // 釋放緩衝區記憶體
                data_allocator::deallocate(*node, iterator::buffer_size());
            }
            if (start.node != finish.node)
            {                                      // 至少有頭尾兩個緩衝區
                destroy(start.cur, start.last);    // 將頭緩衝區的目前所有元素解構
                destroy(finish.first, finish.cur); // 將尾緩衝區的目前所有元素解構
                // 以㆘釋放尾緩衝區。注意，頭緩衝區保留。
                data_allocator::deallocate(finish.first, iterator::buffer_size());
            }
            else                                // 只有㆒個緩衝區
                destroy(start.cur, finish.cur); // 將此唯㆒緩衝區內的所有元素解構
            // 注意，並不釋放緩衝區空間。這唯㆒的緩衝區將保留。
            finish = start; // 調整狀態
        }

        // 清除 pos 所指的元素。 pos 為清除點。
        iterator erase(iterator pos)
        {
            iterator next = pos;
            ++next;
            difference_type index = pos - start; // 清除點之前的元素個數
            if (index < (size() >> 1))
            {                                    // 如果清除點之前的元素比較少，
                copy_backward(start, pos, next); // 就搬移清除點之前的元素
                pop_front();                     // 搬移完畢，最前㆒個元素贅餘，去除之
            }
            else
            {                            // 清除點之後的元素比較少，
                copy(next, finish, pos); // 就搬移清除點之後的元素
                pop_back();              // 搬移完畢，最後㆒個元素贅餘，去除之
            }
            return start + index;
        }

        iterator erase(iterator first, iterator last)
        {
            if (first == start && last == finish)
            {            // 如果清除區間就是整個 deque
                clear(); // 直接呼叫 clear() 即可
                return finish;
            }
            else
            {
                difference_type n = last - first;             // 清除區間的長度
                difference_type elems_before = first - start; // 清除區間前方的元素個數
                if (elems_before < (size() - n) / 2)
                {                                      // 如果前方的元素比較少，
                    copy_backward(start, first, last); // 向後搬移前方元素（覆蓋清除區間）
                    iterator new_start = start + n;    // 標記 deque 的新起點
                    destroy(start, new_start);         // 搬移完畢，將贅餘的元素解構
                    // 以㆘將贅餘的緩衝區釋放
                    for (map_pointer cur = start.node; cur < new_start.node; ++cur)
                        data_allocator::deallocate(*cur, iterator::buffer_size());
                    start = new_start; // 設定 deque 的新起點
                }
                else
                {                                     // 如果清除區間後方的元素比較少
                    copy(last, finish, first);        // 向前搬移後方元素（覆蓋清除區間）
                    iterator new_finish = finish - n; // 標記 deque 的新尾點
                    destroy(new_finish, finish);      // 搬移完畢，將贅餘的元素解構
                    // 以㆘將贅餘的緩衝區釋放
                    for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
                        data_allocator::deallocate(*cur, iterator::buffer_size());
                    finish = new_finish; // 設定 deque 的新尾點
                }
                return start + elems_before;
            }
        }

        // 在 position 處安插㆒個元素，其值為 x
        iterator insert(iterator position, const value_type &x)
        {
            if (position.cur == start.cur)
            {                  // 如果安插點是 deque 最前端
                push_front(x); // 交給 push_front 去做
                return start;
            }
            else if (position.cur == finish.cur)
            {                 // 如果安插點是 deque 最尾端
                push_back(x); // 交給 push_back 去做
                iterator tmp = finish;
                --tmp;
                return tmp;
            }
            else
            {
                return insert_aux(position, x); // 交給 insert_aux 去做
            }
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
            return *(start);
        }
        reference back() const
        {
            return *(finish - 1);
        }

        size_type size() const
        {
            return size_type(finish - start);
        }
        //the maximum number of elements that the deque container can hold.
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