#include "sprite_batch.h"

bool SpriteBatch::initialize()
{
    const GLsizeiptr vbo_size = (GLsizeiptr)sizeof(SpriteVertex) * MAX_VERTS * FRAMES;
    const GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, vbo_size, nullptr, flags);
    mapped = (SpriteVertex*)glMapNamedBufferRange(vbo, 0, vbo_size, flags);
    if (!mapped) return false;

    uint32_t* indices = new uint32_t[MAX_SPRITES * 6];
    for (uint32_t i = 0; i < MAX_SPRITES; i++) {
        uint32_t v = i * 4;
        indices[i * 6 + 0] = v + 0;
        indices[i * 6 + 1] = v + 1;
        indices[i * 6 + 2] = v + 2;
        indices[i * 6 + 3] = v + 0;
        indices[i * 6 + 4] = v + 2;
        indices[i * 6 + 5] = v + 3;
    }
    glCreateBuffers(1, &ibo);
    glNamedBufferStorage(ibo, (GLsizeiptr)sizeof(uint32_t) * MAX_SPRITES * 6, indices, 0);
    delete[] indices;

    glCreateVertexArrays(1, &vao);
    glVertexArrayElementBuffer(vao, ibo);
    
    glEnableVertexArrayAttrib (vao, 0);
    glVertexArrayAttribFormat (vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(SpriteVertex, pos));
    glVertexArrayAttribBinding(vao, 0, 0);

    glEnableVertexArrayAttrib (vao, 1);
    glVertexArrayAttribFormat (vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(SpriteVertex, uv));
    glVertexArrayAttribBinding(vao, 1, 0);

    glEnableVertexArrayAttrib (vao, 2);
    glVertexArrayAttribFormat (vao, 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, offsetof(SpriteVertex, color));
    glVertexArrayAttribBinding(vao, 2, 0);

    glEnableVertexArrayAttrib  (vao, 3);
    glVertexArrayAttribIFormat (vao, 3, 1, GL_UNSIGNED_INT, offsetof(SpriteVertex, tex));
    glVertexArrayAttribBinding (vao, 3, 0);

    return true;
}

void SpriteBatch::shutdown()
{
    for (uint32_t i = 0; i < FRAMES; i++) {
        if (fences[i]) { glDeleteSync(fences[i]); fences[i] = nullptr; }
    }
    if (mapped) { glUnmapNamedBuffer(vbo); mapped = nullptr; }
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}

void SpriteBatch::begin()
{
    if (fences[frame]) {
        glClientWaitSync(fences[frame], GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
        glDeleteSync(fences[frame]);
        fences[frame] = nullptr;
    }
    write = mapped + (size_t)frame * MAX_VERTS;
    count = 0;
}

void SpriteBatch::push(glm::vec2 min, glm::vec2 max, glm::vec2 uv_min, glm::vec2 uv_max, uint32_t color, uint32_t tex)
{
    if (count >= MAX_SPRITES) return;
    write[0] = { {min.x, min.y}, {uv_min.x, uv_min.y}, color, tex };
    write[1] = { {max.x, min.y}, {uv_max.x, uv_min.y}, color, tex };
    write[2] = { {max.x, max.y}, {uv_max.x, uv_max.y}, color, tex };
    write[3] = { {min.x, max.y}, {uv_min.x, uv_max.y}, color, tex };
    write += 4;
    count++;
}

// void SpriteBatch::push(glm::vec2 min, glm::vec2 max, const SpriteRegion& r, uint32_t color)
// {
//     push(min, max, r.uv_min, r.uv_max, color, r.tex);
// }

// void SpriteBatch::push_sprite(glm::vec2 origin, const SpriteRegion& r, uint32_t color)
// {
//     glm::vec2 min = origin + r.offset;
//     push(min, min + r.size, r.uv_min, r.uv_max, color, r.tex);
// }

void SpriteBatch::flush()
{
    if (count == 0) return;
    GLintptr offset = (GLintptr)((size_t)frame * MAX_VERTS * sizeof(SpriteVertex));
    glVertexArrayVertexBuffer(vao, 0, vbo, offset, sizeof(SpriteVertex));
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)(count * 6), GL_UNSIGNED_INT, nullptr);
}

void SpriteBatch::end_frame()
{
    fences[frame] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    frame = (frame + 1) % FRAMES;
}
