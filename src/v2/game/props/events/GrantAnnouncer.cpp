#include "v2/game/props/events/GrantAnnouncer.hpp"

#include "net/ProtocolConstants.hpp"
#include "v2/net/wire/WireWriter.hpp"

namespace v2::game {

GrantAnnouncer::GrantAnnouncer(net::IBroadcaster& broadcaster) : broadcaster_(broadcaster) {}

void GrantAnnouncer::announce(uint32_t prop_id, const std::string& holder_id, std::byte flags) {
    const auto grant = net::WireWriter{}.u32(prop_id).str(holder_id).byte(flags).take();
    broadcaster_.broadcast(protocol::MSG_PROP_GRANT, grant);
}

}  // namespace v2::game
