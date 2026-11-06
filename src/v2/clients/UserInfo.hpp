#pragma once

#include <asio/ip/udp.hpp>
#include <chrono>
#include <string>

namespace v2 {

/// A connected client. Plain data owned by IUserRepository.
struct UserInfo {
    /// Server-assigned id, "player-<millis>[-n]".
    std::string id{};

    /// Sanitized display name; empty until the client sends SET_NAME.
    std::string username{};

    /// Address the client sends from; packets to it go there too.
    asio::ip::udp::endpoint endpoint{};

    /// Time of the last packet from the client (idle timeout).
    std::chrono::steady_clock::time_point last_seen{};
};

}  // namespace v2
