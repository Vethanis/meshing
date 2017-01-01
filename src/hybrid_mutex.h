#pragma once

#include <chrono>
#include <atomic>
#include <thread>
#include <shared_mutex>

class hybrid_mutex{
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    std::atomic<long long> p = 1;
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

class hyshare_mutex{
    std::shared_mutex mut;
    std::atomic<long long> p = 1;
public:
    void unique_lock(){
        using clock_t = std::chrono::high_resolution_clock;

        auto begin = clock_t::now();
        long long measured = 0;

        while(mut.try_lock()){
            measured = (clock_t::now() - begin).count();

            if(measured >= p * 2){
                mut.lock();
                break;
            }
        }

        p += (measured - p) / 8;
    }
    void unique_unlock(){
        mut.unlock();
    }
    bool try_unique_lock(){
        return mut.try_lock();
    }
    void shared_lock(){
        using clock_t = std::chrono::high_resolution_clock;
        
        auto begin = clock_t::now();
        long long measured = 0;

        while(mut.try_lock_shared()){
            measured = (clock_t::now() - begin).count();

            if(measured >= p * 2){
                mut.lock_shared();
                break;
            }
        }

        p += (measured - p) / 8;
    }
    void shared_unlock(){
        mut.unlock_shared();
    }
    bool try_shared_lock(){
        return mut.try_lock_shared();
    }
};

class shared_guard{
    hyshare_mutex* lock;
public:
    shared_guard(hyshare_mutex& mut) : lock(&mut){
        lock->shared_lock();
    }
    ~shared_guard(){
        lock->shared_unlock();
    }
};

class unique_guard{
    hyshare_mutex* lock;
public:
    unique_guard(hyshare_mutex& mut) : lock(&mut){
        lock->unique_lock();
    }
    ~unique_guard(){
        lock->unique_unlock();
    }
};
