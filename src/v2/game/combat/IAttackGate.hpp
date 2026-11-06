#pragma once

#include <chrono>
#include <string>

namespace v2::game {

/// Decides whether an attack may land: the target is within kAttackRange and
/// the same attacker did not hit it within kAttackCooldown.
///
/// Thread-safety: NOT required — accessed only on the transport strand.
struct IAttackGate {
    virtual ~IAttackGate() = default;

    /// True if `attacker_id` may hit `target_id` at `now`; a granted hit restarts the cooldown.
    virtual bool allow_attack(const std::string& attacker_id,
                              const std::string& target_id,
                              std::chrono::steady_clock::time_point now) = 0;
};

}  // namespace v2::game
