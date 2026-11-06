#include "v2/game/players/PositionReader.hpp"

#include "v2/net/wire/WireReader.hpp"

namespace v2::game {

std::optional<Vec3> PositionReader::read(const std::span<const uint8_t> payload) const {
    net::WireReader reader(payload);
    reader.skip_str();
    const Vec3 position = reader.vec3();
    if (!reader.ok()) {
        return std::nullopt;
    }
    return position;
}

}  // namespace v2::game
