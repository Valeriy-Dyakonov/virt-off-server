#pragma once

#include <cstdint>
#include <vector>

namespace v2::game {

/// Builds MANIFEST_ACK: [count:2][propId:4]* (without the type byte).
///
/// Thread-safety: REQUIRED — stateless.
struct IManifestAckEncoder {
    virtual ~IManifestAckEncoder() = default;

    /// `prop_ids` in the order of the client manifest.
    [[nodiscard]] virtual std::vector<uint8_t> encode(const std::vector<uint32_t>& prop_ids) const = 0;
};

}  // namespace v2::game
