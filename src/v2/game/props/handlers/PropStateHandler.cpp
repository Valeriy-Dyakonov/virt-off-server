#include "v2/game/props/handlers/PropStateHandler.hpp"

#include "net/ProtocolConstants.hpp"
#include "v2/game/props/model/PropFlags.hpp"

#include <chrono>
#include <cstddef>
#include <format>

namespace v2::game {

PropStateHandler::PropStateHandler(const IUserRepository& users,
                                   const IPropStateReader& reader,
                                   IPropRepository& props,
                                   IPropSimulation& simulation,
                                   const net::ISenderStamp& stamp,
                                   net::IBroadcaster& broadcaster,
                                   ILogger& logger)
    : users_(users)
    , reader_(reader)
    , props_(props)
    , simulation_(simulation)
    , stamp_(stamp)
    , broadcaster_(broadcaster)
    , logger_(logger) {}

void PropStateHandler::handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t> payload) {
    const auto sender_id = users_.find_id_by_endpoint(from);
    const auto state = reader_.read(payload);
    if (!sender_id.has_value() || !state.has_value()) {
        return;
    }

    if (const auto prop = props_.get(state->prop_id); !prop.has_value() || prop->owner_id != *sender_id) {
        return;
    }

    const auto stamped = stamp_.stamp(payload, *sender_id);
    if (!stamped.has_value()) {
        return;
    }

    props_.update_position(state->prop_id, state->position);
    simulation_.touch(state->prop_id, std::chrono::steady_clock::now());
    broadcaster_.broadcast(protocol::MSG_PROP_STATE, *stamped);

    const bool settled = (state->flags & kPropStateFlagSettled) != std::byte{0};
    if (settled && simulation_.end_simulation(state->prop_id, *sender_id)) {
        logger_.info(std::format("[PropState] prop={} settled, free", state->prop_id));
    }
}

}  // namespace v2::game
