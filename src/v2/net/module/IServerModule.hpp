#pragma once

#include "v2/net/module/IPacketObserver.hpp"
#include "v2/net/module/IPeriodicTask.hpp"
#include "v2/net/module/PacketRoute.hpp"

#include <functional>
#include <vector>

namespace v2::net {

/// What a feature module plugs into the server.
struct ModuleBindings {
    /// Message types the module handles.
    std::vector<PacketRoute> routes{};

    /// Observers of every received datagram, run before routing.
    std::vector<std::reference_wrapper<IPacketObserver>> observers{};

    /// Housekeeping run on every tick.
    std::vector<std::reference_wrapper<IPeriodicTask>> tasks{};
};

/// A feature of the server (relay, props, names…): owns its components and
/// exposes them to the server through bindings().
///
/// Thread-safety: NOT required — bindings() is called once, before start.
struct IServerModule {
    virtual ~IServerModule() = default;

    /// References stay valid for the lifetime of the module.
    virtual ModuleBindings bindings() = 0;
};

}  // namespace v2::net
