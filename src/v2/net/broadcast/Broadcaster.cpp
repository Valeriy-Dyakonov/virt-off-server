#include "v2/net/broadcast/Broadcaster.hpp"

#include <vector>

namespace v2::net {

namespace {

/// Prepends the type byte to `payload`.
std::vector<uint8_t> make_datagram(uint8_t type, std::span<const uint8_t> payload) {
    std::vector<uint8_t> datagram;
    datagram.reserve(1 + payload.size());
    datagram.push_back(type);
    datagram.insert(datagram.end(), payload.begin(), payload.end());
    return datagram;
}

}  // namespace

Broadcaster::Broadcaster(const IUserRepository& users, ITransport& transport) : users_(users), transport_(transport) {}

void Broadcaster::broadcast(uint8_t type, std::span<const uint8_t> payload) {
    const auto datagram = make_datagram(type, payload);
    users_.for_each([this, &datagram](const UserInfo& user) { transport_.send_to(user.endpoint, datagram); });
}

void Broadcaster::broadcast_except(const std::string& excluded_id, uint8_t type, std::span<const uint8_t> payload) {
    const auto datagram = make_datagram(type, payload);
    users_.for_each_except(excluded_id,
                           [this, &datagram](const UserInfo& user) { transport_.send_to(user.endpoint, datagram); });
}

void Broadcaster::send_to(const std::string& id, uint8_t type, std::span<const uint8_t> payload) {
    if (const auto user = users_.find_by_id(id)) {
        transport_.send_to(user->endpoint, make_datagram(type, payload));
    }
}

}  // namespace v2::net
