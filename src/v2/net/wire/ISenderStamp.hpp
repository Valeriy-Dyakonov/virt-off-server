#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace v2::net {

/// Replaces the sender id a client wrote at the start of a payload with the id
/// the server bound to its endpoint, so nobody can speak for another player.
///
/// Applies to payloads that start with [sender:str]: VOICE, POSITION, SOUND,
/// ATTACK, PROP_STATE.
///
/// Thread-safety: REQUIRED — stateless.
struct ISenderStamp {
    virtual ~ISenderStamp() = default;

    /// Returns `payload` (without the type byte) with its leading string
    /// replaced by `sender_id`, or nullopt if the payload does not start with
    /// a complete string or `sender_id` is longer than kMaxStringBytes.
    virtual std::optional<std::vector<uint8_t>> stamp(std::span<const uint8_t> payload,
                                                      const std::string& sender_id) const = 0;
};

}  // namespace v2::net
