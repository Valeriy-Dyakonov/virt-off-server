#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

namespace v2::net {

/// Builds a wire payload field by field (format: see WireSizes.hpp).
///
/// A string longer than kMaxStringBytes is not written and turns ok() to false.
///
/// Thread-safety: an instance belongs to one thread.
class WireWriter {
public:
    WireWriter& u8(uint8_t value);

    /// Writes a flags byte.
    WireWriter& byte(std::byte value);

    WireWriter& u16(uint16_t value);

    WireWriter& u32(uint32_t value);

    /// Writes [len:1][utf8…].
    WireWriter& str(std::string_view value);

    /// Writes `value` as is, without a length prefix.
    WireWriter& bytes(std::span<const uint8_t> value);

    /// True while every string fitted its length prefix.
    [[nodiscard]] bool ok() const;

    /// Moves the payload out.
    [[nodiscard]] std::vector<uint8_t> take();

private:
    std::vector<uint8_t> bytes_;

    bool ok_ = true;
};

}  // namespace v2::net
