#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

struct GPULight
{
    glm::vec2 pos;
    float radius;
    uint32_t color;
};

struct LightBatch
{
    static constexpr uint32_t MAX_LIGHTS = 1024;
    static constexpr uint32_t FRAMES = 3;

    uint32_t ssbo = 0;

    GPULight* mapped = nullptr;
    GPULight* write = nullptr;

    GLsync fences[FRAMES] = {};
    uint32_t frame = 0;
    uint32_t count = 0;

    bool initialize();
    void shutdown();

    void begin();
    void push(const GPULight& light);
    void upload();
    void end_frame();
};
