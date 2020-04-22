#include <memory>
#include <thread>
#include <vector>

#include "OptParser.hh"
#include "dataframe.hpp"
#include "demo.hpp"
#include "kvnetTcp.hpp"
#include "kvstore.hpp"
#include "put.hpp"

int main(int argc, char* argv[]) {
    OptParser optParser;
    Opt opt;
    std::string address;
    std::string port;
    std::string idx;

    optParser.addFlag("address", 1);
    optParser.addFlag("port", 1);
    optParser.addFlag("idx", 1);

    while (!(opt = optParser.getOpt(argc, argv)).first.empty()) {
        const std::string& flag = opt.first;
        const std::vector<std::string>& optVals = opt.second;

        if (flag == "address") {
            address = optVals.front();
        } else if (flag == "port") {
            port = optVals.front();
        } else if (flag == "idx") {
            idx = optVals.front();
        }
    }

    if (address.empty() || port.empty() || idx.empty()) {
        throw std::runtime_error("Failed to get app parameters");
    }

    KVNetTCP net;
    KVStore store(net, address.c_str(), port.c_str());
    Demo app(std::stoul(idx), store);

    app.run_();

    while (net.ready()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    // Basic serialization/deserialization test
    // auto df = std::make_shared<DataFrame>();

    // auto s1 = std::make_shared<std::string>("hello");
    // auto s2 = std::make_shared<std::string>("world");

    // auto col1 = std::make_shared<Column<ExtString>>();

    // col1->push_back(s1);
    // col1->push_back(s2);

    // ColPtr<int32_t> col2(new Column<int32_t>{0, 1});

    // df->addCol(col1);
    // df->addCol(col2);

    // Key key("test", 0);

    // Put origMsg(0, key, df);

    // auto translated = Message::deserialize(origMsg.serialize());

    // auto translatedPut =
    //     std::unique_ptr<Put>(dynamic_cast<Put*>(translated.release()));

    // auto translatedDF = translatedPut->value();

    // std::cout << *translatedDF->getString(0, 0);
    // std::cout << *translatedDF->getString(0, 1);
    // std::cout << translatedDF->getInt(1, 0);
    // std::cout << translatedDF->getInt(1, 1);

    return 0;
}