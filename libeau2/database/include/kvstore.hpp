/**
 * @file kvstore.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

#include "commondefs.hpp"
#include "key.hpp"

// Forward declarations
class Get;
class Reply;
class WaitAndGet;
class KVNet;
class Message;

// Alias for map of DataFrames
using DFMap = std::unordered_map<Key, DFPtr>;

// A mix of local and remote DataFrames keyed by name and node location.
class KVStore {
   private:
    std::shared_mutex _storeMutex;  //  provides multiple read, single write
                                    //  access to the key-value store
    DFMap _store;                   // the store itself
    KVNet& _kvNet;  // network interface for communicating with other KVStores
    std::thread _listener;  // listener thread that handles network messages
    size_t _idx;            // node index
    std::condition_variable_any
        _cv;  // conditional variable, used for waiting for new data to be
              // submitted to the store
    std::unordered_map<uint64_t, std::shared_ptr<Message>>
        _pending;  // pending transactions that need responses
    std::mutex
        _pendingMutex;  // ensures single read/write access to pending messages

    // Listening logic for handling network communications
    void _listen();

    // Processes a reply and adds/redirects data as needed
    void _postReply(std::shared_ptr<Reply> reply);

    // Sends a reply to a Get message with the data requested
    void _sendGetReply(std::shared_ptr<Get> msg);

    // Waits for the data requested to become available and then sends it back
    // if added before the timeout finishes
    void _startWaitAndGetReply(std::shared_ptr<WaitAndGet> msg);

   public:
    // Constructs a KVStore using the communication layer provided
    KVStore(KVNet& kvNet);

    // KVStores cannot be copied
    KVStore(const KVStore& other) = delete;
    void operator=(const KVStore& other) = delete;

    // Shuts down listener and destroys the KVStore
    ~KVStore();

    // Adds a DataFrame to the store at the key provided, assuming it does not
    // already exist
    void insert(const Key& key, DFPtr value);

    // Waits for the DataFrame at the given key to become available locally,
    // otherwise nullptr
    DFPtr waitAndGet(const Key& key);

    // Fetches a remote DataFrame by posting either a Get or WaitAndGet message
    // to the network
    void fetch(const Key& key, bool wait);

    // Pushes a DataFrame to a remote KVStore
    void push(const Key& key, DFPtr value);
};