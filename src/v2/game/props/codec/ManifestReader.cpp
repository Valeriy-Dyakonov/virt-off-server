#include "v2/game/props/codec/ManifestReader.hpp"

#include "v2/net/wire/WireReader.hpp"

namespace v2::game {

std::optional<std::vector<PropDescriptor>> ManifestReader::read(const std::span<const uint8_t> payload) const {
    net::WireReader reader(payload);
    reader.skip_str();
    const uint16_t count = reader.u16();
    if (!reader.ok()) {
        return std::nullopt;
    }

    std::vector<PropDescriptor> descriptors;
    for (uint16_t i = 0; i < count; ++i) {
        PropDescriptor descriptor;
        descriptor.key = reader.str();
        descriptor.half_extents = reader.vec3();
        descriptor.mass = reader.f32();
        if (!reader.ok()) {
            break;
        }
        descriptors.push_back(std::move(descriptor));
    }
    return descriptors;
}

}  // namespace v2::game
