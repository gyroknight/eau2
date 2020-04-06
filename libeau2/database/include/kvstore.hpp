#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include "key.hpp"
#include "kvnet.hpp"

class DataFrame;
class Get;
class Reply;
class WaitAndGet;

using DFMap = std::unordered_map<Key, std::shared_ptr<DataFrame>>;

// A mix of local and remote DataFrames keyed by name and node location.
class KVStore {
   private:
    std::shared_mutex _storeMutex;
    DFMap _store;
    KVNet& _kvNet;
    std::thread _listener;
    size_t _idx;
    std::condition_variable_any _cv;
    std::unordered_map<uint64_t, std::shared_ptr<Message>> _pending;
    std::mutex _pendingMutex;

    void _listen();
    void _postReply(std::shared_ptr<Reply> reply);
    void _sendGetReply(std::shared_ptr<Get> msg);
    void _startWaitAndGetReply(std::shared_ptr<WaitAndGet> msg);

   public:
    KVStore(KVNet& kvNet);
    KVStore(const KVStore& other) = delete;
    void operator=(const KVStore& other) = delete;
    ~KVStore();

    void insert(const Key& key, std::shared_ptr<DataFrame> value);
    std::shared_ptr<DataFrame> waitAndGet(const Key& key);
    void fetch(const Key& key, bool wait);
    void push(const Key& key, std::shared_ptr<DataFrame> value);
};