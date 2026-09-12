"""
Shared UnicodeData.txt parsing for gen_str_case_map.py and gen_wchar_case_map.py.

Both scripts read UCD fields 12-13 (simple uppercase / simple lowercase) out of the
same UnicodeData.txt; they differ only in which code points fit their output type
(gen_str_case_map.py's lh_char_t is one byte, so it drops anything outside 0..255,
while gen_wchar_case_map.py's lh_wchar_t keeps every code point and splits BMP/SMP
itself). That one filtering knob is the only real difference, so it is a parameter
here rather than two near-identical copies of the same parsing loop.
"""

from __future__ import annotations


def parse_unicode_data(
    text: str, max_code: int | None = None
) -> tuple[list[tuple[int, int]], list[tuple[int, int]]]:
    """Return (to_lower_pairs, to_upper_pairs) as sorted lists of (code, mapped).

    If max_code is given, both code and mapped must be <= max_code for a pair to be
    kept (used by gen_str_case_map.py to restrict to lh_char_t's single byte); with
    max_code=None every mapping in the data is kept (gen_wchar_case_map.py's own
    BMP/supplementary-plane split happens separately, on the unrestricted result).
    """
    to_lower: dict[int, int] = {}
    to_upper: dict[int, int] = {}
    for raw in text.splitlines():
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        parts = line.split(";")
        if len(parts) < 15:
            continue
        code = int(parts[0], 16)
        if max_code is not None and code > max_code:
            continue
        su = parts[12].strip()
        sl = parts[13].strip()
        if sl:
            mapped = int(sl, 16)
            if max_code is None or mapped <= max_code:
                to_lower[code] = mapped
        if su:
            mapped = int(su, 16)
            if max_code is None or mapped <= max_code:
                to_upper[code] = mapped
    lo = sorted(to_lower.items(), key=lambda kv: kv[0])
    up = sorted(to_upper.items(), key=lambda kv: kv[0])
    return lo, up
