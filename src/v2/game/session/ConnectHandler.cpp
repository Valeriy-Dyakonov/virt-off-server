#include "v2/game/session/ConnectHandler.hpp"

#include "net/ProtocolConstants.hpp"

#include <chrono>
#include <format>
#include <string>
#include <vector>

namespace v2::game {

ConnectHandler::ConnectHandler(IIdGenerator& ids, IUserRepository& users, net::IBroadcaster& broadcaster,
                               ILogger& logger)
    : ids_(ids), users_(users), broadcaster_(broadcaster), logger_(logger) {}

void ConnectHandler::handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t>) {
    const std::string id = ids_.next();
    users_.add({.id = id, .endpoint = from, .last_seen = std::chrono::steady_clock::now()});
    logger_.info(std::format("[Session] {} connected", id));

    const std::vector<uint8_t> raw_id(id.begin(), id.end());
    broadcaster_.send_to(id, protocol::MSG_ASSIGN_ID, raw_id);
}

}  // namespace v2::game
