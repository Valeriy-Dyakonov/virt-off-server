#include "v2/game/props/PropModule.hpp"

#include "net/ProtocolConstants.hpp"
#include "v2/game/props/codec/ManifestAckEncoder.hpp"
#include "v2/game/props/codec/ManifestReader.hpp"
#include "v2/game/props/codec/PropIdReader.hpp"
#include "v2/game/props/codec/PropStateReader.hpp"
#include "v2/game/props/codec/ReleaseEventEncoder.hpp"
#include "v2/game/props/events/GrantAnnouncer.hpp"
#include "v2/game/props/events/PropReleaser.hpp"
#include "v2/game/props/handlers/ManifestHandler.hpp"
#include "v2/game/props/handlers/PickupHandler.hpp"
#include "v2/game/props/handlers/PropStateHandler.hpp"
#include "v2/game/props/handlers/PushHandler.hpp"
#include "v2/game/props/handlers/ReleaseHandler.hpp"
#include "v2/game/props/registry/PropReach.hpp"
#include "v2/game/props/registry/PropRepository.hpp"
#include "v2/game/props/registry/SimulationSweeper.hpp"

namespace v2::game {

PropModule::PropModule(const IUserRepository& users,
                       const IPlayerState& players,
                       const net::ISenderStamp& stamp,
                       net::IBroadcaster& broadcaster,
                       ILogger& logger)
    : repository_(std::make_unique<PropRepository>())
    , manifest_reader_(std::make_unique<ManifestReader>())
    , manifest_ack_encoder_(std::make_unique<ManifestAckEncoder>())
    , prop_id_reader_(std::make_unique<PropIdReader>())
    , state_reader_(std::make_unique<PropStateReader>())
    , release_encoder_(std::make_unique<ReleaseEventEncoder>())
    , reach_(std::make_unique<PropReach>(players, *repository_, logger))
    , grants_(std::make_unique<GrantAnnouncer>(broadcaster))
    , releaser_(std::make_unique<PropReleaser>(*repository_, *release_encoder_, broadcaster))
    , manifest_handler_(std::make_unique<ManifestHandler>(users, *manifest_reader_, *repository_,
                                                          *manifest_ack_encoder_, broadcaster, logger))
    , pickup_handler_(std::make_unique<PickupHandler>(users, *prop_id_reader_, *reach_, *repository_, *repository_,
                                                      *grants_, logger))
    , push_handler_(std::make_unique<PushHandler>(users, *prop_id_reader_, *reach_, *repository_, *grants_, logger))
    , release_handler_(std::make_unique<ReleaseHandler>(users, *prop_id_reader_, *repository_, *release_encoder_,
                                                        broadcaster, logger))
    , state_handler_(std::make_unique<PropStateHandler>(users, *state_reader_, *repository_, *repository_, stamp,
                                                        broadcaster, logger))
    , sweeper_(std::make_unique<SimulationSweeper>(*repository_, logger)) {}

PropModule::~PropModule() = default;

net::ModuleBindings PropModule::bindings() {
    return {
        .routes = {{protocol::MSG_MANIFEST, *manifest_handler_},
                   {protocol::MSG_PICKUP_REQ, *pickup_handler_},
                   {protocol::MSG_PROP_PUSH, *push_handler_},
                   {protocol::MSG_PROP_RELEASE, *release_handler_},
                   {protocol::MSG_PROP_STATE, *state_handler_}},
        .tasks = {*sweeper_},
    };
}

IPropReleaser& PropModule::releaser() const {
    return *releaser_;
}

}  // namespace v2::game
