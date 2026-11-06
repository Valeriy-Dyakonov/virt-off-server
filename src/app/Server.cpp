#include "app/Server.hpp"

#include "net/ProtocolConstants.hpp"

#include <format>
#include <string>
#include <utility>

Server::Server(ServerDependencies deps, uint16_t port) : deps_(std::move(deps)), port_(port) {}

Server::~Server() {
    Stop();
}

void Server::Start() {
    deps_.logger->info(std::format("Starting server on port {}", port_));
    deps_.transport->start(udp::endpoint(udp::v4(), port_));

    deps_.router->register_handler(protocol::MSG_CONNECT,
                                   [this](const udp::endpoint& from, std::span<const uint8_t>) {
                                       deps_.connect_handler->handle(from);
                                   });

    for (const uint8_t type : {protocol::MSG_VOICE, protocol::MSG_POSITION}) {
        deps_.router->register_handler(type, [this, type](const udp::endpoint& from, std::span<const uint8_t> payload) {
            deps_.relay_handler->relay(type, from, payload);
        });
    }

    deps_.transport->set_receive_callback(
        [this](const udp::endpoint& from, const std::vector<uint8_t>& data) { deps_.router->route(from, data); });

    deps_.logger->info("Server is running");
}

void Server::Stop() noexcept {
    if (deps_.transport) {
        deps_.transport->stop();
    }
}
