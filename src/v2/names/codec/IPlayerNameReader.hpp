#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string>

namespace v2::names {

/// Extracts the raw name from SET_NAME: [id:str][name:str] (payload without the type byte).
///
/// Thread-safety: REQUIRED — stateless.
struct IPlayerNameReader {
    virtual ~IPlayerNameReader() = default;

    /// Returns the name bytes as sent (not validated), or nullopt if the payload is truncated.
    [[nodiscard]] virtual std::optional<std::string> read(std::span<const uint8_t> payload) const = 0;
};

}  // namespace v2::names
