#pragma once
#include <cstdint>

struct Input;

constexpr int WORLD_W = 128;
constexpr int WORLD_H = 128;
constexpr int WORLD_TILES = WORLD_W * WORLD_H;
constexpr int CULL_MARGIN = 3;

inline int tile_index(int x, int y) { return y * WORLD_W + x; }
inline bool in_bounds(int x, int y) { return (unsigned)x < (unsigned)WORLD_W && (unsigned)y < (unsigned)WORLD_H; }

struct Camera
{
    float pos_x = 64.0f, pos_y = 64.0f;
    float target_x = 64.0f, target_y = 64.0f;

    float zoom = 32.0f;
    float target_zoom = 32.0f;

    float viewport_w = 1.0f, viewport_h = 1.0f;

    bool grabbing = false;
    float grab_world_x = 0.0f, grab_world_y = 0.0f;

    static constexpr float ZOOM_MIN = 4.0f;
    static constexpr float ZOOM_MAX = 96.0f;
    static constexpr float ZOOM_STEP = 1.12f;
    static constexpr float PAN_PIXELS_PER_SEC = 900.0f;
    static constexpr float SMOOTH_RATE = 18.0f;
    static constexpr float MIN_VISIBLE_TILES = 8.0f;

    void set_viewport(float w, float h);
    void update(const Input& in, float dt);
    void fit_world();

    void screen_to_world(float sx, float sy, float& wx, float& wy) const;
    void world_to_screen(float wx, float wy, float& sx, float& sy) const;
    void clip_transform(float out_scale_offset[4]) const;
    void visible_tile_rect(int margin, int& x0, int& y0, int& x1, int& y1) const;
};
