#include "net/UdpTransport.hpp"

#include <asio/bind_executor.hpp>
#include <asio/post.hpp>
#include <format>
#include <string>
#include <system_error>
#include <vector>

UdpTransport::UdpTransport(asio::io_context& io_ctx, ILogger& logger)
    : io_ctx_(io_ctx), logger_(logger), strand_(asio::make_strand(io_ctx)), socket_(io_ctx) {}

void UdpTransport::start(udp::endpoint local) {
    std::error_code ec;
    socket_.open(local.protocol(), ec);
    if (!ec) {
        socket_.bind(local, ec);
    }
    if (ec) {
        logger_.error(std::format("Cannot open or bind the UDP socket: {}", ec.message()));
        throw std::system_error(ec, "Cannot open or bind the UDP socket");
    }

    if (on_tick_ && tick_interval_ms_ > 0) {
        tick_timer_ = std::make_unique<asio::steady_timer>(io_ctx_);
        schedule_tick();
    }

    async_receive();
}

void UdpTransport::schedule_tick() {
    tick_timer_->expires_after(std::chrono::milliseconds(tick_interval_ms_));
    tick_timer_->async_wait(asio::bind_executor(strand_, [this](std::error_code ec) {
        if (ec) {
            return;  // cancelled by stop()
        }
        on_tick_();
        schedule_tick();
    }));
}

void UdpTransport::send_to(const udp::endpoint& dest, std::span<const uint8_t> data) {
    auto buffer = std::make_shared<std::vector<uint8_t>>(data.begin(), data.end());

    asio::post(strand_, [this, dest, buffer]() {
        // UDP loss is expected by the protocol, so send errors are not reported.
        socket_.async_send_to(asio::buffer(*buffer), dest,
                              asio::bind_executor(strand_, [buffer](std::error_code, std::size_t) {}));
    });
}

void UdpTransport::set_receive_callback(ReceiveCallback on_packet) {
    on_packet_ = std::move(on_packet);
}

void UdpTransport::set_tick_callback(TickCallback callback, unsigned int interval_ms) {
    on_tick_ = std::move(callback);
    tick_interval_ms_ = interval_ms;
}

void UdpTransport::stop() {
    std::error_code ec;
    if (tick_timer_) {
        tick_timer_->cancel(ec);
    }
    socket_.close(ec);
}

void UdpTransport::async_receive() {
    auto buffer = std::make_shared<ReceiveBuffer>();
    auto remote = std::make_shared<udp::endpoint>();

    socket_.async_receive_from(
        asio::buffer(*buffer), *remote,
        asio::bind_executor(strand_, [this, buffer, remote](std::error_code ec, std::size_t bytes) {
            if (ec) {
                return;  // socket closed by stop()
            }
            if (on_packet_ && bytes > 0) {
                on_packet_(*remote, std::vector<uint8_t>(buffer->begin(), buffer->begin() + bytes));
            }
            async_receive();
        }));
}
