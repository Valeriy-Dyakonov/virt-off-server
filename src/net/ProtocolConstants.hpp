#pragma once

#include <cstdint>

/// Wire-protocol control bytes shared by the server and the Unity client.
///
/// Byte 0 of every datagram identifies the message kind; the remaining bytes
/// are the payload. Centralised here so V1 and V2 code use the same values.
namespace protocol {

/// 0xFF — client → server: request to connect.
inline constexpr uint8_t MSG_CONNECT = 0xFF;

/// 0xFE — server → client: assigned client ID.
/// V1 payload: 8-byte native-endian uint64 id.
/// V2 payload: UTF-8 string id, no length prefix (see v2::game::ConnectHandler).
inline constexpr uint8_t MSG_ASSIGN_ID = 0xFE;

/// 0x01 — client → server → other clients: voice packet (Opus), relayed as-is.
inline constexpr uint8_t MSG_VOICE = 0x01;

/// 0x02 — client → server → other clients: position-sync packet, relayed as-is.
inline constexpr uint8_t MSG_POSITION = 0x02;

/// 0x03 — C→S: attack request. Server validates (cooldown + distance) then relays to target.
inline constexpr uint8_t MSG_ATTACK = 0x03;

/// 0x04 — C→S → S→all: prop transform state from owner.
inline constexpr uint8_t MSG_PROP_STATE = 0x04;

/// 0x05 — C→S → S→all: sound event relay (includes author).
inline constexpr uint8_t MSG_SOUND = 0x05;

/// 0x06 — C→S: scene prop manifest (key + halfExtents + mass × count).
inline constexpr uint8_t MSG_MANIFEST = 0x06;

/// 0x07 — C→S: pickup request for a specific prop.
inline constexpr uint8_t MSG_PICKUP_REQ = 0x07;

/// 0x08 — C→S: prop release / throw from holder.
inline constexpr uint8_t MSG_PROP_RELEASE = 0x08;

/// 0x09 — S→C: manifest ACK — authoritative propId mapping.
inline constexpr uint8_t MSG_MANIFEST_ACK = 0x09;

/// 0x0A — S→all: prop granted to a holder.
inline constexpr uint8_t MSG_PROP_GRANT = 0x0A;

/// 0x0B — S→all: prop released. Owner empty → FREE; owner set → that player
/// threw it and keeps simulating it until it settles.
inline constexpr uint8_t MSG_PROP_RELEASE_EVENT = 0x0B;

/// 0x0C — S→C: full prop ownership snapshot on join.
inline constexpr uint8_t MSG_PROP_SNAPSHOT = 0x0C;

/// 0x0D — C→S: graceful disconnect (owner stops sending).
inline constexpr uint8_t MSG_DISCONNECT = 0x0D;

/// 0x0E — S→others: a player left (DISCONNECT or idle timeout). Payload: [id:str].
inline constexpr uint8_t MSG_PLAYER_LEFT = 0x0E;

/// 0x0F — C→S: the client pushed or hit a free prop and asks to simulate it.
/// Payload: [id:str][propId:4]. Answered with PROP_GRANT (flags: kGrantFlagSimulate).
inline constexpr uint8_t MSG_PROP_PUSH = 0x0F;

/// 0x10 — C→S: the player's display name. Payload: [id:str][name:str].
inline constexpr uint8_t MSG_SET_NAME = 0x10;

/// 0x11 — S→C: a player's display name. Payload: [id:str][name:str].
inline constexpr uint8_t MSG_PLAYER_NAME = 0x11;

}  // namespace protocol
