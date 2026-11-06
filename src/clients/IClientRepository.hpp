#pragma once

#include <asio/ip/udp.hpp>
#include <cstdint>
#include <functional>
#include <optional>

using asio::ip::udp;

/// Registry of all currently connected clients.
///
/// Thread-safety: NOT required — called on the transport strand only.
struct IClientRepository {
    /// Visitor of a client: (id, endpoint).
    using Visitor = std::function<void(uint64_t, const udp::endpoint&)>;

    virtual ~IClientRepository() = default;

    /// Registers a client endpoint and returns the assigned ID.
    virtual uint64_t add(const udp::endpoint& ep) = 0;

    /// Removes a client by ID. Safe to call with an unknown ID.
    virtual void remove(uint64_t id) = 0;

    /// Finds a client ID by endpoint. Returns std::nullopt if not found.
    [[nodiscard]] virtual std::optional<uint64_t> find_by_endpoint(const udp::endpoint& ep) const = 0;

    /// Invokes `visit(id, endpoint)` for every connected client whose ID is
    /// NOT equal to `exclude_id`. Useful for relay broadcasting.
    virtual void for_each_except(uint64_t exclude_id, const Visitor& visit) const = 0;
};
