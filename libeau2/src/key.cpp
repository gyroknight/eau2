#include "key.hpp"

Key::Key(std::string& name, size_t home) : __name(name), __home(home) {}
Key::Key(const char* name, size_t home) : __name(name), __home(home) {}

std::string Key::name() const { return __name; }

size_t Key::home() const { return __home; }

bool Key::operator==(const Key& other) const {
    return __name == other.__name && __home == other.__home;
}

size_t std::hash<Key>::operator()(const Key& key) const {
    return std::hash<std::string>{}(key.name()) +
           std::hash<size_t>{}(key.home());
}