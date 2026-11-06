#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace v2::game {

/// Physics ownership of a prop that nobody holds.
///
/// After a throw, push or hit the client that moved the prop simulates it and
/// sends PROP_STATE until it settles; the server only remembers who that is.
/// The owner is PropRecord::owner_id with PropRecord::held == false.
///
/// Thread-safety: NOT required — accessed only on the transport strand.
struct IPropSimulation {
    virtual ~IPropSimulation() = default;

    /// Gives the simulation of `prop_id` to `owner_id` if the prop is free or
    /// `owner_id` already simulates it, and records `now` as its last activity.
    /// Returns false if the prop is unknown, held, or simulated by another player.
    [[nodiscard]] virtual bool try_simulate(uint32_t prop_id, const std::string& owner_id,
                              std::chrono::steady_clock::time_point now) = 0;

    /// The holder `owner_id` threw or dropped `prop_id`: it stays the owner and
    /// keeps simulating, `held` becomes false, `now` is the last activity.
    /// Returns false if `owner_id` is not the owner (or the prop is unknown).
    [[nodiscard]] virtual bool begin_simulation(uint32_t prop_id, const std::string& owner_id,
                                  std::chrono::steady_clock::time_point now) = 0;

    /// The prop settled: clears the owner if it is `owner_id`.
    /// Returns false if `owner_id` is not the owner (or the prop is unknown).
    [[nodiscard]] virtual bool end_simulation(uint32_t prop_id, const std::string& owner_id) = 0;

    /// Records `now` as the last activity of the owner of `prop_id`. Unknown ids are ignored.
    virtual void touch(uint32_t prop_id, std::chrono::steady_clock::time_point now) = 0;

    /// Frees every owned prop whose last activity is older than `now - timeout`
    /// (a lost settle packet or a frozen client). Returns the ids of the freed props.
    [[nodiscard]] virtual std::vector<uint32_t> expire(std::chrono::steady_clock::time_point now,
                                         std::chrono::milliseconds timeout) = 0;
};

}  // namespace v2::game
