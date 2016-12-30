#pragma once

#include <atomic>
#include <cassert>

template<typename T, size_t capacity>
class CircularQueue{
    T* data;
    std::atomic<size_t> head, tail;
public:
    CircularQueue() : head(0), tail(0){
        assert( ( (capacity-1) & capacity ) == 0);
        data = new T[capacity];
    }
    ~CircularQueue(){
        delete[] data;
    }
    bool empty(){
        return head == tail;
    }
    bool full(){
        return ((head + 1) & (capacity - 1)) == tail;
    }
    size_t count(){
        if(head < tail){
            return (capacity - tail) + head;
        }
        return head - tail;
    }
    void push(const T& item){
        data[head] = item;
        head = ((head + 1) & (capacity - 1));

    }
    T pop(){
        T item = data[tail];
        tail = ((tail + 1) & (capacity - 1));
        return item;
    }
    void clear(){
        head.store(tail.load());
    }
};
