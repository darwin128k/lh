#!/usr/bin/env python3
"""
Parse Unicode UnicodeData.txt and emit map.c (simple to_lower / to_upper tables).

Uses UCD fields 12–13 (simple uppercase / simple lowercase). This is separate from
CaseFolding.txt used by lh_wchar_fold_case: folding is for comparison; these mappings are
for display-style case conversion (and differ from fold for some code points).

lh_wstr_ptr_to_lower / lh_wstr_ptr_to_upper scan a lh_wstr buffer in place, using
lh_interval_ropen_binary_search() from lh/util/interval.h on the sorted tables.

Input: tools/data/unicode/UnicodeData-16.0.0.txt
Output: src/lh/util/wstr/case/map.c
"""

from __future__ import annotations

import argparse
import os
import sys


def parse_unicode_data(text: str) -> tuple[list[tuple[int, int]], list[tuple[int, int]]]:
    """Return (to_lower_pairs, to_upper_pairs) as sorted lists of (code, mapped)."""
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
        su = parts[12].strip()
        sl = parts[13].strip()
        if sl:
            to_lower[code] = int(sl, 16)
        if su:
            to_upper[code] = int(su, 16)
    lo = sorted(to_lower.items(), key=lambda kv: kv[0])
    up = sorted(to_upper.items(), key=lambda kv: kv[0])
    return lo, up


def emit_c(
    to_lower: list[tuple[int, int]],
    to_upper: list[tuple[int, int]],
    unicode_version: str,
    out_path: str,
) -> None:
    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    lines = [
        "/*",
        " * AUTO-GENERATED — do not edit.",
        f" * Source: Unicode UCD UnicodeData.txt (Unicode {unicode_version}).",
        " * Simple lowercase (field 13) and simple uppercase (field 12) only.",
        " *",
        " * Unicode data copyright Unicode, Inc.; see https://www.unicode.org/terms_of_use.html",
        " *",
        " * Lookup: lh_interval_ropen_binary_search() in lh/util/interval.h",
        " * (lh_math_gt vs LH_WCHAR_T_MAX for mapped scalar).",
        " */",
        "",
        "#include <lh/util/wstr/ptr.h>",
        "#include <lh/util/interval.h>",
        "#include <lh/assert.h>",
        "#include <lh/wchar/case/pair.h>",
        "",
        "static const lh_wchar_case_pair_t m_wchar_to_lower_table[] = {",
        "    /* clang-format off */",
    ]
    lines.append(",\n".join(f"    {{ {a}U, {b}U }}" for a, b in to_lower) + ",")
    lines.append("    /* clang-format on */")
    lines.append("};")
    lines.append("")
    lines.append("static const lh_wchar_case_pair_t m_wchar_to_upper_table[] = {")
    lines.append("    /* clang-format off */")
    lines.append(",\n".join(f"    {{ {a}U, {b}U }}" for a, b in to_upper) + ",")
    lines.append("    /* clang-format on */")
    lines.append("};")
    lines.extend(
        [
            "",
            "lh_wstr_ptr lh_wstr_ptr_to_lower(lh_wstr_ptr str, lh_usize_t n) {",
            "    lh_assert_runtime_ref(str);",
            "",
            "    static const lh_usize_t sz = lh_array_ptr_get_size(m_wchar_to_lower_table);",
            "    lh_wstr_ptr p = str;",
            "",
            "    while (n--) {",
            "        lh_wchar_t c = *p;",
            "",
            "        lh_wchar_case_pair_t result;",
            "        lh_bool_t found = lh_bool_false;",
            "",
            "        lh_interval_ropen_binary_search(lh_usize_t, m_wchar_to_lower_table, sz, c, first, result,",
            "                                        found);",
            "",
            "        *p++ = found ? result.second : c;",
            "    }",
            "",
            "    return p;",
            "}",
            "",
            "lh_wstr_ptr lh_wstr_ptr_to_upper(lh_wstr_ptr str, lh_usize_t n) {",
            "    lh_assert_runtime_ref(str);",
            "",
            "    static const lh_usize_t sz = lh_array_ptr_get_size(m_wchar_to_upper_table);",
            "    lh_wstr_ptr p = str;",
            "",
            "    while (n--) {",
            "        lh_wchar_t c = *p;",
            "",
            "        lh_wchar_case_pair_t result;",
            "        lh_bool_t found = lh_bool_false;",
            "",
            "        lh_interval_ropen_binary_search(lh_usize_t, m_wchar_to_upper_table, sz, c, first, result,",
            "                                        found);",
            "",
            "        *p++ = found ? result.second : c;",
            "    }",
            "",
            "    return p;",
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
        default=os.path.join(
            root, "tools", "data", "unicode", "UnicodeData-16.0.0.txt"
        ),
    )
    ap.add_argument(
        "--output",
        default=os.path.join(root, "src", "lh", "util", "wstr", "case", "map.c"),
    )
    ap.add_argument("--version", default="16.0.0")
    args = ap.parse_args()
    with open(args.input, encoding="utf-8") as f:
        text = f.read()
    lo, up = parse_unicode_data(text)
    emit_c(lo, up, args.version, args.output)
    print(
        f"wrote to_lower={len(lo)} to_upper={len(up)} entries -> {args.output}",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
