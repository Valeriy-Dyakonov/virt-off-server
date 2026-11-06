#include "v2/game/props/handlers/ReleaseHandler.hpp"

#include "net/ProtocolConstants.hpp"

#include <chrono>
#include <format>

namespace v2::game {

ReleaseHandler::ReleaseHandler(const IUserRepository& users,
                               const IPropIdReader& reader,
                               IPropSimulation& simulation,
                               const IReleaseEventEncoder& encoder,
                               net::IBroadcaster& broadcaster,
                               ILogger& logger)
    : users_(users)
    , reader_(reader)
    , simulation_(simulation)
    , encoder_(encoder)
    , broadcaster_(broadcaster)
    , logger_(logger) {}

void ReleaseHandler::handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t> payload) {
    const auto sender_id = users_.find_id_by_endpoint(from);
    if (!sender_id.has_value()) {
        return;
    }

    const auto prop_id = reader_.read(payload);
    const auto event = encoder_.encode_throw(payload, *sender_id);
    if (!prop_id.has_value() || !event.has_value()) {
        return;
    }

    if (!simulation_.begin_simulation(*prop_id, *sender_id, std::chrono::steady_clock::now())) {
        logger_.warn(std::format("[Release] from={} prop={} not owner", *sender_id, *prop_id));
        return;
    }

    logger_.info(std::format("[Release] from={} prop={}", *sender_id, *prop_id));
    broadcaster_.broadcast(protocol::MSG_PROP_RELEASE_EVENT, *event);
}

}  // namespace v2::game
