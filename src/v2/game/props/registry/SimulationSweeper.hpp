#pragma once

#include "core/ILogger.hpp"
#include "v2/game/props/registry/IPropSimulation.hpp"
#include "v2/net/module/IPeriodicTask.hpp"

namespace v2::game {

/// Frees props whose owner sent no PROP_STATE for kPropSimulationTimeout.
/// Clients are not told: they already consider such a prop settled.
class SimulationSweeper final : public net::IPeriodicTask {
public:
    SimulationSweeper(IPropSimulation& simulation, ILogger& logger);

    void run() override;

private:
    IPropSimulation& simulation_;

    ILogger& logger_;
};

}  // namespace v2::game
