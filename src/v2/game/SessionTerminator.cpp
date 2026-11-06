#include "v2/game/SessionTerminator.hpp"
#include "v2/game/WireSizes.hpp"
#include "net/ProtocolConstants.hpp"

#include <cstring>
#include <vector>

namespace v2::game {

SessionTerminator::SessionTerminator(IUserRepository& users,
                                     IPropRepository& props,
                                     IPlayerState& player_state,
                                     IBroadcastHandler& broadcast)
    : users_(users), props_(props), player_state_(player_state), broadcast_(broadcast) {}

void SessionTerminator::terminate(const std::string& id) {
    if (!users_.find_by_id(id).has_value()) {
        return;
    }

    for (const uint32_t prop_id : props_.release_all_of(id)) {
        // PROP_RELEASE_EVENT: [propId:4][ownerId:str][vel:12][angVel:12][flags:1]
        // Empty owner + settled flag: the prop is FREE.
        std::vector<uint8_t> release(kU32Bytes + kStrLenBytes + kVec3Bytes + kVec3Bytes + kFlagsBytes, 0);
        std::memcpy(release.data(), &prop_id, kU32Bytes);
        release.back() = kPropFlagSettled;
        broadcast_.broadcast(protocol::MSG_PROP_RELEASE_EVENT, release);
    }

    player_state_.erase(id);
    users_.remove(id);

    // PLAYER_LEFT: [id:str]. The user is already removed, so only the others receive it.
    std::vector<uint8_t> left(kStrLenBytes + id.size());
    left[0] = static_cast<uint8_t>(id.size());
    std::memcpy(left.data() + kStrLenBytes, id.data(), id.size());
    broadcast_.broadcast(protocol::MSG_PLAYER_LEFT, left);

    logger_->info("[Session] " + id + " left");
}

}  // namespace v2::game
