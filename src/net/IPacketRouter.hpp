#pragma once

#include <asio/ip/udp.hpp>
#include <cstdint>
#include <functional>
#include <span>

using asio::ip::udp;

/// Dispatches datagrams to handlers by their first (type) byte.
///
/// Thread-safety: NOT required — register before start, route on the transport strand.
struct IPacketRouter {
    /// Receives the sender and the payload without the type byte.
    using Handler = std::function<void(const udp::endpoint&, std::span<const uint8_t>)>;

    virtual ~IPacketRouter() = default;

    /// Sets the handler for `type`, replacing a previous one.
    virtual void register_handler(uint8_t type, Handler handler) = 0;

    /// Calls the handler of `data[0]` with the rest of `data`.
    /// Empty datagrams and unregistered types are dropped.
    virtual void route(const udp::endpoint& from, std::span<const uint8_t> data) = 0;
};
