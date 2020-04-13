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
    std::queue<std::shared_ptr<Message>> _sending;
    std::queue<std::unique_ptr<Message>> _receiving;
    std::mutex _sendLock;
    std::mutex _receiveLock;
    std::atomic_bool _netUp = false;
    std::thread _sender;
    std::thread _receiver;
    std::unordered_map<uint64_t, uint16_t> _sockfds;
    std::shared_mutex _sockfdsLock;
    uint64_t _idx = 0;
    std::vector<struct sockaddr_in> _dir;

    ssize_t _readPayload(uint64_t idx, std::vector<uint8_t>& msg);
    ssize_t _readPut(uint64_t idx, std::vector<uint8_t>& msg);
    ssize_t _readReply(uint64_t idx, std::vector<uint8_t>& msg);
    ssize_t _readGet(uint64_t idx, std::vector<uint8_t>& msg);
    ssize_t _readWaitAndGet(uint64_t idx, std::vector<uint8_t>& msg);
    ssize_t _readDirectory(uint64_t idx, std::vector<uint8_t>& msg);

   public:
    KVNetTCP();
    ~KVNetTCP();

    size_t registerNode(const char* port) override;
    void send(std::shared_ptr<Message> msg) override;
    std::unique_ptr<Message> receive() override;
    std::unique_ptr<Message> readMsg(uint64_t idx);
};