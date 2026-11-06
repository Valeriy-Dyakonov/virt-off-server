#include "v2/game/session/ActivityTracker.hpp"

#include <chrono>

namespace v2::game {

ActivityTracker::ActivityTracker(IUserRepository& users) : users_(users) {}

void ActivityTracker::observe(const asio::ip::udp::endpoint& from, std::span<const uint8_t>) {
    if (const auto sender_id = users_.find_id_by_endpoint(from)) {
        users_.touch(*sender_id, std::chrono::steady_clock::now());
    }
}

}  // namespace v2::game
