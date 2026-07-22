#pragma once
#include <application/window.h>
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
    Renderer renderer;

    Scene main_scene;
    Scene* active_scene = nullptr;

    bool create(const ApplicationCI& p_ci);
    void destroy();
    void run();
};