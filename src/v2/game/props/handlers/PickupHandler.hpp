#pragma once

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/game/props/codec/IPropIdReader.hpp"
#include "v2/game/props/events/IGrantAnnouncer.hpp"
#include "v2/game/props/registry/IPropReach.hpp"
#include "v2/game/props/registry/IPropRepository.hpp"
#include "v2/game/props/registry/IPropSimulation.hpp"
#include "v2/net/module/IPacketHandler.hpp"

namespace v2::game {

/// 0x07 PICKUP_REQ: reach check → IPropRepository::try_grant → PROP_GRANT (held).
class PickupHandler final : public net::IPacketHandler {
public:
    PickupHandler(const IUserRepository& users,
                  const IPropIdReader& reader,
                  const IPropReach& reach,
                  IPropRepository& props,
                  IPropSimulation& simulation,
                  IGrantAnnouncer& grants,
                  ILogger& logger);

    void handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t> payload) override;

private:
    const IUserRepository& users_;

    const IPropIdReader& reader_;

    const IPropReach& reach_;

    IPropRepository& props_;

    IPropSimulation& simulation_;

    IGrantAnnouncer& grants_;

    ILogger& logger_;
};

}  // namespace v2::game
