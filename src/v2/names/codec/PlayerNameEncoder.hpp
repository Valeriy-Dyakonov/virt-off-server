#pragma once

#include "v2/names/codec/IPlayerNameEncoder.hpp"

namespace v2::names {

/// Stateless IPlayerNameEncoder.
class PlayerNameEncoder final : public IPlayerNameEncoder {
public:
    std::vector<uint8_t> encode(const std::string& id, const std::string& name) const override;
};

}  // namespace v2::names
