#pragma once

#include "v2/game/props/codec/IManifestAckEncoder.hpp"

namespace v2::game {

/// Stateless IManifestAckEncoder.
class ManifestAckEncoder final : public IManifestAckEncoder {
public:
    [[nodiscard]] std::vector<uint8_t> encode(const std::vector<uint32_t>& prop_ids) const override;
};

}  // namespace v2::game
