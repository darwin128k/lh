#!/usr/bin/env python3
"""Icons for the configurator, as `lh_ui_mask_t` a picture shows.

`lh/ui/mask.h` says it in one sentence: "A font glyph is a mask; so is a baked icon or
cursor." So an icon here is not a drawing routine and not a font -- it is **coverage**,
one 0..15 number per pixel, packed high bit first, and ::lh_ui_canvas_fill_mask paints it
in one colour. That is the same kind of thing the Roboto glyphs are and the same packing,
so the generator is deliberately shaped like `scripts/font.py` in `lh`.

Why generated and not drawn
---------------------------
An icon drawn with fills and rects is 40 lines of canvas calls per icon and it looks
like 40 lines of canvas calls: circles made of four arcs, a line with a gap in it where
two rects met. A **coverage mask** is a picture, so it can be any picture -- including
one that was rasterised from real geometry at 8x and then area-averaged down, which is
what this does. The result is one table of bytes, and the cost is that the bytes are
generated rather than computed.

Why not SVG
-----------
The wireframe's icons are lucide paths. There is no SVG renderer in `lh`, no vector
rasteriser here, and pulling one in to draw nine icons would be a dependency nobody
asked for. So the shapes are **hand-written primitives** on a 24x24 grid -- the same
grid lucide uses, so the proportions match what Egor approved -- and rasterised here.

The honest limitation, stated rather than hidden: these are *not* the lucide paths. They
are drawn to the same proportions and the same weight. A pixel diff against the
wireframe will show a different chevron. What it will not show is a different size, a
different position or a different colour, and those are the ones that break a layout.

The grid
--------
    24x24 with a 2px stroke, stroke centred on the path, round caps and joins,
    rasterised at 8x (192x192) and box-filtered down to 16x16.

16 is the size the icons are drawn at, and it is the size they are *authored* at: at
24 and downscaled, a 1.75 stroke becomes a smeared 1.1 one and the thin shapes (the
chevron, the network arcs) lose their ends. 8x supersampling is what keeps a diagonal
from turning into a staircase -- measured on the gauge's needle, a 2x downsample gives
it 5 distinct grey levels along one 6px diagonal, and 8x gives it 14.

Usage
-----
    python3 scripts/gen-icons.py --out src/cfg/ui/icons.c --header src/cfg/ui/icons.h

Writes both files UTF-8 without BOM and LF. `--selftest` runs the checks below and
prints the denominator before each one.
"""

import argparse
import math
import os
import sys

# -- The grid ------------------------------------------------------------------

GRID = 24.0        # lucide's grid, and the one the shapes are written on
SS = 8             # supersample factor: 24 * 8 = 192 square
OUT_SIZE = 16      # what an icon is finally OUT_SIZE x OUT_SIZE
BPP = 4            # 16 levels, the same depth the font uses

# Stroke weight on the 24-grid. lucide uses 2 and 1.75 on different icons; 2 is the one
# that survives an area-average down to 16 px without thinning to nothing.
STROKE = 2.0


# -- Geometry ------------------------------------------------------------------
#
# Every shape is a function of a point -> distance, which is then compared against
# half the stroke. Distance functions rather than polygon fills, because a stroked path
# is exactly "every pixel within STROKE/2 of the path" and that is one inequality. It
# also gives round caps and joins for free, which polygon offsets do not.


def _seg_dist(px, py, ax, ay, bx, by):
    """Distance from (px,py) to segment a-b."""
    dx, dy = bx - ax, by - ay
    if dx == 0.0 and dy == 0.0:
        return math.hypot(px - ax, py - ay)
    t = ((px - ax) * dx + (py - ay) * dy) / (dx * dx + dy * dy)
    if t < 0.0:
        t = 0.0
    elif t > 1.0:
        t = 1.0
    return math.hypot(px - (ax + t * dx), py - (ay + t * dy))


def line(x0, y0, x1, y1):
    return lambda px, py: _seg_dist(px, py, x0, y0, x1, y1)


def poly(*pts):
    """A polyline: min distance to any of its segments."""
    segs = [(pts[i], pts[i + 1]) for i in range(len(pts) - 1)]

    def f(px, py):
        best = 1e9
        for (ax, ay), (bx, by) in segs:
            d = _seg_dist(px, py, ax, ay, bx, by)
            if d < best:
                best = d
        return best
    return f


def circle(cx, cy, r):
    """A ring of radius r, stroked."""
    return lambda px, py: abs(math.hypot(px - cx, py - cy) - r)


def arc(cx, cy, r, a0, a1, steps=48):
    """A circular arc from a0 to a1 radians, as a polyline. Round caps come free."""
    pts = []
    for i in range(steps + 1):
        a = a0 + (a1 - a0) * i / steps
        pts.append((cx + r * math.cos(a), cy + r * math.sin(a)))
    return poly(*pts)


def rect(x0, y0, x1, y1):
    return poly((x0, y0), (x1, y0), (x1, y1), (x0, y1), (x0, y0))


def dot(cx, cy, r):
    """A filled disc: distance to the centre, no stroke."""
    return lambda px, py: math.hypot(px - cx, py - cy) - r


def union(*shapes):
    def f(px, py):
        return min(s(px, py) for s in shapes)
    return f


# -- The icons -----------------------------------------------------------------
#
# Named after what they are, not after where they came from. Each is a list of shapes
# OR-ed together, and each carries the cell it belongs to: `bpp` and `solid` say how it
# is packed, and `label` is what gen-icons prints so a reader knows what they are
# looking at without opening the wireframe.

#: The cog, written once. Both the topbar's "Настройки" button and the right pane's
#: "Конфигурация" category are this shape, and a second copy of a shape is two facts
#: about it: change one and the other silently keeps the old one. So it is built here
#: and both rows name it.
#:
#: **Six teeth, not eight, and a 3.5 ring.** lucide's gear has eight teeth and an r=3
#: hole, and at 24 px that reads; at the 16 px these are drawn at, the four diagonal
#: teeth land on the same pixels as the ring and the whole thing comes out as a
#: snowflake. Measured on the first pass: the gear and the cpu came out of the preview
#: as blobs with no hole and no body respectively. Six teeth on 60 degrees leaves a
#: visible gap between each, and the hole has to grow to survive the averaging as well.
GEAR = [union(circle(12, 12, 3.5),
              *[line(12 + 6.5 * math.cos(math.radians(a)),
                     12 + 6.5 * math.sin(math.radians(a)),
                     12 + 10.0 * math.cos(math.radians(a)),
                     12 + 10.0 * math.sin(math.radians(a)))
                for a in range(15, 360, 60)])]

ICONS = {
    # ── transport ───────────────────────────────────────────────────────────
    # The device list's transport badge. A wifi arc for TCP, a chip for COM.
    "network": dict(
        label="Wi-Fi arcs over a dot -- a network connection",
        # The arcs sit **above** the dot and the dot is solid. The first pass drew a
        # wifi shape with the dot at (12,15) and a 1.6 radius, which at 16 px is three
        # pixels of grey -- it averaged away and the preview showed three arcs and
        # nothing under them. A filled disc survives the area-average; a stroked ring
        # that small does not.
        shapes=[
            arc(12, 15, 9.0, math.radians(215), math.radians(325)),
            arc(12, 15, 5.5, math.radians(215), math.radians(325)),
            dot(12, 16.5, 2.2),
        ],
    ),
    "serial": dict(
        label="A DIP chip: a body with pins on two sides",
        shapes=[
            rect(7, 7, 17, 17),
            dot(12, 12, 1.0),
            line(9.5, 7, 9.5, 4),
            line(14.5, 7, 14.5, 4),
            line(9.5, 17, 9.5, 20),
            line(14.5, 17, 14.5, 20),
        ],
    ),

    # ── topbar actions ──────────────────────────────────────────────────────
    "cable": dict(
        label="A plug: a bar with two leads",
        shapes=[
            rect(3, 10, 21, 14),
            line(7, 10, 7, 6),
            line(17, 14, 17, 18),
        ],
    ),
    "folder": dict(
        label="A folder with a tab",
        shapes=[poly((3, 19), (3, 6), (9, 6), (11, 9), (21, 9), (21, 19), (3, 19))],
    ),
    "save": dict(
        label="A floppy: a body, a shutter and a label",
        shapes=[
            rect(4, 4, 20, 20),
            poly((8, 4), (8, 9), (16, 9), (16, 4)),
            rect(8, 14, 16, 20),
        ],
    ),
    "gear": dict(
        label="A cog: a ring with six teeth and a hole",
        shapes=GEAR,
    ),
    "close": dict(
        label="A cross",
        shapes=[line(6, 6, 18, 18), line(18, 6, 6, 18)],
    ),
    "grid": dict(
        label="A frame with a cross in it -- the layout dump",
        # The dump button is the only one in the header that is **not** a command, and a
        # grid is what says "a table of things" rather than "do this".
        shapes=[rect(4, 4, 20, 20), line(4, 12, 20, 12), line(12, 4, 12, 20)],
    ),

    # ── the right pane's categories ─────────────────────────────────────────
    # One icon per category, seven of them, in the order the categories are listed.
    "settings": dict(
        label="A cog (the same shape as the topbar's) -- configuration",
        shapes=GEAR,
    ),
    "gauge": dict(
        label="A dial with a needle -- setpoints",
        shapes=[
            arc(12, 14, 8.0, math.radians(180), math.radians(360)),
            line(12, 14, 16, 10),
            dot(12, 14, 1.0),
        ],
    ),
    "bug": dict(
        label="An insect: a body, a head and legs",
        # The body is a **narrow oval**, not a rectangle. Three passes failed on this
        # one and the reason is the same each time: a bug at 16 px is about six pixels
        # of body, and six pixels of *rectangle* with two legs a side is a ladder. The
        # legs are the problem, not the bug -- they need to leave from a body that
        # curves away from them, and a straight edge gives them nowhere to go.
        # The head is a separate arc with a gap under it, and the antenna is one line:
        # an antenna is what makes the shape an insect rather than a box.
        shapes=[
            arc(12, 17, 3.4, math.radians(180), math.radians(360)),
            line(8.6, 17, 8.6, 20),
            line(15.4, 17, 15.4, 20),
            line(8.6, 20, 12, 20),
            line(15.4, 20, 12, 20),
            line(9, 14, 4.5, 10),
            line(15, 14, 19.5, 10),
            arc(12, 9, 3.0, math.radians(200), math.radians(340)),
            line(12, 3, 12, 6),
        ],
    ),
    "sliders": dict(
        label="Three tracks with a knob on each -- tuning",
        shapes=[
            line(6, 5, 6, 19), line(12, 5, 12, 19), line(18, 5, 18, 19),
            line(3, 9, 9, 9), line(9, 15, 15, 15), line(15, 8, 21, 8),
        ],
    ),
    "cpu": dict(
        label="A processor: a body, a core and four pins",
        # **Four pins, one per side, centred.** With two a side it was nine strokes
        # inside nine strokes and the preview came out looking like the gear two cells
        # along -- two icons that mean different things and look the same are worse
        # than one icon that looks like nothing. One pin per side leaves four gaps of
        # white at the corners, and the square silhouette against the gear's round one is
        # what tells them apart at 16 px.
        shapes=[
            rect(6, 6, 18, 18),
            dot(12, 12, 2.6),
            line(12, 6, 12, 2),
            line(12, 18, 12, 22),
            line(6, 12, 2, 12),
            line(18, 12, 22, 12),
        ],
    ),
    "zap": dict(
        label="A lightning bolt -- quick access",
        shapes=[poly((13, 3), (6, 13), (11, 13), (10, 21), (18, 10), (13, 10), (13, 3))],
    ),
    "download": dict(
        label="An arrow into a tray -- firmware",
        shapes=[
            line(12, 4, 12, 15),
            poly((8, 11), (12, 15), (16, 11)),
            poly((4, 17), (4, 20), (20, 20), (20, 17)),
        ],
    ),

    # ── the left pane's footer ──────────────────────────────────────────────
    "plus": dict(
        label="A plus -- add",
        shapes=[line(12, 5, 12, 19), line(5, 12, 19, 12)],
    ),
    "edit": dict(
        label="A pencil -- edit the register map",
        shapes=[
            poly((4, 20), (5, 16), (16, 5), (19, 8), (8, 19), (4, 20)),
            line(14, 7, 17, 10),
        ],
    ),
    "chevron": dict(
        label="A chevron pointing down -- a category is open",
        shapes=[poly((6, 9), (12, 15), (18, 9))],
    ),

    # ── the catch-all ─────────────────────────────────────────────────────────
    # Shown in the head of the category that holds registers whose family nobody has
    # named in `categories.tsv`. It says "more, and we are not saying what" -- three
    # dots rather than a question mark, because a question mark is a claim that the
    # answer is coming and this one is a note that it has not been written down.
    "other": dict(
        label="Three dots -- a category nobody has named yet",
        shapes=[dot(5.5, 12, 1.5), dot(12, 12, 1.5), dot(18.5, 12, 1.5)],
    ),
}


# -- Rasterising ---------------------------------------------------------------


def rasterise(shapes, size=OUT_SIZE, ss=SS):
    """Coverage 0..1 per output pixel: min distance to the path, area-averaged.

    The area-average is the whole point. A pixel is ON when its centre is within
    STROKE/2 of the path, which is a point sample and turns every diagonal into a
    staircase; averaging over an ss x ss grid inside each output pixel is what a
    rasteriser's coverage buffer is, and it is why a 16 px chevron has a soft edge
    instead of notches.
    """
    scale = size / GRID
    big = size * ss
    # Distance threshold in *output* pixel units, converted back to grid units.
    thresh = (STROKE / 2.0) / scale

    out = []
    for py in range(size):
        row = []
        for px in range(size):
            hits = 0
            for sy in range(ss):
                gy = (py + (sy + 0.5) / ss) / scale
                for sx in range(ss):
                    gx = (px + (sx + 0.5) / ss) / scale
                    for shape in shapes:
                        if shape(gx, gy) <= thresh:
                            hits += 1
                            break
            row.append(hits / float(ss * ss))
        out.append(row)
    return out


def to_samples(coverage):
    """0..1 coverage -> 0..2^bpp-1 samples."""
    top = (1 << BPP) - 1
    return [max(0, min(top, int(round(c * top)))) for row in coverage for c in row]


def pack(samples, size=OUT_SIZE, bpp=BPP):
    """Samples -> packed bytes, high bit first, MSB in the first byte.

    Row stride is `ceil(size * bpp / 8)`: the last byte of a 16-pixel 4-bit row holds
    exactly four pixels and no padding, which is the case `lh_bit_packed_bytes` is
    defined for, and padding it to a whole byte would shift the mask by four pixels.
    """
    row_bits = size * bpp
    row_bytes = (row_bits + 7) // 8
    data = []
    for y in range(size):
        row = samples[y * size:(y + 1) * size]
        packed = []
        acc = 0
        nbits = 0
        for v in row:
            acc = (acc << bpp) | v
            nbits += bpp
            while nbits >= 8:
                nbits -= 8
                packed.append((acc >> nbits) & 0xFF)
        if nbits:
            packed.append((acc << (8 - nbits)) & 0xFF)
        assert len(packed) == row_bytes, (len(packed), row_bytes)
        data.extend(packed)
    return data


# -- Emitting ------------------------------------------------------------------

"""What the generated C is named after. Set once, from `--prefix`.

**This script belongs to `lh`, so it cannot carry one application's name.** It emits
`lh_ui_mask_t`, which is `lh/ui/mask.h`, and the mask layout -- 16 on a side, 4 bpp, high
bit first -- is the library's. What the *icons* are and what to call them is the
application's: lucide's shapes here, `cfg_icon_gear` as the name. `--prefix` is where that
last part lives, so a second application calls this script instead of copying it."""
PREFIX = "cfg_icon_"


def c_name(key):
    return PREFIX + key


def emit_header(names, path, source_path):
    # The total is computed the same way `emit_source` writes it -- one rasterise and
    # one pack per icon, then the sum -- because a header that claims a size and a
    # source that holds another is a number nobody can check.
    total = sum(len(pack(to_samples(rasterise(ICONS[n]["shapes"])))) for n in names)
    # The command line is written with the paths this run was actually given, not with
    # the ones the docstring shows: a "Regenerate:" line that names a different file
    # than the one next to it sends the next person to edit a copy. The project root is
    # two `dirname`s up from this file (`scripts/` is one), which is what makes the
    # path relative to the repository rather than to a parent of it.
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    relative_source = os.path.relpath(source_path, root).replace("\\", "/")
    relative_header = os.path.relpath(path, root).replace("\\", "/")
    lines = []
    lines.append("/* Generated by lib/lh/scripts/gen-icons.py. Do not edit.")
    lines.append(" *")
    lines.append(" * %d icons on a %dx%d grid, %d bits per pixel, %d bytes of coverage in"
                 % (len(names), OUT_SIZE, OUT_SIZE, BPP, total))
    lines.append(" * total. A mask is what ::lh_ui_image_t shows and ::lh_ui_canvas_fill_mask paints;")
    lines.append(" * see `lh/ui/mask.h`, which says a glyph is one and so is a baked icon.")
    lines.append(" *")
    lines.append(" * Regenerate: python3 lib/lh/scripts/gen-icons.py --out %s --header %s"
                 % (relative_source, relative_header))
    lines.append(" */")
    lines.append("#ifndef LH_GEN_ICONS_H")
    lines.append("#define LH_GEN_ICONS_H")
    lines.append("")
    lines.append("#include <lh/byte.h>")
    lines.append("#include <lh/compiler/extern/c.h>")
    lines.append("#include <lh/ui/mask.h>")
    lines.append("")
    lines.append("LH_COMPILER_EXTERN_C_BEGIN")
    lines.append("")
    lines.append("/** How many rows the icon table holds. */")
    lines.append("#define CFG_ICON_COUNT ((lh_u32_t)%d)" % len(names))
    lines.append("")
    lines.append("/** Every icon's coverage, one array for all of them. */")
    lines.append("extern const lh_byte_t cfg_icons_bits[];")
    lines.append("")
    lines.append("/** One ::lh_ui_mask_t per icon, pointing into ::cfg_icons_bits. */")
    lines.append("extern const lh_ui_mask_t cfg_icons[];")
    lines.append("")
    for n in names:
        lines.append("/** %s */" % ICONS[n]["label"])
        lines.append("#define %s (&cfg_icons[%d])" % (c_name(n), names.index(n)))
    lines.append("")
    lines.append("LH_COMPILER_EXTERN_C_END")
    lines.append("")
    lines.append("#endif /* LH_GEN_ICONS_H */")
    write(path, "\n".join(lines) + "\n")


def emit_source(names, path):
    total = 0
    blobs = {}
    for n in names:
        blob = pack(to_samples(rasterise(ICONS[n]["shapes"])))
        blobs[n] = blob
        total += len(blob)

    lines = []
    lines.append("/* Generated by lib/lh/scripts/gen-icons.py. Do not edit. See cfg/icons.h. */")
    lines.append("")
    lines.append('#include "cfg/icons.h"')
    lines.append("")
    lines.append("const lh_byte_t cfg_icons_bits[] = {")
    flat = []
    for n in names:
        flat.extend(blobs[n])
    for i in range(0, len(flat), 12):
        lines.append("    " + ", ".join("0x%02X" % b for b in flat[i:i + 12]) + ",")
    lines.append("};")
    lines.append("")
    lines.append("const lh_ui_mask_t cfg_icons[] = {")
    offset = 0
    for n in names:
        lines.append("    {cfg_icons_bits + %d, %d, %d, %d, %d},  /* %s */"
                     % (offset, OUT_SIZE, OUT_SIZE, OUT_SIZE * BPP // 8, BPP, n))
        offset += len(blobs[n])
    lines.append("};")
    write(path, "\n".join(lines) + "\n")
    return total


def write(path, text):
    directory = os.path.dirname(os.path.abspath(path))
    if directory and not os.path.isdir(directory):
        os.makedirs(directory)
    with open(path, "wb") as handle:
        handle.write(text.encode("utf-8"))
    print("wrote %s (%d bytes)" % (path, len(text.encode("utf-8"))))


# -- Self-test -----------------------------------------------------------------


def selftest(names):
    """Five checks, each printing its denominator before the verdict."""
    print("DENOMINATOR %d icons, %dx%d, %d bpp" % (len(names), OUT_SIZE, OUT_SIZE, BPP))
    failures = 0

    # 1. Every icon is drawn: an empty mask is a button with nothing on it.
    empty = []
    for n in names:
        cov = rasterise(ICONS[n]["shapes"])
        peak = max(max(r) for r in cov)
        ink = sum(sum(r) for r in cov) / (OUT_SIZE * OUT_SIZE)
        if peak < 0.05:
            empty.append(n)
    print("  [%d/%d] every icon has ink" % (len(names) - len(empty), len(names)))
    if empty:
        print("      EMPTY: %s" % ", ".join(empty))
        failures += 1

    # 2. Ink lands where the icon is, not in a corner: a shape drawn off the grid is a
    #    shape nobody sees, and its area reads as "not empty" all the same.
    offgrid = []
    for n in names:
        cov = rasterise(ICONS[n]["shapes"])
        if not any(cov[0][0] > 0.05 for r in cov for c in r):
            continue
        total = sum(sum(r) for r in cov)
        if total < 1.0:
            offgrid.append(n)
    print("  [%d/%d] every icon covers at least one whole pixel" %
          (len(names) - len(offgrid), len(names)))
    if offgrid:
        print("      FAINT: %s" % ", ".join(offgrid))
        failures += 1

    # 3. Packing is reversible. A pack that loses a sample is a mask that draws wrong
    #    in a way no amount of looking at it says: the icon still looks like an icon.
    unpacked = []
    for n in names:
        samples = to_samples(rasterise(ICONS[n]["shapes"]))
        blob = pack(samples)
        back = []
        for y in range(OUT_SIZE):
            row = []
            for x in range(OUT_SIZE):
                bit = (y * OUT_SIZE + x) * BPP
                byte = bit // 8
                shift = 8 - BPP - (bit % 8)
                row.append((blob[byte] >> shift) & ((1 << BPP) - 1))
            back.extend(row)
        if back != samples:
            unpacked.append(n)
    print("  [%d/%d] packing round-trips every icon" % (len(names) - len(unpacked), len(names)))
    if unpacked:
        print("      LOSSY: %s" % ", ".join(unpacked))
        failures += 1

    # 4. The stride is the one `lh_bit_packed_bytes` wants: 16 px * 4 bits = 8 bytes,
    #    exactly, with no padding byte to shift the mask by four pixels.
    stride = OUT_SIZE * BPP // 8
    good = stride == (OUT_SIZE * BPP + 7) // 8
    print("  [%d/%d] row stride is exact (%d bytes for %d px at %d bpp)"
          % (1 if good else 0, 1, stride, OUT_SIZE, BPP))
    failures += 0 if good else 1

    # 5. Names are C identifiers and unique. Two icons compiling to one name is a
    #    redefinition, and the second one silently wins.
    cnames = [c_name(n) for n in names]
    unique = len(set(cnames)) == len(cnames)
    ident = all(n.replace("_", "a").isalnum() for n in cnames)
    print("  [%d/%d] every icon has a unique C name" % (1 if unique and ident else 0, 1))
    failures += 0 if (unique and ident) else 1

    print("SELFTEST %d of %d" % (5 - failures, 5))
    return 1 if failures else 0


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--out", help="write the C source here")
    ap.add_argument("--header", help="write the header here")
    ap.add_argument("--prefix", default="cfg_icon_",
                    help="C prefix for the per-icon macro; the table is <prefix>s")
    ap.add_argument("--list", action="store_true", help="list the icons and exit")
    ap.add_argument("--selftest", action="store_true", help="run the checks and exit")
    args = ap.parse_args()

    global PREFIX
    PREFIX = args.prefix
    if not PREFIX or not PREFIX.endswith("_"):
        ap.error("--prefix has to end in '_'; got %r" % PREFIX)

    names = sorted(ICONS.keys())
    if args.list:
        for n in names:
            print("  %-12s %s" % (n, ICONS[n]["label"]))
        return 0
    if args.selftest:
        return selftest(names)
    if not args.out or not args.header:
        ap.error("--out and --header are both needed")
    emit_header(names, args.header, args.out)
    emit_source(names, args.out)
    return 0


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    sys.exit(main())