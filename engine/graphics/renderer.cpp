#include "renderer.h"
#include <application/window.h>
#include <scene/scene.h>
#include <scene/world.h>
#include <utils/utils.h>
#include <glm/gtc/type_ptr.hpp>

uint32_t Renderer::_compile_shader(const char* src, GLenum stage)
{
    GLuint shader = glCreateShader(stage);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        char* log = new char[log_length];
        glGetShaderInfoLog(shader, log_length, nullptr, log);
        std::fprintf(stderr, "Shader compilation failed:\n%s\n", log);
        delete[] log;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
};

uint32_t Renderer::_create_program(uint32_t* shaders, uint32_t count)
{
    GLuint program = glCreateProgram();
    for (uint32_t i = 0; i < count; i++) {
        glAttachShader(program, shaders[i]);
    }
    glLinkProgram(program);
    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint log_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        std::string log(log_length, '\0');
        glGetProgramInfoLog(program, log_length, nullptr, log.data());
        std::fprintf(stderr, "Program linking failed:\n%s\n", log.c_str());
        glDeleteProgram(program);
        return 0;
    }
    for (uint32_t i = 0; i < count; i++) {
        glDetachShader(program, shaders[i]);
        glDeleteShader(shaders[i]);
    }
    return program;
}

bool Renderer::initialize(Window* p_window)
{
    window = p_window;

    /*****************/
    /**** SHADERS ****/
    /*****************/

    glCreateVertexArrays(1, &temp_vao);

    {
    std::string v_s = load_file("assets/shaders/blit.vert");
    uint32_t v = _compile_shader(v_s.c_str(), GL_VERTEX_SHADER);
    std::string f_s = load_file("assets/shaders/blit.frag");
    uint32_t f = _compile_shader(f_s.c_str(), GL_FRAGMENT_SHADER);
    uint32_t shaders[] = { v, f };
    blit_program = _create_program(shaders, 2);
    }

    {
    std::string v_s = load_file("assets/shaders/sprite.vert");
    uint32_t v = _compile_shader(v_s.c_str(), GL_VERTEX_SHADER);
    std::string f_s = load_file("assets/shaders/sprite.frag");
    uint32_t f = _compile_shader(f_s.c_str(), GL_FRAGMENT_SHADER);
    uint32_t shaders[] = { v, f };
    sprite_program = _create_program(shaders, 2);
    }

    if (!batch.initialize()) return false;
    // if (!atlas.initialize_white()) return false;

    request_resize(p_window->width, p_window->height);
    set_size();

    return true;
}

void Renderer::shutdown()
{   
    // atlas.shutdown();
    batch.shutdown();

    glDeleteVertexArrays(1, &temp_vao);

    glDeleteFramebuffers(1, &out_framebuffer);
    glDeleteTextures(1, &out_color);

    glDeleteProgram(blit_program);
    glDeleteProgram(sprite_program);
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
    glDeleteTextures(1, &out_color);
    glCreateTextures(GL_TEXTURE_2D, 1, &out_color);
    glTextureStorage2D(out_color, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glDeleteFramebuffers(1, &out_framebuffer);
    glCreateFramebuffers(1, &out_framebuffer);
    glNamedFramebufferTexture(out_framebuffer, GL_COLOR_ATTACHMENT0, out_color, 0);
    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    glNamedFramebufferDrawBuffers(out_framebuffer, 1, draw_buffers);
    
    resize_requested = false;
}

void Renderer::render(Scene* scene)
{    
    if (resize_requested) set_size();

    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, out_framebuffer);
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec4 scale_offset = scene->camera.clip_transform();
    glUseProgram(sprite_program);
    glUniform4fv(0, 1, glm::value_ptr(scale_offset));
    // glBindTextureUnit(0, atlas.texture.id);

    batch.begin();
    glm::ivec4 r = scene->camera.visible_tile_rect(CULL_MARGIN);
    for (int y = r.y; y <= r.w; y++) {
        for (int x = r.x; x <= r.z; x++) {
            batch.push(glm::vec2((float)x, (float)y), glm::vec2((float)x + 1.0f, (float)y + 1.0f), glm::vec2(0.0f), glm::vec2(1.0f), rgba(255, 255, 255));
        }
    }
    batch.flush();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(temp_vao);
    glUseProgram(blit_program);
    glBindTextureUnit(0, out_color);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    batch.end_frame();
}
