#include "v2/game/combat/AttackTargetReader.hpp"

#include "v2/net/wire/WireReader.hpp"
#include "v2/net/wire/WireSizes.hpp"

namespace v2::game {

std::optional<std::string> AttackTargetReader::read(const std::span<const uint8_t> payload) const {
    net::WireReader reader(payload);
    reader.skip_str();
    reader.skip(net::kU8Bytes);  // kind
    std::string target_id = reader.str();
    if (!reader.ok()) {
        return std::nullopt;
    }
    return target_id;
}

}  // namespace v2::game
