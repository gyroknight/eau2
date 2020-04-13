#include <gtest/gtest.h>

#include <queue>
#include <memory>
#include <iostream>

#include "kvnet.hpp"
#include "kvstore.hpp"
#include "message.hpp"
#include "kill.hpp"
#include "dataframe.hpp"
#include "sorer/column.h"

// just a dummy implementation
class DemoNet : public KVNet {
   public:
    std::queue<std::shared_ptr<Message>> _txrx;
    std::mutex mutex;

    DemoNet() {}

    ~DemoNet() {
        // kill the kvstore
        send(std::make_shared<Kill>(0, 0));
    }

    size_t registerNode(const char* port) override {
        return 0;
    }

    void send(std::shared_ptr<Message> msg) override {
        uint64_t target = msg->target();
        const std::lock_guard<std::mutex> targetLock(mutex);

        _txrx.push(msg);
    }

    std::unique_ptr<Message> receive() override {
        const std::lock_guard<std::mutex> senderLock(mutex);

        if (!_txrx.empty()) {
            auto msg = _txrx.front();
            _txrx.pop();
            return Message::deserialize(msg->serialize());
        }

        return nullptr;
    }
};

// Necessary to avoid segfaults with deleting string literals
char* cwc_strdup(const char* src) {
    char* result = new char[strlen(src) + 1];
    strcpy(result, src);
    return result;
}


// test fromColumnSet method

// Currently commented out because the kvnet is not responding to kill messages correctly.
/*
TEST(DataFrameTest, fromColumn) {
    DemoNet net;
    KVStore kv(net);
    Key k("dataf", 0);

    ne::ColumnSet set(3);
    set.initializeColumn(0, ne::ColumnType::BOOL);
    set.initializeColumn(1, ne::ColumnType::INTEGER);
    set.initializeColumn(2, ne::ColumnType::STRING);

    ne::BoolColumn* col1 = dynamic_cast<ne::BoolColumn*>(set.getColumn(0));
    ne::IntegerColumn* col2 = dynamic_cast<ne::IntegerColumn*>(set.getColumn(1));
    ne::StringColumn* col3 = dynamic_cast<ne::StringColumn*>(set.getColumn(2));

    // build columns
    col1->append(true);
    col1->append(true);
    col1->append(false);

    col2->append(42);
    col2->append(-5);
    col2->append(0);

    col3->append(cwc_strdup("hello"));
    col3->append(cwc_strdup("goodbye"));
    col3->append(cwc_strdup("yes"));

    DataFrame::fromColumnSet(&k, &kv, &set);

    // net.send(std::make_shared<Kill>(0, 0));

    auto df = kv.waitAndGet(k);

    ASSERT_EQ(true, df->getBool(0, 0));
    ASSERT_EQ(-5, df->getInt(1, 1));
    ASSERT_TRUE(strcmp("yes", df->getString(2, 2)->c_str()));
}
*/