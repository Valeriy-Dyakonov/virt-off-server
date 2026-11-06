#pragma once

#include "v2/names/codec/IPlayerNameSanitizer.hpp"

namespace v2::names {

/// Stateless IPlayerNameSanitizer limited to kMaxPlayerNameBytes.
class PlayerNameSanitizer final : public IPlayerNameSanitizer {
public:
    std::optional<std::string> sanitize(std::string_view raw) const override;
};

}  // namespace v2::names
