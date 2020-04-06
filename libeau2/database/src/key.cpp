#include "key.hpp"

Key::Key(std::string& name, size_t home) : _name(name), _home(home) {}
Key::Key(const char* name, size_t home) : _name(name), _home(home) {}

std::string Key::name() const { return _name; }

size_t Key::home() const { return _home; }

bool Key::operator==(const Key& other) const {
    return _name == other._name && _home == other._home;
}

size_t std::hash<Key>::operator()(const Key& key) const {
    return std::hash<std::string>{}(key.name()) +
           std::hash<size_t>{}(key.home());
}