#pragma once
#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

struct Input
{
    float mouse_x = 0.0f, mouse_y = 0.0f;
    float mouse_dx = 0.0f, mouse_dy = 0.0f;
    float scroll = 0.0f;

    bool mouse_down[GLFW_MOUSE_BUTTON_LAST + 1] = {};
    bool mouse_pressed [GLFW_MOUSE_BUTTON_LAST + 1] = {};
    bool mouse_released[GLFW_MOUSE_BUTTON_LAST + 1] = {};

    bool keys[GLFW_KEY_LAST + 1] = {};
    bool keys_pressed[GLFW_KEY_LAST + 1] = {};

    bool first_poll = true;

    void poll(GLFWwindow* window);
};

void input_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
