#pragma once

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/game/props/codec/IPropIdReader.hpp"
#include "v2/game/props/codec/IReleaseEventEncoder.hpp"
#include "v2/game/props/registry/IPropSimulation.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"
#include "v2/net/module/IPacketHandler.hpp"

namespace v2::game {

/// 0x08 PROP_RELEASE: the holder threw or dropped the prop and keeps simulating
/// it (IPropSimulation::begin_simulation). Everybody gets PROP_RELEASE_EVENT with
/// that owner and follows its PROP_STATE while the prop flies.
class ReleaseHandler final : public net::IPacketHandler {
public:
    ReleaseHandler(const IUserRepository& users,
                   const IPropIdReader& reader,
                   IPropSimulation& simulation,
                   const IReleaseEventEncoder& encoder,
                   net::IBroadcaster& broadcaster,
                   ILogger& logger);

    void handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t> payload) override;

private:
    const IUserRepository& users_;

    const IPropIdReader& reader_;

    IPropSimulation& simulation_;

    const IReleaseEventEncoder& encoder_;

    net::IBroadcaster& broadcaster_;

    ILogger& logger_;
};

}  // namespace v2::game
