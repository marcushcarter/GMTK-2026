#pragma once
#include <glad/glad.h>
#include <cstdint>

struct Window;
struct Scene;

struct Renderer
{
    Window* window = nullptr;

    uint32_t width = 1, height = 1;
    bool resize_requested = false;

    bool initialize(Window* p_window);
    void shutdown();

    void request_resize(uint32_t w, uint32_t h);
    void set_size();

    void render(Scene* scene);
};
