#pragma once
#include <array>
#include <cstdint>

struct Vertex {
    std::array<float, 3> position;   // x, y, z
    std::array<float, 3> normal;     // nx, ny, nz
    std::array<float, 4> color;      // r, g, b, a
    std::array<float, 2> uv;         // u, v
};
