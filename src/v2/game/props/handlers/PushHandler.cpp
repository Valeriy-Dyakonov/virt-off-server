#include "v2/game/props/handlers/PushHandler.hpp"

#include "v2/game/props/model/PropFlags.hpp"

#include <chrono>
#include <format>

namespace v2::game {

PushHandler::PushHandler(const IUserRepository& users,
                         const IPropIdReader& reader,
                         const IPropReach& reach,
                         IPropSimulation& simulation,
                         IGrantAnnouncer& grants,
                         ILogger& logger)
    : users_(users), reader_(reader), reach_(reach), simulation_(simulation), grants_(grants), logger_(logger) {}

void PushHandler::handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t> payload) {
    const auto sender_id = users_.find_id_by_endpoint(from);
    const auto prop_id = reader_.read(payload);
    if (!sender_id.has_value() || !prop_id.has_value() || !reach_.within_reach(*sender_id, *prop_id)) {
        return;
    }

    // Held or moved by someone else: that player's PROP_STATE wins.
    if (!simulation_.try_simulate(*prop_id, *sender_id, std::chrono::steady_clock::now())) {
        return;
    }

    logger_.info(std::format("[Push] from={} prop={} simulated", *sender_id, *prop_id));
    grants_.announce(*prop_id, *sender_id, kGrantFlagSimulate);
}

}  // namespace v2::game
