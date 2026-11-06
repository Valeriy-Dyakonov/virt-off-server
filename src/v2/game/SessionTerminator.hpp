#pragma once

#include "v2/game/ISessionTerminator.hpp"

#include "core/ConsoleLogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/game/IBroadcastHandler.hpp"
#include "v2/game/IPlayerState.hpp"
#include "v2/game/IPropRepository.hpp"

namespace v2::game {

/// ISessionTerminator over the user / prop / player-state repositories.
///
/// Thread-safety: NOT required — accessed only on the transport strand.
class SessionTerminator final : public ISessionTerminator {
public:
    SessionTerminator(IUserRepository& users,
                      IPropRepository& props,
                      IPlayerState& player_state,
                      IBroadcastHandler& broadcast);

    void terminate(const std::string& id) override;

private:
    IUserRepository& users_;

    IPropRepository& props_;

    IPlayerState& player_state_;

    IBroadcastHandler& broadcast_;

    static inline ILogger* logger_ = new ConsoleLogger();
};

}  // namespace v2::game
