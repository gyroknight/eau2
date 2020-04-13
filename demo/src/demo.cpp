#include "demo.hpp"

#include <iostream>

#include "dataframe.hpp"
#include "key.hpp"

namespace {
Key main("main", 0);
Key verify("verif", 0);
Key check("ck", 0);
}  // namespace

Demo::Demo(size_t idx, KVStore& kv) : Application(idx, kv) {}

void Demo::run_() {
    switch (this_node()) {
        case 0:
            producer();
            break;
        case 1:
            counter();
            break;
        case 2:
            summarizer();
    }
}

void Demo::producer() {
    size_t SZ = 100 * 1000;
    double* vals = new double[SZ];
    double sum = 0;
    for (size_t i = 0; i < SZ; ++i) sum += (vals[i] = i);
    std::cout << "producer() array complete." << std::endl;
    DataFrame::fromArray(&main, &kv, SZ, vals);
    DataFrame::fromScalar(&check, &kv, sum);
    std::cout << "producer() complete." << std::endl;
}

void Demo::counter() {
    std::shared_ptr<DataFrame> v = kv.waitAndGet(main);
    if (!v) {
        std::cerr << "counter() failed\n";
        return;
    }
    double sum = 0;
    for (size_t i = 0; i < 100 * 1000; ++i) sum += v->getDouble(0, i);
    std::cout << "The sum is  " << sum << std::endl;
    DataFrame::fromScalar(&verify, &kv, sum);
    std::cout << "counter() complete." << std::endl;
}

void Demo::summarizer() {
    std::shared_ptr<DataFrame> result = kv.waitAndGet(verify);
    std::shared_ptr<DataFrame> expected = kv.waitAndGet(check);
    if (!result || !expected) {
        std::cerr << "summarizer() failed\n";
        return;
    }
    std::cout << (expected->getDouble(0, 0) == result->getDouble(0, 0)
                      ? "SUCCESS"
                      : "FAILURE")
              << std::endl;
    std::cout << "summarizer() complete." << std::endl;
}