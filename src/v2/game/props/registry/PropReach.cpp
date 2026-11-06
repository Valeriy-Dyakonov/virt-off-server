#include "v2/game/props/registry/PropReach.hpp"

#include "v2/game/GameConstants.hpp"

#include <format>

namespace v2::game {

PropReach::PropReach(const IPlayerState& players, const IPropRepository& props, ILogger& logger)
    : players_(players), props_(props), logger_(logger) {}

bool PropReach::within_reach(const std::string& player_id, uint32_t prop_id) const {
    const auto prop = props_.get(prop_id);
    const auto player = players_.position(player_id);
    if (!prop.has_value() || !player.has_value()) {
        logger_.warn(std::format("[Reach] player={} prop={} unknown", player_id, prop_id));
        return false;
    }
    if (!prop->has_position) {
        return true;
    }

    if (const float dist = distance(*player, prop->position); dist > kPropReachDistance) {
        logger_.info(std::format("[Reach] player={} prop={} too far: {:.2f} m", player_id, prop_id, dist));
        return false;
    }
    return true;
}

}  // namespace v2::game
