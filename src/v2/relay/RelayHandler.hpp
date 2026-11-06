#pragma once

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"
#include "v2/net/module/IPacketHandler.hpp"
#include "v2/net/wire/ISenderStamp.hpp"

#include <cstdint>

namespace v2::relay {

/// Forwards one message type ([sender:str]…) to every user except the sender,
/// with the sender id replaced by the one bound to its endpoint.
class RelayHandler final : public net::IPacketHandler {
public:
    RelayHandler(uint8_t type,
                 const IUserRepository& users,
                 const net::ISenderStamp& stamp,
                 net::IBroadcaster& broadcaster,
                 ILogger& logger);

    void handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t> payload) override;

private:
    uint8_t type_;

    const IUserRepository& users_;

    const net::ISenderStamp& stamp_;

    net::IBroadcaster& broadcaster_;

    ILogger& logger_;
};

}  // namespace v2::relay
