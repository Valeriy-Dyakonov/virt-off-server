#include "v2/game/combat/AttackHandler.hpp"

#include "net/ProtocolConstants.hpp"

#include <chrono>
#include <format>

namespace v2::game {

AttackHandler::AttackHandler(const IUserRepository& users,
                             const IAttackTargetReader& reader,
                             IAttackGate& gate,
                             const net::ISenderStamp& stamp,
                             net::IBroadcaster& broadcaster,
                             ILogger& logger)
    : users_(users), reader_(reader), gate_(gate), stamp_(stamp), broadcaster_(broadcaster), logger_(logger) {}

void AttackHandler::handle(const asio::ip::udp::endpoint& from, const std::span<const uint8_t> payload) {
    const auto sender_id = users_.find_id_by_endpoint(from);
    const auto target_id = reader_.read(payload);
    const auto stamped = sender_id.has_value() ? stamp_.stamp(payload, *sender_id) : std::nullopt;
    if (!target_id.has_value() || !stamped.has_value()) {
        return;
    }

    if (!gate_.allow_attack(*sender_id, *target_id, std::chrono::steady_clock::now())) {
        logger_.info(std::format("[Attack] from={} target={} blocked (cooldown/distance)", *sender_id, *target_id));
        return;
    }

    logger_.info(std::format("[Attack] from={} target={}", *sender_id, *target_id));
    broadcaster_.broadcast(protocol::MSG_ATTACK, *stamped);
}

}  // namespace v2::game
