#include "kvstore.hpp"

#include <iostream>

#include "get.hpp"
#include "key.hpp"
#include "kill.hpp"
#include "put.hpp"
#include "reply.hpp"

namespace {
constexpr size_t WAIT_GET_TIMEOUT_S = 10;
}  // namespace

KVStore::KVStore(KVNet& kvNet) : __kvNet(kvNet) {
    __listener = std::thread(&KVStore::__listen, this);
}

KVStore::~KVStore() {
    auto msg = std::make_shared<Kill>(__idx, __idx);
    __kvNet.send(msg);
    __listener.join();
}

void KVStore::insert(const Key& key, std::shared_ptr<DataFrame> value) {
    if (__validKeys.count(key)) {
        std::cout << "Key " << key.name() << " already exists on node " << __idx
                  << "." << std::endl;
        return;
    }

    __store[key] = value;

    const std::lock_guard<std::mutex> lockKeys(__keyMutex);
    __validKeys.insert(key);
}

std::shared_ptr<DataFrame> KVStore::waitAndGet(const Key& key) {
    std::unique_lock<std::mutex> lockKeys(__keyMutex);

    if (__validKeys.count(key)) {
        return __store[key];
    }

    fetch(key);

    if (__cv.wait_for(lockKeys, std::chrono::seconds(WAIT_GET_TIMEOUT_S),
                      [this, &key] { return __validKeys.count(key) == 1; })) {
        std::shared_ptr<DataFrame> ret = __store[key];
        lockKeys.release();
        return ret;
    } else {
        std::cout << "Request timed out, unable to find dataframe."
                  << std::endl;
        return std::shared_ptr<DataFrame>();
    }
}
void KVStore::fetch(const Key& key) {
    __kvNet.send(std::make_shared<Get>(__idx, key.home(), key));
}

void KVStore::push(const Key& key, std::shared_ptr<DataFrame> value) {
    __kvNet.send(std::make_shared<Put>(__idx, key, value));
}

void KVStore::__listen() {
    __idx = __kvNet.registerNode();
    bool listening = true;
    while (listening) {
        std::shared_ptr<Message> msg = __kvNet.receive(__idx);

        if (msg) {
            switch (msg->kind()) {
                case MsgKind::Put:
                    break;
                case MsgKind::Reply:
                    break;
                case MsgKind::Get:
                    break;
                case MsgKind::WaitAndGet:
                    break;
                case MsgKind::Kill:
                    listening = false;
                    break;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}