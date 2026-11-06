#pragma once

#include "v2/game/props/registry/IPropRepository.hpp"
#include "v2/game/props/registry/IPropSimulation.hpp"

#include <string>
#include <vector>

namespace v2::game {

/// In-memory IPropRepository and IPropSimulation over the same records:
/// a thrown or pushed prop keeps its owner with `held == false`.
///
/// prop_id is the 1-based index of the record; 0 is never assigned.
class PropRepository final : public IPropRepository, public IPropSimulation {
public:
    [[nodiscard]] std::vector<uint32_t> register_manifest(const std::vector<PropDescriptor>& descriptors) override;

    [[nodiscard]] bool try_grant(uint32_t prop_id, const std::string& holder_id) override;

    [[nodiscard]] std::vector<uint32_t> release_all_of(const std::string& owner_id) override;

    void update_position(uint32_t prop_id, const Vec3& position) override;

    [[nodiscard]] std::optional<PropRecord> get(uint32_t prop_id) const override;

    [[nodiscard]] bool try_simulate(uint32_t prop_id, const std::string& owner_id,
                      std::chrono::steady_clock::time_point now) override;

    [[nodiscard]] bool begin_simulation(uint32_t prop_id, const std::string& owner_id,
                          std::chrono::steady_clock::time_point now) override;

    [[nodiscard]] bool end_simulation(uint32_t prop_id, const std::string& owner_id) override;

    void touch(uint32_t prop_id, std::chrono::steady_clock::time_point now) override;

    [[nodiscard]] std::vector<uint32_t> expire(std::chrono::steady_clock::time_point now,
                                 std::chrono::milliseconds timeout) override;

private:
    /// Record of `prop_id`, or nullptr if unknown.
    PropRecord* find(uint32_t prop_id);

    std::vector<PropRecord> records_;
};

}  // namespace v2::game
