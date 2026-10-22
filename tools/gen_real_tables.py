#!/usr/bin/env python3
"""
Compute the two tables lh/real converts numbers with, in exact integers, and write
them into their .c files.

  - m_real_power5 (src/lh/real/power5.c): 5^q for q in [-342, 308], normalized to
    128 bits (top bit set) and truncated, two 64-bit words per power — the table of
    Eisel-Lemire, the same as fast_float's. Rows outside [-65, 38] are only needed
    by binary64 and sit under `#if LH_LIBRARY_OPTION_REAL == LH_REAL_DOUBLE`.
  - m_real_cached (src/lh/real/cached.c): 10^k for k = -348 + 8 i, i in [0, 86], as a
    64-bit significand rounded to nearest and a binary exponent — the cached powers
    of Grisu2, the same as RapidJSON's and double-conversion's.

Only the initializer of each table is replaced; the rest of the file is left as it
is. Python integers are exact, so there is nothing to download: the output is fully
determined by this script (the first entries are checked against the published
tables).

Output: src/lh/real/power5.c, src/lh/real/cached.c
"""

from __future__ import annotations

import argparse
import os
import re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

POWER5_MIN, POWER5_MAX = -342, 308
FLOAT_POWER5_MIN, FLOAT_POWER5_MAX = -65, 38
CACHED_K_MIN, CACHED_K_STEP, CACHED_COUNT = -348, 8, 87

DOUBLE_ONLY = "#    if LH_LIBRARY_OPTION_REAL == LH_REAL_DOUBLE"
END_IF = "#    endif"


def power5(q: int) -> int:
    """5^q in 128 bits: normalized and truncated (fast_float's construction)."""
    if q < 0:
        p = 5 ** -q
        z = 0
        while (1 << z) < p:
            z += 1
        if q >= -27:
            return 2 ** (z + 127) // p + 1
        c = 2 ** (2 * z + 2 * 64) // p + 1
        while c >= (1 << 128):
            c //= 2
        return c
    p = 5 ** q
    while p < (1 << 127):
        p *= 2
    while p >= (1 << 128):
        p //= 2
    return p


def cached(k: int) -> tuple[int, int]:
    """10^k = f * 2^e, 2^63 <= f < 2^64, f rounded to nearest."""
    num, den = (10 ** k, 1) if k >= 0 else (1, 10 ** -k)
    e = num.bit_length() - den.bit_length() - 64
    while True:
        n, d = (num, den << e) if e >= 0 else (num << -e, den)
        f = (2 * n + d) // (2 * d)
        if f >= (1 << 64):
            e += 1
        elif f < (1 << 63):
            e -= 1
        else:
            return f, e


def power5_rows() -> list[str]:
    rows = []
    for q in range(POWER5_MIN, POWER5_MAX + 1):
        if q in (POWER5_MIN, FLOAT_POWER5_MAX + 1):
            rows.append(DOUBLE_ONLY)
        if q == FLOAT_POWER5_MIN:
            rows.append(END_IF)
        v = power5(q)
        rows.append("    0x%016XULL, 0x%016XULL, /* 5^%d */" % (v >> 64, v & (2 ** 64 - 1), q))
    rows.append(END_IF)
    return rows


def cached_rows() -> list[str]:
    entries = []
    for i in range(CACHED_COUNT):
        k = CACHED_K_MIN + CACHED_K_STEP * i
        f, e = cached(k)
        entries.append(("    {0x%016XULL, %d}," % (f, e), "/* 10^%d */" % k))
    # Trailing comments in one column, as clang-format aligns them.
    width = max(len(code) for code, _ in entries)
    return [code.ljust(width) + " " + comment for code, comment in entries]


def check() -> None:
    first = power5(POWER5_MIN)
    assert first == (0xEEF453D6923BD65A << 64) | 0x113FAA2906A13B3F, hex(first)
    assert cached(CACHED_K_MIN) == (0xFA8FD5A0081C0288, -1220)
    assert cached(CACHED_K_MIN + CACHED_K_STEP * (CACHED_COUNT - 1)) == (0xAF87023B9BF0EE6B, 1066)


def replace_initializer(path: str, name: str, rows: list[str]) -> None:
    with open(path, encoding="utf-8", newline="") as h:
        text = h.read()
    pattern = re.compile(r"(\b" + re.escape(name) + r"\b[^\n]*= \{\n)(.*?)(\n\};)", re.S)
    text, n = pattern.subn(lambda m: m.group(1) + "\n".join(rows) + m.group(3), text)
    if n != 1:
        raise SystemExit(f"{path}: no single initializer of {name}")
    with open(path, "w", encoding="utf-8", newline="\n") as h:
        h.write(text)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[1])
    parser.parse_args()
    check()
    replace_initializer(os.path.join(ROOT, "src/lh/real/power5.c"), "m_real_power5", power5_rows())
    replace_initializer(os.path.join(ROOT, "src/lh/real/cached.c"), "m_real_cached", cached_rows())


if __name__ == "__main__":
    main()
