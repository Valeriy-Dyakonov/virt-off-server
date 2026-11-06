#pragma once

#include "v2/game/props/codec/IPropStateReader.hpp"

namespace v2::game {

/// Stateless IPropStateReader.
class PropStateReader final : public IPropStateReader {
public:
    std::optional<PropStateFields> read(std::span<const uint8_t> payload) const override;
};

}  // namespace v2::game
