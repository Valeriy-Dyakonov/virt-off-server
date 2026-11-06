#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

/// Sizes of wire-protocol fields, in bytes. Numbers are little-endian;
/// `str` is [len:1][utf8…].
namespace v2::net {

inline constexpr size_t kU8Bytes = 1;

inline constexpr size_t kU16Bytes = 2;

inline constexpr size_t kU32Bytes = 4;

/// float × 3: position, velocity, angular velocity, half-extents.
inline constexpr size_t kVec3Bytes = 12;

/// float × 4: rotation quaternion.
inline constexpr size_t kQuatBytes = 16;

/// Longest string a [len:1] prefix can describe.
inline constexpr size_t kMaxStringBytes = std::numeric_limits<uint8_t>::max();

}  // namespace v2::net
