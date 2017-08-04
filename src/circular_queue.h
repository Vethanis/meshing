#pragma once

#include <atomic>
#include <cassert>
#include "ints.h"

template<typename T, u16 capacity>
class CircularQueue{
    T* data;
    std::atomic<u16> head, tail;
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
    inline u16 count(){
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
        for(u16 i = tail; i != head; i = (i + 1) & (capacity - 1)){
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
