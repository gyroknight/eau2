/**
 * @file kvnetTcp.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <netinet/in.h>
#include <sys/types.h>

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "kvnet.hpp"

class Message;

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
    std::thread _senderThread;        // sends pending messages
    std::thread _receiverThread;      // receives pending messages
    std::unordered_map<uint64_t, uint16_t>
        _sendSocks;  // map of connections from node index to socket
    uint64_t _idx =
        0;  // the index of this KVStore, populated by registerNode()
    std::vector<struct sockaddr_in>
        _dir;  // directory indexed by node to address and port
    std::condition_variable
        _senderCv;  // Wakes up the sender when messages are available to send

    // Reads an entire Message from the socket specified
    std::unique_ptr<Message> _readMsg(int sock);

    // Read a specific serial format
    ssize_t _readPayload(int sock, std::vector<uint8_t>& msg);
    ssize_t _readPut(int sock, std::vector<uint8_t>& msg);
    ssize_t _readReply(int sock, std::vector<uint8_t>& msg);
    ssize_t _readGet(int sock, std::vector<uint8_t>& msg);
    ssize_t _readWaitAndGet(int sock, std::vector<uint8_t>& msg);
    ssize_t _readDirectory(int sock, std::vector<uint8_t>& msg);

    // Sender logic
    void _sender();
    // Receiver logic
    void _receiver(const char* port);

    bool _inDirectory(in_addr_t address);

   public:
    KVNetTCP();
    virtual ~KVNetTCP();

    // Registers this node with the registrar using the port provided
    size_t registerNode(const char* address, const char* port) override;

    // Sends a message to the target encoded
    void send(std::shared_ptr<Message> msg) override;

    // Pops a received message if possible for processing
    std::unique_ptr<Message> receive() override;

    bool ready() override;
};