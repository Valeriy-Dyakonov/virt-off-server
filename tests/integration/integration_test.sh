#!/usr/bin/env bash
#
# Multiplayer integration test for virt-off-server (V2 API).
#
# Usage: integration_test.sh <server-binary> <test-client-binary>
#
# Starts a real v2 server, connects a listener and a sender, and asserts that:
#   - both clients get a "player-..." id assigned;
#   - the listener receives the sender's relayed voice packet;
#   - the relayed packet carries the sender's real id, not the one it claimed;
#   - the sender does NOT receive its own relayed packet;
#   - the listener is told PLAYER_LEFT after the sender disconnects;
#   - an attack between two players with known positions reaches the target.
#
# No framework: plain checks, non-zero exit on failure so CTest can report it.
set -u

SERVER_BIN="${1:?usage: integration_test.sh <server-binary> <test-client-binary>}"
CLIENT_BIN="${2:?usage: integration_test.sh <server-binary> <test-client-binary>}"

readonly PORT=39221
readonly ID_PREFIX="CONNECTED player-"
readonly VOICE_PAYLOAD="hello-voice"
readonly SPOOFED_ID="spoofed-id"

FAILURES=0
WORK_DIR="$(mktemp -d)"
SERVER_PID=""

# Stops the server (if running) and removes the work directory.
cleanup() {
    if [ -n "$SERVER_PID" ]; then
        kill -TERM "$SERVER_PID" 2>/dev/null
        wait "$SERVER_PID" 2>/dev/null
    fi
    rm -rf "$WORK_DIR"
}
trap cleanup EXIT

# Asserts that a log file contains the expected substring.
check_contains() {
    local log_file="$1"
    local needle="$2"
    local label="$3"

    if grep -qF -- "$needle" "$log_file"; then
        echo "PASS: $label"
    else
        echo "FAIL: $label (missing: '$needle')"
        FAILURES=$((FAILURES + 1))
    fi
}

# Asserts that a log file does NOT contain the given substring.
check_absent() {
    local log_file="$1"
    local needle="$2"
    local label="$3"

    if grep -qF -- "$needle" "$log_file"; then
        echo "FAIL: $label (unexpected: '$needle')"
        FAILURES=$((FAILURES + 1))
    else
        echo "PASS: $label"
    fi
}

"$SERVER_BIN" --port "$PORT" --threads 2 --api v2 >"$WORK_DIR/server.log" 2>&1 &
SERVER_PID=$!
sleep 1

"$CLIENT_BIN" --port "$PORT" --position --listen 6 >"$WORK_DIR/listener.log" 2>&1 &
LISTENER_PID=$!
sleep 1

LISTENER_ID="$(grep -m1 "^CONNECTED " "$WORK_DIR/listener.log" | cut -d' ' -f2)"

"$CLIENT_BIN" --port "$PORT" --send voice --payload "$VOICE_PAYLOAD" --claim "$SPOOFED_ID" \
    --attack "$LISTENER_ID" --listen 2 --disconnect \
    >"$WORK_DIR/sender.log" 2>&1 &
SENDER_PID=$!

wait "$LISTENER_PID"
wait "$SENDER_PID"

SENDER_ID="$(grep -m1 "^CONNECTED " "$WORK_DIR/sender.log" | cut -d' ' -f2)"

check_contains "$WORK_DIR/listener.log" "$ID_PREFIX" "listener: assigned id"
check_contains "$WORK_DIR/sender.log" "$ID_PREFIX" "sender: assigned id"
check_contains "$WORK_DIR/listener.log" "RECV VOICE $SENDER_ID $VOICE_PAYLOAD" "listener: received voice with the real sender id"
check_absent "$WORK_DIR/listener.log" "$SPOOFED_ID" "listener: claimed sender id was replaced"
check_absent "$WORK_DIR/sender.log" "RECV VOICE" "sender: did not receive its own voice"
check_contains "$WORK_DIR/listener.log" "RECV PLAYER_LEFT $SENDER_ID" "listener: told that the sender left"
check_contains "$WORK_DIR/listener.log" "RECV ATTACK $SENDER_ID $LISTENER_ID" "listener: received the attack"

if [ "$FAILURES" -eq 0 ]; then
    echo "ALL INTEGRATION TESTS PASSED"
    exit 0
fi

echo "$FAILURES CHECK(S) FAILED"
exit 1
