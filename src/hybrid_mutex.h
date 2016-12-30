#pragma once

#include <mutex>
#include <chrono>
#include <atomic>

//#define HYBRID_MUTEX_CAP

class hybrid_mutex{
    std::mutex mut;
    std::atomic<long long> p = 1;
public:
    void lock(){
        using clock_t = std::chrono::high_resolution_clock;

        auto begin = clock_t::now();
        long long measured;

        while(!mut.try_lock()){
            auto now = clock_t::now();
            measured = (now - begin).count();

            if(measured >= p * 2){
                mut.lock();

                #ifdef HYBRID_MUTEX_CAP
                    long long sleep_time = (clock_t::now() - now).count();

                    if(sleep_time < p){
                        p = sleep_time;
                        return;
                    }
                #endif

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
