#pragma once

#include "v2/game/props/events/IPropReleaser.hpp"

#include "v2/game/props/codec/IReleaseEventEncoder.hpp"
#include "v2/game/props/registry/IPropRepository.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"

namespace v2::game {

/// IPropReleaser over the repository, the release event encoder and the broadcaster.
class PropReleaser final : public IPropReleaser {
public:
    PropReleaser(IPropRepository& props, const IReleaseEventEncoder& encoder, net::IBroadcaster& broadcaster);

    void release_all_of(const std::string& owner_id) override;

private:
    IPropRepository& props_;

    const IReleaseEventEncoder& encoder_;

    net::IBroadcaster& broadcaster_;
};

}  // namespace v2::game
