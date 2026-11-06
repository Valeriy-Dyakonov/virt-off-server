#pragma once

#include "v2/clients/IUserRepository.hpp"
#include "v2/game/session/ISessionTerminator.hpp"
#include "v2/net/module/IPacketHandler.hpp"

namespace v2::game {

/// 0x0D DISCONNECT: ends the sender's session at once instead of waiting for the timeout.
class DisconnectHandler final : public net::IPacketHandler {
public:
    DisconnectHandler(const IUserRepository& users, ISessionTerminator& terminator);

    void handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t> payload) override;

private:
    const IUserRepository& users_;

    ISessionTerminator& terminator_;
};

}  // namespace v2::game
