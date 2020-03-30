#pragma once

#include <memory>
#include <unordered_map>

using DFMap = std::unordered_map<Key, std::shared_ptr<DataFrame>>;

class DataFrame;
class Key;

class KVStore {
   private:
    std::unordered_map<Key, std::shared_ptr<DataFrame>> store;

   public:
    void insert(const Key& key, std::shared_ptr<DataFrame> value);
    std::shared_ptr<DataFrame> waitAndGet(const Key& key);
};