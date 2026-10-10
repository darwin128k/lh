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


# The blocks themselves, named for what they are, and **the only place a code point is
# written down**. A language below is an addition of these and nothing else.
#
# That composition is the whole point, and it was got wrong first: `ru` was written out
# as its own list of runs, so cutting `cyrillic` down to 0x400..0x430 left `ru` claiming
# the full alphabet. Nothing said so, the font simply drew Russian only up to U+0430,
# and the selftest passed -- because `ru` and the hand-written ranges still agreed with
# each other while both being wrong about the block they were built from. Two copies of
# one fact compare equal forever.
ASCII = [(0x20, 0x7E)]
LATIN1 = [(0xA0, 0xBF)]
CYRILLIC = [(0x400, 0x45F)]
# En dash, em dash and numero: the three a status line turns out to want the moment the
# interface is written in a language that is not English.
PUNCT = [(0x2013, 0x2014), (0x2116, 0x2116)]

# Named blocks, so that nobody has to remember that Russian is 0x400..0x45F and that the
# guillemets are somewhere else entirely. Writing those numbers by hand is a thing you
# get wrong **silently**: the font comes out looking perfectly good and the text that
# needed it simply is not drawn, which reads as a drawing bug and is a coverage bug.
#
# Latin-1 keeps U+00AD (soft hyphen) and U+00B8 (spacing diaeresis), which Roboto has no
# glyph for; those are dropped below rather than asked for again. The guillemets a
# Russian text leans on live in Latin-1 rather than in the Cyrillic block, which is why
# `ru` is `latin` plus `cyrillic` plus `punct` and not something narrower.
LANGUAGES = {
    # What "the characters a program prints" has meant all along, and the default.
    "ascii": ASCII,
    "latin": ASCII + LATIN1,
    "cyrillic": CYRILLIC,
    "punct": PUNCT,
    "ru": ASCII + LATIN1 + CYRILLIC + PUNCT,
}
DEFAULT_LANGUAGE = "ascii"


def expand_languages(text):
    """Code points named by a --lang list.

    Returns the names in the order they were given and the codes they cover, or None
    after saying which name it did not know: a font that quietly misses a language is
    worse than a build that stops.
    """
    names = []
    codes = []
    for part in text.split(","):
        piece = part.strip()
        if not piece:
            continue
        if piece not in LANGUAGES:
            known = ", ".join(sorted(LANGUAGES))
            print(f"unknown language {piece!r}; known: {known}", file=sys.stderr)
            return None
        names.append(piece)
        for first, last in LANGUAGES[piece]:
            codes.extend(range(first, last + 1))
    return names, codes


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
    parser.add_argument("--range", default="",
                        help="Code points, e.g. 32-126,48,0x400-0x45F (a comma-separated "
                             "list; each run of them becomes one range in the font). "
                             "Added to whatever --lang asks for.")
    parser.add_argument("--lang", default="",
                        help="Named blocks of code points, e.g. ru,latin. Known: "
                             + ", ".join(sorted(LANGUAGES))
                             + f". Neither --lang nor --range means {DEFAULT_LANGUAGE}.")
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

    # What was asked for is the union of the named blocks and the typed ranges, and the
    # default only applies when neither was said. The names are kept so the generated
    # file can say where its coverage came from: a reader who finds Cyrillic missing has
    # one place to look before they go looking in the rasterizer.
    languages = []
    codes = []
    if args.lang:
        expanded = expand_languages(args.lang)
        if expanded is None:
            return 1
        languages, codes = expanded
    codes.extend(parse_range(args.range))
    if not codes:
        languages, codes = expand_languages(DEFAULT_LANGUAGE)
    for ch in args.symbols:
        code = ord(ch)
        if code not in codes:
            codes.append(code)
    bad = [c for c in codes if c < 0 or c > 0x10FFFF]
    if bad:
        print("code points outside Unicode: %s" % ", ".join(str(c) for c in bad), file=sys.stderr)
        return 1
    codes = sorted(set(codes))
    if not codes:
        print("no characters asked for", file=sys.stderr)
        return 1

    face = ImageFont.truetype(str(args.font), args.size)
    ascent, descent = face.getmetrics()
    line_height = ascent + descent
    if line_height <= 0:
        print("font size has no line height", file=sys.stderr)
        return 1

    levels = (1 << args.bpp) - 1

    # Which codes the font actually has. An advance of zero is the answer FreeType
    # gives for a code point with no glyph, and it is a different answer from a space:
    # a space has no ink and still moves the pen four pixels. Measured on
    # Roboto-Regular 16px, a range over Latin-1 loses U+00AD (soft hyphen) and
    # U+00B8 (a spacing diaeresis), and every other code in 0x20..0x7E and 0x400..0x45F
    # keeps an advance.
    #
    # Those two are dropped rather than refused, and the two are different failures: a
    # code the font has no glyph for cannot be drawn, so a run that spans it would
    # promise a glyph that is not there. Dropping ends the run and starts another, which
    # is what the ranges are for.
    asked = list(codes)
    codes = []
    holes = []
    advances = []
    for code in asked:
        ch = chr(code)
        advance = int(round(face.getlength(ch)))
        if advance == 0:
            holes.append(code)
            continue
        if advance > 255:
            print(f"advance of U+{code:04X} is {advance}, wider than a byte", file=sys.stderr)
            return 1
        codes.append(code)
        advances.append(advance)
    if not codes:
        print("no character in the range has a glyph in this font", file=sys.stderr)
        if holes:
            print("  asked for: %s" % ", ".join(f"U+{c:04X}" for c in holes[:16]), file=sys.stderr)
        return 1
    count = len(codes)

    # One run per stretch of consecutive codes, and `base` is where that run's glyphs
    # land in the dense tables -- so the tables hold exactly the codes there are, and a
    # hole between two stretches costs a run rather than a thousand empty masks.
    index_of = {code: i for i, code in enumerate(codes)}
    ranges = []  # (first, length, base)
    at = 0
    while at < count:
        start = at
        while at + 1 < count and codes[at + 1] == codes[at] + 1:
            at += 1
        ranges.append((codes[start], at - start + 1, start))
        at += 1

    # Plate wide enough for any glyph: advance and ink may overhang.
    plate_width = 1
    for i, code in enumerate(codes):
        bbox = face.getbbox(chr(code))
        ink_right = 0 if bbox is None else max(bbox[2], 0)
        plate_width = max(plate_width, advances[i], ink_right, 1)

    bits = []
    glyphs = []  # (offset, width, height, row_bytes) or None for empty
    tops = [0] * count
    bit_offset = 0
    for i, code in enumerate(codes):
        ch = chr(code)
        plate = Image.new("L", (plate_width, line_height), 0)
        ImageDraw.Draw(plate).text((0, ascent), ch, font=face, fill=255, anchor="ls")
        samples = plate.get_flattened_data() if hasattr(plate, "get_flattened_data") else plate.getdata()
        raw = list(samples)
        box = ink_bbox(raw, plate_width, line_height)
        if box is None:
            glyphs.append(None)
            tops[i] = 0
            continue
        left, top, right, bottom = box
        # Horizontal: keep from the pen (x=0) through the rightmost ink so
        # ofs_x stays 0 without a field — cropping the left bearing would
        # shift ink on screen. Vertical: crop to ink; place via tops[].
        left = 0
        width = right - left
        height = bottom - top
        tops[i] = top - ascent
        row_bytes = (width * args.bpp + 7) // 8
        glyph_bytes = []
        for y in range(top, bottom):
            row = []
            # `row_at`, not `base`: `base` is a run's offset into these tables above,
            # and a shadowed name is how one of them ends up meaning the other.
            row_at = y * plate_width
            for x in range(left, right):
                pixel = raw[row_at + x]
                row.append((pixel * levels + 127) // 255)
            glyph_bytes.extend(pack_row(row, args.bpp))
        glyphs.append((bit_offset, width, height, row_bytes))
        bits.extend(glyph_bytes)
        bit_offset += len(glyph_bytes)

    # The cap line: the top of a capital letter above the baseline. Measured off
    # the same tops the glyphs use, so the metric and the ink cannot disagree.
    # A range with no capital falls back to its tallest glyph, which is the
    # ascender and the closest thing the font has.
    #
    # 'H' and not the first capital of whatever alphabet the font covers: the cap
    # height is a property of the Latin design of the face, and Roboto's Cyrillic
    # capitals happen to sit on it too, but a font where they do not would then
    # centre its Latin text by a line that is not Latin's.
    cap_height = 0
    cap_code = ord("H")
    if cap_code in index_of and glyphs[index_of[cap_code]] is not None:
        cap_height = -tops[index_of[cap_code]]
    elif any(tops):
        cap_height = -min(tops)

    name = identifier(args.name or f"{args.font.stem}_{args.size}")
    spans = " ".join(f"U+{r[0]:04X}-{r[0] + r[1] - 1:04X}" for r in ranges)
    comment = [
        "/* Generated by scripts/font.py.",
        f" * Source: {args.font.name}  size: {args.size}px  bpp: {args.bpp}",
        f" * Languages: {', '.join(languages)}" if languages else " * Languages: (code points only)",
        f" * Ranges ({len(ranges)}): {spans}",
        f" * Glyphs: {count}  line: {line_height}px  ascent: {ascent}  "
        f"cap: {cap_height}  glyphs cropped to ink",
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
            f"/** The runs of code points of ::{name}, in table order. */",
            f"extern const lh_ui_font_range_t {name}_ranges[];",
            "",
            f"/** {args.font.stem} at {args.size}px, {count} glyphs over {len(ranges)} run(s): {spans}",
            f" *  ({args.bpp} bits per pixel). */",
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
    lines.append("};")
    lines.append("")
    lines.append(f"const lh_ui_font_range_t {name}_ranges[] = {{")
    for r_first, r_length, r_base in ranges:
        lines.append(f"    {{{r_first}, {r_length}, {r_base}}},")
    lines.append("};")
    lines.append("")
    lines += [
        "",
        f"const lh_ui_font_t {name} = {{",
        f"    {name}_glyphs, {name}_advances, {name}_tops, {name}_ranges,",
        f"    {line_height}, {ascent}, {cap_height}, {len(ranges)}",
        "};",
        "",
    ]
    args.output.write_text("\n".join(lines), encoding="utf-8", newline="\n")
    inked = sum(1 for g in glyphs if g is not None)
    print(
        f"{args.output} line {line_height} ascent {ascent} cap {cap_height} bpp {args.bpp} "
        f"glyphs {count} over {len(ranges)} run(s): {spans}   inked {inked}   bits {len(bits)}"
    )
    print("  asked for: " + (", ".join(languages) if languages else "code points only"))
    if holes:
        # Printed whether or not it is a problem: a code the font has no glyph for was
        # dropped, and a run was cut either side of it. Silence here would leave the
        # only trace of it in the run count, where nobody looks for an explanation.
        print("  dropped, no glyph in this font: " +
              ", ".join(f"U+{c:04X}" for c in holes))
    return 0


if __name__ == "__main__":
    sys.exit(main())
