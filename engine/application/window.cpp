#include "window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

bool Window::create(std::string_view p_title, int w, int h)
{
    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(w, h, p_title.data(), nullptr, nullptr);
    if (!window) {
        std::cout << "window not worky\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        destroy();
        return false;
    }

    return true;
}

void Window::destroy()
{
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

bool Window::should_close()
{
    return glfwWindowShouldClose(window);
}

void Window::poll_events()
{
    glfwPollEvents();
}

void Window::swap_buffers()
{
    glfwSwapBuffers(window);
}
