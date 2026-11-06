#include "v2/game/props/handlers/ManifestHandler.hpp"

#include "net/ProtocolConstants.hpp"

#include <format>

namespace v2::game {

ManifestHandler::ManifestHandler(const IUserRepository& users,
                                 const IManifestReader& reader,
                                 IPropRepository& props,
                                 const IManifestAckEncoder& encoder,
                                 net::IBroadcaster& broadcaster,
                                 ILogger& logger)
    : users_(users), reader_(reader), props_(props), encoder_(encoder), broadcaster_(broadcaster), logger_(logger) {}

void ManifestHandler::handle(const asio::ip::udp::endpoint& from, const std::span<const uint8_t> payload) {
    const auto sender_id = users_.find_id_by_endpoint(from);
    if (!sender_id.has_value()) {
        return;
    }

    const auto descriptors = reader_.read(payload);
    if (!descriptors.has_value()) {
        logger_.warn(std::format("[Manifest] from={} truncated header", *sender_id));
        return;
    }

    const auto prop_ids = props_.register_manifest(*descriptors);
    logger_.info(std::format("[Manifest] from={} props={}", *sender_id, prop_ids.size()));
    broadcaster_.send_to(*sender_id, protocol::MSG_MANIFEST_ACK, encoder_.encode(prop_ids));
}

}  // namespace v2::game
