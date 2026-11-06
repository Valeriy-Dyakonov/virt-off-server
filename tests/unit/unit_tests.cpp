// Unit tests: wire codecs, packet parsers, prop ownership and attack rules.
//
// No framework: each check prints PASS/FAIL, the exit code is the number of failures.
#include "v2/game/combat/AttackGate.hpp"
#include "v2/game/combat/AttackTargetReader.hpp"
#include "v2/game/players/PlayerState.hpp"
#include "v2/game/props/codec/ManifestAckEncoder.hpp"
#include "v2/game/props/codec/ManifestReader.hpp"
#include "v2/game/props/codec/PropIdReader.hpp"
#include "v2/game/props/codec/PropStateReader.hpp"
#include "v2/game/props/codec/ReleaseEventEncoder.hpp"
#include "v2/game/props/model/PropFlags.hpp"
#include "v2/game/props/registry/PropRepository.hpp"
#include "v2/names/codec/PlayerNameEncoder.hpp"
#include "v2/names/codec/PlayerNameReader.hpp"
#include "v2/names/codec/PlayerNameSanitizer.hpp"
#include "v2/net/wire/WireReader.hpp"
#include "v2/net/wire/WireWriter.hpp"

#include <array>
#include <chrono>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace {

int failures = 0;

void check(const bool condition, const std::string& label) {
    std::cout << (condition ? "PASS: " : "FAIL: ") << label << '\n';
    if (!condition) {
        ++failures;
    }
}

void append_string(std::vector<uint8_t>& bytes, const std::string& value) {
    bytes.push_back(static_cast<uint8_t>(value.size()));
    bytes.insert(bytes.end(), value.begin(), value.end());
}

template <typename T>
void append_raw(std::vector<uint8_t>& bytes, const T value) {
    std::array<uint8_t, sizeof(T)> raw{};
    std::memcpy(raw.data(), &value, sizeof(T));
    bytes.insert(bytes.end(), raw.begin(), raw.end());
}

void append_u32(std::vector<uint8_t>& bytes, const uint32_t value) {
    append_raw(bytes, value);
}

void append_floats(std::vector<uint8_t>& bytes, const std::initializer_list<float> values) {
    for (const float value : values) {
        append_raw(bytes, value);
    }
}

/// Repository with two props registered from a manifest.
v2::game::PropRepository make_repository() {
    v2::game::PropRepository repo;
    std::vector<v2::game::PropDescriptor> manifest(2);
    manifest[0].key = "Chair";
    manifest[1].key = "Table";
    check(repo.register_manifest(manifest) == std::vector<uint32_t>{1, 2}, "manifest: ids assigned in order");
    return repo;
}

void test_simulation_ownership() {
    using namespace std::chrono_literals;
    const auto t0 = std::chrono::steady_clock::now();
    auto repo = make_repository();

    check(repo.try_grant(1, "a"), "grant: free prop goes to the hands");
    check(!repo.try_grant(1, "b"), "grant: held prop is refused to another player");
    check(!repo.begin_simulation(1, "b", t0), "throw: only the owner can throw");
    check(repo.begin_simulation(1, "a", t0), "throw: the holder keeps simulating");
    check(repo.get(1)->owner_id == "a" && !repo.get(1)->held, "throw: owner kept, not held");
    check(!repo.try_grant(1, "b"), "grant: a flying prop is refused to another player");
    check(!repo.try_simulate(1, "b", t0), "push: a flying prop is refused to another player");
    check(repo.try_grant(1, "a"), "grant: the thrower can catch its own prop");
    check(repo.begin_simulation(1, "a", t0), "throw again");
    check(!repo.end_simulation(1, "b"), "settle: only the owner can free the prop");
    check(repo.end_simulation(1, "a"), "settle: the owner frees the prop");
    check(repo.get(1)->owner_id.empty(), "settle: no owner left");

    check(repo.try_simulate(2, "b", t0), "push: free prop goes to the pusher");
    check(repo.try_simulate(2, "b", t0), "push: the same pusher may push again");
    check(!repo.get(2)->held, "push: the prop is not held");
    check(repo.try_grant(2, "b"), "grant: the pusher can pick it up");
    check(!repo.try_simulate(2, "b", t0), "push: a held prop is not pushed");
    check(!repo.try_simulate(99, "b", t0), "push: unknown prop");

    repo.touch(2, t0);
    check(repo.expire(t0 + 1s, 3s).empty(), "expire: active owner keeps the prop");
    const auto expired = repo.expire(t0 + 4s, 3s);
    check(expired.size() == 1 && expired[0] == 2, "expire: silent owner loses the prop");
    check(repo.get(2)->owner_id.empty() && !repo.get(2)->held, "expire: prop is free");
}

void test_prop_readers() {
    std::vector<uint8_t> pickup;
    append_string(pickup, "player-1");
    append_u32(pickup, 7);
    check(v2::game::PropIdReader{}.read(pickup) == 7u, "prop id reader: reads the id after the sender");
    pickup.pop_back();
    check(!v2::game::PropIdReader{}.read(pickup).has_value(), "prop id reader: truncated payload");

    std::vector<uint8_t> state;
    append_string(state, "p");
    append_u32(state, 3);
    append_floats(state, {1.f, 2.f, 3.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f});
    state.push_back(std::to_integer<uint8_t>(v2::game::kPropStateFlagSettled));
    const auto fields = v2::game::PropStateReader{}.read(state);
    check(fields.has_value() && fields->prop_id == 3 && fields->position[2] == 3.f &&
              fields->flags == v2::game::kPropStateFlagSettled,
          "prop state reader: id, position and flags");
    state.pop_back();
    check(!v2::game::PropStateReader{}.read(state).has_value(), "prop state reader: truncated payload");

    std::vector<uint8_t> release;
    append_string(release, "claimed");
    append_u32(release, 5);
    append_floats(release, {1.f, 0.f, 0.f, 0.f, 0.f, 0.f});
    release.push_back(0);
    const auto event = v2::game::ReleaseEventEncoder{}.encode_throw(release, "real");
    std::vector<uint8_t> expected;
    append_u32(expected, 5);
    append_string(expected, "real");
    append_floats(expected, {1.f, 0.f, 0.f, 0.f, 0.f, 0.f});
    expected.push_back(0);
    check(event.has_value() && *event == expected, "release event: owner replaced, tail copied");

    std::vector<uint8_t> free_event;
    append_u32(free_event, 9);
    append_string(free_event, "");
    append_floats(free_event, {0.f, 0.f, 0.f, 0.f, 0.f, 0.f});
    free_event.push_back(std::to_integer<uint8_t>(v2::game::kReleaseFlagSettled));
    check(v2::game::ReleaseEventEncoder{}.encode_free(9) == free_event, "release event: free prop");

    std::vector<uint8_t> manifest;
    append_string(manifest, "p");
    append_raw<uint16_t>(manifest, 2);
    append_string(manifest, "Chair");
    append_floats(manifest, {0.5f, 1.f, 0.5f, 7.f});
    append_string(manifest, "Tab");  // truncated: no extents and mass
    const auto descriptors = v2::game::ManifestReader{}.read(manifest);
    check(descriptors.has_value() && descriptors->size() == 1 && (*descriptors)[0].key == "Chair" &&
              (*descriptors)[0].half_extents[1] == 1.f && (*descriptors)[0].mass == 7.f,
          "manifest reader: stops at a truncated entry");
    check(!v2::game::ManifestReader{}.read(std::vector<uint8_t>{1, 'p', 2}).has_value(),
          "manifest reader: truncated header");

    std::vector<uint8_t> ack;
    append_raw<uint16_t>(ack, 2);
    append_u32(ack, 4);
    append_u32(ack, 1);
    check(v2::game::ManifestAckEncoder{}.encode({4, 1}) == ack, "manifest ack: count and ids");
}

void test_wire() {
    const auto bytes = v2::net::WireWriter{}.u8(7).u16(513).u32(70000).str("abc").take();
    v2::net::WireReader reader(bytes);
    check(reader.u8() == 7 && reader.u16() == 513 && reader.u32() == 70000 && reader.str() == "abc" && reader.ok() &&
              reader.rest().empty(),
          "wire: writer and reader agree");
    reader.u8();
    check(!reader.ok(), "wire: reading past the end fails");

    v2::net::WireWriter writer;
    writer.str(std::string(256, 'x'));
    check(!writer.ok() && writer.take().empty(), "wire: a string longer than 255 bytes is refused");
}

void test_combat() {
    using namespace std::chrono_literals;
    std::vector<uint8_t> attack;
    append_string(attack, "claimed");
    attack.push_back(1);  // kind
    append_string(attack, "player-2");
    check(v2::game::AttackTargetReader{}.read(attack) == std::string("player-2"), "attack reader: target id");
    attack.pop_back();
    check(!v2::game::AttackTargetReader{}.read(attack).has_value(), "attack reader: truncated payload");

    v2::game::PlayerState players;
    players.update("a", {0.f, 0.f, 0.f});
    players.update("b", {3.f, 4.f, 0.f});
    players.update("far", {100.f, 0.f, 0.f});
    v2::game::AttackGate gate(players);
    const auto t0 = std::chrono::steady_clock::now();
    check(gate.allow_attack("a", "b", t0), "attack gate: target within range");
    check(!gate.allow_attack("a", "b", t0 + 100ms), "attack gate: cooldown");
    check(gate.allow_attack("a", "b", t0 + 600ms), "attack gate: cooldown over");
    check(!gate.allow_attack("a", "far", t0), "attack gate: target out of range");
    check(!gate.allow_attack("a", "ghost", t0), "attack gate: unknown target");
}

void test_names() {
    const v2::names::PlayerNameSanitizer sanitizer;
    check(sanitizer.sanitize("  Vasya  ") == std::string("Vasya"), "name: spaces trimmed");
    check(sanitizer.sanitize("Ва\x01ся\n") == std::string("Вася"), "name: control characters dropped");
    check(!sanitizer.sanitize("   ").has_value(), "name: blank name refused");
    check(!sanitizer.sanitize("\xFF\xFE").has_value(), "name: invalid UTF-8 refused");
    check(sanitizer.sanitize("A\xC0\xAF" "B") == std::string("AB"), "name: overlong sequence dropped");

    // 26 Cyrillic letters = 52 bytes: cut to 24 letters (48 bytes).
    const auto long_name = sanitizer.sanitize("АААААААААААААААААААААААААА");
    check(long_name.has_value() && long_name->size() == 48, "name: cut to 48 bytes");

    // "x" + 24 Cyrillic letters = 49 bytes: the last letter does not fit and is dropped whole.
    const auto odd_name = sanitizer.sanitize("xББББББББББББББББББББББББ");
    check(odd_name.has_value() && odd_name->size() == 47, "name: a letter that does not fit is not split");

    std::vector<uint8_t> set_name;
    append_string(set_name, "claimed");
    append_string(set_name, "Вася");
    check(v2::names::PlayerNameReader{}.read(set_name) == std::string("Вася"), "name reader: reads the name");
    set_name.pop_back();
    check(!v2::names::PlayerNameReader{}.read(set_name).has_value(), "name reader: truncated payload");

    std::vector<uint8_t> expected;
    append_string(expected, "player-1");
    append_string(expected, "Вася");
    check(v2::names::PlayerNameEncoder{}.encode("player-1", "Вася") == expected, "name encoder: [id][name]");
}

}  // namespace

int main() {
    test_simulation_ownership();
    test_prop_readers();
    test_wire();
    test_combat();
    test_names();

    std::cout << (failures == 0 ? "ALL UNIT TESTS PASSED" : std::to_string(failures) + " CHECK(S) FAILED") << '\n';
    return failures;
}
