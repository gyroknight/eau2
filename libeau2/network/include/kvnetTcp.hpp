/**
 * @file kvnetTcp.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * Lang::Cpp
 */

#pragma once

#include <atomic>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

#include "kvnet.hpp"

class KVNetTCP : public KVNet {
   private:
    std::queue<std::shared_ptr<Message>>
        _sending;  // queue for messages to send, monitored by sender thread
    std::queue<std::unique_ptr<Message>>
        _receiving;        // queue for messages received, populated by receiver
                           // thread and processed by listener in KVStore
    std::mutex _sendLock;  // sending queue lock
    std::mutex _receiveLock;          // receiving queue lock
    std::atomic_bool _netUp = false;  // is the network currently connected?
    std::thread _sender;              // sends pending messages
    std::thread _receiver;            // receives pending messages
    std::unordered_map<uint64_t, uint16_t>
        _sockfds;  // map of connections from node index to socket
    std::shared_mutex _sockfdsLock;  // connection map lock
    uint64_t _idx =
        0;  // the index of this KVStore, populated by registerNode()
    std::vector<struct sockaddr_in>
        _dir;  // directory indexed by node to address and port

    // Read a specific serial format
    ssize_t _readPayload(uint64_t idx, std::vector<uint8_t>& msg);
    ssize_t _readPut(uint64_t idx, std::vector<uint8_t>& msg);
    ssize_t _readReply(uint64_t idx, std::vector<uint8_t>& msg);
    ssize_t _readGet(uint64_t idx, std::vector<uint8_t>& msg);
    ssize_t _readWaitAndGet(uint64_t idx, std::vector<uint8_t>& msg);
    ssize_t _readDirectory(uint64_t idx, std::vector<uint8_t>& msg);

    // Reads an entire Message from the node specified
    std::unique_ptr<Message> _readMsg(uint64_t idx);

   public:
    KVNetTCP();
    ~KVNetTCP();

    // Registers this node with the registrar using the port provided
    size_t registerNode(const char* port) override;

    // Sends a message to the target encoded
    void send(std::shared_ptr<Message> msg) override;

    // Pops a received message if possible for processing
    std::unique_ptr<Message> receive() override;
};