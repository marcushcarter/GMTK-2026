#include "input.h"

static double g_scroll_accum = 0.0;

void input_scroll_callback(GLFWwindow*, double, double yoffset)
{
    g_scroll_accum += yoffset;
}

void Input::poll(GLFWwindow* window)
{
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    if (first_poll) {
        mouse_x = (float)mx;
        mouse_y = (float)my;
        first_poll = false;
    }

    mouse_dx = (float)mx - mouse_x;
    mouse_dy = (float)my - mouse_y;
    mouse_x = (float)mx;
    mouse_y = (float)my;

    scroll = (float)g_scroll_accum;
    g_scroll_accum = 0.0;

    for (int b = 0; b <= GLFW_MOUSE_BUTTON_LAST; b++) {
        bool now = glfwGetMouseButton(window, b) == GLFW_PRESS;
        mouse_pressed[b] = now && !mouse_down[b];
        mouse_released[b] = !now &&  mouse_down[b];
        mouse_down[b] = now;
    }

    for (int k = GLFW_KEY_SPACE; k <= GLFW_KEY_LAST; k++) {
        bool now = glfwGetKey(window, k) == GLFW_PRESS;
        keys_pressed[k] = now && !keys[k];
        keys[k] = now;
    }
}
