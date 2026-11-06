#pragma once

#include "v2/game/combat/IAttackTargetReader.hpp"

namespace v2::game {

/// Stateless IAttackTargetReader.
class AttackTargetReader final : public IAttackTargetReader {
public:
    std::optional<std::string> read(std::span<const uint8_t> payload) const override;
};

}  // namespace v2::game
