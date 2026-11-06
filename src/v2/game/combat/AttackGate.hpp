#pragma once

#include "v2/game/combat/IAttackGate.hpp"
#include "v2/game/players/IPlayerState.hpp"

#include <string>
#include <unordered_map>

namespace v2::game {

/// IAttackGate with a cooldown per (attacker, target) pair and positions from IPlayerState.
class AttackGate final : public IAttackGate {
public:
    explicit AttackGate(const IPlayerState& players);

    bool allow_attack(const std::string& attacker_id,
                      const std::string& target_id,
                      std::chrono::steady_clock::time_point now) override;

private:
    const IPlayerState& players_;

    /// "attacker:target" → time of the last granted hit.
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> last_hits_;
};

}  // namespace v2::game
