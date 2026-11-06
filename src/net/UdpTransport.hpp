#pragma once

#include "core/ILogger.hpp"
#include "net/ITransport.hpp"

#include <array>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <asio/strand.hpp>
#include <memory>

/// ITransport over an asio UDP socket. Receives, sends and ticks are
/// serialised on one strand, so callbacks never run concurrently.
class UdpTransport final : public ITransport {
public:
    UdpTransport(asio::io_context& io_ctx, ILogger& logger);

    void start(const udp::endpoint& local) override;

    void send_to(const udp::endpoint& dest, std::span<const uint8_t> data) override;

    void set_receive_callback(ReceiveCallback on_packet) override;

    void set_tick_callback(TickCallback callback, unsigned int interval_ms) override;

    void stop() override;

private:
    /// Largest datagram the server accepts; longer ones are truncated by the OS.
    static constexpr size_t kReceiveBufferBytes = 4096;

    using ReceiveBuffer = std::array<uint8_t, kReceiveBufferBytes>;

    /// Queues the next asynchronous receive.
    void async_receive();

    /// Re-arms the tick timer for the next interval.
    void schedule_tick();

    asio::io_context& io_ctx_;

    ILogger& logger_;

    asio::strand<asio::io_context::executor_type> strand_;

    udp::socket socket_;

    ReceiveCallback on_packet_;

    /// Created on start() when a tick callback is set.
    std::unique_ptr<asio::steady_timer> tick_timer_;

    TickCallback on_tick_;

    unsigned int tick_interval_ms_{};
};
