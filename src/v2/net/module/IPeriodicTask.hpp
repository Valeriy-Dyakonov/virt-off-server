#pragma once

namespace v2::net {

/// Housekeeping run on every transport tick (game::kSweepInterval).
///
/// Thread-safety: NOT required — called on the transport strand only.
struct IPeriodicTask {
    virtual ~IPeriodicTask() = default;

    virtual void run() = 0;
};

}  // namespace v2::net
