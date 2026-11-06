# virt-off-server

C++ UDP relay server for the Virt-Off virtual office voice chat. Receives Opus-encoded voice packets from Unity clients and relays them to all other connected clients.

## Architecture

```
┌──────────────┐  UDP :7777  ┌──────────────────┐
│  Unity Client │ ◄─────────► │  virt-off-server  │
│  (C#)          │             │  (C++ / asio)      │
└──────────────┘             └──────────────────┘
```

Single-threaded `asio::io_context` event loop. No external dependencies beyond asio (header-only).

## Protocol

V2 API. Byte 0 is the message type; numbers are little-endian, `str` is `[len:1][utf8]`.
Packets that start with `[sender:str]` are relayed with the sender id replaced by the
id the server bound to the sender's endpoint.

| Byte 0 | Name               | Direction      | Payload                                                          |
|--------|--------------------|----------------|------------------------------------------------------------------|
| `0xFF` | CONNECT            | C → S          | —; resending creates a new id for the same endpoint              |
| `0xFE` | ASSIGN_ID          | S → C          | utf8 id (no length prefix)                                       |
| `0x01` | VOICE              | C → S → others | `[sender:str][seq:2][opus…]`                                     |
| `0x02` | POSITION           | C → S → others | `[sender:str][pos:12][yaw:4][flags:1]`; updates server position  |
| `0x03` | ATTACK             | C → S → all    | `[attacker:str][kind:1][target:str][point:12][impulse:12][hitObject:1]`; cooldown + distance checked |
| `0x04` | PROP_STATE         | C → S → all    | `[sender:str][propId:4][pos:12][rot:16][vel:12][flags:1]`; owner only |
| `0x05` | SOUND              | C → S → others | `[sender:str][sfx:1][pos:12][volume:4]`                          |
| `0x06` | MANIFEST           | C → S          | `[sender:str][count:2]{[key:str][halfExtents:12][mass:4]}*`      |
| `0x07` | PICKUP_REQ         | C → S          | `[sender:str][propId:4]`; distance to the prop checked           |
| `0x08` | PROP_RELEASE       | C → S          | `[sender:str][propId:4][vel:12][angVel:12][flags:1]`             |
| `0x09` | MANIFEST_ACK       | S → C          | `[count:2][propId:4]*` in the client's manifest order            |
| `0x0A` | PROP_GRANT         | S → all        | `[propId:4][holder:str][flags:1]`                                |
| `0x0B` | PROP_RELEASE_EVENT | S → all        | `[propId:4][prevOwner:str][vel:12][angVel:12][flags:1]`          |
| `0x0C` | PROP_SNAPSHOT      | S → C          | `[count:2]{[propId:4][owner:str][held:1]}*` (not sent yet)       |
| `0x0D` | DISCONNECT         | C → S          | `[sender:str]`                                                   |
| `0x0E` | PLAYER_LEFT        | S → others     | `[id:str]`; on DISCONNECT and idle timeout                       |

## Build & Run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/virt-off-server           # port 7777
./build/virt-off-server 8888      # custom port
```

Stop with `Ctrl+C`.

## Docker

```bash
docker build -t virt-off-server .
docker run --rm -p 7777:7777/udp virt-off-server
```

## Deploy to Yandex Cloud

```bash
./deploy.sh
```

Prompts for Container Registry ID on first run.

## Project Structure

```
src/
├── core/              — ILogger, IIdGenerator, ConsoleLogger, MonotonicIdGenerator
├── clients/           — IClientRepository, ClientRepository
├── net/               — ITransport, IPacketRouter, UdpTransport, PacketRouter
├── handlers/          — IConnectHandler, IRelayHandler, ConnectHandler, RelayHandler
└── app/               — IServer, Server (composition root)
```

## Development Rules

See `RULES.md` — interface-first design, SRP, composition over implementation.
