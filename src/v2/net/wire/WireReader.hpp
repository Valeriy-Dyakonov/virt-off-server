#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

namespace v2::net {

/// Sequential reader of a wire payload (format: see WireSizes.hpp).
///
/// Reads past the end do not throw: they return zero / empty values and turn
/// ok() to false, so a parser reads every field and checks ok() once.
///
/// Thread-safety: an instance belongs to one thread; it only views `bytes`.
class WireReader {
public:
    explicit WireReader(std::span<const uint8_t> bytes);

    uint8_t u8();

    /// Reads a flags byte.
    std::byte byte();

    uint16_t u16();

    uint32_t u32();

    float f32();

    std::array<float, 3> vec3();

    /// Reads [len:1][utf8…].
    std::string str();

    /// Skips [len:1][utf8…] without copying it.
    void skip_str();

    /// Skips `count` bytes.
    void skip(size_t count);

    /// Bytes not read yet (empty after a failed read).
    std::span<const uint8_t> rest() const;

    /// True while nothing was read past the end.
    bool ok() const;

private:
    /// Returns the next `count` bytes and advances, or fails and returns an empty span.
    std::span<const uint8_t> take(size_t count);

    std::span<const uint8_t> bytes_;

    size_t offset_ = 0;

    bool ok_ = true;
};

}  // namespace v2::net
