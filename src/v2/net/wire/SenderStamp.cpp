#include "v2/net/wire/SenderStamp.hpp"

#include "v2/net/wire/WireReader.hpp"
#include "v2/net/wire/WireWriter.hpp"

namespace v2::net {

std::optional<std::vector<uint8_t>> SenderStamp::stamp(std::span<const uint8_t> payload,
                                                       const std::string& sender_id) const {
    WireReader reader(payload);
    reader.skip_str();
    if (!reader.ok()) {
        return std::nullopt;
    }

    WireWriter writer;
    writer.str(sender_id).bytes(reader.rest());
    if (!writer.ok()) {
        return std::nullopt;
    }
    return writer.take();
}

}  // namespace v2::net
