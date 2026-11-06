#include "v2/app/ServerBuilder.hpp"

#include "core/ConsoleLogger.hpp"
#include "net/PacketRouter.hpp"
#include "net/UdpTransport.hpp"
#include "v2/app/Server.hpp"
#include "v2/clients/UserRepository.hpp"
#include "v2/game/combat/CombatModule.hpp"
#include "v2/game/players/PlayerModule.hpp"
#include "v2/game/props/PropModule.hpp"
#include "v2/game/session/SessionModule.hpp"
#include "v2/names/NameModule.hpp"
#include "v2/net/broadcast/Broadcaster.hpp"
#include "v2/net/wire/SenderStamp.hpp"
#include "v2/relay/RelayModule.hpp"

#include <memory>

namespace v2 {

ServerBuilder::ServerBuilder(asio::io_context& io_ctx, uint16_t port) : io_ctx_(io_ctx), port_(port) {}

std::unique_ptr<IServer> ServerBuilder::build() {
    ServerComponents c;
    c.logger = std::make_unique<ConsoleLogger>();
    c.users = std::make_unique<UserRepository>();
    c.transport = std::make_unique<UdpTransport>(io_ctx_, *c.logger);
    c.router = std::make_unique<PacketRouter>();
    c.broadcaster = std::make_unique<net::Broadcaster>(*c.users, *c.transport);
    c.sender_stamp = std::make_unique<net::SenderStamp>();

    ILogger& logger = *c.logger;
    IUserRepository& users = *c.users;
    net::IBroadcaster& broadcaster = *c.broadcaster;
    const net::ISenderStamp& stamp = *c.sender_stamp;

    auto players = std::make_unique<game::PlayerModule>(users);
    auto props = std::make_unique<game::PropModule>(users, players->state(), stamp, broadcaster, logger);
    auto session =
        std::make_unique<game::SessionModule>(users, players->state(), props->releaser(), broadcaster, logger);
    auto combat = std::make_unique<game::CombatModule>(users, players->state(), stamp, broadcaster, logger);

    // Dependencies first: modules are destroyed in reverse order.
    c.modules.push_back(std::move(players));
    c.modules.push_back(std::move(props));
    c.modules.push_back(std::move(session));
    c.modules.push_back(std::move(combat));
    c.modules.push_back(std::make_unique<names::NameModule>(users, broadcaster, logger));
    c.modules.push_back(std::make_unique<relay::RelayModule>(users, stamp, broadcaster, logger));

    return std::make_unique<Server>(std::move(c), port_);
}

}  // namespace v2
