#include "v2/game/props/registry/PropRepository.hpp"

#include <algorithm>

namespace v2::game {

namespace {

void make_free(PropRecord& record) {
    record.owner_id.clear();
    record.held = false;
}

}  // namespace

PropRecord* PropRepository::find(const uint32_t prop_id) {
    if (prop_id == 0 || prop_id > records_.size()) {
        return nullptr;
    }
    return &records_[prop_id - 1];
}

std::vector<uint32_t> PropRepository::register_manifest(const std::vector<PropDescriptor>& descriptors) {
    std::vector<uint32_t> ids;
    ids.reserve(descriptors.size());
    for (const auto& descriptor : descriptors) {
        const auto known = std::ranges::find(records_, descriptor.key, &PropRecord::key);
        if (known != records_.end()) {
            ids.push_back(known->prop_id);
            continue;
        }

        PropRecord record;
        record.prop_id = static_cast<uint32_t>(records_.size() + 1);
        record.key = descriptor.key;
        ids.push_back(record.prop_id);
        records_.push_back(std::move(record));
    }
    return ids;
}

bool PropRepository::try_grant(const uint32_t prop_id, const std::string& holder_id) {
    auto* record = find(prop_id);
    if (record == nullptr || (!record->owner_id.empty() && record->owner_id != holder_id)) {
        return false;
    }
    record->owner_id = holder_id;
    record->held = true;
    return true;
}

std::vector<uint32_t> PropRepository::release_all_of(const std::string& owner_id) {
    std::vector<uint32_t> released;
    for (auto& record : records_) {
        if (record.owner_id == owner_id) {
            make_free(record);
            released.push_back(record.prop_id);
        }
    }
    return released;
}

void PropRepository::update_position(const uint32_t prop_id, const Vec3& position) {
    if (auto* record = find(prop_id)) {
        record->position = position;
        record->has_position = true;
    }
}

std::optional<PropRecord> PropRepository::get(const uint32_t prop_id) const {
    if (prop_id == 0 || prop_id > records_.size()) {
        return std::nullopt;
    }
    return records_[prop_id - 1];
}

bool PropRepository::try_simulate(const uint32_t prop_id, const std::string& owner_id,
                                  const std::chrono::steady_clock::time_point now) {
    auto* record = find(prop_id);
    if (record == nullptr || record->held || (!record->owner_id.empty() && record->owner_id != owner_id)) {
        return false;
    }
    record->owner_id = owner_id;
    record->last_activity = now;
    return true;
}

bool PropRepository::begin_simulation(const uint32_t prop_id, const std::string& owner_id,
                                      const std::chrono::steady_clock::time_point now) {
    auto* record = find(prop_id);
    if (record == nullptr || record->owner_id != owner_id) {
        return false;
    }
    record->held = false;
    record->last_activity = now;
    return true;
}

bool PropRepository::end_simulation(const uint32_t prop_id, const std::string& owner_id) {
    auto* record = find(prop_id);
    if (record == nullptr || record->owner_id.empty() || record->owner_id != owner_id) {
        return false;
    }
    make_free(*record);
    return true;
}

void PropRepository::touch(const uint32_t prop_id, const std::chrono::steady_clock::time_point now) {
    if (auto* record = find(prop_id)) {
        record->last_activity = now;
    }
}

std::vector<uint32_t> PropRepository::expire(const std::chrono::steady_clock::time_point now,
                                             const std::chrono::milliseconds timeout) {
    std::vector<uint32_t> expired;
    for (auto& record : records_) {
        if (!record.owner_id.empty() && now - record.last_activity > timeout) {
            make_free(record);
            expired.push_back(record.prop_id);
        }
    }
    return expired;
}

}  // namespace v2::game
