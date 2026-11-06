#include "v2/clients/UserRepository.hpp"

#include <utility>

namespace v2 {

void UserRepository::add(UserInfo user) {
    const std::scoped_lock lock(mutex_);

    if (const auto it = by_endpoint_.find(user.endpoint); it != by_endpoint_.end()) {
        by_id_.erase(it->second);
        by_endpoint_.erase(it);
    }
    if (const auto it = by_id_.find(user.id); it != by_id_.end()) {
        by_endpoint_.erase(it->second.endpoint);
        by_id_.erase(it);
    }

    by_endpoint_[user.endpoint] = user.id;
    by_id_[user.id] = std::move(user);
}

void UserRepository::remove(const std::string& id) {
    const std::scoped_lock lock(mutex_);

    if (const auto it = by_id_.find(id); it != by_id_.end()) {
        by_endpoint_.erase(it->second.endpoint);
        by_id_.erase(it);
    }
}

std::optional<UserInfo> UserRepository::find_by_id(const std::string& id) const {
    const std::scoped_lock lock(mutex_);

    const auto it = by_id_.find(id);
    if (it == by_id_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::optional<std::string> UserRepository::find_id_by_endpoint(const asio::ip::udp::endpoint& endpoint) const {
    const std::scoped_lock lock(mutex_);

    const auto it = by_endpoint_.find(endpoint);
    if (it == by_endpoint_.end()) {
        return std::nullopt;
    }
    return it->second;
}

void UserRepository::touch(const std::string& id, std::chrono::steady_clock::time_point now) {
    const std::scoped_lock lock(mutex_);

    if (const auto it = by_id_.find(id); it != by_id_.end()) {
        it->second.last_seen = now;
    }
}

void UserRepository::set_username(const std::string& id, std::string username) {
    const std::scoped_lock lock(mutex_);

    if (const auto it = by_id_.find(id); it != by_id_.end()) {
        it->second.username = std::move(username);
    }
}

std::vector<UserInfo> UserRepository::snapshot_except(const std::string& excluded_id) const {
    const std::scoped_lock lock(mutex_);

    std::vector<UserInfo> users;
    users.reserve(by_id_.size());
    for (const auto& [id, user] : by_id_) {
        if (id != excluded_id) {
            users.push_back(user);
        }
    }
    return users;
}

void UserRepository::for_each_except(const std::string& excluded_id, const Visitor& visit) const {
    // Visiting outside the lock lets `visit` call back into the repository.
    for (const auto& user : snapshot_except(excluded_id)) {
        visit(user);
    }
}

void UserRepository::for_each(const Visitor& visit) const {
    // Ids are never empty, so nobody is excluded.
    for_each_except({}, visit);
}

}  // namespace v2
