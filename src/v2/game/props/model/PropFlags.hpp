#pragma once

#include <cstddef>

/// Flag bytes of the prop messages.
namespace v2::game {

/// PROP_STATE, bit 1: the prop stopped and its owner gives it back.
inline constexpr std::byte kPropStateFlagSettled{0x02};

/// PROP_GRANT: the prop is taken into the hands.
inline constexpr std::byte kGrantFlagHeld{0x00};

/// PROP_GRANT, bit 0: the prop was pushed; the holder simulates it but does not hold it.
inline constexpr std::byte kGrantFlagSimulate{0x01};

/// PROP_RELEASE_EVENT, bit 0: the prop rests and is free.
inline constexpr std::byte kReleaseFlagSettled{0x01};

}  // namespace v2::game
