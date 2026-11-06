#pragma once

#include "app/IServer.hpp"
#include "core/ILogger.hpp"
#include "net/IPacketRouter.hpp"
#include "net/ITransport.hpp"
#include "v2/clients/IUserRepository.hpp"
#include "v2/net/broadcast/IBroadcaster.hpp"
#include "v2/net/module/IPacketObserver.hpp"
#include "v2/net/module/IPeriodicTask.hpp"
#include "v2/net/module/IServerModule.hpp"
#include "v2/net/wire/ISenderStamp.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace v2 {

/// Everything the V2 server owns. Members are destroyed in reverse order, so
/// each one is declared after what it depends on; modules depend on the
/// shared services and on earlier modules.
struct ServerComponents {
    std::unique_ptr<ILogger> logger;

    std::unique_ptr<IUserRepository> users;

    std::unique_ptr<ITransport> transport;

    std::unique_ptr<IPacketRouter> router;

    std::unique_ptr<net::IBroadcaster> broadcaster;

    std::unique_ptr<net::ISenderStamp> sender_stamp;

    std::vector<std::unique_ptr<net::IServerModule>> modules;
};

/// V2 server: plugs the modules' routes, observers and periodic tasks into
/// the transport. Holds no game logic of its own.
class Server final : public IServer {
public:
    Server(ServerComponents components, uint16_t port);

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    ~Server() override;

    void Start() override;

    void Stop() noexcept override;

private:
    /// Runs the observers on `datagram`, then routes it.
    void on_datagram(const udp::endpoint& from, const std::vector<uint8_t>& datagram);

    /// Runs every periodic task.
    void on_tick();

    ServerComponents components_;

    uint16_t port_;

    std::vector<std::reference_wrapper<net::IPacketObserver>> observers_;

    std::vector<std::reference_wrapper<net::IPeriodicTask>> tasks_;
};

}  // namespace v2
