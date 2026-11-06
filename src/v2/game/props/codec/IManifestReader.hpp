#pragma once

#include "v2/game/props/model/PropDescriptor.hpp"

#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace v2::game {

/// Parses MANIFEST: [sender:str][count:2]{[key:str][halfExtents:12][mass:4]}*
/// (without the type byte).
///
/// Thread-safety: REQUIRED — stateless.
struct IManifestReader {
    virtual ~IManifestReader() = default;

    /// Returns the descriptors up to the first truncated one, or nullopt if
    /// even the header is truncated.
    [[nodiscard]] virtual std::optional<std::vector<PropDescriptor>> read(std::span<const uint8_t> payload) const = 0;
};

}  // namespace v2::game
