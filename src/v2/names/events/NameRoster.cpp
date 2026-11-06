#include "v2/names/events/NameRoster.hpp"

#include "net/ProtocolConstants.hpp"

namespace v2::names {

NameRoster::NameRoster(const IUserRepository& users, const IPlayerNameEncoder& encoder,
                       net::IBroadcaster& broadcaster)
    : users_(users), encoder_(encoder), broadcaster_(broadcaster) {}

void NameRoster::send_roster(const std::string& to_id) {
    users_.for_each_except(to_id, [this, &to_id](const UserInfo& user) {
        if (!user.username.empty()) {
            broadcaster_.send_to(to_id, protocol::MSG_PLAYER_NAME, encoder_.encode(user.id, user.username));
        }
    });
}

}  // namespace v2::names
