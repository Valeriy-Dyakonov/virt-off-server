#pragma once

#include "core/ILogger.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/names/codec/IPlayerNameReader.hpp"
#include "v2/names/codec/IPlayerNameSanitizer.hpp"
#include "v2/names/events/INameBroadcaster.hpp"
#include "v2/names/events/INameRoster.hpp"
#include "v2/net/module/IPacketHandler.hpp"

namespace v2::names {

/// 0x10 SET_NAME: stores the sanitized name, announces it to everyone and
/// sends the author the names of the others. Clients resend SET_NAME
/// periodically (UDP is lossy), so every call does all of it again.
class NameHandler final : public net::IPacketHandler {
public:
    NameHandler(IUserRepository& users,
                const IPlayerNameReader& reader,
                const IPlayerNameSanitizer& sanitizer,
                INameBroadcaster& broadcaster,
                INameRoster& roster,
                ILogger& logger);

    void handle(const asio::ip::udp::endpoint& from, std::span<const uint8_t> payload) override;

private:
    IUserRepository& users_;

    const IPlayerNameReader& reader_;

    const IPlayerNameSanitizer& sanitizer_;

    INameBroadcaster& broadcaster_;

    INameRoster& roster_;

    ILogger& logger_;
};

}  // namespace v2::names
