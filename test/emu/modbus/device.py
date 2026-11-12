#!/usr/bin/env python3
"""A Modbus device that misbehaves on request: the other end of the LH emulator tests.

The tests in ``test/src/lh/os/net/modbus_emu.cpp`` drive this device through a
pseudo-terminal pair (``socat``) for RTU, or a TCP socket for MBAP. It answers
function codes 0x03, 0x04, 0x06 and 0x10 for one unit (17 by default) over a full
65536-register map, and it turns any answer into a fault the test asks for:

    unit 247, "write one register" (0x06) at address 0 with value N

arms fault N for the next question to the device; the device then goes back to
normal. Unit 247 answers that write like any device would, so the test sends it
through the very link it is testing. The faults are listed in ``FAULTS``.

The map is plain arithmetic so a test can check a value without asking for it:
holding register ``a`` starts at ``(a * 3) & 0xFFFF``; input register ``a`` is
``a ^ 0x5A5A`` and never changes.

Run ``device.py rtu /dev/pts/N`` or ``device.py tcp PORT``; it serves until killed.
It writes one line per question to stderr with ``-v``.
"""

import socket
import struct
import sys
import time

import serial

UNIT = 17
CONTROL = 247

FC_READ_HOLDING = 0x03
FC_READ_INPUT = 0x04
FC_WRITE_ONE = 0x06
FC_WRITE_MANY = 0x10

# Fault numbers: the value written to unit 247, address 0.
FAULTS = {
    0: "none",
    1: "bad_crc",  # the answer with its CRC broken
    2: "other_unit_only",  # a whole, valid answer -- from unit 18
    3: "other_unit_then_ours_gap",  # unit 18's answer, 30 ms of silence, ours
    4: "other_unit_then_ours_tight",  # the two back to back, no silence between
    5: "byte_by_byte",  # ours, one byte every 1 ms
    6: "silence",  # nothing at all
    7: "late",  # ours, 600 ms after the question
    8: "trailing_junk",  # ours, then three bytes that are no frame
    9: "junk_prefix",  # two bytes of noise, then ours
    10: "truncated",  # ours less its last three bytes
    11: "count_more",  # one word more than asked, byte count to match
    12: "count_fewer",  # one word fewer than asked, byte count to match
    13: "count_odd",  # an odd byte count
    14: "exception_04",  # "server device failure"
    15: "exception_06",  # "server device busy"
    16: "wrong_code",  # the answer with the other read's function code
    17: "echo_wrong_address",  # a write echoed with address + 1
    18: "echo_wrong_value",  # a 0x06 echoed with value + 1
    19: "duplicate",  # ours, twice, 30 ms apart
    20: "split_pause",  # ours, with 50 ms of silence in its middle
    21: "count_lies_long",  # byte count says 2 more words than are sent
    22: "mbap_wrong_tx",  # MBAP: the answer under transaction id + 1
    23: "mbap_bad_protocol",  # MBAP: protocol id 1
    24: "mbap_late_then_ours",  # MBAP: answer late; next question gets both
    25: "close",  # MBAP: close the connection instead of answering
}


def crc16(data: bytes) -> int:
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ 0xA001 if crc & 1 else crc >> 1
    return crc


def rtu(unit: int, pdu: bytes) -> bytes:
    body = bytes([unit]) + pdu
    return body + struct.pack("<H", crc16(body))


class Map:
    def __init__(self):
        self.holding = [(a * 3) & 0xFFFF for a in range(65536)]

    def input(self, a: int) -> int:
        return a ^ 0x5A5A


def exception(fc: int, code: int) -> bytes:
    return bytes([fc | 0x80, code])


def answer(regs: Map, pdu: bytes) -> bytes:
    """The PDU a conforming device answers @p pdu with."""
    fc = pdu[0]
    if fc in (FC_READ_HOLDING, FC_READ_INPUT):
        if len(pdu) != 5:
            return exception(fc, 3)
        address, count = struct.unpack(">HH", pdu[1:5])
        if count < 1 or count > 125:
            return exception(fc, 3)
        if address + count > 65536:
            return exception(fc, 2)
        if fc == FC_READ_HOLDING:
            words = regs.holding[address : address + count]
        else:
            words = [regs.input(a) for a in range(address, address + count)]
        return bytes([fc, count * 2]) + b"".join(struct.pack(">H", w) for w in words)
    if fc == FC_WRITE_ONE:
        if len(pdu) != 5:
            return exception(fc, 3)
        address, value = struct.unpack(">HH", pdu[1:5])
        regs.holding[address] = value
        return pdu
    if fc == FC_WRITE_MANY:
        if len(pdu) < 6:
            return exception(fc, 3)
        address, count, nbytes = struct.unpack(">HHB", pdu[1:6])
        if count < 1 or count > 123 or nbytes != count * 2 or len(pdu) != 6 + nbytes:
            return exception(fc, 3)
        if address + count > 65536:
            return exception(fc, 2)
        for i in range(count):
            regs.holding[address + i] = struct.unpack(">H", pdu[6 + 2 * i : 8 + 2 * i])[0]
        return pdu[:5]
    return exception(fc, 1)


def request_length(frame: bytes) -> int:
    """How long the RTU request starting @p frame is; 0 while it cannot be told yet."""
    if len(frame) < 2:
        return 0
    fc = frame[1]
    if fc in (FC_READ_HOLDING, FC_READ_INPUT, FC_WRITE_ONE):
        return 8
    if fc == FC_WRITE_MANY:
        return 9 + frame[6] if len(frame) >= 7 else 0
    return -1  # a code this device does not size: wait for silence


def faulted(fault: int, unit: int, pdu: bytes, good: bytes, framing) -> list:
    """The list of (delay_s, bytes) that fault @p fault turns the answer @p good into."""
    wire = framing(unit, good)
    fc = good[0] & 0x7F
    is_read = fc in (FC_READ_HOLDING, FC_READ_INPUT) and not good[0] & 0x80
    name = FAULTS.get(fault, "none")
    if name == "bad_crc":
        return [(0, wire[:-1] + bytes([wire[-1] ^ 0xFF]))]
    if name == "other_unit_only":
        return [(0, framing(18, good))]
    if name == "other_unit_then_ours_gap":
        return [(0, framing(18, good)), (0.03, wire)]
    if name == "other_unit_then_ours_tight":
        return [(0, framing(18, good) + wire)]
    if name == "byte_by_byte":
        return [(0.001 if i else 0, wire[i : i + 1]) for i in range(len(wire))]
    if name == "silence":
        return []
    if name == "late":
        return [(0.6, wire)]
    if name == "trailing_junk":
        return [(0, wire + b"\x00\xff\x55")]
    if name == "junk_prefix":
        return [(0, b"\xaa\x13" + wire)]
    if name == "truncated":
        return [(0, wire[:-3])]
    if name in ("count_more", "count_fewer", "count_odd", "count_lies_long") and is_read:
        words = good[2:]
        if name == "count_more":
            p = bytes([good[0], len(words) + 2]) + words + b"\x12\x34"
        elif name == "count_fewer":
            p = bytes([good[0], len(words) - 2]) + words[:-2]
        elif name == "count_odd":
            p = bytes([good[0], len(words) - 1]) + words[:-1]
        else:
            p = bytes([good[0], len(words) + 4]) + words
        return [(0, framing(unit, p))]
    if name.startswith("exception_"):
        return [(0, framing(unit, exception(pdu[0], int(name[-2:], 16))))]
    if name == "wrong_code" and is_read:
        other = FC_READ_INPUT if fc == FC_READ_HOLDING else FC_READ_HOLDING
        return [(0, framing(unit, bytes([other]) + good[1:]))]
    if name == "echo_wrong_address" and fc in (FC_WRITE_ONE, FC_WRITE_MANY):
        a = (struct.unpack(">H", good[1:3])[0] + 1) & 0xFFFF
        return [(0, framing(unit, good[:1] + struct.pack(">H", a) + good[3:]))]
    if name == "echo_wrong_value" and fc == FC_WRITE_ONE:
        v = (struct.unpack(">H", good[3:5])[0] + 1) & 0xFFFF
        return [(0, framing(unit, good[:3] + struct.pack(">H", v)))]
    if name == "duplicate":
        return [(0, wire), (0.03, wire)]
    if name == "split_pause":
        half = len(wire) // 2
        return [(0, wire[:half]), (0.05, wire[half:])]
    return [(0, wire)]


class Device:
    def __init__(self, verbose: bool):
        self.regs = Map()
        self.fault = 0
        self.verbose = verbose

    def log(self, *args):
        if self.verbose:
            print(*args, file=sys.stderr, flush=True)

    def handle(self, unit: int, pdu: bytes, framing) -> list:
        """Everything the device puts on the wire for one question, as (delay, bytes)."""
        if unit == CONTROL:
            if pdu[0] == FC_WRITE_ONE and len(pdu) == 5:
                address, value = struct.unpack(">HH", pdu[1:5])
                if address == 0:
                    self.fault = value
                    self.log("arm", FAULTS.get(value, value))
                    return [(0, framing(unit, pdu))]
            return [(0, framing(unit, exception(pdu[0], 1)))]
        if unit not in (UNIT, 0):
            return []  # another device's question: not ours to answer
        good = answer(self.regs, pdu)
        if unit == 0:
            self.log("broadcast", pdu.hex())
            return []  # a broadcast is done and never answered
        fault, self.fault = self.fault, 0
        self.log("ask", pdu.hex(), "fault", FAULTS.get(fault, fault))
        return faulted(fault, unit, pdu, good, framing)


def send(out, plan) -> None:
    for delay, data in plan:
        if delay:
            time.sleep(delay)
        out(data)


def serve_rtu(path: str, device: Device) -> None:
    line = serial.Serial(path, 9600, timeout=0)
    held = b""
    last = time.monotonic()
    while True:
        chunk = line.read(512)
        now = time.monotonic()
        if chunk:
            if held and now - last > 0.02:
                device.log("silence drops", held.hex())
                held = b""  # a frame does not continue across silence
            held += chunk
            last = now
        elif held and now - last > 0.02:
            device.log("dropped", held.hex())
            held = b""
        while held:
            need = request_length(held)
            if need == 0 or (need > 0 and len(held) < need):
                break
            if need < 0:
                need = len(held)  # unknown code: wait for silence, then take it all
                if now - last < 0.005:
                    break
            frame, held = held[:need], held[need:]
            if crc16(frame[:-2]) != struct.unpack("<H", frame[-2:])[0]:
                device.log("bad crc", frame.hex())
                held = b""
                break
            send(line.write, device.handle(frame[0], frame[1:-2], rtu))
        if not chunk:
            time.sleep(0.0005)


def mbap_framing(tx_box):
    def framing(unit, pdu):
        tx = tx_box[0]
        return struct.pack(">HHHB", tx, 0, len(pdu) + 1, unit) + pdu

    return framing


def serve_tcp(port: int, device: Device) -> None:
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(("127.0.0.1", port))
    server.listen(4)
    while True:
        conn, _ = server.accept()
        conn.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        held = b""
        pending = b""  # a late answer that goes out with the next one
        try:
            while True:
                chunk = conn.recv(1024)
                if not chunk:
                    break
                held += chunk
                while len(held) >= 7:
                    tx, proto, length, unit = struct.unpack(">HHHB", held[:7])
                    if len(held) < 6 + length:
                        break
                    pdu, held = held[7 : 6 + length], held[6 + length :]
                    box = [tx]
                    fault = device.fault if unit != CONTROL else 0
                    name = FAULTS.get(fault, "none")
                    if name == "close":
                        device.fault = 0
                        raise ConnectionResetError
                    if name == "mbap_wrong_tx":
                        box[0] = (tx + 1) & 0xFFFF
                        device.fault = 0
                    plan = device.handle(unit, pdu, mbap_framing(box))
                    if name == "mbap_bad_protocol":
                        plan = [(d, b[:2] + b"\x00\x01" + b[4:]) for d, b in plan]
                    if name == "mbap_late_then_ours":
                        pending = b"".join(b for _, b in plan)
                        continue
                    if pending:
                        plan = [(0, pending + b"".join(b for _, b in plan))]
                        pending = b""
                    send(conn.sendall, plan)
        except (ConnectionResetError, BrokenPipeError):
            pass
        conn.close()


def main() -> None:
    args = [a for a in sys.argv[1:] if a != "-v"]
    device = Device("-v" in sys.argv)
    if args[0] == "rtu":
        serve_rtu(args[1], device)
    else:
        serve_tcp(int(args[1]), device)


if __name__ == "__main__":
    main()
