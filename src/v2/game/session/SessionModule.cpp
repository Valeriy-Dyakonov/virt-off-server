#include "v2/game/session/SessionModule.hpp"

#include "net/ProtocolConstants.hpp"
#include "v2/core/TimestampIdGenerator.hpp"
#include "v2/game/session/ActivityTracker.hpp"
#include "v2/game/session/ConnectHandler.hpp"
#include "v2/game/session/DisconnectHandler.hpp"
#include "v2/game/session/IdleSessionSweeper.hpp"
#include "v2/game/session/SessionTerminator.hpp"

namespace v2::game {

SessionModule::SessionModule(IUserRepository& users,
                             IPlayerState& players,
                             IPropReleaser& props,
                             net::IBroadcaster& broadcaster,
                             ILogger& logger)
    : ids_(std::make_unique<TimestampIdGenerator>())
    , terminator_(std::make_unique<SessionTerminator>(users, props, players, broadcaster, logger))
    , connect_handler_(std::make_unique<ConnectHandler>(*ids_, users, broadcaster, logger))
    , disconnect_handler_(std::make_unique<DisconnectHandler>(users, *terminator_))
    , activity_tracker_(std::make_unique<ActivityTracker>(users))
    , idle_sweeper_(std::make_unique<IdleSessionSweeper>(users, *terminator_, logger)) {}

net::ModuleBindings SessionModule::bindings() {
    return {
        .routes = {{protocol::MSG_CONNECT, *connect_handler_}, {protocol::MSG_DISCONNECT, *disconnect_handler_}},
        .observers = {*activity_tracker_},
        .tasks = {*idle_sweeper_},
    };
}

}  // namespace v2::game
