#pragma once

#include "v2/clients/IUserRepository.hpp"

#include <mutex>
#include <unordered_map>
#include <vector>

namespace v2 {

/// Thread-safe IUserRepository: both indexes are kept in sync under one mutex.
class UserRepository final : public IUserRepository {
public:
    void add(UserInfo user) override;

    void remove(const std::string& id) override;

    std::optional<UserInfo> find_by_id(const std::string& id) const override;

    std::optional<std::string> find_id_by_endpoint(const asio::ip::udp::endpoint& endpoint) const override;

    void touch(const std::string& id, std::chrono::steady_clock::time_point now) override;

    void set_username(const std::string& id, std::string username) override;

    void for_each_except(const std::string& excluded_id, const Visitor& visit) const override;

    void for_each(const Visitor& visit) const override;

private:
    /// Copies the users other than `excluded_id` under the lock.
    std::vector<UserInfo> snapshot_except(const std::string& excluded_id) const;

    mutable std::mutex mutex_;

    /// id → record.
    std::unordered_map<std::string, UserInfo> by_id_;

    /// endpoint → id.
    std::unordered_map<asio::ip::udp::endpoint, std::string> by_endpoint_;
};

}  // namespace v2
