#include "net/UdpTransport.hpp"

#include <asio/bind_executor.hpp>
#include <asio/error.hpp>
#include <asio/post.hpp>
#include <format>
#include <system_error>
#include <vector>

UdpTransport::UdpTransport(asio::io_context& io_ctx, ILogger& logger)
    : io_ctx_(io_ctx), logger_(logger), strand_(asio::make_strand(io_ctx)), socket_(io_ctx) {}

void UdpTransport::start(const udp::endpoint& local) {
    try {
        socket_.open(local.protocol());
        socket_.bind(local);
    } catch (const std::system_error& failure) {
        logger_.error(std::format("Cannot open or bind the UDP socket on port {}: {}", local.port(),
                                  failure.code().message()));
        throw;
    }

    if (on_tick_ && tick_interval_ms_ > 0) {
        tick_timer_ = std::make_unique<asio::steady_timer>(io_ctx_);
        schedule_tick();
    }

    async_receive();
}

void UdpTransport::schedule_tick() {
    tick_timer_->expires_after(std::chrono::milliseconds(tick_interval_ms_));
    tick_timer_->async_wait(asio::bind_executor(strand_, [this](const std::error_code error) {
        if (error == asio::error::operation_aborted) {
            return;  // cancelled by stop()
        }
        if (error) {
            logger_.warn(std::format("Tick timer failed: {}", error.message()));
        } else {
            on_tick_();
        }
        schedule_tick();
    }));
}

void UdpTransport::send_to(const udp::endpoint& dest, const std::span<const uint8_t> data) {
    auto buffer = std::make_shared<std::vector<uint8_t>>(data.begin(), data.end());

    asio::post(strand_, [this, dest, buffer] {
        socket_.async_send_to(asio::buffer(*buffer), dest,
                              asio::bind_executor(strand_, [buffer](std::error_code, std::size_t) {
                                  // Nothing to do: the protocol tolerates UDP loss, so a failed send is
                                  // dropped like a lost datagram. The capture keeps `buffer` alive until here.
                              }));
    });
}

void UdpTransport::set_receive_callback(ReceiveCallback on_packet) {
    on_packet_ = std::move(on_packet);
}

void UdpTransport::set_tick_callback(TickCallback callback, const unsigned int interval_ms) {
    on_tick_ = std::move(callback);
    tick_interval_ms_ = interval_ms;
}

void UdpTransport::stop() {
    if (tick_timer_) {
        tick_timer_->cancel();
    }

    // The socket may already be closed or never opened; that is a normal state on shutdown.
    if (std::error_code error; socket_.close(error)) {
        logger_.warn(std::format("Closing the UDP socket failed: {}", error.message()));
    }
}

void UdpTransport::async_receive() {
    auto buffer = std::make_shared<ReceiveBuffer>();
    auto remote = std::make_shared<udp::endpoint>();

    socket_.async_receive_from(
        asio::buffer(*buffer), *remote,
        asio::bind_executor(strand_, [this, buffer, remote](const std::error_code error, const std::size_t bytes) {
            if (error == asio::error::operation_aborted || !socket_.is_open()) {
                return;  // closed by stop()
            }
            if (error) {
                // E.g. ICMP "port unreachable" from a client that left (Windows reports it on receive).
                logger_.warn(std::format("UDP receive from {} failed: {}", remote->address().to_string(),
                                         error.message()));
            } else if (on_packet_ && bytes > 0) {
                const auto received = std::span<const uint8_t>(*buffer).first(bytes);
                on_packet_(*remote, std::vector(received.begin(), received.end()));
            }
            async_receive();
        }));
}
