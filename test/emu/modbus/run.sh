#!/usr/bin/env bash
# Run the Modbus emulator tests: start the devices, point the LH tests at them, stop
# them again. Linux (WSL included); needs socat, python3, pyserial and pymodbus.
#
#     test/emu/modbus/run.sh BUILD_DIR [gtest filter]
#
# Both directions go through test/src/lh/os/net/modbus_emu.cpp (lh_test_os): the LH
# client against device.py and reference.py, the LH server against client.py. RTU
# runs over a socat pseudo-terminal pair at every rate in RATES; a pty has no parity
# bit to set (WSL refuses one), so the parity of a real line is not exercised here.
set -u
BUILD=$(cd "$1" && pwd)
FILTER=${2:-'lh_mb_emu*:lh_mb_ref*'}
HERE=$(cd "$(dirname "$0")" && pwd)
TMP=$(mktemp -d)
PIDS=()
RATES="9600:1 19200:1 115200:2"
STATUS=0

cleanup() {
    for p in "${PIDS[@]}"; do kill "$p" 2>/dev/null; done
    wait 2>/dev/null
    rm -rf "$TMP"
}
trap cleanup EXIT

pair() { # pair NAME: two linked ptys, $TMP/NAME.a (LH) and $TMP/NAME.b (device)
    socat "pty,raw,echo=0,link=$TMP/$1.a" "pty,raw,echo=0,link=$TMP/$1.b" 2>/dev/null &
    PIDS+=($!)
    for _ in $(seq 50); do [ -e "$TMP/$1.b" ] && return; sleep 0.05; done
    echo "socat did not make $1" >&2
    exit 1
}

free_port() { python3 -c 'import socket; s=socket.socket(); s.bind(("127.0.0.1",0)); print(s.getsockname()[1])'; }

pair emu
EMU_TCP=$(free_port)
python3 "$HERE/device.py" rtu "$TMP/emu.b" & PIDS+=($!)
python3 "$HERE/device.py" tcp "$EMU_TCP" & PIDS+=($!)
REF_TCP=$(free_port)
python3 -W ignore "$HERE/reference.py" tcp "$REF_TCP" 2>/dev/null & PIDS+=($!)

for rate in $RATES; do
    baud=${rate%:*}
    stop=${rate#*:}
    pair "ref$baud"
    python3 -W ignore "$HERE/reference.py" rtu "$TMP/ref$baud.b" "$baud" N "$stop" 2>/dev/null & PIDS+=($!)
    sleep 1.5
    echo "== RTU $baud 8N$stop, TCP =="
    LH_TEST_MB_EMU_RTU="$TMP/emu.a" LH_TEST_MB_EMU_TCP="$EMU_TCP" \
    LH_TEST_MB_REF_RTU="$TMP/ref$baud.a" LH_TEST_MB_REF_BAUD="$baud" LH_TEST_MB_REF_STOP="$stop" \
    LH_TEST_MB_REF_TCP="$REF_TCP" \
        "$BUILD/test/lh_test_os" --gtest_filter="$FILTER" --gtest_brief=1 || STATUS=1
    # device.py and TCP do not depend on the rate: once is enough.
    FILTER="lh_mb_ref/*/rtu"
done

# The other direction: the LH server, and client.py as its masters.
pair serve
SERVE_TCP=$(free_port)
echo "== LH server: pymodbus and raw masters, RTU then TCP =="
LH_TEST_MB_SERVE_RTU="$TMP/serve.a" LH_TEST_MB_SERVE_TCP="$SERVE_TCP"     "$BUILD/test/lh_test_os" --gtest_filter='lh_mb_serve*' --gtest_brief=1 &
SERVER=$!
python3 -W ignore "$HERE/client.py" rtu "$TMP/serve.b" || STATUS=1
python3 -W ignore "$HERE/client.py" tcp "$SERVE_TCP" || STATUS=1
wait "$SERVER" || STATUS=1
exit $STATUS
