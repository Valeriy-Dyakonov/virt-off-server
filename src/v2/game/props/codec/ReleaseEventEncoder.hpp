#pragma once

#include "v2/game/props/codec/IReleaseEventEncoder.hpp"

namespace v2::game {

/// Stateless IReleaseEventEncoder.
class ReleaseEventEncoder final : public IReleaseEventEncoder {
public:
    [[nodiscard]] std::optional<std::vector<uint8_t>> encode_throw(std::span<const uint8_t> release_payload,
                                                     const std::string& owner_id) const override;

    [[nodiscard]] std::vector<uint8_t> encode_free(uint32_t prop_id) const override;
};

}  // namespace v2::game
