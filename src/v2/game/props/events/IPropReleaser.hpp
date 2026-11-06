#pragma once

#include <string>

namespace v2::game {

/// Frees the props of a player who left.
///
/// Thread-safety: NOT required — accessed only on the transport strand.
struct IPropReleaser {
    virtual ~IPropReleaser() = default;

    /// Frees every prop owned by `owner_id` and broadcasts a free
    /// PROP_RELEASE_EVENT for each of them (other props are not touched).
    virtual void release_all_of(const std::string& owner_id) = 0;
};

}  // namespace v2::game
