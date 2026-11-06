#pragma once

#include "v2/handlers/ISenderStamp.hpp"

namespace v2 {

/// Stateless ISenderStamp over the [len:1][utf8…] string layout.
class SenderStamp final : public ISenderStamp {
public:
    std::optional<std::vector<uint8_t>> stamp(std::span<const uint8_t> payload,
                                              const std::string& sender_id) const override;
};

}  // namespace v2
