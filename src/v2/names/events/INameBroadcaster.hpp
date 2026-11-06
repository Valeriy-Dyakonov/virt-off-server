#pragma once

#include <string>

namespace v2::names {

/// Tells every connected client a player's name (PLAYER_NAME), including the
/// player itself — that echo confirms the name was accepted.
///
/// Thread-safety: NOT required — accessed only on the transport strand.
struct INameBroadcaster {
    virtual ~INameBroadcaster() = default;

    /// Broadcasts PLAYER_NAME [id][name] to all users.
    virtual void announce(const std::string& id, const std::string& name) = 0;
};

}  // namespace v2::names
