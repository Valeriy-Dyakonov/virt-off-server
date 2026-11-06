#include "v2/game/combat/CombatModule.hpp"

#include "net/ProtocolConstants.hpp"
#include "v2/game/combat/AttackGate.hpp"
#include "v2/game/combat/AttackHandler.hpp"
#include "v2/game/combat/AttackTargetReader.hpp"

namespace v2::game {

CombatModule::CombatModule(const IUserRepository& users,
                           const IPlayerState& players,
                           const net::ISenderStamp& stamp,
                           net::IBroadcaster& broadcaster,
                           ILogger& logger)
    : reader_(std::make_unique<AttackTargetReader>())
    , gate_(std::make_unique<AttackGate>(players))
    , attack_handler_(std::make_unique<AttackHandler>(users, *reader_, *gate_, stamp, broadcaster, logger)) {}

net::ModuleBindings CombatModule::bindings() {
    return {.routes = {{protocol::MSG_ATTACK, *attack_handler_}}};
}

}  // namespace v2::game
