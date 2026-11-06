#pragma once

#include <chrono>

/// Gameplay rules enforced by the server.
namespace v2::game {

/// Farthest a player may be from a prop to pick it up or push it, in metres.
inline constexpr float kPropReachDistance = 3.0f;

/// Farthest an attacker may be from the target to land a hit, in metres.
inline constexpr float kAttackRange = 5.0f;

/// Shortest interval between two hits of the same attacker on the same target.
inline constexpr std::chrono::milliseconds kAttackCooldown{500};

/// A client silent for this long is disconnected.
inline constexpr std::chrono::milliseconds kSessionTimeout{30'000};

/// A prop owner that sends no PROP_STATE for this long loses the prop
/// (its settle packet was lost or the client froze).
inline constexpr std::chrono::milliseconds kPropSimulationTimeout{3'000};

/// Interval of the housekeeping tick (idle sessions, silent prop owners).
inline constexpr std::chrono::milliseconds kSweepInterval{5'000};

}  // namespace v2::game
