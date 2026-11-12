#!/usr/bin/env python3
"""Masters LH did not write, against the LH server (lh_mb_server_t).

The LH side is the test ``lh_mb_serve`` in ``test/src/lh/os/net/modbus_emu.cpp``:
an LH server at unit 17 over the map device.py has (holding ``a`` starts at
``(a * 3) & 0xFFFF``, input ``a`` is ``a ^ 0x5A5A``). This script talks to it in
two ways:

- through **pymodbus**, an independent master: every function, both ends of the
  map, every refusal, and two thousand random questions checked against a model;
- through **raw bytes**, a master that misbehaves: broken CRCs, other units,
  broadcasts, requests in pieces, requests broken by silence, unknown codes,
  lying byte counts, pipelined and malformed Modbus TCP.

At the end it writes 0xDEAD to register 0xFFFE, which tells the LH server to stop.
Exit status 0 when every check passed; each failure is printed.

    client.py rtu /dev/pts/N
    client.py tcp PORT
"""

import logging
import random
import socket
import struct
import sys
import time

import serial
from pymodbus.client import ModbusSerialClient, ModbusTcpClient

UNIT = 17
FAILS = []


def check(cond, what):
    if not cond:
        FAILS.append(what)
        print("FAIL:", what, flush=True)


def crc16(data: bytes) -> int:
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ 0xA001 if crc & 1 else crc >> 1
    return crc


def rtu(body: bytes) -> bytes:
    return body + struct.pack("<H", crc16(body))


def holding0(a):
    return (a * 3) & 0xFFFF


def inputv(a):
    return a ^ 0x5A5A


# -- pymodbus ------------------------------------------------------------------


def conformance(client):
    r = client.read_holding_registers(0, count=125, device_id=UNIT)
    check(not r.isError() and r.registers == [holding0(a) for a in range(125)], "read 125 holding at 0")
    r = client.read_input_registers(65411, count=125, device_id=UNIT)
    check(not r.isError() and r.registers == [inputv(a) for a in range(65411, 65536)], "read 125 input at the end")
    r = client.read_input_registers(65535, count=2, device_id=UNIT)
    check(r.isError() and r.exception_code == 2, f"read past 65535 is exception 2: {r}")
    r = client.write_register(1000, 0x1234, device_id=UNIT)
    check(not r.isError(), "write one")
    r = client.read_holding_registers(1000, count=1, device_id=UNIT)
    check(not r.isError() and r.registers == [0x1234], "write one reads back")
    words = list(range(0xB000, 0xB000 + 123))
    r = client.write_registers(2000, words, device_id=UNIT)
    check(not r.isError(), "write 123")
    r = client.read_holding_registers(2000, count=123, device_id=UNIT)
    check(not r.isError() and r.registers == words, "write 123 reads back")
    r = client.write_registers(65500, [1] * 40, device_id=UNIT)
    check(r.isError() and r.exception_code == 2, f"write past 65535 is exception 2: {r}")
    r = client.read_holding_registers(65500, count=1, device_id=UNIT)
    check(not r.isError() and r.registers == [holding0(65500)], "a refused write leaves the map alone")
    r = client.read_coils(0, count=8, device_id=UNIT)
    check(r.isError() and r.exception_code == 1, f"read coils is an illegal function: {r}")


def random_model(client, steps=2000, seed=1):
    rng = random.Random(seed)
    base = 30000
    size = 2000
    model = {}
    for at in range(base, base + size, 125):
        r = client.read_holding_registers(at, count=125, device_id=UNIT)
        if r.isError():
            check(False, f"random: model read {at}")
            return
        for i, v in enumerate(r.registers):
            model[at + i] = v
    for step in range(steps):
        what = rng.randrange(3)
        n = 1 + rng.randrange(1 if what == 1 else (123 if what == 2 else 125))
        a = base + rng.randrange(size - n + 1)
        if what == 0:
            r = client.read_holding_registers(a, count=n, device_id=UNIT)
            if r.isError() or r.registers != [model[a + i] for i in range(n)]:
                check(False, f"random step {step}: read {a}+{n}")
                return
        elif what == 1:
            v = rng.randrange(65536)
            r = client.write_register(a, v, device_id=UNIT)
            if r.isError():
                check(False, f"random step {step}: write one {a}")
                return
            model[a] = v
        else:
            vs = [rng.randrange(65536) for _ in range(n)]
            r = client.write_registers(a, vs, device_id=UNIT)
            if r.isError():
                check(False, f"random step {step}: write {a}+{n}")
                return
            for i, v in enumerate(vs):
                model[a + i] = v


# -- raw RTU -------------------------------------------------------------------


class RawRtu:
    def __init__(self, path):
        self.line = serial.Serial(path, 9600, timeout=0)

    def drain(self):
        time.sleep(0.05)
        self.line.read(4096)

    def send(self, data, pieces=None, pause=0.0):
        if pieces:
            for i in range(0, len(data), pieces):
                self.line.write(data[i : i + pieces])
                self.line.flush()
                time.sleep(pause)
        else:
            self.line.write(data)
            self.line.flush()

    def listen(self, wait=0.3):
        """Everything that arrives until 60 ms of quiet, or @p wait with nothing."""
        out = b""
        start = last = time.monotonic()
        while True:
            chunk = self.line.read(512)
            now = time.monotonic()
            if chunk:
                out += chunk
                last = now
            elif out and now - last > 0.06:
                return out
            elif not out and now - start > wait:
                return out
            time.sleep(0.001)


def raw_rtu(path):
    raw = RawRtu(path)
    raw.drain()
    read10 = rtu(bytes([UNIT, 0x03, 0x00, 0x0A, 0x00, 0x01]))
    good = rtu(bytes([UNIT, 0x03, 0x02]) + struct.pack(">H", holding0(10)))

    raw.send(read10)
    check(raw.listen() == good, "raw: a plain read")

    bad = bytearray(read10)
    bad[-1] ^= 0xFF
    raw.send(bytes(bad))
    check(raw.listen() == b"", "raw: a bad CRC is not answered")
    raw.send(read10)
    check(raw.listen() == good, "raw: and the next request is")

    raw.send(rtu(bytes([18, 0x03, 0x00, 0x0A, 0x00, 0x01])))
    check(raw.listen() == b"", "raw: another unit is not answered")

    raw.send(rtu(bytes([0, 0x06, 0x00, 0x0B, 0x77, 0x77])))
    check(raw.listen() == b"", "raw: a broadcast is not answered")
    raw.send(rtu(bytes([UNIT, 0x03, 0x00, 0x0B, 0x00, 0x01])))
    check(raw.listen() == rtu(bytes([UNIT, 0x03, 0x02, 0x77, 0x77])), "raw: but it is done")
    raw.send(rtu(bytes([UNIT, 0x06, 0x00, 0x0B]) + struct.pack(">H", holding0(11))))
    raw.listen()

    raw.send(read10, pieces=1, pause=0.001)
    check(raw.listen() == good, "raw: a request one byte at a time")

    raw.send(read10, pieces=4, pause=0.08)
    check(raw.listen() == b"", "raw: a request broken by silence is not answered")
    raw.send(read10)
    check(raw.listen() == good, "raw: and the line recovers")

    raw.send(rtu(bytes([UNIT, 0x2B, 0x0E, 0x01, 0x00])))
    check(raw.listen() == rtu(bytes([UNIT, 0xAB, 0x01])), "raw: an unknown code is refused at the silence")

    raw.send(b"\x55\xaa\x13")
    time.sleep(0.08)
    raw.send(read10)
    check(raw.listen() == good, "raw: noise, silence, then a request")

    raw.send(rtu(bytes([UNIT, 0x10, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x01])))
    check(raw.listen() == rtu(bytes([UNIT, 0x90, 0x03])), "raw: a byte count that lies is exception 3")
    raw.send(rtu(bytes([UNIT, 0x03, 0x00, 0x00, 0x00, 126])))
    check(raw.listen() == rtu(bytes([UNIT, 0x83, 0x03])), "raw: 126 registers is exception 3")
    raw.send(rtu(bytes([UNIT, 0x03, 0xFF, 0xFF, 0x00, 0x02])))
    check(raw.listen() == rtu(bytes([UNIT, 0x83, 0x02])), "raw: past 65535 is exception 2")
    raw.send(rtu(bytes([UNIT, 0x03, 0x00])))
    check(raw.listen() == b"", "raw: a request cut short is not answered")
    time.sleep(0.08)
    raw.send(read10)
    check(raw.listen() == good, "raw: and after the silence the next one is")

    raw.send(rtu(bytes([18, 0x03, 0x00, 0x00, 0x00, 0x01])) + read10)
    check(raw.listen() == good, "raw: another unit's request and ours back to back")
    raw.line.close()


# -- raw Modbus TCP ------------------------------------------------------------


def mbap(tx, unit, pdu):
    return struct.pack(">HHHB", tx, 0, len(pdu) + 1, unit) + pdu


def recv_all(sock, wait=0.3):
    sock.settimeout(wait)
    out = b""
    try:
        while True:
            chunk = sock.recv(4096)
            if not chunk:
                return out, True
            out += chunk
            sock.settimeout(0.06)
    except socket.timeout:
        return out, False


def connect(port):
    sock = socket.create_connection(("127.0.0.1", port))
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    return sock


def raw_tcp(port):
    sock = connect(port)
    pdu = bytes([0x03, 0x00, 0x0A, 0x00, 0x01])
    good = lambda tx, unit=UNIT: mbap(tx, unit, bytes([0x03, 0x02]) + struct.pack(">H", holding0(10)))

    sock.sendall(mbap(7, UNIT, pdu) + mbap(8, 0xFF, pdu) + mbap(9, UNIT, pdu))
    got, _ = recv_all(sock)
    check(got == good(7) + good(8, 0xFF) + good(9), "tcp: three requests in one segment, three answers in order")

    req = mbap(10, UNIT, pdu)
    for b in req:
        sock.sendall(bytes([b]))
        time.sleep(0.002)
    got, _ = recv_all(sock)
    check(got == good(10), "tcp: a request one byte at a time")

    sock.sendall(mbap(11, 5, pdu))
    got, _ = recv_all(sock)
    check(got == b"", "tcp: another unit is not answered")

    sock.sendall(mbap(12, UNIT, bytes([0x2B, 0x0E, 0x01, 0x00])))
    got, _ = recv_all(sock)
    check(got == mbap(12, UNIT, bytes([0xAB, 0x01])), "tcp: an unknown code is refused at once")

    sock.sendall(b"\x00\x0d\x00\x01\x00\x06" + bytes([UNIT]) + pdu)
    got, closed = recv_all(sock, 1.0)
    check(got == b"" and closed, "tcp: a header that is not Modbus closes the connection")
    sock.close()

    sock = connect(port)
    sock.sendall(mbap(14, UNIT, pdu))
    got, _ = recv_all(sock)
    check(got == good(14), "tcp: a new connection is served")
    sock.close()


def stop(client):
    client.write_register(0xFFFE, 0xDEAD, device_id=UNIT)


def main():
    logging.basicConfig(level=logging.CRITICAL)
    mode, where = sys.argv[1], sys.argv[2]
    if mode == "rtu":
        time.sleep(0.5)
        raw_rtu(where)
        client = ModbusSerialClient(where, baudrate=9600, timeout=1, retries=0)
    else:
        time.sleep(0.5)
        raw_tcp(int(where))
        client = ModbusTcpClient("127.0.0.1", port=int(where), timeout=1, retries=0)
    check(client.connect(), "pymodbus connects")
    conformance(client)
    random_model(client, seed=3 if mode == "rtu" else 4)
    stop(client)
    client.close()
    print(f"client.py {mode}: {len(FAILS)} failed", flush=True)
    sys.exit(1 if FAILS else 0)


if __name__ == "__main__":
    main()
