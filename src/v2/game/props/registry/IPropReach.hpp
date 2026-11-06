#pragma once

#include <cstdint>
#include <string>

namespace v2::game {

/// Decides whether a player is close enough to a prop to take or push it.
///
/// Thread-safety: NOT required — accessed only on the transport strand.
struct IPropReach {
    virtual ~IPropReach() = default;

    /// True if `player_id` is within kPropReachDistance of `prop_id`. A prop
    /// without a reported position yet is reachable; an unknown prop or a
    /// player without a known position is not.
    [[nodiscard]] virtual bool within_reach(const std::string& player_id, uint32_t prop_id) const = 0;
};

}  // namespace v2::game
