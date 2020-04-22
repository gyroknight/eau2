#include "application.hpp"

Application::Application(size_t idx, KVStore& kv) : idx(idx), kv(kv) {}
Application::~Application() {}

size_t Application::this_node() { return idx; }