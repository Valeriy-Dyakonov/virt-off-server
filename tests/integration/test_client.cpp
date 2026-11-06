// Standalone UDP test client for multiplayer integration testing.
//
// Deliberately outside the server project code: it speaks only the raw wire
// protocol (control byte + payload) and does not include any server headers.
//
// Usage:
//   test_client --port N [--host H] [--connect] [--send voice|position|none]
//               [--payload TEXT] [--claim ID] [--repeat N] [--listen SECONDS]
//               [--disconnect] [--position] [--attack TARGET_ID] [--name NAME]
//
// --position sends POSITION at the origin right after connecting; --attack
// does the same and then sends ATTACK at TARGET_ID (the server only lets it
// through when both players' positions are known and close).
//
// --name sends SET_NAME with NAME right after connecting.
//
// Relayed packets are sent as [sender:str][payload]; --claim sets the sender
// id written by the client (the server must replace it with the real one).
//
// Output (stdout, one token per line):
//   CONNECTED <id>                 after the server assigns an id
//   RECV VOICE <sender> <payload>  on a relayed voice packet
//   RECV POSITION <sender> <payload>
//   RECV PLAYER_LEFT <id>          when another player leaves
//   RECV ATTACK <attacker> <target> on a relayed attack
//   RECV PLAYER_NAME <id> <name>   on a player name announcement
//   SENT ATTACK <target>           after sending the attack
//   SENT <kind> <payload>          after sending to the server
//   SENT DISCONNECT                after the graceful disconnect

#include <asio/ip/udp.hpp>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace {

/// Control byte: client -> server, request to connect.
constexpr uint8_t kMsgConnect = 0xFF;

/// Control byte: server -> client, assigned id.
constexpr uint8_t kMsgAssignId = 0xFE;

/// Control byte: voice packet, relayed to other clients.
constexpr uint8_t kMsgVoice = 0x01;

/// Control byte: position packet, relayed to other clients.
constexpr uint8_t kMsgPosition = 0x02;

/// Control byte: attack, validated by the server and broadcast to all.
constexpr uint8_t kMsgAttack = 0x03;

/// Control byte: client -> server, graceful disconnect.
constexpr uint8_t kMsgDisconnect = 0x0D;

/// POSITION body after the sender string: pos:12 + yaw:4 + flags:1.
constexpr std::size_t kPositionBodyBytes = 17;

/// ATTACK body after the target string: point:12 + impulse:12 + hitObject:1.
constexpr std::size_t kAttackTailBytes = 25;

/// Control byte: server -> others, a player left.
constexpr uint8_t kMsgPlayerLeft = 0x0E;

/// Control byte: client -> server, the player's display name.
constexpr uint8_t kMsgSetName = 0x10;

/// Control byte: server -> clients, a player's display name.
constexpr uint8_t kMsgPlayerName = 0x11;

/// Default number of seconds spent listening after connecting.
constexpr int kDefaultListenSeconds = 3;

/// Seconds to wait for the ASSIGN_ID reply before proceeding.
constexpr int kAssignTimeoutSeconds = 1;

/// Default number of times a message is sent (UDP is lossy).
constexpr int kDefaultRepeat = 3;

/// Pause between repeated sends, in milliseconds.
constexpr int kSendGapMs = 200;

/// Maximum datagram size we are willing to receive.
constexpr std::size_t kMaxDatagram = 2048;

using asio::ip::udp;

/// Fully resolved command-line options for the test client.
struct ClientOptions {
    std::string host = "127.0.0.1";
    uint16_t port = 0;
    bool connect = true;
    std::string send = "none";
    std::string payload = "ping";
    std::string claim = "unknown";
    int repeat = kDefaultRepeat;
    int listen_seconds = kDefaultListenSeconds;
    bool disconnect = false;
    bool position = false;
    std::string attack_target;
    std::string name;
};

/// Parses argv into ClientOptions; exits with usage on a missing --port.
ClientOptions parse_args(const int argc, char* argv[]) {
    ClientOptions opts;
    for (int i = 1; i < argc; ++i) {
        const std::string_view arg = argv[i];
        const bool has_value = (i + 1 < argc);
        if (arg == "--host" && has_value) {
            opts.host = argv[++i];
        } else if (arg == "--port" && has_value) {
            opts.port = static_cast<uint16_t>(std::atoi(argv[++i]));
        } else if (arg == "--send" && has_value) {
            opts.send = argv[++i];
        } else if (arg == "--payload" && has_value) {
            opts.payload = argv[++i];
        } else if (arg == "--claim" && has_value) {
            opts.claim = argv[++i];
        } else if (arg == "--disconnect") {
            opts.disconnect = true;
        } else if (arg == "--position") {
            opts.position = true;
        } else if (arg == "--attack" && has_value) {
            opts.attack_target = argv[++i];
            opts.position = true;
        } else if (arg == "--name" && has_value) {
            opts.name = argv[++i];
        } else if (arg == "--repeat" && has_value) {
            opts.repeat = std::atoi(argv[++i]);
        } else if (arg == "--listen" && has_value) {
            opts.listen_seconds = std::atoi(argv[++i]);
        } else if (arg == "--no-connect") {
            opts.connect = false;
        }
    }
    if (opts.port == 0) {
        std::cerr << "test_client: --port is required\n";
        std::exit(2);
    }
    return opts;
}

/// Maps a --send value to its control byte (0 when nothing is sent).
uint8_t send_kind_byte(const std::string& kind) {
    if (kind == "voice") {
        return kMsgVoice;
    }
    if (kind == "position") {
        return kMsgPosition;
    }
    return 0;
}

/// Sends one datagram to the server endpoint.
void send_datagram(udp::socket& sock, const udp::endpoint& server, const std::vector<uint8_t>& bytes) {
    asio::error_code ec;
    sock.send_to(asio::buffer(bytes), server, 0, ec);
}

/// Builds a payload datagram: control byte, [sender:str] with the claimed id, payload bytes.
std::vector<uint8_t> make_payload_datagram(const uint8_t kind, const std::string& claim, const std::string& payload) {
    std::vector<uint8_t> bytes;
    bytes.reserve(2 + claim.size() + payload.size());
    bytes.push_back(kind);
    bytes.push_back(static_cast<uint8_t>(claim.size()));
    bytes.insert(bytes.end(), claim.begin(), claim.end());
    bytes.insert(bytes.end(), payload.begin(), payload.end());
    return bytes;
}

/// Appends a [len:1][utf8] string.
void append_string(std::vector<uint8_t>& bytes, const std::string& value) {
    bytes.push_back(static_cast<uint8_t>(value.size()));
    bytes.insert(bytes.end(), value.begin(), value.end());
}

/// POSITION at the origin: [type][sender:str][pos:12][yaw:4][flags:1], all zero.
std::vector<uint8_t> make_position_datagram(const std::string& claim) {
    std::vector<uint8_t> bytes{kMsgPosition};
    append_string(bytes, claim);
    bytes.insert(bytes.end(), kPositionBodyBytes, 0);
    return bytes;
}

/// ATTACK: [type][attacker:str][kind:1][target:str][point:12][impulse:12][hitObject:1].
std::vector<uint8_t> make_attack_datagram(const std::string& claim, const std::string& target) {
    std::vector<uint8_t> bytes{kMsgAttack};
    append_string(bytes, claim);
    bytes.push_back(0);
    append_string(bytes, target);
    bytes.insert(bytes.end(), kAttackTailBytes, 0);
    return bytes;
}

/// SET_NAME: [type][sender:str][name:str].
std::vector<uint8_t> make_set_name_datagram(const std::string& claim, const std::string& name) {
    std::vector<uint8_t> bytes{kMsgSetName};
    append_string(bytes, claim);
    append_string(bytes, name);
    return bytes;
}

/// Reads the [len:1][utf8] string at `offset`; empty if it does not fit.
std::string read_string(const std::vector<uint8_t>& buf, const std::size_t n, const std::size_t offset) {
    if (offset >= n || offset + 1 + buf[offset] > n) {
        return {};
    }
    return std::string(buf.begin() + offset + 1, buf.begin() + offset + 1 + buf[offset]);
}

/// Prints the assigned id carried by an ASSIGN_ID datagram.
void report_assigned_id(const std::vector<uint8_t>& buf, const std::size_t n) {
    const std::string id(buf.begin() + 1, buf.begin() + n);
    std::cout << "CONNECTED " << id << '\n';
    std::cout.flush();
}

/// Prints a relayed packet (VOICE / POSITION) as "<sender> <payload>".
void report_relay(const std::vector<uint8_t>& buf, const std::size_t n) {
    const std::string sender = read_string(buf, n, 1);
    const std::size_t payload_offset = std::min<std::size_t>(n, 2 + sender.size());
    const std::string payload(buf.begin() + payload_offset, buf.begin() + n);
    if (buf[0] == kMsgVoice) {
        std::cout << "RECV VOICE " << sender << ' ' << payload << '\n';
    } else if (buf[0] == kMsgPosition) {
        std::cout << "RECV POSITION " << sender << ' ' << payload << '\n';
    }
    std::cout.flush();
}

/// Dispatches a received datagram.
void handle_datagram(const std::vector<uint8_t>& buf, const std::size_t n) {
    if (n == 0) {
        return;
    }
    if (buf[0] == kMsgAssignId) {
        report_assigned_id(buf, n);
    } else if (buf[0] == kMsgVoice || buf[0] == kMsgPosition) {
        report_relay(buf, n);
    } else if (buf[0] == kMsgAttack) {
        const std::string attacker = read_string(buf, n, 1);
        const std::string target = read_string(buf, n, 2 + attacker.size() + 1);
        std::cout << "RECV ATTACK " << attacker << ' ' << target << '\n';
        std::cout.flush();
    } else if (buf[0] == kMsgPlayerName) {
        const std::string id = read_string(buf, n, 1);
        const std::string name = read_string(buf, n, 2 + id.size());
        std::cout << "RECV PLAYER_NAME " << id << ' ' << name << '\n';
        std::cout.flush();
    } else if (buf[0] == kMsgPlayerLeft) {
        std::cout << "RECV PLAYER_LEFT " << read_string(buf, n, 1) << '\n';
        std::cout.flush();
    }
}

/// Receives and prints datagrams until the deadline elapses.
void receive_until(udp::socket& sock, const std::chrono::steady_clock::time_point deadline) {
    std::vector<uint8_t> buf(kMaxDatagram);
    while (std::chrono::steady_clock::now() < deadline) {
        asio::error_code ec;
        const std::size_t n = sock.receive(asio::buffer(buf), 0, ec);
        if (ec == asio::error::would_block) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }
        if (ec) {
            break;
        }
        handle_datagram(buf, n);
    }
}

/// Waits for the ASSIGN_ID reply, printing CONNECTED <id>. Returns whether the
/// assignment arrived before the deadline. Other datagrams are ignored.
bool wait_for_assignment(udp::socket& sock, const std::chrono::steady_clock::time_point deadline) {
    std::vector<uint8_t> buf(kMaxDatagram);
    while (std::chrono::steady_clock::now() < deadline) {
        asio::error_code ec;
        const std::size_t n = sock.receive(asio::buffer(buf), 0, ec);
        if (ec == asio::error::would_block) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }
        if (ec) {
            return false;
        }
        if (n > 0 && buf[0] == kMsgAssignId) {
            report_assigned_id(buf, n);
            return true;
        }
    }
    return false;
}

}  // namespace

int main(const int argc, char* argv[]) {
    const ClientOptions opts = parse_args(argc, argv);

    asio::io_context io;
    udp::socket sock(io);
    sock.open(udp::v4());
    sock.bind(udp::endpoint(udp::v4(), 0));
    sock.non_blocking(true);

    const udp::endpoint server(asio::ip::make_address(opts.host), opts.port);

    if (opts.connect) {
        send_datagram(sock, server, std::vector<uint8_t>{kMsgConnect});
        wait_for_assignment(sock, std::chrono::steady_clock::now() + std::chrono::seconds(kAssignTimeoutSeconds));
    }

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(opts.listen_seconds);

    if (opts.position) {
        for (int i = 0; i < opts.repeat; ++i) {
            send_datagram(sock, server, make_position_datagram(opts.claim));
            std::this_thread::sleep_for(std::chrono::milliseconds(kSendGapMs));
        }
    }

    if (!opts.name.empty()) {
        for (int i = 0; i < opts.repeat; ++i) {
            send_datagram(sock, server, make_set_name_datagram(opts.claim, opts.name));
            std::this_thread::sleep_for(std::chrono::milliseconds(kSendGapMs));
        }
    }

    if (!opts.attack_target.empty()) {
        send_datagram(sock, server, make_attack_datagram(opts.claim, opts.attack_target));
        std::cout << "SENT ATTACK " << opts.attack_target << '\n';
        std::cout.flush();
    }

    const uint8_t kind = send_kind_byte(opts.send);
    if (kind != 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(kSendGapMs));
        for (int i = 0; i < opts.repeat; ++i) {
            send_datagram(sock, server, make_payload_datagram(kind, opts.claim, opts.payload));
            std::cout << "SENT " << opts.send << ' ' << opts.payload << '\n';
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(kSendGapMs));
        }
    }

    receive_until(sock, deadline);

    if (opts.disconnect) {
        send_datagram(sock, server, std::vector<uint8_t>{kMsgDisconnect});
        std::cout << "SENT DISCONNECT\n";
        std::cout.flush();
    }
    return 0;
}
