#include <memory>
#include <thread>
#include <vector>

#include "demo.hpp"
#include "kvstore.hpp"
#include "kvnetTcp.hpp"

int main(int argc, char* argv[]) {
    KVNetTCP net;

    std::vector<std::shared_ptr<KVStore>> stores;
    std::vector<Demo> apps;
    std::vector<std::thread> runThreads;

    for (int ii = 0; ii < 3; ii++)
        stores.push_back(std::make_shared<KVStore>(net));

    for (int ii = 0; ii < 3; ii++) {
        apps.emplace_back(ii, *stores[ii]);
        runThreads.emplace_back(std::thread(&Demo::run_, apps[ii]));
    }

    for (std::thread& runThread : runThreads) {
        runThread.join();
    }

    return 0;
}