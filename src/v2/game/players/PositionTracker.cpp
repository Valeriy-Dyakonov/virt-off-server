#include "v2/game/players/PositionTracker.hpp"

#include "net/ProtocolConstants.hpp"

namespace v2::game {

PositionTracker::PositionTracker(const IUserRepository& users, const IPositionReader& reader, IPlayerState& state)
    : users_(users), reader_(reader), state_(state) {}

void PositionTracker::observe(const asio::ip::udp::endpoint& from, std::span<const uint8_t> datagram) {
    if (datagram[0] != protocol::MSG_POSITION) {
        return;
    }

    const auto sender_id = users_.find_id_by_endpoint(from);
    if (const auto position = reader_.read(datagram.subspan(1)); sender_id.has_value() && position.has_value()) {
        state_.update(*sender_id, *position);
    }
}

}  // namespace v2::game
