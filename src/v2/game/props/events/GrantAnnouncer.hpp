#pragma once

#include "v2/game/props/events/IGrantAnnouncer.hpp"

#include "v2/net/broadcast/IBroadcaster.hpp"

namespace v2::game {

/// IGrantAnnouncer over IBroadcaster.
class GrantAnnouncer final : public IGrantAnnouncer {
public:
    explicit GrantAnnouncer(net::IBroadcaster& broadcaster);

    void announce(uint32_t prop_id, const std::string& holder_id, std::byte flags) override;

private:
    net::IBroadcaster& broadcaster_;
};

}  // namespace v2::game
