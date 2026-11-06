#include "v2/net/wire/WireWriter.hpp"

#include "v2/net/wire/WireSizes.hpp"

#include <array>
#include <cstring>
#include <utility>

namespace v2::net {

namespace {

/// Appends the little-endian bytes of `value`.
template <typename T>
void append(std::vector<uint8_t>& bytes, const T value) {
    std::array<uint8_t, sizeof(T)> raw{};
    std::memcpy(raw.data(), &value, sizeof(T));
    bytes.insert(bytes.end(), raw.begin(), raw.end());
}

}  // namespace

WireWriter& WireWriter::u8(const uint8_t value) {
    bytes_.push_back(value);
    return *this;
}

WireWriter& WireWriter::byte(const std::byte value) {
    return u8(std::to_integer<uint8_t>(value));
}

WireWriter& WireWriter::u16(const uint16_t value) {
    append(bytes_, value);
    return *this;
}

WireWriter& WireWriter::u32(const uint32_t value) {
    append(bytes_, value);
    return *this;
}

WireWriter& WireWriter::str(const std::string_view value) {
    if (value.size() > kMaxStringBytes) {
        ok_ = false;
        return *this;
    }
    bytes_.push_back(static_cast<uint8_t>(value.size()));
    bytes_.insert(bytes_.end(), value.begin(), value.end());
    return *this;
}

WireWriter& WireWriter::bytes(const std::span<const uint8_t> value) {
    bytes_.insert(bytes_.end(), value.begin(), value.end());
    return *this;
}

bool WireWriter::ok() const {
    return ok_;
}

std::vector<uint8_t> WireWriter::take() {
    return std::exchange(bytes_, {});
}

}  // namespace v2::net
