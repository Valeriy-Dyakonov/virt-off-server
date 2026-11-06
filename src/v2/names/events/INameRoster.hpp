#pragma once

#include <string>

namespace v2::names {

/// Sends one player the names of everybody else, so a newcomer sees names
/// that were announced before it joined.
///
/// Thread-safety: NOT required — accessed only on the transport strand.
struct INameRoster {
    virtual ~INameRoster() = default;

    /// Sends `to_id` one PLAYER_NAME per other user that has a name. Unknown `to_id`: no-op.
    virtual void send_roster(const std::string& to_id) = 0;
};

}  // namespace v2::names
