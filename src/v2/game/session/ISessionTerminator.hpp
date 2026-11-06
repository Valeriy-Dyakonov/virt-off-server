#pragma once

#include <string>

namespace v2::game {

/// Ends a player's session — on DISCONNECT and on idle timeout alike.
///
/// Frees the player's props (free PROP_RELEASE_EVENT for each), forgets its
/// position, removes the user and tells the others PLAYER_LEFT [id:str].
///
/// Thread-safety: NOT required — accessed only on the transport strand.
struct ISessionTerminator {
    virtual ~ISessionTerminator() = default;

    /// Ends the session of `id`; unknown ids are ignored.
    virtual void terminate(const std::string& id) = 0;
};

}  // namespace v2::game
