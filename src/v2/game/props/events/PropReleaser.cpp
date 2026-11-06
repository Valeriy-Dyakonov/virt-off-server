#include "v2/game/props/events/PropReleaser.hpp"

#include "net/ProtocolConstants.hpp"

namespace v2::game {

PropReleaser::PropReleaser(IPropRepository& props, const IReleaseEventEncoder& encoder,
                           net::IBroadcaster& broadcaster)
    : props_(props), encoder_(encoder), broadcaster_(broadcaster) {}

void PropReleaser::release_all_of(const std::string& owner_id) {
    for (const uint32_t prop_id : props_.release_all_of(owner_id)) {
        broadcaster_.broadcast(protocol::MSG_PROP_RELEASE_EVENT, encoder_.encode_free(prop_id));
    }
}

}  // namespace v2::game
