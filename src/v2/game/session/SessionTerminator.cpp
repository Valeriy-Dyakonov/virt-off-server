#include "v2/game/session/SessionTerminator.hpp"

#include "net/ProtocolConstants.hpp"
#include "v2/net/wire/WireWriter.hpp"

#include <format>

namespace v2::game {

SessionTerminator::SessionTerminator(IUserRepository& users,
                                     IPropReleaser& props,
                                     IPlayerState& players,
                                     net::IBroadcaster& broadcaster,
                                     ILogger& logger)
    : users_(users), props_(props), players_(players), broadcaster_(broadcaster), logger_(logger) {}

void SessionTerminator::terminate(const std::string& id) {
    if (!users_.find_by_id(id).has_value()) {
        return;
    }

    props_.release_all_of(id);
    players_.erase(id);
    users_.remove(id);

    // The user is already removed, so only the others receive it.
    broadcaster_.broadcast(protocol::MSG_PLAYER_LEFT, net::WireWriter{}.str(id).take());
    logger_.info(std::format("[Session] {} left", id));
}

}  // namespace v2::game
