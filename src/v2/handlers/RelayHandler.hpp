#pragma once

#include "core/ConsoleLogger.hpp"
#include "net/ITransport.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/handlers/IRelayHandler.hpp"
#include "v2/handlers/ISenderStamp.hpp"

namespace v2 {

/// Relays voice/position/sound packets between users.
///
/// Resolves the sender via IUserRepository, replaces the sender id in the
/// payload with it (ISenderStamp) and sends to everyone else.
class RelayHandler final : public IRelayHandler {
public:
    RelayHandler(IUserRepository& users, ITransport& transport, const ISenderStamp& stamp);

    void relay(uint8_t type, const udp::endpoint& sender, std::span<const uint8_t> data) override;

private:
    IUserRepository& users_;

    ITransport& transport_;

    const ISenderStamp& stamp_;

    static inline ILogger* logger_ = new ConsoleLogger();
};

}  // namespace v2
