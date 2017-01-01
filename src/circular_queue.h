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
    inline bool empty(){
        return head == tail;
    }
    inline bool full(){
        return ((head + 1) & (capacity - 1)) == tail;
    }
    inline size_t count(){
        if(head < tail){
            return (capacity - tail) + head;
        }
        return head - tail;
    }
    inline void push(const T& item){
        data[head] = item;
        head = ((head + 1) & (capacity - 1));
    }
    inline void set_push(const T& item){
        for(size_t i = tail; i != head; i = (i + 1) & (capacity - 1)){
            if(data[i] == item){
                return;
            }
        }
        push(item);
    }
    inline T pop(){
        T item = data[tail];
        tail = ((tail + 1) & (capacity - 1));
        return item;
    }
    inline void clear(){
        head.store(tail.load());
    }
};
