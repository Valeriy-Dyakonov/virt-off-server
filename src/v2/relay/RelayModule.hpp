#pragma once

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"
#include "v2/net/module/IPacketHandler.hpp"
#include "v2/net/module/IServerModule.hpp"
#include "v2/net/wire/ISenderStamp.hpp"

#include <memory>

namespace v2::relay {

/// Relays VOICE, POSITION and SOUND from a player to everybody else.
class RelayModule final : public net::IServerModule {
public:
    RelayModule(const IUserRepository& users,
                const net::ISenderStamp& stamp,
                net::IBroadcaster& broadcaster,
                ILogger& logger);

    net::ModuleBindings bindings() override;

private:
    std::unique_ptr<net::IPacketHandler> voice_;

    std::unique_ptr<net::IPacketHandler> position_;

    std::unique_ptr<net::IPacketHandler> sound_;
};

}  // namespace v2::relay
