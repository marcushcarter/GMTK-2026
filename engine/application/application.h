#pragma once
#include "window.h"
#include <string>
#include <cstdint>

struct ApplicationCI {
    std::string title;
    uint32_t w, h;
};

struct Application
{
    int foo;

    Window window;

    bool create(const ApplicationCI& p_ci);
    void destroy();
    void run();
};