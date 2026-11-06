#pragma once

#include "v2/net/broadcast/IBroadcaster.hpp"

#include "net/ITransport.hpp"
#include "v2/clients/IUserRepository.hpp"

namespace v2::net {

/// IBroadcaster over the user registry and the transport.
class Broadcaster final : public IBroadcaster {
public:
    Broadcaster(const IUserRepository& users, ITransport& transport);

    void broadcast(uint8_t type, std::span<const uint8_t> payload) override;

    void broadcast_except(const std::string& excluded_id, uint8_t type, std::span<const uint8_t> payload) override;

    void send_to(const std::string& id, uint8_t type, std::span<const uint8_t> payload) override;

private:
    const IUserRepository& users_;

    ITransport& transport_;
};

}  // namespace v2::net
