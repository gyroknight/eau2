#pragma once

#include <string>

// Key for a DataFrame on a KV store, ties the DF to a name and a node
class Key {
   private:
    std::string __name;
    size_t __home;

   public:
    Key(std::string& name, size_t home);
    Key(const char* name, size_t home);
    std::string name() const;
    size_t home() const;
    bool operator==(const Key& other) const;
};

// Template to make Key hashable and usable in maps and sets
template <>
struct std::hash<Key> {
    size_t operator()(const Key& key) const;
};