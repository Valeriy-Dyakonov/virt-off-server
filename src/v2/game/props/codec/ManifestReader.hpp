#pragma once

#include "v2/game/props/codec/IManifestReader.hpp"

namespace v2::game {

/// Stateless IManifestReader.
class ManifestReader final : public IManifestReader {
public:
    std::optional<std::vector<PropDescriptor>> read(std::span<const uint8_t> payload) const override;
};

}  // namespace v2::game
