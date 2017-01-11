#pragma once

#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>

class hybrid_mutex{
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    std::atomic<long long> p{1};
public:
    void lock(){
        using clock_t = std::chrono::high_resolution_clock;

        auto begin = clock_t::now();
        long long measured = 0;

        while(flag.test_and_set(std::memory_order_acquire)){
            measured = (clock_t::now() - begin).count();

            if(measured >= p * 2){
                std::this_thread::yield();
            }
        }

        p += (measured - p) / 8;
    }
    bool try_lock(){
        return !flag.test_and_set(std::memory_order_acquire);
    }
    void unlock(){
        flag.clear(std::memory_order_release);
    }
};

