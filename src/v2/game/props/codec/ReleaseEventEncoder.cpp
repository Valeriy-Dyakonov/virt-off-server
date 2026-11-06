#include "v2/game/props/codec/ReleaseEventEncoder.hpp"

#include "v2/game/props/model/PropFlags.hpp"
#include "v2/net/wire/WireReader.hpp"
#include "v2/net/wire/WireSizes.hpp"
#include "v2/net/wire/WireWriter.hpp"

#include <array>

namespace v2::game {

namespace {

/// [vel:12][angVel:12][flags:1] — copied from the release as is.
constexpr size_t kTailBytes = net::kVec3Bytes + net::kVec3Bytes + net::kU8Bytes;

}  // namespace

std::optional<std::vector<uint8_t>> ReleaseEventEncoder::encode_throw(const std::span<const uint8_t> release_payload,
                                                                      const std::string& owner_id) const {
    net::WireReader reader(release_payload);
    reader.skip_str();
    const uint32_t prop_id = reader.u32();
    const auto tail = reader.rest();
    if (!reader.ok() || tail.size() < kTailBytes) {
        return std::nullopt;
    }

    net::WireWriter writer;
    writer.u32(prop_id).str(owner_id).bytes(tail.first(kTailBytes));
    if (!writer.ok()) {
        return std::nullopt;
    }
    return writer.take();
}

std::vector<uint8_t> ReleaseEventEncoder::encode_free(const uint32_t prop_id) const {
    constexpr std::array<uint8_t, net::kVec3Bytes + net::kVec3Bytes> kNoVelocity{};
    return net::WireWriter{}.u32(prop_id).str({}).bytes(kNoVelocity).byte(kReleaseFlagSettled).take();
}

}  // namespace v2::game
