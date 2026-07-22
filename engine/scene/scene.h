#pragma once
#include "camera.h"
#include "world.h"
#include <chrono>

struct Input;

struct Scene
{
    std::chrono::time_point<std::chrono::high_resolution_clock> last = std::chrono::high_resolution_clock::now();
    float dt = 0.0f;

    Camera camera;
    World world;

    bool initialize();
    void shutdown();
    void update(const Input& in, uint32_t viewport_w, uint32_t viewport_h);
};