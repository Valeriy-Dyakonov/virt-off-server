#include "v2/core/TimestampIdGenerator.hpp"

#include <chrono>
#include <format>

namespace v2 {

uint64_t TimestampIdGenerator::current_millis() {
    using namespace std::chrono;
    return static_cast<uint64_t>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
}

std::string TimestampIdGenerator::next() {
    std::lock_guard<std::mutex> lock(mutex_);

    const uint64_t millis = current_millis();
    if (millis == last_millis_) {
        ++same_millis_counter_;
    } else {
        last_millis_ = millis;
        same_millis_counter_ = 0;
    }

    if (same_millis_counter_ == 0) {
        return std::format("player-{}", millis);
    }
    return std::format("player-{}-{}", millis, same_millis_counter_);
}

}  // namespace v2
