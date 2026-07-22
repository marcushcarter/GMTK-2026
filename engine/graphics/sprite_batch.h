#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cstdint>
#include <cstddef>

struct SpriteVertex {
    glm::vec2 pos;
    glm::vec2 uv;
    uint32_t color;
    uint32_t tex;
};
static_assert(sizeof(SpriteVertex) == 24, "SpriteVertex must stay tightly packed");

inline uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return (uint32_t)r | ((uint32_t)g << 8) | ((uint32_t)b << 16) | ((uint32_t)a << 24);
}

struct SpriteBatch 
{
    static constexpr uint32_t MAX_SPRITES = 32768;
    static constexpr uint32_t MAX_VERTS = MAX_SPRITES * 4;
    static constexpr uint32_t FRAMES = 3;

    uint32_t vao = 0, vbo = 0, ibo = 0;

    SpriteVertex* mapped = nullptr;
    GLsync fences[FRAMES] = {};
    uint32_t frame = 0;

    SpriteVertex* write = nullptr;
    uint32_t count = 0;

    bool initialize();
    void shutdown();

    void begin();
    void push(glm::vec2 min, glm::vec2 max, glm::vec2 uv_min, glm::vec2 uv_max, uint32_t color, uint32_t tex = 0);
    void flush();
    void end_frame();
};