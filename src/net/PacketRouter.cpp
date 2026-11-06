#include "net/PacketRouter.hpp"

void PacketRouter::register_handler(uint8_t type, Handler handler) {
    handlers_[type] = std::move(handler);
}

void PacketRouter::route(const udp::endpoint& from, std::span<const uint8_t> data) {
    if (data.empty()) {
        return;
    }

    const auto it = handlers_.find(data[0]);
    if (it == handlers_.end()) {
        return;
    }

    it->second(from, data.subspan(1));
}
