#include "v2/game/session/DisconnectHandler.hpp"

namespace v2::game {

DisconnectHandler::DisconnectHandler(const IUserRepository& users, ISessionTerminator& terminator)
    : users_(users), terminator_(terminator) {}

void DisconnectHandler::handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t>) {
    if (const auto sender_id = users_.find_id_by_endpoint(from)) {
        terminator_.terminate(*sender_id);
    }
}

}  // namespace v2::game
