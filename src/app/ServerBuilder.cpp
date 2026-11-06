#include "app/ServerBuilder.hpp"

#include "app/Server.hpp"
#include "clients/ClientRepository.hpp"
#include "core/ConsoleLogger.hpp"
#include "handlers/ConnectHandler.hpp"
#include "handlers/RelayHandler.hpp"
#include "net/PacketRouter.hpp"
#include "net/UdpTransport.hpp"

#include <memory>

ServerBuilder::ServerBuilder(asio::io_context& io_ctx, const uint16_t port) : io_ctx_(io_ctx), port_(port) {}

std::unique_ptr<IServer> ServerBuilder::build() {
    auto logger = std::make_unique<ConsoleLogger>();
    auto clients = std::make_unique<ClientRepository>();
    auto transport = std::make_unique<UdpTransport>(io_ctx_, *logger);
    auto connect = std::make_unique<ConnectHandler>(*clients, *transport, *logger);
    auto relay = std::make_unique<RelayHandler>(*clients, *transport, *logger);

    ServerDependencies deps{
        .logger = std::move(logger),
        .client_repository = std::move(clients),
        .transport = std::move(transport),
        .router = std::make_unique<PacketRouter>(),
        .connect_handler = std::move(connect),
        .relay_handler = std::move(relay),
    };
    return std::make_unique<Server>(std::move(deps), port_);
}
