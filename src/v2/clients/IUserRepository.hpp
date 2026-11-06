#pragma once

#include "v2/clients/UserInfo.hpp"

#include <chrono>
#include <functional>
#include <optional>
#include <string>

namespace v2 {

/// Registry of connected clients, indexed by id and by endpoint.
///
/// Thread-safety: REQUIRED — the implementation guards both indexes.
struct IUserRepository {
    /// Visitor of a user snapshot; may call back into the repository.
    using Visitor = std::function<void(const UserInfo&)>;

    virtual ~IUserRepository() = default;

    /// Stores `user`, replacing an entry with the same id or endpoint.
    virtual void add(UserInfo user) = 0;

    /// Removes the user `id`; unknown ids are ignored.
    virtual void remove(const std::string& id) = 0;

    /// Returns a copy of the user `id`, or nullopt.
    virtual std::optional<UserInfo> find_by_id(const std::string& id) const = 0;

    /// Returns the id bound to `endpoint`, or nullopt.
    virtual std::optional<std::string> find_id_by_endpoint(const asio::ip::udp::endpoint& endpoint) const = 0;

    /// Sets `last_seen` of the user `id`; unknown ids are ignored.
    virtual void touch(const std::string& id, std::chrono::steady_clock::time_point now) = 0;

    /// Sets `username` of the user `id`; unknown ids are ignored.
    virtual void set_username(const std::string& id, std::string username) = 0;

    /// Calls `visit` for every user except `excluded_id`, outside the lock.
    virtual void for_each_except(const std::string& excluded_id, const Visitor& visit) const = 0;

    /// Calls `visit` for every user, outside the lock.
    virtual void for_each(const Visitor& visit) const = 0;
};

}  // namespace v2
