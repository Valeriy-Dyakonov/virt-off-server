#pragma once

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/game/combat/IAttackGate.hpp"
#include "v2/game/combat/IAttackTargetReader.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"
#include "v2/net/module/IPacketHandler.hpp"
#include "v2/net/wire/ISenderStamp.hpp"

namespace v2::game {

/// 0x03 ATTACK: checks the hit with IAttackGate and broadcasts it to all
/// (the attacker included, for feedback) with the real attacker id.
class AttackHandler final : public net::IPacketHandler {
public:
    AttackHandler(const IUserRepository& users,
                  const IAttackTargetReader& reader,
                  IAttackGate& gate,
                  const net::ISenderStamp& stamp,
                  net::IBroadcaster& broadcaster,
                  ILogger& logger);

    void handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t> payload) override;

private:
    const IUserRepository& users_;

    const IAttackTargetReader& reader_;

    IAttackGate& gate_;

    const net::ISenderStamp& stamp_;

    net::IBroadcaster& broadcaster_;

    ILogger& logger_;
};

}  // namespace v2::game
