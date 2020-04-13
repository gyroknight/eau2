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