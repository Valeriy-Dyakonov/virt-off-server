#include "v2/names/NameModule.hpp"

#include "net/ProtocolConstants.hpp"
#include "v2/names/codec/PlayerNameEncoder.hpp"
#include "v2/names/codec/PlayerNameReader.hpp"
#include "v2/names/codec/PlayerNameSanitizer.hpp"
#include "v2/names/events/NameBroadcaster.hpp"
#include "v2/names/events/NameRoster.hpp"
#include "v2/names/NameHandler.hpp"

namespace v2::names {

NameModule::NameModule(IUserRepository& users, net::IBroadcaster& broadcaster, ILogger& logger)
    : reader_(std::make_unique<PlayerNameReader>())
    , sanitizer_(std::make_unique<PlayerNameSanitizer>())
    , encoder_(std::make_unique<PlayerNameEncoder>())
    , name_broadcaster_(std::make_unique<NameBroadcaster>(*encoder_, broadcaster))
    , roster_(std::make_unique<NameRoster>(users, *encoder_, broadcaster))
    , handler_(std::make_unique<NameHandler>(users, *reader_, *sanitizer_, *name_broadcaster_, *roster_, logger)) {}

net::ModuleBindings NameModule::bindings() {
    return {.routes = {{protocol::MSG_SET_NAME, *handler_}}};
}

}  // namespace v2::names
