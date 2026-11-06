#pragma once

#include <asio/ip/udp.hpp>
#include <cstdint>
#include <span>

namespace v2::net {

/// Handles one message type routed by its type byte.
///
/// Thread-safety: NOT required — called on the transport strand only.
struct IPacketHandler {
    virtual ~IPacketHandler() = default;

    /// `payload` is the datagram without the type byte. Unknown senders and
    /// malformed or refused requests are dropped.
    virtual void handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t> payload) = 0;
};

}  // namespace v2::net
