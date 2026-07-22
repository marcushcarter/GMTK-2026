#include "light_batch.h"

bool LightBatch::initialize()
{
    const GLsizeiptr size = sizeof(GPULight) * MAX_LIGHTS * FRAMES;
    const GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    glCreateBuffers(1, &ssbo);
    glNamedBufferStorage(ssbo, size, nullptr, flags);
    mapped = (GPULight*)glMapNamedBufferRange(ssbo, 0, size, flags);

    return mapped != nullptr;
}

void LightBatch::shutdown()
{
    for (uint32_t i = 0; i < FRAMES; i++) {
        if (fences[i]) { glDeleteSync(fences[i]); fences[i] = nullptr; }
    }
    if (mapped) { glUnmapNamedBuffer(ssbo); mapped = nullptr; }
    glDeleteBuffers(1, &ssbo);
}

void LightBatch::begin()
{
    if (fences[frame]) {
        glClientWaitSync(fences[frame], GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
        glDeleteSync(fences[frame]);
        fences[frame] = nullptr;
    }
    write = mapped + frame * MAX_LIGHTS;
    count = 0;
}

void LightBatch::push(const GPULight& light)
{
    if (count >= MAX_LIGHTS) return;
    write[count++] = light;
}

void LightBatch::upload()
{
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, ssbo, frame * MAX_LIGHTS * sizeof(GPULight), count * sizeof(GPULight));
}

void LightBatch::end_frame()
{
    fences[frame] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    frame = (frame + 1) % FRAMES;    
}
