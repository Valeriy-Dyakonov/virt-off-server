#pragma once

#include <asio/ip/udp.hpp>
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

using asio::ip::udp;

/// UDP socket wrapper.
///
/// Thread-safety: REQUIRED — public methods may be called from any thread.
/// Receive and tick callbacks are serialised: they never run concurrently.
struct ITransport {
    /// Called with the sender and the whole datagram (type byte included).
    using ReceiveCallback = std::function<void(const udp::endpoint&, const std::vector<uint8_t>&)>;

    /// Called every tick interval.
    using TickCallback = std::function<void()>;

    virtual ~ITransport() = default;

    /// Binds the socket to `local` and starts receiving. Throws std::system_error on failure.
    virtual void start(udp::endpoint local) = 0;

    /// Queues `data` for sending to `dest`. Non-blocking; send errors are dropped.
    virtual void send_to(const udp::endpoint& dest, std::span<const uint8_t> data) = 0;

    /// Sets the callback for received datagrams. Call before start().
    virtual void set_receive_callback(ReceiveCallback on_packet) = 0;

    /// Sets a callback run every `interval_ms`. Call before start(); the timer starts with it.
    virtual void set_tick_callback(TickCallback callback, unsigned int interval_ms) = 0;

    /// Closes the socket and cancels the timer and pending operations.
    virtual void stop() = 0;
};
