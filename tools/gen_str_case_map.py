#!/usr/bin/env python3
"""
Parse Unicode UnicodeData.txt and emit map.c (narrow lh_char_t simple to_lower / to_upper).

Uses UCD fields 12-13 (simple uppercase / simple lowercase), same source as
gen_wchar_case_map.py, but restricted to code points where BOTH the source and the
mapped scalar fit in a single byte (0..255) — lh_char_t is one code unit, so a
mapping whose target escapes that range (e.g. MICRO SIGN U+00B5 -> GREEK CAPITAL
MU U+039C, or LATIN SMALL LETTER Y WITH DIAERESIS U+00FF -> U+0178, which has no
code point in 0..255 at all) cannot be represented and is dropped.

Emits both implementations lh_str_ptr_to_lower / lh_str_ptr_to_upper switch on
(src/lh/util/str/case/map.c), guarded by #if LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE:
  - ON:  a dense 256-entry table per direction (table[(unsigned char)c] is always the
         mapped byte; identity for bytes with no mapping — no branch needed).
  - OFF: the sorted sparse pair table, via lh_interval_ropen_binary_search() from
         lh/util/interval.h (same lookup gen_wchar_case_map.py's output uses).

128..255 (and their mapped values) are written as their lh_char_t (signed 8-bit)
values (v - 256 for v in [128, 255]) so initializers are well-defined instead of
relying on implementation-defined narrowing of an out-of-range unsigned literal
into a signed char.

Input: tools/data/unicode/UnicodeData-16.0.0.txt
Output: src/lh/util/str/case/map.c
"""

from __future__ import annotations

import argparse
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from unicode_data import parse_unicode_data  # noqa: E402


def signed_byte(v: int) -> int:
    """0..255 -> its lh_char_t (signed 8-bit) value: unchanged for 0..127, v - 256 for 128..255."""
    return v if v < 128 else v - 256


def emit_sparse_table(name: str, pairs: list[tuple[int, int]]) -> list[str]:
    lines = [
        f"static const lh_char_case_pair_t {name}[] = {{",
        "    /* clang-format off */",
    ]
    ascii_pairs = [(a, b) for a, b in pairs if a < 128]
    ext_pairs = [(a, b) for a, b in pairs if a >= 128]
    for a, b in ascii_pairs:
        lines.append(f"    {{ {a}U, {b}U }},")
    if ext_pairs:
        lines.append(
            "    /* 128..255 written as their lh_char_t (signed 8-bit) values: v - 256 for v in [128, 255],"
        )
        lines.append(
            "     * so the initializer is well-defined instead of relying on implementation-defined narrowing"
        )
        lines.append("     * of an out-of-range unsigned literal into a signed char. */")
        for a, b in ext_pairs:
            lines.append(f"    {{ {signed_byte(a)}, {signed_byte(b)} }},")
    lines.append("    /* clang-format on */")
    lines.append("};")
    return lines


def emit_dense_table(name: str, pairs: list[tuple[int, int]]) -> list[str]:
    table = list(range(256))
    for a, b in pairs:
        table[a] = b
    values = [str(signed_byte(v)) for v in table]
    lines = [f"static const lh_char_t {name}[256] = {{", "    /* clang-format off */"]
    for i in range(0, 256, 16):
        lines.append("    " + ", ".join(values[i : i + 16]) + ",")
    lines.append("    /* clang-format on */")
    lines.append("};")
    return lines


def emit_c(
    to_lower: list[tuple[int, int]],
    to_upper: list[tuple[int, int]],
    unicode_version: str,
    out_path: str,
) -> None:
    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    lines = [
        "/*",
        " * AUTO-GENERATED — do not edit. Regenerate with tools/gen_str_case_map.py.",
        f" * Source: Unicode UCD UnicodeData.txt (Unicode {unicode_version}), fields 12-13",
        " * (simple uppercase / simple lowercase), restricted to source and mapped scalars",
        " * both in 0..255 (lh_char_t is one code unit) — see the script docstring for what",
        " * that excludes (e.g. MICRO SIGN, LATIN SMALL LETTER Y WITH DIAERESIS).",
        " *",
        " * Unicode data copyright Unicode, Inc.; see https://www.unicode.org/terms_of_use.html",
        " *",
        " * Two implementations selected by LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE",
        " * (include/lh/config.h) — see that option's doc comment in cmake/library_options.cmake.",
        " */",
        "",
        "#include <lh/util/str/ptr.h>",
        "#include <lh/util/interval.h>",
        "#include <lh/size.h>",
        "#include <lh/char/case/pair.h>",
        "#include <lh/util/array/ptr.h>",
        "#include <lh/assert.h>",
        "#include <lh/cast/static.h>",
        "#include <lh/config.h>",
        "#include <lh/foreach.h>",
        "",
        "#if LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE",
        "",
    ]
    lines.extend(emit_dense_table("m_char_to_lower_dense", to_lower))
    lines.append("")
    lines.extend(emit_dense_table("m_char_to_upper_dense", to_upper))
    lines.extend(
        [
            "",
            "#else /* !LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE */",
            "",
        ]
    )
    lines.extend(emit_sparse_table("m_char_to_lower_table", to_lower))
    lines.append(
        "static const lh_usize_t m_char_to_lower_table_size = lh_array_ptr_get_size(m_char_to_lower_table);"
    )
    lines.append("")
    lines.extend(emit_sparse_table("m_char_to_upper_table", to_upper))
    lines.append(
        "static const lh_usize_t m_char_to_upper_table_size = lh_array_ptr_get_size(m_char_to_upper_table);"
    )
    lines.extend(
        [
            "",
            "#endif /* LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE */",
            "",
            "lh_str_ptr",
            "lh_str_ptr_to_lower(lh_str_ptr str, lh_usize_t n)",
            "{",
            "    lh_assert_runtime_ref(str);",
            "",
            "    lh_foreach(lh_char_t, p, str, n)",
            "    {",
            "        lh_char_t c = lh_ptr_deref(p);",
            "",
            "#if LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE",
            "        *p = m_char_to_lower_dense[(lh_uchar_t)c];",
            "#else",
            "        lh_char_case_pair_t r;",
            "        lh_bool_t is_founded = lh_bool_false;",
            "",
            "        lh_interval_ropen_binary_search(lh_usize_t, m_char_to_lower_table,",
            "                                        m_char_to_lower_table_size, lh_cast_static(lh_usize_t, c),",
            "                                        first, r, is_founded);",
            "",
            "        *p = is_founded ? r.second : c;",
            "#endif",
            "    }",
            "",
            "    return str + n;",
            "}",
            "",
            "lh_str_ptr",
            "lh_str_ptr_to_upper(lh_str_ptr str, lh_usize_t n)",
            "{",
            "    lh_assert_runtime_ref(str);",
            "",
            "    lh_foreach(lh_char_t, p, str, n)",
            "    {",
            "        lh_char_t c = lh_ptr_deref(p);",
            "",
            "#if LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE",
            "        *p = m_char_to_upper_dense[(lh_uchar_t)c];",
            "#else",
            "        lh_char_case_pair_t r;",
            "        lh_bool_t is_founded = lh_bool_false;",
            "",
            "        lh_interval_ropen_binary_search(lh_usize_t, m_char_to_upper_table,",
            "                                        m_char_to_upper_table_size, lh_cast_static(lh_usize_t, c),",
            "                                        first, r, is_founded);",
            "",
            "        *p = is_founded ? r.second : c;",
            "#endif",
            "    }",
            "",
            "    return str + n;",
            "}",
            "",
        ]
    )
    with open(out_path, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines))


def main() -> int:
    ap = argparse.ArgumentParser()
    here = os.path.dirname(os.path.abspath(__file__))
    root = os.path.dirname(here)
    ap.add_argument(
        "--input",
        default=os.path.join(root, "tools", "data", "unicode", "UnicodeData-16.0.0.txt"),
    )
    ap.add_argument(
        "--output",
        default=os.path.join(root, "src", "lh", "util", "str", "case", "map.c"),
    )
    ap.add_argument("--version", default="16.0.0")
    args = ap.parse_args()
    with open(args.input, encoding="utf-8") as f:
        text = f.read()
    lo, up = parse_unicode_data(text, max_code=255)
    emit_c(lo, up, args.version, args.output)
    print(
        f"wrote to_lower={len(lo)} to_upper={len(up)} entries -> {args.output}",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
