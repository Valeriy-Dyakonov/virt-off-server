#pragma once

#include "v2/net/module/IPacketHandler.hpp"

#include <cstdint>

namespace v2::net {

/// Binds a message type byte to its handler.
struct PacketRoute {
    uint8_t type;

    IPacketHandler& handler;
};

}  // namespace v2::net
