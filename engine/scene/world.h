#pragma once

struct World
{
    constexpr int world_w = 128;
    constexpr int world_h = 128;
    constexpr int world_tiles = world_w * world_h;

    constexpr cull_margin = 6;

    static int tile_index(int x, int y) { return y * world_w + x; }
    static bool in_bounds(int x, int y) { return (unsigned)x < (unsigned)world_w && (unsigned)y < (unsigned)world_h; }
};
