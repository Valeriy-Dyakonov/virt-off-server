#include "v2/game/players/PlayerModule.hpp"

#include "v2/game/players/PlayerState.hpp"
#include "v2/game/players/PositionReader.hpp"
#include "v2/game/players/PositionTracker.hpp"

namespace v2::game {

PlayerModule::PlayerModule(const IUserRepository& users)
    : state_(std::make_unique<PlayerState>())
    , reader_(std::make_unique<PositionReader>())
    , tracker_(std::make_unique<PositionTracker>(users, *reader_, *state_)) {}

net::ModuleBindings PlayerModule::bindings() {
    return {.observers = {*tracker_}};
}

IPlayerState& PlayerModule::state() {
    return *state_;
}

}  // namespace v2::game
