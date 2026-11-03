#include "v2/handlers/SenderStamp.hpp"

#include <cstring>
#include <limits>

namespace v2 {

std::optional<std::vector<uint8_t>> SenderStamp::stamp(std::span<const uint8_t> payload,
                                                       const std::string& sender_id) const {
    if (payload.empty() || sender_id.size() > std::numeric_limits<uint8_t>::max()) {
        return std::nullopt;
    }

    const size_t rest_offset = 1 + static_cast<size_t>(payload[0]);
    if (payload.size() < rest_offset) {
        return std::nullopt;
    }

    const size_t rest_size = payload.size() - rest_offset;
    std::vector<uint8_t> out(1 + sender_id.size() + rest_size);
    out[0] = static_cast<uint8_t>(sender_id.size());
    std::memcpy(out.data() + 1, sender_id.data(), sender_id.size());
    if (rest_size > 0) {
        std::memcpy(out.data() + 1 + sender_id.size(), payload.data() + rest_offset, rest_size);
    }
    return out;
}

}  // namespace v2
