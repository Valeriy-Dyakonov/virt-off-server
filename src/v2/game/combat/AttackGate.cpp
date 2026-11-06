#include "v2/game/combat/AttackGate.hpp"

#include "v2/game/GameConstants.hpp"

#include <format>
#include <optional>

namespace v2::game {

namespace {

/// Distance between two players, or nullopt if either position is unknown.
std::optional<float> distance_between(const IPlayerState& players, const std::string& a, const std::string& b) {
    const auto first = players.position(a);
    if (!first.has_value()) {
        return std::nullopt;
    }
    const auto second = players.position(b);
    if (!second.has_value()) {
        return std::nullopt;
    }
    return distance(*first, *second);
}

}  // namespace

AttackGate::AttackGate(const IPlayerState& players) : players_(players) {}

bool AttackGate::allow_attack(const std::string& attacker_id,
                              const std::string& target_id,
                              const std::chrono::steady_clock::time_point now) {
    const std::string pair_key = std::format("{}:{}", attacker_id, target_id);
    if (const auto it = last_hits_.find(pair_key); it != last_hits_.end() && now - it->second < kAttackCooldown) {
        return false;
    }

    if (const auto dist = distance_between(players_, attacker_id, target_id);
        !dist.has_value() || *dist > kAttackRange) {
        return false;
    }

    last_hits_[pair_key] = now;
    return true;
}

}  // namespace v2::game
