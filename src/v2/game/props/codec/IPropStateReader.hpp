#pragma once

#include "v2/game/Vec3.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

namespace v2::game {

/// Fields of PROP_STATE the server acts upon.
struct PropStateFields {
    uint32_t prop_id{};

    /// Position reported by the owner.
    Vec3 position{};

    /// See kPropStateFlagSettled.
    std::byte flags{};
};

/// Parses PROP_STATE: [sender:str][propId:4][pos:12][rot:16][vel:12][flags:1]
/// (without the type byte).
///
/// Thread-safety: REQUIRED — stateless.
struct IPropStateReader {
    virtual ~IPropStateReader() = default;

    /// Returns the fields, or nullopt if the payload is truncated.
    virtual std::optional<PropStateFields> read(std::span<const uint8_t> payload) const = 0;
};

}  // namespace v2::game
