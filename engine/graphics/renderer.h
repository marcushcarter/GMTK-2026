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

    uint32_t out_color = 0;
    uint32_t out_framebuffer = 0;

    uint32_t blit_program = 0;
    // uint32_t tile_program = 0;

    uint32_t temp_vao = 0;

    bool initialize(Window* p_window);
    void shutdown();

    void request_resize(uint32_t w, uint32_t h);
    void set_size();

    void render(Scene* scene);
    
    uint32_t _compile_shader(const char* src, GLenum stage);
    uint32_t _create_program(uint32_t* shaders, uint32_t count);
};
