#pragma once

#include "v2/game/props/registry/IPropReach.hpp"

#include "core/ILogger.hpp"
#include "v2/game/players/IPlayerState.hpp"
#include "v2/game/props/registry/IPropRepository.hpp"

namespace v2::game {

/// IPropReach by the last POSITION of the player and the last PROP_STATE of the prop.
class PropReach final : public IPropReach {
public:
    PropReach(const IPlayerState& players, const IPropRepository& props, ILogger& logger);

    [[nodiscard]] bool within_reach(const std::string& player_id, uint32_t prop_id) const override;

private:
    const IPlayerState& players_;

    const IPropRepository& props_;

    ILogger& logger_;
};

}  // namespace v2::game
