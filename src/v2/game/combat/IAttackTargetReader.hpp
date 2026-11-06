#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string>

namespace v2::game {

/// Reads the target id of ATTACK:
/// [attacker:str][kind:1][target:str][point:12][impulse:12][hitObject:1] (without the type byte).
///
/// Thread-safety: REQUIRED — stateless.
struct IAttackTargetReader {
    virtual ~IAttackTargetReader() = default;

    /// Returns the target id, or nullopt if the payload is truncated before its end.
    virtual std::optional<std::string> read(std::span<const uint8_t> payload) const = 0;
};

}  // namespace v2::game
