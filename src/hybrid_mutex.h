#pragma once

#include <mutex>
#include <chrono>
#include <atomic>

class hybrid_mutex{
    std::mutex mut;
    std::atomic<long long> p = 1;
public:
    void lock(){
        using clock_t = std::chrono::high_resolution_clock;

        auto begin = clock_t::now();
        long long measured;

        while(!mut.try_lock()){
            measured = (clock_t::now() - begin).count();

            if(measured >= p * 2){
                mut.lock();
                break;
            }
        }

        p += (measured - p) / 8;
    }
    bool try_lock(){
        return mut.try_lock();
    }
    void unlock(){
        mut.unlock();
    }
};

#undef HYBRID_MUTEX_CAP
