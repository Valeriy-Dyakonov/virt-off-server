#pragma once

#include "v2/game/players/IPositionReader.hpp"

namespace v2::game {

/// Stateless IPositionReader.
class PositionReader final : public IPositionReader {
public:
    [[nodiscard]] std::optional<Vec3> read(std::span<const uint8_t> payload) const override;
};

}  // namespace v2::game
