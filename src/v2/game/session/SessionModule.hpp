#pragma once

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/core/IIdGenerator.hpp"
#include "v2/game/players/IPlayerState.hpp"
#include "v2/game/props/events/IPropReleaser.hpp"
#include "v2/game/session/ISessionTerminator.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"
#include "v2/net/module/IPacketHandler.hpp"
#include "v2/net/module/IPacketObserver.hpp"
#include "v2/net/module/IPeriodicTask.hpp"
#include "v2/net/module/IServerModule.hpp"

#include <memory>

namespace v2::game {

/// Session lifecycle: CONNECT, DISCONNECT, activity tracking and idle timeout.
class SessionModule final : public net::IServerModule {
public:
    SessionModule(IUserRepository& users,
                  IPlayerState& players,
                  IPropReleaser& props,
                  net::IBroadcaster& broadcaster,
                  ILogger& logger);

    net::ModuleBindings bindings() override;

private:
    std::unique_ptr<IIdGenerator> ids_;

    std::unique_ptr<ISessionTerminator> terminator_;

    std::unique_ptr<net::IPacketHandler> connect_handler_;

    std::unique_ptr<net::IPacketHandler> disconnect_handler_;

    std::unique_ptr<net::IPacketObserver> activity_tracker_;

    std::unique_ptr<net::IPeriodicTask> idle_sweeper_;
};

}  // namespace v2::game
