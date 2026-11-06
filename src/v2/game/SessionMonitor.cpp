#include "v2/game/SessionMonitor.hpp"
#include "v2/game/GameConstants.hpp"

#include <vector>

namespace v2::game {

SessionMonitor::SessionMonitor(IUserRepository& users, ISessionTerminator& terminator)
    : users_(users), terminator_(terminator) {}

void SessionMonitor::mark_active(const std::string& id) {
    users_.touch(id, std::chrono::system_clock::now());
}

void SessionMonitor::sweep(std::chrono::system_clock::time_point now) {
    const auto timeout = std::chrono::milliseconds(kSessionTimeoutMs);

    // Collect idle users first: terminate() mutates the repository.
    std::vector<std::string> idle_ids;
    users_.for_each([&](const UserInfo& user) {
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - user.last_seen);
        if (elapsed > timeout) {
            idle_ids.push_back(user.id);
        }
    });

    for (const auto& id : idle_ids) {
        terminator_.terminate(id);
    }
}

}  // namespace v2::game
