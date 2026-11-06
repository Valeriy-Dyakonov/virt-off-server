#pragma once

#include <cstdint>
#include <span>
#include <string>

namespace v2::net {

/// Sends server messages to connected users.
///
/// Thread-safety: REQUIRED — the user registry may change concurrently.
struct IBroadcaster {
    virtual ~IBroadcaster() = default;

    /// Sends [type][payload] to every user.
    virtual void broadcast(uint8_t type, std::span<const uint8_t> payload) = 0;

    /// Sends [type][payload] to every user except `excluded_id`.
    virtual void broadcast_except(const std::string& excluded_id, uint8_t type, std::span<const uint8_t> payload) = 0;

    /// Sends [type][payload] to the user `id`; unknown ids are ignored.
    virtual void send_to(const std::string& id, uint8_t type, std::span<const uint8_t> payload) = 0;
};

}  // namespace v2::net
