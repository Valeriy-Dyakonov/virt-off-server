#pragma once

#include "v2/clients/IUserRepository.hpp"
#include "v2/game/players/IPlayerState.hpp"
#include "v2/game/players/IPositionReader.hpp"
#include "v2/net/module/IPacketObserver.hpp"

namespace v2::game {

/// Stores the sender position of every POSITION datagram before it is relayed,
/// so attacks and pickups are checked against the latest position.
class PositionTracker final : public net::IPacketObserver {
public:
    PositionTracker(const IUserRepository& users, const IPositionReader& reader, IPlayerState& state);

    void observe(const asio::ip::udp::endpoint& from, std::span<const uint8_t> datagram) override;

private:
    const IUserRepository& users_;

    const IPositionReader& reader_;

    IPlayerState& state_;
};

}  // namespace v2::game
