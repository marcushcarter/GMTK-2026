#pragma once
#include <cstdint>

constexpr int WORLD_W = 128;
constexpr int WORLD_H = 128;
constexpr int WORLD_TILES = WORLD_W * WORLD_H;
constexpr int CULL_MARGIN = 3;

inline int tile_index(int x, int y) { return y * WORLD_W + x; }
inline bool in_bounds(int x, int y) { return (unsigned)x < (unsigned)WORLD_W && (unsigned)y < (unsigned)WORLD_H; }

struct World
{
    uint32_t seed = 1337;

    void generate(uint32_t p_seed);
};
