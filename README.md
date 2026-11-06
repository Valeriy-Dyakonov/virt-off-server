# virt-off-server

C++ UDP server for the Virt-Off virtual office: relays voice, positions and sounds between Unity
clients and arbitrates the shared game state (props, attacks, names).

## Architecture

```
┌──────────────┐  UDP :7777  ┌──────────────────┐
│  Unity Client │ ◄─────────► │  virt-off-server  │
│  (C#)          │             │  (C++ / asio)      │
└──────────────┘             └──────────────────┘
```

`asio::io_context` run by `--threads` workers; socket I/O, packet handling and the 5 s housekeeping
tick are serialised on one strand, so game state needs no locks. No dependencies beyond asio (header-only).

V2 is a set of feature modules (`v2::net::IServerModule`). Each module owns its components and
exposes **routes** (type byte → handler), **observers** (see every datagram before routing) and
**periodic tasks** (run on the tick). `v2::Server` only plugs them into the transport;
`v2::ServerBuilder` is the composition root. See [Project Structure](#project-structure).

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
| `0x04` | PROP_STATE         | C → S → all    | `[sender:str][propId:4][pos:12][rot:16][vel:12][flags:1]`; owner only; flag `0x02` settled frees the prop |
| `0x05` | SOUND              | C → S → others | `[sender:str][sfx:1][pos:12][volume:4]`                          |
| `0x06` | MANIFEST           | C → S          | `[sender:str][count:2]{[key:str][halfExtents:12][mass:4]}*`      |
| `0x07` | PICKUP_REQ         | C → S          | `[sender:str][propId:4]`; distance checked; free or own prop     |
| `0x08` | PROP_RELEASE       | C → S          | `[sender:str][propId:4][vel:12][angVel:12][flags:1]`; thrower keeps simulating |
| `0x09` | MANIFEST_ACK       | S → C          | `[count:2][propId:4]*` in the client's manifest order            |
| `0x0A` | PROP_GRANT         | S → all        | `[propId:4][holder:str][flags:1]`; flag `0x01` — pushed, not held |
| `0x0B` | PROP_RELEASE_EVENT | S → all        | `[propId:4][owner:str][vel:12][angVel:12][flags:1]`; empty owner — free |
| `0x0C` | PROP_SNAPSHOT      | S → C          | `[count:2]{[propId:4][owner:str][held:1]}*` (not sent yet)       |
| `0x0D` | DISCONNECT         | C → S          | `[sender:str]`                                                   |
| `0x0E` | PLAYER_LEFT        | S → others     | `[id:str]`; on DISCONNECT and idle timeout                       |
| `0x0F` | PROP_PUSH          | C → S          | `[sender:str][propId:4]`; free prop pushed/hit → PROP_GRANT `0x01` |
| `0x10` | SET_NAME           | C → S          | `[sender:str][name:str]`; sanitized, up to 48 UTF-8 bytes        |
| `0x11` | PLAYER_NAME        | S → all / C    | `[id:str][name:str]`; to all on SET_NAME, plus the others' names to the author |

### Prop ownership

A prop has an owner who runs its physics and sends PROP_STATE; others follow it.

- **Free** — no owner. PICKUP_REQ gives it into the hands, PROP_PUSH gives its simulation.
- **Held** — in the owner's hands. PROP_RELEASE (throw/drop) keeps the owner as the simulator.
- **Simulated** — thrown or pushed; the owner sends PROP_STATE until the prop settles
  (flag `0x02`), then it is free. An owner silent for 3 s loses the prop.

## Build & Run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel                       # also runs smoke and unit tests
./build/virt-off-server --api v2                     # port 7777, threads = CPU cores
./build/virt-off-server --api v2 --port 8888 --threads 4
ctest --test-dir build -R integration_server         # two-client integration test
```

`--api v1` (the default) is the legacy numeric-id relay; the Unity client needs `--api v2`.

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

Every folder holds interfaces (`I*.hpp`) next to their implementations. A class depends only on
interfaces; concrete classes are created in the module classes (`*Module`) and `ServerBuilder`.

### How a datagram flows (V2)

```
UdpTransport ──► Server::on_datagram
                   ├─► observers (ActivityTracker, PositionTracker)   see every datagram
                   └─► PacketRouter ──► IPacketHandler of the type byte
                                          ├─ codec    parse the payload
                                          ├─ registry check / change server state
                                          └─ events   tell the clients (IBroadcaster)
UdpTransport tick (5 s) ──► periodic tasks (IdleSessionSweeper, SimulationSweeper)
```

### Folders

```
src/
├── main.cpp                 entry point: CLI → ServerFactory → worker threads
├── app/                     version-agnostic start-up
│   ├── IServer              Start / Stop of a server of any API version
│   ├── CliParser            --port, --threads, --api (table of options)
│   ├── ServerFactory        picks the v1 or v2 ServerBuilder
│   └── Server, ServerBuilder   legacy V1 server
├── core/                    ILogger, ConsoleLogger
├── net/                     transport shared by V1 and V2
│   ├── ITransport, UdpTransport     UDP socket on an asio strand, receive and tick callbacks
│   ├── IPacketRouter, PacketRouter  type byte → handler
│   └── ProtocolConstants            message type bytes
├── clients/, handlers/      legacy V1: numeric ids, CONNECT, plain relay
│
└── v2/                      V2 API — the one the Unity client uses
    ├── app/                 Server (plugs modules into the transport), ServerBuilder (composition root)
    ├── net/                 building blocks of every module
    │   ├── module/          IServerModule + what a module exposes: IPacketHandler, PacketRoute,
    │   │                    IPacketObserver, IPeriodicTask
    │   ├── wire/            wire format: WireReader, WireWriter, WireSizes; SenderStamp (real sender id)
    │   └── broadcast/       IBroadcaster: send to all / all but one / one user by id
    ├── clients/             UserInfo, IUserRepository: connected users, by id and by endpoint
    ├── core/                IIdGenerator: "player-<millis>" ids
    ├── relay/               RelayModule: VOICE, POSITION, SOUND → everybody else
    ├── names/               NameModule: SET_NAME in, PLAYER_NAME out
    │   ├── NameHandler      read → sanitize → store → announce → send the roster
    │   ├── codec/           PlayerNameReader, PlayerNameSanitizer, PlayerNameEncoder
    │   └── events/          NameBroadcaster (a name to all), NameRoster (all names to a newcomer)
    └── game/
        ├── GameConstants    distances, cooldowns, timeouts
        ├── Vec3             position type and distance
        ├── players/         PlayerModule: last POSITION of every player (PositionTracker)
        ├── combat/          CombatModule: ATTACK — AttackTargetReader, AttackGate (range, cooldown)
        ├── session/         SessionModule: CONNECT, DISCONNECT, activity, idle timeout, SessionTerminator
        └── props/           PropModule: shared props (ownership rules: see "Prop ownership")
            ├── model/       PropRecord, PropDescriptor, PropFlags
            ├── registry/    PropRepository (+ IPropSimulation), PropReach, SimulationSweeper
            ├── codec/       readers and encoders of MANIFEST, PICKUP/PUSH/RELEASE, PROP_STATE, MANIFEST_ACK,
            │                PROP_RELEASE_EVENT
            ├── events/      GrantAnnouncer (PROP_GRANT), PropReleaser (frees a leaver's props)
            └── handlers/    MANIFEST, PICKUP_REQ, PROP_PUSH, PROP_RELEASE, PROP_STATE
```

### Adding a feature

1. Create a folder under `v2/` (or `v2/game/`) with the interfaces, implementations and a `*Module`
   implementing `net::IServerModule` that returns its routes, observers and tasks.
2. Construct the module in `v2::ServerBuilder::build()`.
3. Add the message type to `net/ProtocolConstants.hpp` and the table above.

`Server` and the transport do not change.

## Development Rules

See `RULES.md` — interface-first design, SRP, composition over implementation.
