#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace v2::game {

/// Tells every client who got a prop: PROP_GRANT [propId:4][holder:str][flags:1].
///
/// Thread-safety: NOT required — accessed only on the transport strand.
struct IGrantAnnouncer {
    virtual ~IGrantAnnouncer() = default;

    /// Broadcasts PROP_GRANT to all users, the holder included.
    /// `flags`: kGrantFlagHeld or kGrantFlagSimulate.
    virtual void announce(uint32_t prop_id, const std::string& holder_id, std::byte flags) = 0;
};

}  // namespace v2::game
