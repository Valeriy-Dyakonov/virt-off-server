#pragma once

#include "v2/game/Vec3.hpp"

#include <string>

namespace v2::game {

/// One prop of a client MANIFEST.
struct PropDescriptor {
    /// Scene-unique key (hierarchy path); props with equal keys are the same prop.
    std::string key;

    /// Half-extents of the collider, in metres.
    Vec3 half_extents{};

    /// Mass, in kg.
    float mass{};
};

}  // namespace v2::game
