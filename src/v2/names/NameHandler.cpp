#include "v2/names/NameHandler.hpp"

#include <format>

namespace v2::names {

NameHandler::NameHandler(IUserRepository& users,
                         const IPlayerNameReader& reader,
                         const IPlayerNameSanitizer& sanitizer,
                         INameBroadcaster& broadcaster,
                         INameRoster& roster,
                         ILogger& logger)
    : users_(users)
    , reader_(reader)
    , sanitizer_(sanitizer)
    , broadcaster_(broadcaster)
    , roster_(roster)
    , logger_(logger) {}

void NameHandler::handle(const asio::ip::udp::endpoint& from, const std::span<const uint8_t> payload) {
    const auto sender_id = users_.find_id_by_endpoint(from);
    const auto raw = reader_.read(payload);
    const auto name = raw.has_value() ? sanitizer_.sanitize(*raw) : std::nullopt;
    if (!sender_id.has_value() || !name.has_value()) {
        return;
    }

    if (const auto user = users_.find_by_id(*sender_id); user.has_value() && user->username != *name) {
        logger_.info(std::format("[Name] {} is now '{}'", *sender_id, *name));
    }

    users_.set_username(*sender_id, *name);
    broadcaster_.announce(*sender_id, *name);
    roster_.send_roster(*sender_id);
}

}  // namespace v2::names
