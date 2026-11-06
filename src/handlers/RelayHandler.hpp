#pragma once

#include "clients/IClientRepository.hpp"
#include "core/ILogger.hpp"
#include "handlers/IRelayHandler.hpp"
#include "net/ITransport.hpp"

/// IRelayHandler for the legacy API: forwards payloads unchanged.
class RelayHandler final : public IRelayHandler {
public:
    RelayHandler(IClientRepository& clients, ITransport& transport, ILogger& logger);

    void relay(uint8_t type, const udp::endpoint& sender, std::span<const uint8_t> data) override;

private:
    IClientRepository& clients_;

    ITransport& transport_;

    ILogger& logger_;
};
