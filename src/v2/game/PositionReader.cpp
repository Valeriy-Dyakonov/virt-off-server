#include "v2/game/PositionReader.hpp"
#include "v2/game/WireSizes.hpp"

#include <cstring>

namespace v2::game {

std::optional<std::array<float, 3>> PositionReader::read(std::span<const uint8_t> payload) const {
    if (payload.size() < kStrLenBytes) {
        return std::nullopt;
    }

    const size_t position_offset = kStrLenBytes + static_cast<size_t>(payload[0]);
    if (payload.size() < position_offset + kVec3Bytes) {
        return std::nullopt;
    }

    std::array<float, 3> position{};
    std::memcpy(position.data(), payload.data() + position_offset, kVec3Bytes);
    return position;
}

}  // namespace v2::game
