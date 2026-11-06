#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace v2::names {

/// Builds the PLAYER_NAME payload: [id:str][name:str] (without the type byte).
///
/// Thread-safety: REQUIRED — stateless.
struct IPlayerNameEncoder {
    virtual ~IPlayerNameEncoder() = default;

    /// `id` and `name` must each fit in 255 bytes (ids are short, names are sanitized).
    [[nodiscard]] virtual std::vector<uint8_t> encode(const std::string& id, const std::string& name) const = 0;
};

}  // namespace v2::names
