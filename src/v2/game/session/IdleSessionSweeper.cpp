#include "v2/game/session/IdleSessionSweeper.hpp"

#include "v2/game/GameConstants.hpp"

#include <chrono>
#include <format>
#include <string>
#include <vector>

namespace v2::game {

IdleSessionSweeper::IdleSessionSweeper(const IUserRepository& users, ISessionTerminator& terminator,
                                       ILogger& logger)
    : users_(users), terminator_(terminator), logger_(logger) {}

void IdleSessionSweeper::run() {
    const auto now = std::chrono::steady_clock::now();

    // Collected first: terminate() removes users from the repository.
    std::vector<std::string> idle_ids;
    users_.for_each([&idle_ids, now](const UserInfo& user) {
        if (now - user.last_seen > kSessionTimeout) {
            idle_ids.push_back(user.id);
        }
    });

    for (const auto& id : idle_ids) {
        logger_.info(std::format("[Session] {} timed out", id));
        terminator_.terminate(id);
    }
}

}  // namespace v2::game
