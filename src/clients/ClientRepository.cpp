#include "clients/ClientRepository.hpp"

uint64_t ClientRepository::add(const udp::endpoint& ep) {
    const uint64_t id = next_id_++;
    by_id_[id] = ep;
    by_endpoint_[ep] = id;
    return id;
}

void ClientRepository::remove(const uint64_t id) {
    const auto it = by_id_.find(id);
    if (it == by_id_.end()) {
        return;
    }

    by_endpoint_.erase(it->second);
    by_id_.erase(it);
}

std::optional<uint64_t> ClientRepository::find_by_endpoint(const udp::endpoint& ep) const {
    const auto it = by_endpoint_.find(ep);
    if (it == by_endpoint_.end()) {
        return std::nullopt;
    }

    return it->second;
}

void ClientRepository::for_each_except(const uint64_t exclude_id, const Visitor& visit) const {
    for (const auto& [id, ep] : by_id_) {
        if (id != exclude_id) {
            visit(id, ep);
        }
    }
}
