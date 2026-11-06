#pragma once

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/game/combat/IAttackGate.hpp"
#include "v2/game/combat/IAttackTargetReader.hpp"
#include "v2/game/players/IPlayerState.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"
#include "v2/net/module/IPacketHandler.hpp"
#include "v2/net/module/IServerModule.hpp"
#include "v2/net/wire/ISenderStamp.hpp"

#include <memory>

namespace v2::game {

/// Combat: validates and broadcasts ATTACK.
class CombatModule final : public net::IServerModule {
public:
    CombatModule(const IUserRepository& users,
                 const IPlayerState& players,
                 const net::ISenderStamp& stamp,
                 net::IBroadcaster& broadcaster,
                 ILogger& logger);

    net::ModuleBindings bindings() override;

private:
    std::unique_ptr<IAttackTargetReader> reader_;

    std::unique_ptr<IAttackGate> gate_;

    std::unique_ptr<net::IPacketHandler> attack_handler_;
};

}  // namespace v2::game
