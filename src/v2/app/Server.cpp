#include "v2/app/Server.hpp"

#include "v2/game/GameConstants.hpp"

#include <format>
#include <string>
#include <utility>

namespace v2 {

Server::Server(ServerComponents components, uint16_t port) : components_(std::move(components)), port_(port) {}

Server::~Server() {
    Stop();
}

void Server::Start() {
    components_.logger->info(std::format("Starting server (v2) on port {}", port_));

    for (const auto& module : components_.modules) {
        auto bindings = module->bindings();
        for (const auto& route : bindings.routes) {
            components_.router->register_handler(
                route.type, [&handler = route.handler](const udp::endpoint& from, std::span<const uint8_t> payload) {
                    handler.handle(from, payload);
                });
        }
        observers_.insert(observers_.end(), bindings.observers.begin(), bindings.observers.end());
        tasks_.insert(tasks_.end(), bindings.tasks.begin(), bindings.tasks.end());
    }

    components_.transport->set_receive_callback(
        [this](const udp::endpoint& from, const std::vector<uint8_t>& datagram) { on_datagram(from, datagram); });
    components_.transport->set_tick_callback([this] { on_tick(); },
                                             static_cast<unsigned int>(game::kSweepInterval.count()));
    components_.transport->start(udp::endpoint(udp::v4(), port_));

    components_.logger->info("Server (v2) is running");
}

void Server::Stop() noexcept {
    if (components_.transport) {
        components_.transport->stop();
    }
}

void Server::on_datagram(const udp::endpoint& from, const std::vector<uint8_t>& datagram) {
    if (datagram.empty()) {
        return;
    }
    for (net::IPacketObserver& observer : observers_) {
        observer.observe(from, datagram);
    }
    components_.router->route(from, datagram);
}

void Server::on_tick() {
    for (net::IPeriodicTask& task : tasks_) {
        task.run();
    }
}

}  // namespace v2
