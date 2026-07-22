#pragma once
#include <chrono>

struct Scene
{
    std::chrono::time_point<std::chrono::high_resolution_clock> last = std::chrono::high_resolution_clock::now();
    float dt;

    bool initialize();
    void shutdown();

    void update();
};
