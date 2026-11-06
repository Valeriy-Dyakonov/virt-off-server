#include "v2/game/props/handlers/PickupHandler.hpp"

#include "v2/game/props/model/PropFlags.hpp"

#include <chrono>
#include <format>

namespace v2::game {

PickupHandler::PickupHandler(const IUserRepository& users,
                             const IPropIdReader& reader,
                             const IPropReach& reach,
                             IPropRepository& props,
                             IPropSimulation& simulation,
                             IGrantAnnouncer& grants,
                             ILogger& logger)
    : users_(users)
    , reader_(reader)
    , reach_(reach)
    , props_(props)
    , simulation_(simulation)
    , grants_(grants)
    , logger_(logger) {}

void PickupHandler::handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t> payload) {
    const auto sender_id = users_.find_id_by_endpoint(from);
    const auto prop_id = reader_.read(payload);
    if (!sender_id.has_value() || !prop_id.has_value() || !reach_.within_reach(*sender_id, *prop_id)) {
        return;
    }

    if (!props_.try_grant(*prop_id, *sender_id)) {
        logger_.info(std::format("[Pickup] from={} prop={} owned by another", *sender_id, *prop_id));
        return;
    }

    // The holder must report state from now on, otherwise the prop expires.
    simulation_.touch(*prop_id, std::chrono::steady_clock::now());

    logger_.info(std::format("[Pickup] from={} prop={} granted", *sender_id, *prop_id));
    grants_.announce(*prop_id, *sender_id, kGrantFlagHeld);
}

}  // namespace v2::game
