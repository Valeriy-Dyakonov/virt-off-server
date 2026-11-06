#pragma once

#include "net/IPacketRouter.hpp"

#include <unordered_map>

/// IPacketRouter over a hash map: type byte → handler.
///
/// Thread-safety: NOT required — see IPacketRouter.
class PacketRouter final : public IPacketRouter {
public:
    void register_handler(uint8_t type, Handler handler) override;

    void route(const udp::endpoint& from, std::span<const uint8_t> data) override;

private:
    std::unordered_map<uint8_t, Handler> handlers_;
};
