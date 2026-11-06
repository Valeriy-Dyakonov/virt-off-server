#pragma once

#include "v2/game/Vec3.hpp"

#include <chrono>
#include <cstdint>
#include <string>

namespace v2::game {

/// Server state of one prop.
///
/// No owner: free, resting at its last reported position. Owner and `held`:
/// in the owner's hands. Owner without `held`: thrown or pushed, the owner
/// simulates it until it settles. Only the owner may send PROP_STATE.
struct PropRecord {
    /// Id sent in MANIFEST_ACK; 1-based.
    uint32_t prop_id{};

    /// PropDescriptor::key of the manifest.
    std::string key;

    /// Empty when free.
    std::string owner_id;

    bool held{};

    /// When the owner last got the prop or reported its state (see IPropSimulation::expire).
    std::chrono::steady_clock::time_point last_activity{};

    /// Last position from PROP_STATE; valid when `has_position`.
    Vec3 position{};

    /// False until the first PROP_STATE: the manifest carries no positions.
    bool has_position{};
};

}  // namespace v2::game
