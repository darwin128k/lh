#!/usr/bin/env python3
"""
Parse Unicode CaseFolding.txt and emit fold.c (table + lh_wchar_fold_case in one TU).

Uses only mappings where the folded form is a single code point (excludes full "F" expansions
like ß -> ss). Omits status "T" (Turkic) by default — same as common non-Turkic case fold.

lh_wchar_fold_case uses lh_interval_ropen_binary_search() from lh/util/interval.h on the
sorted table (same interval search as wstr case map), then lh_math_gt vs LH_WCHAR_T_MAX before returning
the mapped scalar as lh_wchar_t.

Input: tools/data/unicode/CaseFolding-16.0.0.txt (vendored; refresh from unicode.org when bumping UCD).
Output: src/lh/util/wchar/case/fold.c
"""

from __future__ import annotations

import argparse
import os
import re
import sys

LINE_RE = re.compile(
    r"^([0-9A-Fa-f]+)\s*;\s*([CSTF])\s*;\s*([^;#]+?)\s*(?:;.*)?$"
)


def parse_case_folding(text: str) -> list[tuple[int, int]]:
    pairs: dict[int, int] = {}
    for raw in text.splitlines():
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue
        m = LINE_RE.match(line)
        if not m:
            continue
        code = int(m.group(1), 16)
        status = m.group(2)
        if status == "T":
            continue
        mapping = m.group(3).strip()
        parts = mapping.split()
        if len(parts) != 1:
            continue
        folded = int(parts[0], 16)
        if folded == code:
            continue
        pairs[code] = folded
    out = sorted(pairs.items(), key=lambda kv: kv[0])
    return out


def emit_c(pairs: list[tuple[int, int]], unicode_version: str, out_path: str) -> None:
    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    lines = [
        "/*",
        " * AUTO-GENERATED — do not edit.",
        f" * Source: Unicode CaseFolding-{unicode_version}.txt (UCD).",
        " * Policy: single-code-point mappings only; status T (Turkic) omitted.",
        " * Multi-code-point full folds (e.g. ß -> ss) are omitted — use different API for those.",
        " *",
        " * Unicode data copyright Unicode, Inc.; see https://www.unicode.org/terms_of_use.html",
        " *",
        " * Lookup: lh_interval_ropen_binary_search() in lh/util/interval.h",
        " * (lh_math_gt vs LH_WCHAR_T_MAX for mapped scalar).",
        " */",
        "",
        "#include <lh/util/wchar.h>",
        "#include <lh/util/interval.h>",
        "#include <lh/size.h>",
        "#include <lh/wchar/case/pair.h>",
        "#include <lh/util/array/raw.h>",
        "",
        "static const lh_wchar_case_pair_t m_wchar_case_fold_table[] = {",
        "    /* clang-format off */",
    ]
    elems = [f"    {{ {a}U, {b}U }}" for a, b in pairs]
    lines.append(",\n".join(elems) + ",")
    lines.append("    /* clang-format on */")
    lines.append("};")
    lines.extend(
        [
            "",
            "/* Table lookup: lh_interval_ropen_binary_search() — lh/util/interval.h */",
            "lh_wchar_t lh_wchar_fold_case(lh_wchar_t c) {",
            "    const lh_usize_t n = lh_array_raw_size(m_wchar_case_fold_table);",
            "    lh_wchar_case_pair_t result;",
            "    lh_bool_t is_founded;",
            "",
            "    lh_interval_ropen_binary_search(lh_usize_t, m_wchar_case_fold_table, n,",
            "                                    lh_cast_static(lh_uchar32_t, c), first, result, is_founded);",
            "",
            "    if (is_founded) {",
            "        const lh_uchar32_t mapped = result.second;",
            "        if (lh_math_gt(mapped, lh_cast_static(lh_uchar32_t, LH_WCHAR_T_MAX))) {",
            "            return c;",
            "        }",
            "        return lh_cast_static(lh_wchar_t, mapped);",
            "    }",
            "    return c;",
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
            root, "tools", "data", "unicode", "CaseFolding-16.0.0.txt"
        ),
    )
    ap.add_argument(
        "--output",
        default=os.path.join(root, "src", "lh", "util", "wchar", "case", "fold.c"),
    )
    ap.add_argument("--version", default="16.0.0")
    args = ap.parse_args()
    with open(args.input, encoding="utf-8") as f:
        text = f.read()
    pairs = parse_case_folding(text)
    emit_c(pairs, args.version, args.output)
    print(f"wrote {len(pairs)} entries -> {args.output}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
