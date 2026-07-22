#include "renderer.h"
#include <application/window.h>
#include <scene/scene.h>
#include <utils/utils.h>

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
    std::string v_s = load_file("assets/shaders/fullscreen.vert");
    uint32_t v = _compile_shader(v_s.c_str(), GL_VERTEX_SHADER);
    std::string f_s = load_file("assets/shaders/gamma_blit.frag");
    uint32_t f = _compile_shader(f_s.c_str(), GL_FRAGMENT_SHADER);
    uint32_t shaders[] = { v, f };
    blit_program = _create_program(shaders, 2);
    }

    request_resize(p_window->width, p_window->height);
    set_size();

    return true;
}

void Renderer::shutdown()
{
    glDeleteVertexArrays(1, &temp_vao);

    glDeleteFramebuffers(1, &out_framebuffer);
    glDeleteTextures(1, &out_color);

    glDeleteProgram(blit_program);
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
    (void)scene;
    
    if (resize_requested) {
        set_size();
    }

    glViewport(0,0,width,height);

    glBindFramebuffer(GL_FRAMEBUFFER, out_framebuffer);
    glClearColor(1,0,0,1);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1,0,1,1);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glBindVertexArray(temp_vao);
    glUseProgram(blit_program);
    glBindTextureUnit(0, out_color);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // render

}
