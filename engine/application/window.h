#pragma once
#include <string_view>

struct GLFWwindow;

struct  Window
{
    GLFWwindow* window = nullptr;

    bool create(std::string_view p_title, int w, int h);
    void destroy();

    bool should_close();
    static void poll_events();
    void swap_buffers();
};
