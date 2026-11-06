#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace v2::game {

/// Builds PROP_RELEASE_EVENT: [propId:4][owner:str][vel:12][angVel:12][flags:1]
/// (without the type byte).
///
/// Thread-safety: REQUIRED — stateless.
struct IReleaseEventEncoder {
    virtual ~IReleaseEventEncoder() = default;

    /// Event for a client PROP_RELEASE [sender:str][propId:4][vel:12][angVel:12][flags:1]:
    /// `owner_id` threw the prop and keeps simulating it. Nullopt if the release is truncated.
    [[nodiscard]] virtual std::optional<std::vector<uint8_t>> encode_throw(std::span<const uint8_t> release_payload,
                                                             const std::string& owner_id) const = 0;

    /// Event for a prop that became free: empty owner, zero velocities, kReleaseFlagSettled.
    [[nodiscard]] virtual std::vector<uint8_t> encode_free(uint32_t prop_id) const = 0;
};

}  // namespace v2::game
