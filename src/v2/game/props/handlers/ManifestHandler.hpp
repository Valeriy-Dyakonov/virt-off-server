#pragma once

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/game/props/codec/IManifestAckEncoder.hpp"
#include "v2/game/props/codec/IManifestReader.hpp"
#include "v2/game/props/registry/IPropRepository.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"
#include "v2/net/module/IPacketHandler.hpp"

namespace v2::game {

/// 0x06 MANIFEST: registers the client's props and replies MANIFEST_ACK with
/// their server ids.
class ManifestHandler final : public net::IPacketHandler {
public:
    ManifestHandler(const IUserRepository& users,
                    const IManifestReader& reader,
                    IPropRepository& props,
                    const IManifestAckEncoder& encoder,
                    net::IBroadcaster& broadcaster,
                    ILogger& logger);

    void handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t> payload) override;

private:
    const IUserRepository& users_;

    const IManifestReader& reader_;

    IPropRepository& props_;

    const IManifestAckEncoder& encoder_;

    net::IBroadcaster& broadcaster_;

    ILogger& logger_;
};

}  // namespace v2::game
