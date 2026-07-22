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

    {
    std::string c_s = load_file("assets/shaders/grayscale.comp");
    uint32_t c = _compile_shader(c_s.c_str(), GL_COMPUTE_SHADER);
    grayscale_program = _create_program(&c, 1);
    post_programs.push_back(grayscale_program);
    }

    if (!sprite_batch.initialize()) return false;
    if (!light_batch.initialize()) return false;
    // if (!atlas.initialize_white()) return false;

    request_resize(p_window->width, p_window->height);
    set_size();

    return true;
}

void Renderer::shutdown()
{   
    // atlas.shutdown();
    light_batch.shutdown();
    sprite_batch.shutdown();

    glDeleteVertexArrays(1, &temp_vao);

    glDeleteFramebuffers(1, &scene_framebuffer);
    glDeleteTextures(1, &scene_color);
    for (uint32_t i = 0; i < 2; i++) {
        glDeleteTextures(1, &post_color[i]);
    }

    glDeleteProgram(blit_program);
    glDeleteProgram(sprite_program);
    glDeleteProgram(grayscale_program);
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
    auto create_texture = [&](uint32_t& tex) {
        glDeleteTextures(1, &tex);
        glCreateTextures(GL_TEXTURE_2D, 1, &tex);
        glTextureStorage2D(tex, 1, GL_RGBA8, width, height);
        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    };

    create_texture(scene_color);

    for(int i = 0; i < 2; i++) {
        create_texture(post_color[i]);
    }

    glDeleteFramebuffers(1, &scene_framebuffer);
    glCreateFramebuffers(1, &scene_framebuffer);
    glNamedFramebufferTexture(scene_framebuffer, GL_COLOR_ATTACHMENT0, scene_color, 0);
    GLenum draw = GL_COLOR_ATTACHMENT0;
    glNamedFramebufferDrawBuffers(scene_framebuffer, 1, &draw);
    
    resize_requested = false;
}

void Renderer::render(Scene* scene)
{    
    if (resize_requested) set_size();

    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, scene_framebuffer);
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

    sprite_batch.begin();
    glm::ivec4 r = scene->camera.visible_tile_rect(CULL_MARGIN);
    for (int y = r.y; y <= r.w; y++) {
        for (int x = r.x; x <= r.z; x++) {
            sprite_batch.push(glm::vec2((float)x, (float)y), glm::vec2((float)x + 1.0f, (float)y + 1.0f), glm::vec2(0.0f), glm::vec2(1.0f), rgba(255, 255, 255));
        }
    }
    sprite_batch.flush();

    light_batch.begin();
    light_batch.push({ glm::vec2(0), 12.0f, rgba(255,200,120) });
    light_batch.upload();

    post_index = 0;
    uint32_t input = scene_color;
    for (uint32_t program : post_programs) {
        uint32_t output = post_color[post_index];
        glUseProgram(program);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, light_batch.ssbo);
        glBindImageTexture(0, input, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
        glBindImageTexture(1, output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
        uint32_t groups_x = (width + 7) / 8;
        uint32_t groups_y = (height + 7) / 8;
        glDispatchCompute(groups_x, groups_y, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        input = output;
        post_index ^= 1;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
    glUseProgram(blit_program);
    glBindTextureUnit(0, input);
    glBindVertexArray(temp_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    sprite_batch.end_frame();
    light_batch.end_frame();
}
