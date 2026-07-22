#pragma once
#include <scene/world.h>
#include <glm/glm.hpp>

struct Input;

struct Camera
{
    glm::vec2 pos = {64.0f, 64.0f};
    glm::vec2 target_pos = {64.0f, 64.0f};

    float zoom = 32.0f;
    float target_zoom = 32.0f;

    glm::vec2 viewport = {1.0f, 1.0f};

    bool grabbing = false;
    glm::vec2 grab_world = {0.0f, 0.0f};

    static constexpr float ZOOM_MIN = 4.0f;
    static constexpr float ZOOM_MAX = 96.0f;
    static constexpr float ZOOM_STEP = 1.12f;
    static constexpr float PAN_PIXELS_PER_SEC = 900.0f;
    static constexpr float SMOOTH_RATE = 18.0f;
    static constexpr float MIN_VISIBLE_TILES = 8.0f;

    void set_viewport(glm::vec2 v);
    void update(const Input& in, float dt);
    void fit_world();

    glm::vec2 screen_to_world(glm::vec2 s) const;
    glm::vec2 world_to_screen(glm::vec2 w) const;
    glm::vec4 clip_transform() const;
    glm::ivec4 visible_tile_rect(int margin) const;
};
