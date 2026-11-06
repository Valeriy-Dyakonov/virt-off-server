#include "v2/relay/RelayHandler.hpp"

#include <format>

namespace v2::relay {

RelayHandler::RelayHandler(const uint8_t type,
                           const IUserRepository& users,
                           const net::ISenderStamp& stamp,
                           net::IBroadcaster& broadcaster,
                           ILogger& logger)
    : type_(type), users_(users), stamp_(stamp), broadcaster_(broadcaster), logger_(logger) {}

void RelayHandler::handle(const asio::ip::udp::endpoint& from, const std::span<const uint8_t> payload) {
    const auto sender_id = users_.find_id_by_endpoint(from);
    if (!sender_id.has_value()) {
        logger_.warn(std::format("[Relay] type={:#04x} from unknown sender, dropped", type_));
        return;
    }

    const auto stamped = stamp_.stamp(payload, *sender_id);
    if (!stamped.has_value()) {
        logger_.warn(std::format("[Relay] type={:#04x} from={} without sender id, dropped", type_, *sender_id));
        return;
    }

    broadcaster_.broadcast_except(*sender_id, type_, *stamped);
}

}  // namespace v2::relay
