#pragma once

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/names/codec/IPlayerNameEncoder.hpp"
#include "v2/names/codec/IPlayerNameReader.hpp"
#include "v2/names/codec/IPlayerNameSanitizer.hpp"
#include "v2/names/events/INameBroadcaster.hpp"
#include "v2/names/events/INameRoster.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"
#include "v2/net/module/IPacketHandler.hpp"
#include "v2/net/module/IServerModule.hpp"

#include <memory>

namespace v2::names {

/// Player display names: SET_NAME in, PLAYER_NAME out.
class NameModule final : public net::IServerModule {
public:
    NameModule(IUserRepository& users, net::IBroadcaster& broadcaster, ILogger& logger);

    net::ModuleBindings bindings() override;

private:
    std::unique_ptr<IPlayerNameReader> reader_;

    std::unique_ptr<IPlayerNameSanitizer> sanitizer_;

    std::unique_ptr<IPlayerNameEncoder> encoder_;

    std::unique_ptr<INameBroadcaster> name_broadcaster_;

    std::unique_ptr<INameRoster> roster_;

    std::unique_ptr<net::IPacketHandler> handler_;
};

}  // namespace v2::names
