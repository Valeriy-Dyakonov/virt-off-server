#pragma once

#include <asio/ip/udp.hpp>

using asio::ip::udp;

/// Handles initial client connection (control byte 0xFF).
///
/// On connect: registers the endpoint in IClientRepository (which assigns the
/// id) and sends the id back as a 0xFE packet.
///
/// Thread-safety: NOT required — called on the transport strand only.
struct IConnectHandler {
    virtual ~IConnectHandler() = default;

    /// Processes a connection request from `client_endpoint`.
    virtual void handle(const udp::endpoint& client_endpoint) = 0;
};
