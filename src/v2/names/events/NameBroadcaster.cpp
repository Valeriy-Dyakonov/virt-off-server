#include "v2/names/events/NameBroadcaster.hpp"

#include "net/ProtocolConstants.hpp"

namespace v2::names {

NameBroadcaster::NameBroadcaster(const IPlayerNameEncoder& encoder, net::IBroadcaster& broadcaster)
    : encoder_(encoder), broadcaster_(broadcaster) {}

void NameBroadcaster::announce(const std::string& id, const std::string& name) {
    broadcaster_.broadcast(protocol::MSG_PLAYER_NAME, encoder_.encode(id, name));
}

}  // namespace v2::names
