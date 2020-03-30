#include "demo.hpp"

#include <iostream>

#include "dataframe.hpp"

namespace {
Key main("main", 0);
Key verify("verif", 0);
Key check("ck", 0);
}  // namespace

Demo::Demo(size_t idx) : Application(idx) {}

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
    DataFrame::fromArray(&main, &kv, SZ, vals);
    DataFrame::fromScalar(&check, &kv, sum);
}

void Demo::counter() {
    DataFrame* v = kv.waitAndGet(main);
    size_t sum = 0;
    for (size_t i = 0; i < 100 * 1000; ++i) sum += v->getDouble(0, i);
    std::cout << "The sum is  " << sum << std::endl;
    DataFrame::fromScalar(&verify, &kv, sum);
}

void Demo::summarizer() {
    DataFrame* result = kv.waitAndGet(verify);
    DataFrame* expected = kv.waitAndGet(check);
    std::cout << (expected->getDouble(0, 0) == result->getDouble(0, 0)
                      ? "SUCCESS"
                      : "FAILURE")
              << std::endl;
}