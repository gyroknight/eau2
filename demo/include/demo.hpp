#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "application.hpp"
#include "kvnet.hpp"

class Demo : public Application {
   public:
    Demo(size_t idx, KVStore& kv);

    void run_() override;

    void producer();

    void counter();

    void summarizer();
};

class DemoNet : public KVNet {
   public:
    std::vector<std::queue<std::shared_ptr<Message>>> nodeMsgs;
    std::mutex netMutexes[3];

    DemoNet();
    size_t registerNode() override;
    void send(std::shared_ptr<Message> msg) override;
    std::shared_ptr<Message> receive(size_t idx) override;
};