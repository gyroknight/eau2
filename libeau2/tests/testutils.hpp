/**
 * @file testutils.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <gtest/gtest.h>

#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include "commondefs.hpp"
#include "dataframe.hpp"
#include "key.hpp"
#include "kvnet.hpp"
#include "kvstore.hpp"
#include "message.hpp"

static const char col_types[] = {'I', 'B', 'D', 'S'};

#define EXPECT_SCHEMA_EQ(a, b) EXPECT_PRED2(isSchemaEq, a, b);
#define ASSERT_SCHEMA_EQ(a, b) ASSERT_PRED2(isSchemaEq, a, b);

[[maybe_unused]] static bool isSchemaEq(const Schema& a, const Schema& b) {
    if (a.width() != b.width()) return false;
    if (a.length() != b.length()) return false;

    for (size_t i = 0; i < a.width(); i++) {
        if (a.colName(i) != b.colName(i)) return false;
        if (a.colType(i) != b.colType(i)) return false;
    }

    for (size_t i = 0; i < a.length(); i++) {
        if (a.rowName(i) != b.rowName(i)) return false;
    }

    return true;
}

// same as above but for pointers
[[maybe_unused]] static bool isSchemaEq(const Schema* a, const Schema* b) {
    if (a == b) return true;
    if (a != nullptr) return isSchemaEq(*a, *b);

    return true;
}

/**
 * @brief Provides simple 3x4 dataframe.
 *
 * column 1 <bool>:     true    false   true
 * column 2 <int>:      10      -5      42
 * column 3 <string>:   abc     def     ghi
 * column 4 <double:    5.0     0.0     -10000.0
 *
 */
class FixtureWithSmallDataFrame : public ::testing::Test {
   protected:
    DFPtr df;
    std::shared_ptr<Column<bool>> c1;
    std::shared_ptr<Column<int>> c2;
    std::shared_ptr<Column<ExtString>> c3;
    std::shared_ptr<Column<double>> c4;
    ExtString exts1;
    ExtString exts2;
    ExtString exts3;
    ExtString exts4;

    FixtureWithSmallDataFrame()
        : df(std::make_shared<DataFrame>()),
          c1(std::make_shared<Column<bool>>()),
          c2(std::make_shared<Column<int>>()),
          c3(std::make_shared<Column<ExtString>>()),
          c4(std::make_shared<Column<double>>()),
          exts1(std::make_shared<std::string>("abc")),
          exts2(std::make_shared<std::string>("def")),
          exts3(std::make_shared<std::string>("ghi")),
          exts4(std::make_shared<std::string>("ghi")) {
        c1->push_back(true);
        c1->push_back(false);
        c1->push_back(true);
        c2->push_back(10);
        c2->push_back(-5);
        c2->push_back(42);
        c3->push_back(exts1);
        c3->push_back(exts2);
        c3->push_back(exts3);
        c4->push_back(5.0);
        c4->push_back(0.0);
        c4->push_back(-10000.0);

        df->addCol(c1, std::make_shared<std::string>("boolcol"));
        df->addCol(c2, std::make_shared<std::string>("intcol"));
        df->addCol(c3, std::make_shared<std::string>("ExtStringcol"));
        df->addCol(c4, std::make_shared<std::string>("doublecol"));
    }
};

// mock KVNet to give to the KVStore
class KVNetMock : public KVNet {
   public:
    std::queue<std::shared_ptr<Message>> nodeMsgs;

   public:
    KVNetMock() : KVNet() {}

    virtual size_t registerNode(const char* address,
                                const char* port) override {
        return 0;
    }

    virtual void send(std::shared_ptr<Message> msg) override {
        nodeMsgs.push(msg);
    }

    virtual std::unique_ptr<Message> receive() override {
        if (!nodeMsgs.empty()) {
            auto msg = std::move(nodeMsgs.front());
            nodeMsgs.pop();
            return Message::deserialize(msg->serialize());
        }

        return nullptr;
    }

    virtual bool ready() override { return true; }

    virtual void shutdown() override {}
};

class FixtureWithKVStore : public FixtureWithSmallDataFrame {
   protected:
    std::unique_ptr<KVNetMock> net;
    std::unique_ptr<KVStore> store;
    std::unique_ptr<Key> key;

    FixtureWithKVStore()
        : net(std::make_unique<KVNetMock>()),
          store(std::make_unique<KVStore>(*net, "address", "port")),
          key(std::make_unique<Key>("dataframe", 0)) {
        store->insert(*key, df);
    }
};