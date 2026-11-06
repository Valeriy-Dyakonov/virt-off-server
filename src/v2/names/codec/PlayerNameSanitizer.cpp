#include "v2/names/codec/PlayerNameSanitizer.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace v2::names {

namespace {

/// Length of the UTF-8 sequence that starts with `lead`, 0 for an invalid lead byte.
size_t sequence_length(const uint8_t lead) {
    if (lead < 0x80) {
        return 1;
    }
    if (lead >= 0xC2 && lead <= 0xDF) {
        return 2;
    }
    if (lead >= 0xE0 && lead <= 0xEF) {
        return 3;
    }
    if (lead >= 0xF0 && lead <= 0xF4) {
        return 4;
    }
    return 0;
}

/// High bits of a UTF-8 continuation byte: 10xxxxxx.
constexpr std::byte kContinuationMask{0xC0};
constexpr std::byte kContinuationTag{0x80};

/// Code point bits of a continuation byte and how many there are.
constexpr std::byte kContinuationPayload{0x3F};
constexpr unsigned kContinuationPayloadBits = 6;

/// Code point bits of the lead byte, by sequence length (index 1..4).
constexpr std::array kLeadPayload{std::byte{0x00}, std::byte{0x7F}, std::byte{0x1F}, std::byte{0x0F}, std::byte{0x07}};

/// Decodes the code point of a sequence already checked by sequence_length.
uint32_t decode(const std::string_view sequence) {
    auto code_point = std::to_integer<uint32_t>(static_cast<std::byte>(sequence[0]) & kLeadPayload[sequence.size()]);
    for (const char continuation : sequence.substr(1)) {
        code_point <<= kContinuationPayloadBits;
        code_point |= std::to_integer<uint32_t>(static_cast<std::byte>(continuation) & kContinuationPayload);
    }
    return code_point;
}

/// True if every continuation byte is 10xxxxxx and the encoding is the shortest one.
bool is_well_formed(const std::string_view sequence) {
    for (size_t i = 1; i < sequence.size(); ++i) {
        if ((static_cast<std::byte>(sequence[i]) & kContinuationMask) != kContinuationTag) {
            return false;
        }
    }
    const uint32_t code_point = decode(sequence);
    const bool overlong =
        (sequence.size() == 3 && code_point < 0x800) || (sequence.size() == 4 && code_point < 0x10000);
    const bool surrogate = code_point >= 0xD800 && code_point <= 0xDFFF;
    return !overlong && !surrogate && code_point <= 0x10FFFF;
}

/// C0, DEL and C1 control characters, plus line and paragraph separators.
bool is_control(const uint32_t code_point) {
    return code_point < 0x20 || (code_point >= 0x7F && code_point <= 0x9F) || code_point == 0x2028 ||
           code_point == 0x2029;
}

}  // namespace

std::optional<std::string> PlayerNameSanitizer::sanitize(const std::string_view raw) const {
    std::string clean;
    clean.reserve(kMaxPlayerNameBytes);

    size_t i = 0;
    while (i < raw.size()) {
        const size_t length = sequence_length(static_cast<uint8_t>(raw[i]));
        if (length == 0 || i + length > raw.size()) {
            ++i;  // invalid byte: skip it alone
            continue;
        }

        const std::string_view sequence = raw.substr(i, length);
        i += length;
        if (!is_well_formed(sequence) || is_control(decode(sequence))) {
            continue;
        }
        if (clean.empty() && sequence == " ") {
            continue;  // leading spaces
        }
        if (clean.size() + length > kMaxPlayerNameBytes) {
            break;
        }
        clean.append(sequence);
    }

    while (!clean.empty() && clean.back() == ' ') {
        clean.pop_back();
    }

    if (clean.empty()) {
        return std::nullopt;
    }
    return clean;
}

}  // namespace v2::names
