#pragma once

#include <string>

namespace v2::game {

/// Ends a player's session — on DISCONNECT and on idle timeout alike.
///
/// Releases the props the player owned and broadcasts PROP_RELEASE_EVENT for
/// each of them (only those — other props keep their place on the clients),
/// forgets the player's transform, removes the user and broadcasts
/// PLAYER_LEFT [id:str] to the remaining users.
///
/// Thread-safety: NOT required — accessed only on the transport strand.
struct ISessionTerminator {
    virtual ~ISessionTerminator() = default;

    /// Terminates the session of `id`; unknown ids are ignored.
    virtual void terminate(const std::string& id) = 0;
};

}  // namespace v2::game
