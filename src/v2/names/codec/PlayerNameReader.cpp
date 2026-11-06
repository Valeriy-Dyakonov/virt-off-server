#include "v2/names/codec/PlayerNameReader.hpp"

#include "v2/net/wire/WireReader.hpp"

namespace v2::names {

std::optional<std::string> PlayerNameReader::read(const std::span<const uint8_t> payload) const {
    net::WireReader reader(payload);
    reader.skip_str();
    std::string name = reader.str();
    if (!reader.ok()) {
        return std::nullopt;
    }
    return name;
}

}  // namespace v2::names
