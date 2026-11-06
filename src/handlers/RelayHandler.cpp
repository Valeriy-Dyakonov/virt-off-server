#include "handlers/RelayHandler.hpp"

#include <vector>

RelayHandler::RelayHandler(IClientRepository& clients, ITransport& transport, ILogger& logger)
    : clients_(clients), transport_(transport), logger_(logger) {}

void RelayHandler::relay(uint8_t type, const udp::endpoint& sender, std::span<const uint8_t> data) {
    const auto sender_id = clients_.find_by_endpoint(sender);
    if (!sender_id.has_value()) {
        logger_.warn("Relay from unknown sender, dropping");
        return;
    }

    std::vector<uint8_t> outgoing;
    outgoing.reserve(1 + data.size());
    outgoing.push_back(type);
    outgoing.insert(outgoing.end(), data.begin(), data.end());

    clients_.for_each_except(*sender_id,
                             [this, &outgoing](uint64_t, const udp::endpoint& ep) { transport_.send_to(ep, outgoing); });
}
