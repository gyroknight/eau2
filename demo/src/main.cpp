#include <memory>
#include <thread>
#include <vector>

#include "dataframe.hpp"
#include "demo.hpp"
#include "kvnetTcp.hpp"
#include "kvstore.hpp"
#include "put.hpp"

int main(int argc, char* argv[]) {
    // KVNetTCP net;

    // std::vector<std::shared_ptr<KVStore>> stores;
    // std::vector<Demo> apps;
    // std::vector<std::thread> runThreads;

    // for (int ii = 0; ii < 3; ii++)
    //     stores.push_back(std::make_shared<KVStore>(net));

    // for (int ii = 0; ii < 3; ii++) {
    //     apps.emplace_back(ii, *stores[ii]);
    //     runThreads.emplace_back(std::thread(&Demo::run_, apps[ii]));
    // }

    // for (std::thread& runThread : runThreads) {
    //     runThread.join();
    // }

    // Basic serialization/deserialization test
    auto df = std::make_shared<DataFrame>();

    auto s1 = std::make_shared<std::string>("hello");
    auto s2 = std::make_shared<std::string>("world");

    auto col1 = std::make_shared<Column<ExtString>>();

    col1->push_back(s1);
    col1->push_back(s2);

    ColPtr<int32_t> col2(new Column<int32_t>{0, 1});

    df->addCol(col1);
    df->addCol(col2);

    Key key("test", 0);

    Put origMsg(0, key, df);

    auto translated = Message::deserialize(origMsg.serialize());

    auto translatedPut =
        std::unique_ptr<Put>(dynamic_cast<Put*>(translated.release()));

    auto translatedDF = translatedPut->value();

    std::cout << *translatedDF->getString(0, 0);
    std::cout << *translatedDF->getString(0, 1);
    std::cout << translatedDF->getInt(1, 0);
    std::cout << translatedDF->getInt(1, 1);

    return 0;
}