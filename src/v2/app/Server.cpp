#include "v2/app/Server.hpp"

#include "net/ProtocolConstants.hpp"
#include "v2/game/GameConstants.hpp"
#include "v2/game/WireSizes.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace v2 {

Server::Server(ServerDependencies deps, uint16_t port)
    : io_ctx_(std::move(deps.io_ctx))
    , id_generator_(std::move(deps.id_generator))
    , user_repository_(std::move(deps.user_repository))
    , transport_(std::move(deps.transport))
    , router_(std::move(deps.router))
    , connect_handler_(std::move(deps.connect_handler))
    , relay_handler_(std::move(deps.relay_handler))
    , sender_stamp_(std::move(deps.sender_stamp))
    , game_orchestrator_(std::move(deps.game_orchestrator))
    , port_(port) {
    logger_->info("Server (v2) object created, port=" + std::to_string(port_));
}

Server::~Server() {
    Stop();
}

void Server::Start() {
    logger_->info("Starting server (v2) on port " + std::to_string(port_));

    auto local = udp::endpoint(asio::ip::udp::v4(), port_);
    transport_->start(local);

    router_->register_handler(
        protocol::MSG_CONNECT,
        [this](const udp::endpoint& from, std::span<const uint8_t> /*payload*/) { connect_handler_->handle(from); });

    router_->register_handler(protocol::MSG_ASSIGN_ID,
                              [](const udp::endpoint& /*from*/, std::span<const uint8_t> /*payload*/) {});

    router_->register_handler(protocol::MSG_VOICE, [this](const udp::endpoint& from, std::span<const uint8_t> payload) {
        relay_handler_->relay(protocol::MSG_VOICE, from, payload);
    });

    router_->register_handler(protocol::MSG_POSITION,
                              [this](const udp::endpoint& from, std::span<const uint8_t> payload) {
                                  relay_handler_->relay(protocol::MSG_POSITION, from, payload);
                              });

    // -- Game handlers --

    router_->register_handler(protocol::MSG_ATTACK,
                              [this](const udp::endpoint& from, std::span<const uint8_t> payload) {
                                  game_orchestrator_->attack_handler().handle(from, payload);
                              });

    router_->register_handler(protocol::MSG_PROP_STATE,
                              [this](const udp::endpoint& from, std::span<const uint8_t> payload) {
                                  game_orchestrator_->prop_state_handler().handle(from, payload);
                              });

    router_->register_handler(protocol::MSG_SOUND,
                              [this](const udp::endpoint& from, std::span<const uint8_t> payload) {
                                  relay_handler_->relay(protocol::MSG_SOUND, from, payload);
                              });

    router_->register_handler(protocol::MSG_MANIFEST,
                              [this](const udp::endpoint& from, std::span<const uint8_t> payload) {
                                  game_orchestrator_->manifest_handler().handle(from, payload);
                              });

    router_->register_handler(protocol::MSG_PICKUP_REQ,
                              [this](const udp::endpoint& from, std::span<const uint8_t> payload) {
                                  game_orchestrator_->pickup_handler().handle(from, payload);
                              });

    router_->register_handler(protocol::MSG_PROP_RELEASE,
                              [this](const udp::endpoint& from, std::span<const uint8_t> payload) {
                                  game_orchestrator_->release_handler().handle(from, payload);
                              });

    router_->register_handler(protocol::MSG_DISCONNECT,
                              [this](const udp::endpoint& from, std::span<const uint8_t> /*payload*/) {
                                  const auto sender_id = user_repository_->find_id_by_endpoint(from);
                                  if (!sender_id.has_value()) {
                                      return;
                                  }
                                  logger_->info("[Server] DISCONNECT " + *sender_id);
                                  game_orchestrator_->session_terminator().terminate(*sender_id);
                              });

    // Receive wrapper: mark_active + player_state update for POSITION.
    transport_->set_receive_callback(
        [this](udp::endpoint from, std::vector<uint8_t> data) {
            if (!data.empty()) {
                const auto sender_id = user_repository_->find_id_by_endpoint(from);
                if (sender_id.has_value()) {
                    game_orchestrator_->session_monitor().mark_active(*sender_id);

                    // For POSITION packets, update player state before routing:
                    // attacks and pickups are validated against it.
                    if (data[0] == protocol::MSG_POSITION) {
                        const auto payload = std::span<const uint8_t>(data).subspan(1);
                        if (const auto position = game_orchestrator_->position_reader().read(payload)) {
                            game::PlayerTransform transform;
                            std::copy(position->begin(), position->end(), transform.position);
                            game_orchestrator_->player_state().update(*sender_id, transform);
                        }
                    }
                }
            }
            router_->route(from, data);
        });

    // Tick callback: sweep idle sessions.
    transport_->set_tick_callback(
        [this]() {
            const auto before = user_repository_->size();
            game_orchestrator_->session_monitor().sweep(std::chrono::system_clock::now());
            const auto after = user_repository_->size();
            if (before != after) {
                logger_->info("[Server] sweep: " + std::to_string(before - after) + " idle sessions dropped");
            }
        },
        game::kSweepIntervalMs);

    logger_->info("Server (v2) is running");
}

void Server::Stop() noexcept {
    if (transport_) {
        transport_->stop();
    }

    if (io_ctx_ && !io_ctx_->stopped()) {
        io_ctx_->stop();
    }
}

}  // namespace v2
