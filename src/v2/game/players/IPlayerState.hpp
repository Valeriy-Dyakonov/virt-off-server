#pragma once

#include "v2/game/Vec3.hpp"

#include <optional>
#include <string>

namespace v2::game {

/// Last known position of every player, for attack and reach checks.
///
/// Thread-safety: NOT required — accessed only on the transport strand.
struct IPlayerState {
    virtual ~IPlayerState() = default;

    /// Stores the latest position of `id`.
    virtual void update(const std::string& id, const Vec3& position) = 0;

    /// Returns the latest position of `id`, or nullopt if none was reported.
    [[nodiscard]] virtual std::optional<Vec3> position(const std::string& id) const = 0;

    /// Forgets `id` (the player left).
    virtual void erase(const std::string& id) = 0;
};

}  // namespace v2::game
