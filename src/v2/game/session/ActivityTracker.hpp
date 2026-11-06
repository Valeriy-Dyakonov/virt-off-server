#pragma once

#include "v2/clients/IUserRepository.hpp"
#include "v2/net/module/IPacketObserver.hpp"

namespace v2::game {

/// Marks the sender of every datagram as active, postponing its idle timeout.
class ActivityTracker final : public net::IPacketObserver {
public:
    explicit ActivityTracker(IUserRepository& users);

    void observe(const asio::ip::udp::endpoint& from, std::span<const uint8_t> datagram) override;

private:
    IUserRepository& users_;
};

}  // namespace v2::game
