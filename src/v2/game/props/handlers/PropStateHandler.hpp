#pragma once

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/game/props/codec/IPropStateReader.hpp"
#include "v2/game/props/registry/IPropRepository.hpp"
#include "v2/game/props/registry/IPropSimulation.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"
#include "v2/net/module/IPacketHandler.hpp"
#include "v2/net/wire/ISenderStamp.hpp"

namespace v2::game {

/// 0x04 PROP_STATE, accepted from the owner only: remembers the position,
/// refreshes the owner activity, broadcasts the state to all (the author
/// included) and frees the prop when the owner reports it settled.
class PropStateHandler final : public net::IPacketHandler {
public:
    PropStateHandler(const IUserRepository& users,
                     const IPropStateReader& reader,
                     IPropRepository& props,
                     IPropSimulation& simulation,
                     const net::ISenderStamp& stamp,
                     net::IBroadcaster& broadcaster,
                     ILogger& logger);

    void handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t> payload) override;

private:
    const IUserRepository& users_;

    const IPropStateReader& reader_;

    IPropRepository& props_;

    IPropSimulation& simulation_;

    const net::ISenderStamp& stamp_;

    net::IBroadcaster& broadcaster_;

    ILogger& logger_;
};

}  // namespace v2::game
