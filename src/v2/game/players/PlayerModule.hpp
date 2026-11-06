#pragma once

#include "v2/clients/IUserRepository.hpp"
#include "v2/game/players/IPlayerState.hpp"
#include "v2/game/players/IPositionReader.hpp"
#include "v2/net/module/IPacketObserver.hpp"
#include "v2/net/module/IServerModule.hpp"

#include <memory>

namespace v2::game {

/// Player positions: remembers where every player is from POSITION packets.
class PlayerModule final : public net::IServerModule {
public:
    explicit PlayerModule(const IUserRepository& users);

    net::ModuleBindings bindings() override;

    /// Positions read by the props and combat modules.
    [[nodiscard]] IPlayerState& state() const;

private:
    std::unique_ptr<IPlayerState> state_;

    std::unique_ptr<IPositionReader> reader_;

    std::unique_ptr<net::IPacketObserver> tracker_;
};

}  // namespace v2::game
