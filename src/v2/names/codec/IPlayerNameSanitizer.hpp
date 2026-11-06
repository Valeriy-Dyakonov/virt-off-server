#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

namespace v2::names {

/// Longest display name, in UTF-8 bytes (24 Cyrillic letters).
inline constexpr size_t kMaxPlayerNameBytes = 48;

/// Turns a name typed by a player into one that is safe to show to others.
///
/// Thread-safety: REQUIRED — stateless.
struct IPlayerNameSanitizer {
    virtual ~IPlayerNameSanitizer() = default;

    /// Drops control characters and invalid UTF-8, trims surrounding spaces and
    /// cuts the name to kMaxPlayerNameBytes without splitting a character.
    /// Returns nullopt if nothing is left.
    virtual std::optional<std::string> sanitize(std::string_view raw) const = 0;
};

}  // namespace v2::names
