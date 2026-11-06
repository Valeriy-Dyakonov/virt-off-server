#include "v2/net/wire/WireReader.hpp"

#include "v2/net/wire/WireSizes.hpp"

#include <bit>
#include <cstring>

namespace v2::net {

static_assert(std::endian::native == std::endian::little, "the wire format is little-endian");

namespace {

/// Copies `field` into a value of type T; zero if the field is empty (failed read).
template <typename T>
T decode(const std::span<const uint8_t> field) {
    T value{};
    if (field.size() == sizeof(T)) {
        std::memcpy(&value, field.data(), sizeof(T));
    }
    return value;
}

}  // namespace

WireReader::WireReader(const std::span<const uint8_t> bytes) : bytes_(bytes) {}

std::span<const uint8_t> WireReader::take(const size_t count) {
    if (!ok_ || bytes_.size() - offset_ < count) {
        ok_ = false;
        return {};
    }
    const size_t start = offset_;
    offset_ += count;
    return bytes_.subspan(start, count);
}

uint8_t WireReader::u8() {
    return decode<uint8_t>(take(kU8Bytes));
}

std::byte WireReader::byte() {
    return std::byte{u8()};
}

uint16_t WireReader::u16() {
    return decode<uint16_t>(take(kU16Bytes));
}

uint32_t WireReader::u32() {
    return decode<uint32_t>(take(kU32Bytes));
}

float WireReader::f32() {
    return decode<float>(take(sizeof(float)));
}

std::array<float, 3> WireReader::vec3() {
    return decode<std::array<float, 3>>(take(kVec3Bytes));
}

std::string WireReader::str() {
    const auto field = take(u8());
    return {field.begin(), field.end()};
}

void WireReader::skip_str() {
    take(u8());
}

void WireReader::skip(const size_t count) {
    take(count);
}

std::span<const uint8_t> WireReader::rest() const {
    return ok_ ? bytes_.subspan(offset_) : std::span<const uint8_t>{};
}

bool WireReader::ok() const {
    return ok_;
}

}  // namespace v2::net
