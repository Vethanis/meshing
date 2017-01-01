#pragma once

#include <vector>
#include <functional>
#include "hybrid_mutex.h"

template<typename T>
class Resource{
    std::vector<T> data;
    std::vector<bool> valid;
    size_t in_use;
    inline bool should_reuse(){
        return in_use < valid.size();
    }
    inline size_t unused(){
        if(!should_reuse()){
            return valid.size();
        }
        for(size_t i = 0, j = valid.size() - 1; i <= j; i++, j--){
            if(!valid[i]){
                return i;
            }
            if(!valid[j]){
                return j;
            }
        }
        return valid.size();
    }
public:
    Resource(size_t res_size = 0) : in_use(0){
        data.reserve(res_size);
        valid.reserve(res_size);
    }
    size_t insert(const T& item){
        size_t position = unused();
        if(position == valid.size()){
            valid.push_back(true);
            in_use++;
            data.push_back(item);
            return position;
        }
        valid[position] = true;
        in_use++;
        data[position] = item;
        return position;
    }
    void remove(size_t id){
        valid[id] = false;
        in_use--;
    }
    T& operator[](size_t id){
        return data[id];
    }
    bool is_valid(size_t id){
        return id < valid.size() && valid[id];
    }
    void for_each(std::function<void(T&)> callback){
        for(size_t i = 0; i < data.size(); i++){
            if(valid[i]){
                callback(data[i]);
            }
        }
    }
    void clear(){
        data.clear();
        valid.clear();
        in_use = 0;
    }
    void reserve(size_t ct){
        data.reserve(ct);
        valid.reserve(ct);
    }
    size_t size(){
        return data.size();
    }

};

template<typename T>
class ThreadedResource{
    Resource<T> data;
    hyshare_mutex mut;
public:
    ThreadedResource(size_t res=0){
        data.reserve(res);
    }
    size_t insert(const T& item){
        unique_guard(mut);
        return data.insert(item);
    }
    void remove(size_t id){
        {
            shared_guard(mut);
            if(!data.is_valid(id)){return;}
        }
        unique_guard(mut);
        data.remove(id);
    }
    void reserve(size_t ct){
        unique_guard(mut);
        data.reserve(ct);
    }
    size_t size(){
        shared_guard(mut);
        return data.size();
    }
    void clear(){
        unique_guard(mut);
        data.clear();
    }
    bool is_valid(size_t id){
        shared_guard(mut);
        return data.is_valid(id);
    }
    void for_each(std::function<void(const T&)>& callback){
        shared_guard(mut);
        data.for_each(callback);
    }
    void for_each_mut(std::function<void(T&)>& callback){
        unique_guard(mut);
        data.for_each(callback);
    }
    void act(size_t id, std::function<void(const T&)> callback){
        shared_guard(mut);
        if(data.is_valid(id)){
            callback(data[id]);
        }
    }
    void act_mut(size_t id, std::function<void(T&)> callback){
        unique_guard(mut);
        if(data.ish_valid(id)){
            callback(data[id]);
        }
    }
};