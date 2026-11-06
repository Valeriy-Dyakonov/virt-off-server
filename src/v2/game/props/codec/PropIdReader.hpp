#pragma once

#include "v2/game/props/codec/IPropIdReader.hpp"

namespace v2::game {

/// Stateless IPropIdReader.
class PropIdReader final : public IPropIdReader {
public:
    [[nodiscard]] std::optional<uint32_t> read(std::span<const uint8_t> payload) const override;
};

}  // namespace v2::game
