#pragma once

#include <gtest/gtest.h>

#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include "kvnet.hpp"
#include "kvstore.hpp"
#include "dataframe.hpp"

namespace {

// mock KVNet to give to the KVStore
class MockNet : public KVNet {
   public:
    std::vector<std::queue<std::shared_ptr<Message>>> nodeMsgs;
    std::mutex netMutexes[3];

   public:
    MockNet() : KVNet() {}

    virtual size_t registerNode() {
        nodeMsgs.emplace_back();
        return nodeMsgs.size() - 1;
    }

    virtual void send(std::shared_ptr<Message> msg) {
        uint64_t target = msg->target();
        const std::lock_guard<std::mutex> targetLock(netMutexes[target]);

        nodeMsgs[target].push(msg);
    }

    virtual std::shared_ptr<Message> receive(size_t idx) {
        const std::lock_guard<std::mutex> senderLock(netMutexes[idx]);

        if (!nodeMsgs[idx].empty()) {
            std::shared_ptr<Message> msg = nodeMsgs[idx].front();
            nodeMsgs[idx].pop();
            return msg;
        }

        return nullptr;
    }
};

class KVStoreTest : public testing::Test {
   protected:

    MockNet net;
    KVStore store;

    KVStoreTest() : net(), store(net) {}
};

TEST_F(KVStoreTest, basic_io) {
    Key key("", 0);

    DataFrame::fromScalar<int>(&key, &store, 42);

    ASSERT_EQ(42, store.waitAndGet(key)->getInt(0, 0));
}

}  // namespace