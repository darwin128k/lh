#!/usr/bin/env python3
"""Turn a TTF/OTF into a C font the canvas can draw from memory.

Same knobs as LVGL's font converter (lv_font_conv): a font file, a pixel
size, bits per pixel, and the characters to keep. Each glyph is cropped to
its ink (Android/LVGL style): an lh_ui_mask_t over a slice of a shared bits
array, plus a top relative to the baseline and an advance byte. Rows are
packed high-bit first at 1, 2, 4 or 8 bpp. The result is an lh_ui_font_t,
not an operating-system font.
"""

import argparse
import sys
from pathlib import Path


def identifier(name):
    cleaned = []
    for ch in name:
        cleaned.append(ch if ch.isalnum() else "_")
    text = "".join(cleaned)
    if not text or text[0].isdigit():
        text = "font_" + text
    return text


def parse_range(text):
    codes = []
    for part in text.split(","):
        piece = part.strip()
        if not piece:
            continue
        if "-" in piece:
            start, end = piece.split("-", 1)
            first = int(start, 0)
            last = int(end, 0)
            codes.extend(range(first, last + 1))
        else:
            codes.append(int(piece, 0))
    return codes


def pack_row(values, bpp):
    row = []
    accumulator = 0
    filled = 0
    for value in values:
        accumulator = (accumulator << bpp) | value
        filled += bpp
        if filled == 8:
            row.append(accumulator)
            accumulator = 0
            filled = 0
    if filled:
        row.append(accumulator << (8 - filled))
    return row


def write_bytes(lines, values, per_line):
    row = []
    for value in values:
        row.append(f"0x{value:02X}")
        if len(row) == per_line:
            lines.append("    " + ", ".join(row) + ",")
            row = []
    if row:
        lines.append("    " + ", ".join(row) + ",")


def write_s32s(lines, values, per_line):
    row = []
    for value in values:
        row.append(str(int(value)))
        if len(row) == per_line:
            lines.append("    " + ", ".join(row) + ",")
            row = []
    if row:
        lines.append("    " + ", ".join(row) + ",")


def ink_bbox(raw, width, height):
    """Inclusive-exclusive bbox of non-zero pixels, or None if empty."""
    left = width
    top = height
    right = 0
    bottom = 0
    found = False
    for y in range(height):
        row = y * width
        for x in range(width):
            if raw[row + x] != 0:
                found = True
                if x < left:
                    left = x
                if x >= right:
                    right = x + 1
                if y < top:
                    top = y
                if y >= bottom:
                    bottom = y + 1
    if not found:
        return None
    return left, top, right, bottom


def main():
    parser = argparse.ArgumentParser(description="TTF/OTF to an lh font")
    parser.add_argument("--font", required=True, type=Path)
    parser.add_argument("--size", required=True, type=int)
    parser.add_argument("--bpp", default=4, type=int, choices=[1, 2, 4, 8])
    parser.add_argument("--range", default="32-126", help="Code points, e.g. 32-126,48")
    parser.add_argument("--symbols", default="", help="Extra characters, kept in order")
    parser.add_argument("--name", help="C identifier, default is the file stem plus size")
    parser.add_argument("--notice", default="", help="Licence line for the file comment")
    parser.add_argument("--header", type=Path, help="Also write a header declaring the font")
    parser.add_argument("--include", help="How the .c includes that header, default its file name")
    parser.add_argument("-o", "--output", required=True, type=Path)
    args = parser.parse_args()

    try:
        from PIL import Image, ImageDraw, ImageFont
    except ImportError:
        print("font.py needs Pillow: pip install pillow", file=sys.stderr)
        return 1

    codes = parse_range(args.range)
    for ch in args.symbols:
        code = ord(ch)
        if code not in codes:
            codes.append(code)
    codes = [code for code in codes if 0 <= code <= 255]
    if not codes:
        print("no characters in 0..255", file=sys.stderr)
        return 1
    first = min(codes)
    last = max(codes)
    # Dense run from first to last, like LVGL's format-0 cmap.
    count = last - first + 1

    face = ImageFont.truetype(str(args.font), args.size)
    ascent, descent = face.getmetrics()
    line_height = ascent + descent
    if line_height <= 0:
        print("font size has no line height", file=sys.stderr)
        return 1

    levels = (1 << args.bpp) - 1
    # Plate wide enough for any glyph: advance and ink may overhang.
    plate_width = 1
    advances = [0] * count
    for code in range(first, last + 1):
        ch = chr(code)
        advance = int(round(face.getlength(ch)))
        if advance > 255:
            print(f"advance of U+{code:04X} is {advance}, wider than a byte", file=sys.stderr)
            return 1
        advances[code - first] = advance
        bbox = face.getbbox(ch)
        ink_right = 0 if bbox is None else max(bbox[2], 0)
        plate_width = max(plate_width, advance, ink_right, 1)

    bits = []
    glyphs = []  # (offset, width, height, row_bytes) or None for empty
    tops = [0] * count
    bit_offset = 0
    for code in range(first, last + 1):
        ch = chr(code)
        plate = Image.new("L", (plate_width, line_height), 0)
        ImageDraw.Draw(plate).text((0, ascent), ch, font=face, fill=255, anchor="ls")
        samples = plate.get_flattened_data() if hasattr(plate, "get_flattened_data") else plate.getdata()
        raw = list(samples)
        box = ink_bbox(raw, plate_width, line_height)
        if box is None:
            glyphs.append(None)
            tops[code - first] = 0
            continue
        left, top, right, bottom = box
        # Horizontal: keep from the pen (x=0) through the rightmost ink so
        # ofs_x stays 0 without a field — cropping the left bearing would
        # shift ink on screen. Vertical: crop to ink; place via tops[].
        left = 0
        width = right - left
        height = bottom - top
        tops[code - first] = top - ascent
        row_bytes = (width * args.bpp + 7) // 8
        glyph_bytes = []
        for y in range(top, bottom):
            row = []
            base = y * plate_width
            for x in range(left, right):
                pixel = raw[base + x]
                row.append((pixel * levels + 127) // 255)
            glyph_bytes.extend(pack_row(row, args.bpp))
        glyphs.append((bit_offset, width, height, row_bytes))
        bits.extend(glyph_bytes)
        bit_offset += len(glyph_bytes)

    name = identifier(args.name or f"{args.font.stem}_{args.size}")
    comment = [
        "/* Generated by scripts/font.py.",
        f" * Source: {args.font.name}  size: {args.size}px  bpp: {args.bpp}",
        f" * Range: {first}..{last}  line: {line_height}px  ascent: {ascent}  glyphs cropped to ink",
        " * Rows are packed high-bit first, the same way LVGL's font converter packs them.",
    ]
    if args.notice:
        comment.append(f" * {args.notice}")
    comment.append(" */")
    if args.header:
        guard = identifier(name).upper() + "_H"
        header = comment + [
            f"#ifndef {guard}",
            f"#define {guard}",
            "",
            "#include <lh/byte.h>",
            "#include <lh/compiler/extern/c.h>",
            "#include <lh/ui/font.h>",
            "#include <lh/ui/mask.h>",
            "",
            "LH_COMPILER_EXTERN_C_BEGIN",
            "",
            f"/** Packed glyph coverage of ::{name} (glyphs are slices of this). */",
            f"extern const lh_byte_t {name}_bits[];",
            "",
            f"/** Cropped masks of ::{name}, one per code. */",
            f"extern const lh_ui_mask_t {name}_glyphs[];",
            "",
            f"/** One advance byte per glyph of ::{name}. */",
            f"extern const lh_byte_t {name}_advances[];",
            "",
            f"/** Mask top relative to the baseline per glyph of ::{name}. */",
            f"extern const lh_s32_t {name}_tops[];",
            "",
            f"/** {args.font.stem} at {args.size}px, codes {first}..{last}, {args.bpp} bits per pixel. */",
            f"extern const lh_ui_font_t {name};",
            "",
            "LH_COMPILER_EXTERN_C_END",
            "",
            f"#endif /* {guard} */",
            "",
        ]
        args.header.write_text("\n".join(header), encoding="utf-8", newline="\n")
    include = args.include or (args.header.name if args.header else "lh/ui/font.h")
    lines = comment + [
        f"#include <{include}>",
        "",
        "#include <lh/null.h>",
        "#include <lh/numeric/fixed/types.h>",
        "",
        f"const lh_byte_t {name}_bits[] = {{",
    ]
    if bits:
        write_bytes(lines, bits, 12)
    else:
        lines.append("    0,")
    lines.append("};")
    lines.append("")
    lines.append(f"const lh_ui_mask_t {name}_glyphs[] = {{")
    for g in glyphs:
        if g is None:
            lines.append(f"    {{lh_null, 0, 0, 0, {args.bpp}}},")
        else:
            offset, width, height, row_bytes = g
            lines.append(
                f"    {{{name}_bits + {offset}, {width}, {height}, {row_bytes}, {args.bpp}}},"
            )
    lines.append("};")
    lines.append("")
    lines.append(f"const lh_byte_t {name}_advances[] = {{")
    write_bytes(lines, advances, 12)
    lines.append("};")
    lines.append("")
    lines.append(f"const lh_s32_t {name}_tops[] = {{")
    write_s32s(lines, tops, 12)
    lines += [
        "};",
        "",
        f"const lh_ui_font_t {name} = {{",
        f"    {name}_glyphs, {name}_advances, {name}_tops, {line_height}, {ascent}, {first}, {count}",
        "};",
        "",
    ]
    args.output.write_text("\n".join(lines), encoding="utf-8", newline="\n")
    inked = sum(1 for g in glyphs if g is not None)
    print(
        f"{args.output} line {line_height} ascent {ascent} bpp {args.bpp} "
        f"codes {first}..{last} inked {inked}/{count} bits {len(bits)}"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
