#include "renderer.h"
#include <application/window.h>
#include <scene/scene.h>

bool Renderer::initialize(Window* p_window)
{
    window = p_window;
    return true;
}

void Renderer::shutdown()
{

}

void Renderer::request_resize(uint32_t w, uint32_t h)
{
    if (w == 0 || h == 0) return;
    if (w == width && h == height) return;

    width = w;
    height = h;
    resize_requested = true;
}

void Renderer::set_size()
{
    resize_requested = false;
}

void Renderer::render(Scene* scene)
{
    (void)scene;
    
    if (resize_requested) {
        set_size();
    }

    // render

}
