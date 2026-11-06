#pragma once

#include "v2/game/IPositionReader.hpp"

namespace v2::game {

/// Stateless IPositionReader over the client POSITION layout.
class PositionReader final : public IPositionReader {
public:
    std::optional<std::array<float, 3>> read(std::span<const uint8_t> payload) const override;
};

}  // namespace v2::game
