#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace v2 {

/// Replaces the sender id that a client wrote into a payload with the id the
/// server associates with the sender's endpoint, so a client cannot speak on
/// behalf of another player.
///
/// Applies to payloads that start with [sender:str] — VOICE, POSITION, SOUND,
/// ATTACK, PROP_STATE. String layout: [len:1][utf8…].
///
/// Thread-safety: REQUIRED — stateless, called concurrently from io_context
/// worker threads (relay) and from the transport strand (game handlers).
struct ISenderStamp {
    virtual ~ISenderStamp() = default;

    /// Returns `payload` (without type byte) with its leading string replaced
    /// by `sender_id`; the rest of the payload is copied unchanged.
    /// Returns nullopt if the payload does not start with a complete string
    /// or `sender_id` is longer than 255 bytes.
    virtual std::optional<std::vector<uint8_t>> stamp(std::span<const uint8_t> payload,
                                                      const std::string& sender_id) const = 0;
};

}  // namespace v2
