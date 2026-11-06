#include "handlers/ConnectHandler.hpp"

#include "net/ProtocolConstants.hpp"

#include <cstring>
#include <format>
#include <string>
#include <vector>

ConnectHandler::ConnectHandler(IClientRepository& clients, ITransport& transport, ILogger& logger)
    : clients_(clients), transport_(transport), logger_(logger) {}

void ConnectHandler::handle(const udp::endpoint& client_endpoint) {
    const uint64_t id = clients_.add(client_endpoint);
    logger_.info(std::format("Client {} connected", id));

    // ASSIGN_ID: [type:1][id:8 native-endian]
    std::vector<uint8_t> response(1 + sizeof(id));
    response[0] = protocol::MSG_ASSIGN_ID;
    std::memcpy(response.data() + 1, &id, sizeof(id));
    transport_.send_to(client_endpoint, response);
}
