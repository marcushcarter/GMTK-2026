#pragma once
#include <application/window.h>
#include <application/input.h>
#include <graphics/renderer.h>
#include <scene/scene.h>
#include <string>
#include <cstdint>

struct ApplicationCI {
    std::string title;
    uint32_t w, h;
};

struct Application
{
    Window window;
    Input input;

    Renderer renderer;

    Scene scene;

    bool create(const ApplicationCI& p_ci);
    void destroy();
    void run();

    void _draw_ui();
};