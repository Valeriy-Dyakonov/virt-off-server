#pragma once

#include <asio/ip/udp.hpp>
#include <cstdint>
#include <span>

namespace v2::net {

/// Sees every received datagram before it is routed to its handler.
///
/// Thread-safety: NOT required — called on the transport strand only.
struct IPacketObserver {
    virtual ~IPacketObserver() = default;

    /// `datagram` includes the type byte and is never empty.
    virtual void observe(const asio::ip::udp::endpoint& from, std::span<const uint8_t> datagram) = 0;
};

}  // namespace v2::net
