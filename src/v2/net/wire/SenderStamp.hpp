#pragma once

#include "v2/net/wire/ISenderStamp.hpp"

namespace v2::net {

/// Stateless ISenderStamp.
class SenderStamp final : public ISenderStamp {
public:
    std::optional<std::vector<uint8_t>> stamp(std::span<const uint8_t> payload,
                                              const std::string& sender_id) const override;
};

}  // namespace v2::net
