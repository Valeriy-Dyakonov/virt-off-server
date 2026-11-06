#pragma once

#include "v2/names/codec/IPlayerNameReader.hpp"

namespace v2::names {

/// Stateless IPlayerNameReader over the [id:str][name:str] layout.
class PlayerNameReader final : public IPlayerNameReader {
public:
    std::optional<std::string> read(std::span<const uint8_t> payload) const override;
};

}  // namespace v2::names
