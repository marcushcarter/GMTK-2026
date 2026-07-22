#pragma once
#include <string_view>

struct GLFWwindow;

struct  Window
{
    GLFWwindow* window = nullptr;
    int width = 1, height = 1;

    bool create(std::string_view p_title, int w, int h);
    void destroy();

    bool should_close();
    void poll_events();
    void swap_buffers();

    void set_title(std::string_view p_title);
};
