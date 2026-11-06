#pragma once

#include "app/IServer.hpp"
#include "clients/IClientRepository.hpp"
#include "core/ILogger.hpp"
#include "handlers/IConnectHandler.hpp"
#include "handlers/IRelayHandler.hpp"
#include "net/IPacketRouter.hpp"
#include "net/ITransport.hpp"

#include <cstdint>
#include <memory>

/// Everything the legacy server owns. Members are destroyed in reverse order,
/// so dependents are declared after their dependencies.
struct ServerDependencies {
    std::unique_ptr<ILogger> logger;

    std::unique_ptr<IClientRepository> client_repository;

    std::unique_ptr<ITransport> transport;

    std::unique_ptr<IPacketRouter> router;

    std::unique_ptr<IConnectHandler> connect_handler;

    std::unique_ptr<IRelayHandler> relay_handler;
};

/// Legacy (V1) server: numeric ids, relays VOICE and POSITION as-is.
class Server final : public IServer {
public:
    Server(ServerDependencies deps, uint16_t port);

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    ~Server() override;

    void Start() override;

    void Stop() noexcept override;

private:
    ServerDependencies deps_;

    uint16_t port_;
};
