#include "v2/game/props/registry/SimulationSweeper.hpp"

#include "v2/game/GameConstants.hpp"

#include <chrono>
#include <format>

namespace v2::game {

SimulationSweeper::SimulationSweeper(IPropSimulation& simulation, ILogger& logger)
    : simulation_(simulation), logger_(logger) {}

void SimulationSweeper::run() {
    for (const uint32_t prop_id : simulation_.expire(std::chrono::steady_clock::now(), kPropSimulationTimeout)) {
        logger_.info(std::format("[Simulation] prop={} owner silent, freed", prop_id));
    }
}

}  // namespace v2::game
