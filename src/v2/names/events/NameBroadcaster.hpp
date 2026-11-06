#pragma once

#include "v2/names/events/INameBroadcaster.hpp"

#include "v2/names/codec/IPlayerNameEncoder.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"

namespace v2::names {

/// INameBroadcaster over IPlayerNameEncoder and IBroadcaster.
class NameBroadcaster final : public INameBroadcaster {
public:
    NameBroadcaster(const IPlayerNameEncoder& encoder, net::IBroadcaster& broadcaster);

    void announce(const std::string& id, const std::string& name) override;

private:
    const IPlayerNameEncoder& encoder_;

    net::IBroadcaster& broadcaster_;
};

}  // namespace v2::names
