#pragma once

#include "v2/game/Vec3.hpp"

#include <cstdint>
#include <optional>
#include <span>

namespace v2::game {

/// Parses the position from POSITION: [sender:str][pos:12][yaw:4][flags:1]
/// (payload without the type byte).
///
/// Thread-safety: REQUIRED — stateless.
struct IPositionReader {
    virtual ~IPositionReader() = default;

    /// Returns the sender position, or nullopt if the payload is truncated.
    virtual std::optional<Vec3> read(std::span<const uint8_t> payload) const = 0;
};

}  // namespace v2::game
