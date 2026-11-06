#include "v2/relay/RelayModule.hpp"

#include "net/ProtocolConstants.hpp"
#include "v2/relay/RelayHandler.hpp"

namespace v2::relay {

RelayModule::RelayModule(const IUserRepository& users,
                         const net::ISenderStamp& stamp,
                         net::IBroadcaster& broadcaster,
                         ILogger& logger)
    : voice_(std::make_unique<RelayHandler>(protocol::MSG_VOICE, users, stamp, broadcaster, logger))
    , position_(std::make_unique<RelayHandler>(protocol::MSG_POSITION, users, stamp, broadcaster, logger))
    , sound_(std::make_unique<RelayHandler>(protocol::MSG_SOUND, users, stamp, broadcaster, logger)) {}

net::ModuleBindings RelayModule::bindings() {
    return {.routes = {{protocol::MSG_VOICE, *voice_},
                       {protocol::MSG_POSITION, *position_},
                       {protocol::MSG_SOUND, *sound_}}};
}

}  // namespace v2::relay
