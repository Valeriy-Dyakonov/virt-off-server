#include "v2/game/props/codec/PropIdReader.hpp"

#include "v2/net/wire/WireReader.hpp"

namespace v2::game {

std::optional<uint32_t> PropIdReader::read(const std::span<const uint8_t> payload) const {
    net::WireReader reader(payload);
    reader.skip_str();
    const uint32_t prop_id = reader.u32();
    if (!reader.ok()) {
        return std::nullopt;
    }
    return prop_id;
}

}  // namespace v2::game
