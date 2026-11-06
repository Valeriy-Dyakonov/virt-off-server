#pragma once

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/core/IIdGenerator.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"
#include "v2/net/module/IPacketHandler.hpp"

namespace v2::game {

/// 0xFF CONNECT: registers the endpoint under a new id and replies
/// ASSIGN_ID [id utf8, no length prefix]. Resending gives a new id.
class ConnectHandler final : public net::IPacketHandler {
public:
    ConnectHandler(IIdGenerator& ids, IUserRepository& users, net::IBroadcaster& broadcaster, ILogger& logger);

    void handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t> payload) override;

private:
    IIdGenerator& ids_;

    IUserRepository& users_;

    net::IBroadcaster& broadcaster_;

    ILogger& logger_;
};

}  // namespace v2::game
