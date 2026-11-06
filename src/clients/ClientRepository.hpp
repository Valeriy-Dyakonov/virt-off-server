#pragma once

#include "clients/IClientRepository.hpp"

#include <unordered_map>

/// IClientRepository с двумя индексами (id → endpoint, endpoint → id).
///
/// Id назначает сам репозиторий, начиная с 1 (0 зарезервирован за сервером);
/// remove() чистит оба индекса.
///
/// Thread-safety: не требуется — вызывается только на strand транспорта.
class ClientRepository final : public IClientRepository {
public:
    uint64_t add(udp::endpoint ep) override;

    void remove(uint64_t id) override;

    std::optional<uint64_t> find_by_endpoint(const udp::endpoint& ep) const override;

    void for_each_except(uint64_t exclude_id, std::function<void(uint64_t, const udp::endpoint&)> fn) const override;

private:
    std::unordered_map<uint64_t, udp::endpoint> by_id_;

    std::unordered_map<udp::endpoint, uint64_t> by_endpoint_;

    /// Счётчик для назначения ID. Стартует с 1 (0 зарезервирован за сервером).
    uint64_t next_id_ = 1;
};
