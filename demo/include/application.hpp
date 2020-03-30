#pragma once

#include <cstddef>

#include "kvstore.hpp"

class Application {
   public:
    size_t idx;
    KVStore& kv;

    Application(size_t idx, KVStore& kv);
    virtual ~Application();
    virtual void run_() = 0;

    virtual size_t this_node() final;
};