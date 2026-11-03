#pragma once

#include "v2/game/ISessionMonitor.hpp"

#include "v2/game/ISessionTerminator.hpp"
#include "v2/clients/IUserRepository.hpp"

#include <chrono>
#include <string>

namespace v2::game {

/// Session liveness tracker.
///
/// mark_active is called on every incoming packet. sweep hands idle sessions
/// to ISessionTerminator.
///
/// Thread-safety: NOT required — accessed only on the transport strand.
class SessionMonitor final : public ISessionMonitor {
public:
    SessionMonitor(IUserRepository& users, ISessionTerminator& terminator);

    void mark_active(const std::string& id) override;

    void sweep(std::chrono::system_clock::time_point now) override;

private:
    IUserRepository& users_;

    ISessionTerminator& terminator_;
};

}  // namespace v2::game
