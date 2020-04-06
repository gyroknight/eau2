// lang::Cpp
#pragma once
#include <gtest/gtest.h>

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include "fielder.hpp"
#include "row.hpp"
#include "testutils.hpp"

namespace {

class RowTest : public testing::Test {
   public:
    RowTest() {}
    ~RowTest() {}

    template <typename... Args>
    std::shared_ptr<Schema> MakeSchema(Args... args) {
        auto schema = std::make_shared<Schema>(args...);
        schemas.push_back(schema);
        return schema;
    }

    template <typename... Args>
    std::shared_ptr<Schema> MakeSchema(int height, Args... args) {
        auto schema = std::make_shared<Schema>(args...);
        schemas.push_back(schema);

        for (int i = 0; i < height; i++) {
            schema->addRow(std::make_shared<std::string>(std::to_string(i)));
        }

        return schema;
    }

    std::vector<std::shared_ptr<Schema>> schemas;
};

TEST_F(RowTest, construct_empty) {
    Row r(*MakeSchema());

    ASSERT_EQ(0, r.width());
}

TEST_F(RowTest, construct_many) {
    Row r(*MakeSchema("IDBS"));

    ASSERT_EQ(4, r.width());

    ASSERT_EQ('I', r.col_type(0));
    ASSERT_EQ('D', r.col_type(1));
    ASSERT_EQ('B', r.col_type(2));
    ASSERT_EQ('S', r.col_type(3));
}

class RowTestExample : public RowTest {
   public:
    Row r;

    RowTestExample() : r(*MakeSchema("DBIDIDBIIIS")) {
        r.set(0, 1.0);
        r.set(1, true);
        r.set(2, 5);
        r.set(3, 5.0);
        r.set(4, -10);
        r.set(5, 0.5);
        r.set(6, false);
        r.set(7, 1);
        r.set(8, 2);
        r.set(9, 3);
        r.set(10, std::make_shared<std::string>("Hello"));
    }
};

TEST_F(RowTestExample, set_and_get) {
    ASSERT_EQ(1.0, r.getDouble(0));
    ASSERT_EQ(true, r.getBool(1));
    ASSERT_EQ(5, r.getInt(2));
    ASSERT_EQ(5.0, r.getDouble(3));
    ASSERT_EQ(-10, r.getInt(4));
    ASSERT_EQ(0.5, r.getDouble(5));
    ASSERT_EQ(false, r.getBool(6));
    ASSERT_EQ(1, r.getInt(7));
    ASSERT_EQ(2, r.getInt(8));
    ASSERT_EQ(3, r.getInt(9));
    ASSERT_STREQ("Hello", r.getString(10)->c_str());
}

TEST_F(RowTest, set_get_idx) {
    Row r(*MakeSchema(10, "I"));

    ASSERT_EQ(0, r.getIdx());
    r.setIdx(5);
    ASSERT_EQ(5, r.getIdx());
}

TEST_F(RowTestExample, col_type_and_width) {
    ASSERT_EQ('D', r.col_type(0));
    ASSERT_EQ('B', r.col_type(1));
    ASSERT_EQ('I', r.col_type(2));
    ASSERT_EQ('D', r.col_type(3));
    ASSERT_EQ('I', r.col_type(4));
    ASSERT_EQ('D', r.col_type(5));
    ASSERT_EQ('B', r.col_type(6));
    ASSERT_EQ('I', r.col_type(7));
    ASSERT_EQ('I', r.col_type(8));
    ASSERT_EQ('I', r.col_type(9));
    ASSERT_EQ('S', r.col_type(10));

    ASSERT_EQ(11, r.width());
}

// also tests Fielder
class RowTestExampleFielder : public Fielder {
   public:
    std::vector<std::string>& v_;
    RowTestExampleFielder(std::vector<std::string>& v) : v_(v) {}

    virtual void start(size_t r) override {
        v_.push_back("start:" + std::to_string(r));
    }

    virtual void accept(bool b) override {
        v_.push_back("bool:" + std::to_string(b));
    }
    virtual void accept(double f) override {
        v_.push_back("double:" + std::to_string(f));
    }
    virtual void accept(int i) override {
        v_.push_back("int:" + std::to_string(i));
    }
    virtual void accept(std::shared_ptr<std::string> s) override {
        v_.push_back(std::string("string:") + s->c_str());
    }

    virtual void done() override { v_.emplace_back("done"); }
};

TEST_F(RowTestExample, visit) {
    std::vector<std::string> fielderout;

    RowTestExampleFielder f(fielderout);

    r.visit(0, f);

    for (const auto& e : fielderout) {
        std::cout << e << std::endl;
    }

    ASSERT_EQ(13, fielderout.size());

    ASSERT_EQ("start:0", fielderout[0]);
    ASSERT_EQ("double:1.000000", fielderout[1]);
    ASSERT_EQ("bool:1", fielderout[2]);
    ASSERT_EQ("int:5", fielderout[3]);
    ASSERT_EQ("double:5.000000", fielderout[4]);
    ASSERT_EQ("int:-10", fielderout[5]);
    ASSERT_EQ("double:0.500000", fielderout[6]);
    ASSERT_EQ("bool:0", fielderout[7]);
    ASSERT_EQ("int:1", fielderout[8]);
    ASSERT_EQ("int:2", fielderout[9]);
    ASSERT_EQ("int:3", fielderout[10]);
    ASSERT_EQ("string:Hello", fielderout[11]);
    ASSERT_EQ("done", fielderout[12]);
}

}  // namespace
