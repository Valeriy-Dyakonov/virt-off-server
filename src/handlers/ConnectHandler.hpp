#pragma once

#include "clients/IClientRepository.hpp"
#include "core/ILogger.hpp"
#include "handlers/IConnectHandler.hpp"
#include "net/ITransport.hpp"

/// IConnectHandler for the legacy API: registers the endpoint and replies
/// ASSIGN_ID with the id the repository assigned.
class ConnectHandler final : public IConnectHandler {
public:
    ConnectHandler(IClientRepository& clients, ITransport& transport, ILogger& logger);

    void handle(const udp::endpoint& client_endpoint) override;

private:
    IClientRepository& clients_;

    ITransport& transport_;

    ILogger& logger_;
};
