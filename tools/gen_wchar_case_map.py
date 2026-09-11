#!/usr/bin/env python3
"""
Parse Unicode UnicodeData.txt and emit map.c (simple to_lower / to_upper tables).

Uses UCD fields 12-13 (simple uppercase / simple lowercase). This is separate from
CaseFolding.txt used by lh_wchar_fold_case: folding is for comparison; these mappings are
for display-style case conversion (and differ from fold for some code points).

Emits both implementations lh_wstr_ptr_to_lower / lh_wstr_ptr_to_upper switch on, guarded
by #if LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE:

  - ON:  a two-level (block) table over the BMP (code points 0x0000..0xFFFF — every
         lh_wchar_t value on a 16-bit wchar_t platform, and effectively all real-world
         text on any platform): a 256-entry stage-1 index (one byte per 256-code-point
         block; most blocks have no case mapping at all and collapse to a single
         sentinel) into a stage-2 table holding only the handful of blocks that actually
         have mappings. O(1) per character. Falls back to a small binary search (the
         much rarer supplementary-plane scripts with case pairs — Deseret, Osage,
         Adlam, ...) only for code points above the BMP, which needs a 32-bit wchar_t to
         even reach — that fallback (and the c > 0xFFFF check) is compiled out entirely
         on a platform where LH_WCHAR_T_MAX already can't exceed 0xFFFF.
  - OFF: the original single sorted sparse pair table covering every code point, via
         lh_interval_ropen_binary_search() from lh/util/interval.h. O(log n) per
         character (~11 comparisons for the current ~1.4k entries per direction).

Every code point in the current UCD data maps within its own plane (no BMP source has a
supplementary-plane target or vice versa) — verified when splitting, not assumed; the
script aborts if that ever stops holding, since the two-level table's stage-2 entries are
stored as 16-bit values and could not represent a supplementary-plane target.

Input: tools/data/unicode/UnicodeData-16.0.0.txt
Output: src/lh/util/wstr/case/map.c
"""

from __future__ import annotations

import argparse
import os
import sys

BLOCK_SIZE = 256
BMP_MAX = 0xFFFF
NO_BLOCK = 0xFF


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


def split_bmp_smp(
    pairs: list[tuple[int, int]], name: str
) -> tuple[dict[int, int], list[tuple[int, int]]]:
    """Split into (bmp pairs as a dict, smp pairs as a sorted list). Aborts if any pair
    crosses the BMP/supplementary-plane boundary (source and mapped scalar must be on the
    same side), since the two-level table cannot represent that."""
    bmp: dict[int, int] = {}
    smp: list[tuple[int, int]] = []
    for code, mapped in pairs:
        code_is_bmp = code <= BMP_MAX
        mapped_is_bmp = mapped <= BMP_MAX
        if code_is_bmp != mapped_is_bmp:
            raise SystemExit(
                f"{name}: pair ({code:#x} -> {mapped:#x}) crosses the BMP boundary on only "
                "one side — the two-level table generator assumes this never happens; "
                "review before proceeding."
            )
        if code_is_bmp:
            bmp[code] = mapped
        else:
            smp.append((code, mapped))
    return bmp, smp


def build_two_level(bmp_pairs: dict[int, int]) -> tuple[list[int], list[list[int]]]:
    """Return (stage1, stage2_blocks): stage1[block] is NO_BLOCK or an index into
    stage2_blocks; each stage2 block holds 256 mapped values (identity where unmapped)."""
    stage1: list[int] = []
    stage2_blocks: list[list[int]] = []
    n_blocks = (BMP_MAX + 1) // BLOCK_SIZE
    for b in range(n_blocks):
        base = b * BLOCK_SIZE
        block = [bmp_pairs.get(base + i, base + i) for i in range(BLOCK_SIZE)]
        if all(block[i] == base + i for i in range(BLOCK_SIZE)):
            stage1.append(NO_BLOCK)
        else:
            stage1.append(len(stage2_blocks))
            stage2_blocks.append(block)
    if len(stage2_blocks) >= NO_BLOCK:
        raise SystemExit(
            f"too many stage-2 blocks ({len(stage2_blocks)}) for an 8-bit stage-1 index "
            f"(sentinel {NO_BLOCK} reserved) — widen stage1's element type first."
        )
    return stage1, stage2_blocks


def emit_u8_table(name: str, values: list[int]) -> list[str]:
    lines = [f"static const lh_u8_t {name}[{len(values)}] = {{", "    /* clang-format off */"]
    for i in range(0, len(values), 16):
        lines.append("    " + ", ".join(f"{v}U" for v in values[i : i + 16]) + ",")
    lines.append("    /* clang-format on */")
    lines.append("};")
    return lines


def emit_u16_table(name: str, values: list[int]) -> list[str]:
    lines = [f"static const lh_u16_t {name}[{len(values)}] = {{", "    /* clang-format off */"]
    for i in range(0, len(values), 16):
        lines.append("    " + ", ".join(f"{v}U" for v in values[i : i + 16]) + ",")
    lines.append("    /* clang-format on */")
    lines.append("};")
    return lines


def emit_sparse_table(name: str, pairs: list[tuple[int, int]]) -> list[str]:
    lines = [f"static const lh_wchar_case_pair_t {name}[] = {{", "    /* clang-format off */"]
    lines.append(",\n".join(f"    {{ {a}U, {b}U }}" for a, b in pairs) + ",")
    lines.append("    /* clang-format on */")
    lines.append("};")
    return lines


def emit_two_level_direction(
    direction: str, bmp_pairs: dict[int, int], smp_pairs: list[tuple[int, int]]
) -> list[str]:
    stage1, stage2_blocks = build_two_level(bmp_pairs)
    stage2_flat = [v for block in stage2_blocks for v in block]

    lines = emit_u8_table(f"m_wchar_to_{direction}_stage1", stage1)
    lines.append("")
    lines.extend(emit_u16_table(f"m_wchar_to_{direction}_stage2", stage2_flat))
    lines.append("")
    lines.append("#if LH_WCHAR_T_MAX > 0xFFFF")
    lines.append("")
    lines.extend(emit_sparse_table(f"m_wchar_to_{direction}_smp_table", smp_pairs))
    lines.append(
        f"static const lh_usize_t m_wchar_to_{direction}_smp_table_size ="
        f" lh_array_ptr_get_size(m_wchar_to_{direction}_smp_table);"
    )
    lines.append("")
    lines.append("#endif /* LH_WCHAR_T_MAX > 0xFFFF */")
    return lines


def emit_two_level_function(direction: str, func: str) -> list[str]:
    return [
        f"lh_wstr_ptr {func}(lh_wstr_ptr str, lh_usize_t n) {{",
        "    lh_assert_runtime_ref(str);",
        "",
        "    lh_wstr_ptr p = str;",
        "",
        "    while (n--) {",
        "        lh_wchar_t c = *p;",
        "",
        "#if LH_WCHAR_T_MAX > 0xFFFF",
        "        if (c > 0xFFFF) {",
        f"            lh_wchar_case_pair_t result;",
        f"            lh_bool_t found = lh_bool_false;",
        "",
        f"            lh_interval_ropen_binary_search(lh_usize_t, m_wchar_to_{direction}_smp_table,",
        f"                                            m_wchar_to_{direction}_smp_table_size, c, first,",
        "                                            result, found);",
        "",
        "            *p++ = found ? result.second : c;",
        "        } else",
        "#endif /* LH_WCHAR_T_MAX > 0xFFFF */",
        "        {",
        f"            const lh_u8_t block = m_wchar_to_{direction}_stage1[(lh_usize_t)c >> 8];",
        "            *p++ = (block == 0xFFU)",
        "                       ? c",
        f"                       : (lh_wchar_t)m_wchar_to_{direction}_stage2[(lh_usize_t)block * 256U +"
        " ((lh_usize_t)c & 0xFFU)];",
        "        }",
        "    }",
        "",
        "    return p;",
        "}",
        "",
    ]


def emit_sparse_function(direction: str, func: str) -> list[str]:
    return [
        f"lh_wstr_ptr {func}(lh_wstr_ptr str, lh_usize_t n) {{",
        "    lh_assert_runtime_ref(str);",
        "",
        f"    static const lh_usize_t sz = lh_array_ptr_get_size(m_wchar_to_{direction}_table);",
        "    lh_wstr_ptr p = str;",
        "",
        "    while (n--) {",
        "        lh_wchar_t c = *p;",
        "",
        "        lh_wchar_case_pair_t result;",
        "        lh_bool_t found = lh_bool_false;",
        "",
        f"        lh_interval_ropen_binary_search(lh_usize_t, m_wchar_to_{direction}_table, sz, c, first,"
        " result,",
        "                                        found);",
        "",
        "        *p++ = found ? result.second : c;",
        "    }",
        "",
        "    return p;",
        "}",
        "",
    ]


def emit_c(
    to_lower: list[tuple[int, int]],
    to_upper: list[tuple[int, int]],
    unicode_version: str,
    out_path: str,
) -> None:
    lower_bmp, lower_smp = split_bmp_smp(to_lower, "to_lower")
    upper_bmp, upper_smp = split_bmp_smp(to_upper, "to_upper")

    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    lines = [
        "/*",
        " * AUTO-GENERATED — do not edit. Regenerate with tools/gen_wchar_case_map.py.",
        f" * Source: Unicode UCD UnicodeData.txt (Unicode {unicode_version}).",
        " * Simple lowercase (field 13) and simple uppercase (field 12) only.",
        " *",
        " * Unicode data copyright Unicode, Inc.; see https://www.unicode.org/terms_of_use.html",
        " *",
        " * Two implementations selected by LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE",
        " * (include/lh/config.h) — see that option's doc comment in cmake/library_options.cmake.",
        " */",
        "",
        "#include <lh/util/wstr/ptr.h>",
        "#include <lh/util/interval.h>",
        "#include <lh/wchar/case/pair.h>",
        "#include <lh/wchar.h>",
        "#include <lh/numeric/fixed/types.h>",
        "#include <lh/assert.h>",
        "#include <lh/config.h>",
        "",
        "#if LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE",
        "",
    ]
    lines.extend(emit_two_level_direction("lower", lower_bmp, lower_smp))
    lines.append("")
    lines.extend(emit_two_level_direction("upper", upper_bmp, upper_smp))
    lines.extend(["", "#else /* !LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE */", ""])
    lines.extend(emit_sparse_table("m_wchar_to_lower_table", to_lower))
    lines.append("")
    lines.extend(emit_sparse_table("m_wchar_to_upper_table", to_upper))
    lines.extend(["", "#endif /* LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE */", ""])

    lines.append("#if LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE")
    lines.extend(emit_two_level_function("lower", "lh_wstr_ptr_to_lower"))
    lines.append("#else")
    lines.extend(emit_sparse_function("lower", "lh_wstr_ptr_to_lower"))
    lines.append("#endif /* LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE */")
    lines.append("")
    lines.append("#if LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE")
    lines.extend(emit_two_level_function("upper", "lh_wstr_ptr_to_upper"))
    lines.append("#else")
    lines.extend(emit_sparse_function("upper", "lh_wstr_ptr_to_upper"))
    lines.append("#endif /* LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE */")
    lines.append("")

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
