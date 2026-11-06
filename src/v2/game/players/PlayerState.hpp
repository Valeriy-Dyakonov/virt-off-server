#pragma once

#include "v2/game/players/IPlayerState.hpp"

#include <string>
#include <unordered_map>

namespace v2::game {

/// In-memory IPlayerState.
class PlayerState final : public IPlayerState {
public:
    void update(const std::string& id, const Vec3& position) override;

    std::optional<Vec3> position(const std::string& id) const override;

    void erase(const std::string& id) override;

private:
    std::unordered_map<std::string, Vec3> positions_;
};

}  // namespace v2::game
