#pragma once

#include "application.hpp"

class Demo : public Application {
   public:
    Demo(size_t idx);

    void run_() override;

    void producer();

    void counter();

    void summarizer();
};