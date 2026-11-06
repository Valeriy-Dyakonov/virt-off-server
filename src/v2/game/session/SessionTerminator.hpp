#pragma once

#include "v2/game/session/ISessionTerminator.hpp"

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/game/players/IPlayerState.hpp"
#include "v2/game/props/events/IPropReleaser.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"

namespace v2::game {

/// ISessionTerminator over the user, prop and player registries.
class SessionTerminator final : public ISessionTerminator {
public:
    SessionTerminator(IUserRepository& users,
                      IPropReleaser& props,
                      IPlayerState& players,
                      net::IBroadcaster& broadcaster,
                      ILogger& logger);

    void terminate(const std::string& id) override;

private:
    IUserRepository& users_;

    IPropReleaser& props_;

    IPlayerState& players_;

    net::IBroadcaster& broadcaster_;

    ILogger& logger_;
};

}  // namespace v2::game
