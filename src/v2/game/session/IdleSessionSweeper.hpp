#pragma once

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/game/session/ISessionTerminator.hpp"
#include "v2/net/module/IPeriodicTask.hpp"

namespace v2::game {

/// Ends the sessions of clients silent for longer than kSessionTimeout.
class IdleSessionSweeper final : public net::IPeriodicTask {
public:
    IdleSessionSweeper(const IUserRepository& users, ISessionTerminator& terminator, ILogger& logger);

    void run() override;

private:
    const IUserRepository& users_;

    ISessionTerminator& terminator_;

    ILogger& logger_;
};

}  // namespace v2::game
