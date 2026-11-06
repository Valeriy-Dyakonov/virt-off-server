#pragma once

#include "v2/game/Vec3.hpp"
#include "v2/game/props/model/PropDescriptor.hpp"
#include "v2/game/props/model/PropRecord.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace v2::game {

/// Prop registry and the "into the hands" part of ownership.
///
/// Thread-safety: NOT required — accessed only on the transport strand.
struct IPropRepository {
    virtual ~IPropRepository() = default;

    /// Registers the props of a client manifest; a known key keeps its id (first
    /// manifest wins). Returns the prop id of every descriptor, in manifest order.
    [[nodiscard]] virtual std::vector<uint32_t> register_manifest(const std::vector<PropDescriptor>& descriptors) = 0;

    /// Gives `prop_id` into the hands of `holder_id` if it is free or already
    /// owned by `holder_id` (catching its own throw). False otherwise.
    [[nodiscard]] virtual bool try_grant(uint32_t prop_id, const std::string& holder_id) = 0;

    /// Frees every prop owned by `owner_id`. Returns their ids.
    [[nodiscard]] virtual std::vector<uint32_t> release_all_of(const std::string& owner_id) = 0;

    /// Stores the last reported position of `prop_id`; unknown ids are ignored.
    virtual void update_position(uint32_t prop_id, const Vec3& position) = 0;

    /// Returns a copy of the record of `prop_id`, or nullopt.
    [[nodiscard]] virtual std::optional<PropRecord> get(uint32_t prop_id) const = 0;
};

}  // namespace v2::game
