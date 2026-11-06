#pragma once

#include <cstdint>
#include <optional>
#include <span>

namespace v2::game {

/// Reads the prop id of payloads that start with [sender:str][propId:4]:
/// PICKUP_REQ, PROP_RELEASE, PROP_PUSH (without the type byte).
///
/// Thread-safety: REQUIRED — stateless.
struct IPropIdReader {
    virtual ~IPropIdReader() = default;

    /// Returns the prop id, or nullopt if the payload is truncated.
    [[nodiscard]] virtual std::optional<uint32_t> read(std::span<const uint8_t> payload) const = 0;
};

}  // namespace v2::game
