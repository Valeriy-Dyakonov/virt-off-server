#pragma once

#include <array>
#include <cmath>

namespace v2::game {

/// World-space vector (x, y, z) in metres, as sent by the Unity client.
using Vec3 = std::array<float, 3>;

/// Euclidean distance between `a` and `b`.
inline float distance(const Vec3& a, const Vec3& b) {
    return std::hypot(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}

}  // namespace v2::game
