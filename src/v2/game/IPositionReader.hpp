#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <span>

namespace v2::game {

/// Extracts the player's world position from a POSITION payload.
///
/// Client layout (after type byte): [id:str][pos:12][yaw:4][flags:1].
/// Only the position is needed for attack / pickup distance checks.
///
/// Thread-safety: REQUIRED — stateless, called from the receive callback.
struct IPositionReader {
    virtual ~IPositionReader() = default;

    /// Returns x, y, z of the sender, or nullopt if the payload is too short.
    virtual std::optional<std::array<float, 3>> read(std::span<const uint8_t> payload) const = 0;
};

}  // namespace v2::game
