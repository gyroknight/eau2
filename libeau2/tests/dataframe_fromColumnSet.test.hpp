#include <gtest/gtest.h>

#include <queue>
#include <memory>
#include <iostream>

#include "kvnet.hpp"
#include "kvstore.hpp"
#include "dataframe.hpp"
#include "sorer/column.h"

// just a dummy implementation
class DemoNet : public KVNet {
   public:
    std::vector<std::queue<std::shared_ptr<Message>>> nodeMsgs;
    std::mutex netMutexes[3];

    DemoNet() {}

    size_t registerNode() override {
        nodeMsgs.emplace_back();
        return nodeMsgs.size() - 1;
    }
    void send(std::shared_ptr<Message> msg) override {
        uint64_t target = msg->target();
        const std::lock_guard<std::mutex> targetLock(netMutexes[target]);

        nodeMsgs[target].push(msg);
    }
    std::shared_ptr<Message> receive(size_t idx) override {
        const std::lock_guard<std::mutex> senderLock(netMutexes[idx]);

        if (!nodeMsgs[idx].empty()) {
            std::shared_ptr<Message> msg = nodeMsgs[idx].front();
            nodeMsgs[idx].pop();
            return msg;
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
TEST(DataFrameTest, fromColumn) {
    std::cout << "RUNNING" << std::endl;
    DemoNet net;
    KVStore kv(net);
    Key k("data", 0);

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

    auto df = kv.waitAndGet(k);
    std::cout << "hello" << std::endl;
    std::cout << df->getBool(0, 0) << std::endl;
}