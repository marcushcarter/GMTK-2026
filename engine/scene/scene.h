#pragma once
#include <chrono>
#include <scene/world.h>

struct Scene
{
    std::chrono::time_point<std::chrono::high_resolution_clock> last = std::chrono::high_resolution_clock::now();
    float dt;

    World world;

    bool initialize();
    void shutdown();

    void update();
};
