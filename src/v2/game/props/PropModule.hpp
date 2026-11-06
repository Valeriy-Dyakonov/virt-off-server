#pragma once

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/game/players/IPlayerState.hpp"
#include "v2/game/props/codec/IManifestAckEncoder.hpp"
#include "v2/game/props/codec/IManifestReader.hpp"
#include "v2/game/props/codec/IPropIdReader.hpp"
#include "v2/game/props/codec/IPropStateReader.hpp"
#include "v2/game/props/codec/IReleaseEventEncoder.hpp"
#include "v2/game/props/events/IGrantAnnouncer.hpp"
#include "v2/game/props/events/IPropReleaser.hpp"
#include "v2/game/props/registry/IPropReach.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"
#include "v2/net/module/IPacketHandler.hpp"
#include "v2/net/module/IPeriodicTask.hpp"
#include "v2/net/module/IServerModule.hpp"
#include "v2/net/wire/ISenderStamp.hpp"

#include <memory>

namespace v2::game {

class PropRepository;

/// Props: registry from manifests, pickup, push, throw and state sync
/// (ownership rules: see README, "Prop ownership").
class PropModule final : public net::IServerModule {
public:
    PropModule(const IUserRepository& users,
               const IPlayerState& players,
               const net::ISenderStamp& stamp,
               net::IBroadcaster& broadcaster,
               ILogger& logger);

    ~PropModule() override;

    PropModule(const PropModule&) = delete;
    PropModule& operator=(const PropModule&) = delete;

    net::ModuleBindings bindings() override;

    /// Frees the props of a player who left; used by the session module.
    [[nodiscard]] IPropReleaser& releaser() const;

private:
    /// Serves both IPropRepository and IPropSimulation.
    std::unique_ptr<PropRepository> repository_;

    std::unique_ptr<IManifestReader> manifest_reader_;

    std::unique_ptr<IManifestAckEncoder> manifest_ack_encoder_;

    std::unique_ptr<IPropIdReader> prop_id_reader_;

    std::unique_ptr<IPropStateReader> state_reader_;

    std::unique_ptr<IReleaseEventEncoder> release_encoder_;

    std::unique_ptr<IPropReach> reach_;

    std::unique_ptr<IGrantAnnouncer> grants_;

    std::unique_ptr<IPropReleaser> releaser_;

    std::unique_ptr<net::IPacketHandler> manifest_handler_;

    std::unique_ptr<net::IPacketHandler> pickup_handler_;

    std::unique_ptr<net::IPacketHandler> push_handler_;

    std::unique_ptr<net::IPacketHandler> release_handler_;

    std::unique_ptr<net::IPacketHandler> state_handler_;

    std::unique_ptr<net::IPeriodicTask> sweeper_;
};

}  // namespace v2::game
