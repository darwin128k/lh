#!/usr/bin/env python3
"""Draw every icon as a PNG sheet, so `gen-icons.py`'s geometry can be looked at.

The self-test answers questions about *packing*: is the stride right, does a sample
survive the round trip, is there ink at all. None of those say the icon is the shape it
claims to be -- a chevron drawn as a plus has ink, packs exactly and passes 17 of 17.

So this writes one image: every icon at 4x (64x64) with its name under it, on white,
which is what a person can actually check. `--selftest` is a packing test; this is the
other kind.

    python3 scripts/icons-preview.py --out %TEMP%\\icons.png
"""

import argparse
import importlib.util
import os
import sys

from PIL import Image, ImageDraw  # noqa: E402


def _load():
    """Load `gen-icons.py` by path: a dash is legal in a filename and not in a module
    name, so `import gen_icons` cannot work however the path is set up. Loading by
    file location is the one spelling that is true for either name."""
    here = os.path.dirname(os.path.abspath(__file__))
    spec = importlib.util.spec_from_file_location("gen_icons", os.path.join(here, "gen-icons.py"))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


gen_icons = _load()

SCALE = 4          # 16 px icon -> 64 px cell
PAD = 8
LABEL_H = 14
COLUMNS = 6


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--out", required=True)
    args = ap.parse_args()

    names = sorted(gen_icons.ICONS.keys())
    cell = gen_icons.OUT_SIZE * SCALE
    rows = (len(names) + COLUMNS - 1) // COLUMNS
    width = COLUMNS * (cell + PAD) + PAD
    height = rows * (cell + PAD + LABEL_H) + PAD

    sheet = Image.new("L", (width, height), 255)
    draw = ImageDraw.Draw(sheet)

    for index, name in enumerate(names):
        col = index % COLUMNS
        row = index // COLUMNS
        x0 = PAD + col * (cell + PAD)
        y0 = PAD + row * (cell + PAD + LABEL_H)

        # The frame of the cell, so a missing icon is visibly a missing icon and not
        # just white space where one should be.
        draw.rectangle([x0, y0, x0 + cell - 1, y0 + cell - 1], outline=210)

        coverage = gen_icons.rasterise(gen_icons.ICONS[name]["shapes"])
        tile = Image.new("L", (cell, cell), 255)
        pixels = tile.load()
        for py in range(cell):
            for px in range(cell):
                c = coverage[py // SCALE][px // SCALE]
                pixels[px, py] = max(0, min(255, int(round(255 * (1.0 - c)))))
        sheet.paste(tile, (x0, y0))
        draw.text((x0, y0 + cell + 2), name, fill=0)

    sheet.save(args.out)
    print("wrote %s (%dx%d, %d icons, %d columns)"
          % (args.out, width, height, len(names), COLUMNS))
    return 0


if __name__ == "__main__":
    sys.exit(main())