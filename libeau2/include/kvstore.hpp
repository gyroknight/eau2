#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include "key.hpp"
#include "kvnet.hpp"

class DataFrame;

using DFMap = std::unordered_map<Key, std::shared_ptr<DataFrame>>;

// A mix of local and remote DataFrames keyed by name and node location.
class KVStore {
   private:
    std::mutex __keyMutex;
    std::unordered_set<Key> __validKeys;
    DFMap __store;
    KVNet& __kvNet;
    std::thread __listener;
    size_t __idx;
    std::condition_variable __cv;

    void __listen();

   public:
    KVStore(KVNet& kvNet);
    KVStore(const KVStore& other) = delete;
    void operator=(const KVStore& other) = delete;
    ~KVStore();

    void insert(const Key& key, std::shared_ptr<DataFrame> value);
    std::shared_ptr<DataFrame> waitAndGet(const Key& key);
    void fetch(const Key& key);
    void push(const Key& key, std::shared_ptr<DataFrame> value);
};