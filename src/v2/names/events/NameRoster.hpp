#pragma once

#include "v2/names/events/INameRoster.hpp"

#include "v2/clients/IUserRepository.hpp"
#include "v2/names/codec/IPlayerNameEncoder.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"

namespace v2::names {

/// INameRoster over the user repository: one PLAYER_NAME per named user.
class NameRoster final : public INameRoster {
public:
    NameRoster(const IUserRepository& users, const IPlayerNameEncoder& encoder, net::IBroadcaster& broadcaster);

    void send_roster(const std::string& to_id) override;

private:
    const IUserRepository& users_;

    const IPlayerNameEncoder& encoder_;

    net::IBroadcaster& broadcaster_;
};

}  // namespace v2::names
