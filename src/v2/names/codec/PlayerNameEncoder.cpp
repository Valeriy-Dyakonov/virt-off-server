#include "v2/names/codec/PlayerNameEncoder.hpp"

#include "v2/net/wire/WireWriter.hpp"

namespace v2::names {

std::vector<uint8_t> PlayerNameEncoder::encode(const std::string& id, const std::string& name) const {
    return net::WireWriter{}.str(id).str(name).take();
}

}  // namespace v2::names
