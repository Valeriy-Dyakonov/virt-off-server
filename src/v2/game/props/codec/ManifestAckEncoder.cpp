#include "v2/game/props/codec/ManifestAckEncoder.hpp"

#include "v2/net/wire/WireWriter.hpp"

namespace v2::game {

std::vector<uint8_t> ManifestAckEncoder::encode(const std::vector<uint32_t>& prop_ids) const {
    // The manifest count is a uint16, so the ids always fit.
    net::WireWriter writer;
    writer.u16(static_cast<uint16_t>(prop_ids.size()));
    for (const uint32_t prop_id : prop_ids) {
        writer.u32(prop_id);
    }
    return writer.take();
}

}  // namespace v2::game
