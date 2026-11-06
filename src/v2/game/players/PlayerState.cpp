#include "v2/game/players/PlayerState.hpp"

namespace v2::game {

void PlayerState::update(const std::string& id, const Vec3& position) {
    positions_[id] = position;
}

std::optional<Vec3> PlayerState::position(const std::string& id) const {
    const auto it = positions_.find(id);
    if (it == positions_.end()) {
        return std::nullopt;
    }
    return it->second;
}

void PlayerState::erase(const std::string& id) {
    positions_.erase(id);
}

}  // namespace v2::game
