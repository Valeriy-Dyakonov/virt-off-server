#include "v2/game/props/codec/PropStateReader.hpp"

#include "v2/net/wire/WireReader.hpp"
#include "v2/net/wire/WireSizes.hpp"

namespace v2::game {

std::optional<PropStateFields> PropStateReader::read(std::span<const uint8_t> payload) const {
    net::WireReader reader(payload);
    reader.skip_str();

    PropStateFields fields;
    fields.prop_id = reader.u32();
    fields.position = reader.vec3();
    reader.skip(net::kQuatBytes + net::kVec3Bytes);  // rotation, velocity
    fields.flags = reader.byte();

    if (!reader.ok()) {
        return std::nullopt;
    }
    return fields;
}

}  // namespace v2::game
