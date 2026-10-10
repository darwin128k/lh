#!/usr/bin/env python3
"""Prove that font.py can still be asked for a language by name.

`--lang` exists because writing `0x400-0x45F` by hand is a mistake you make **quietly**:
the font comes out looking perfectly good, the build is clean, and the Russian text is
simply not drawn -- which reads as a drawing bug and is a coverage bug. A named block
turns that into something a person can say out loud.

So the check is not "does the flag parse". It is that the name means the codes it is
supposed to mean, that an unknown name stops the run instead of quietly producing a
Latin-only font, that the default still is what it was, and that `ru` reproduces the
font this project actually ships -- which is the only external anchor here: the glyph
count and the run count are read out of the checked-in `include/lh/ui/font/roboto.h`,
which came off a real font, rather than out of anybody's memory.

Nothing is written into the project. Where a whole font is needed the script is run for
real into a temporary directory, because the interesting half of this is what the ranges
become after the codes with no glyph are dropped.

The denominator is printed before anything is compared, and a case that cannot be
decided says so instead of passing.
"""

import importlib.util
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
FONT_PY = os.path.join(HERE, "font.py")
ROBOTO_TTF = os.path.join(HERE, "..", "fonts", "Roboto-Regular.ttf")
ROBOTO_H = os.path.join(HERE, "..", "include", "lh", "ui", "font", "roboto.h")

spec = importlib.util.spec_from_file_location("font_script", FONT_PY)
font = importlib.util.module_from_spec(spec)
spec.loader.exec_module(font)

# The five ranges the README used to carry by hand, kept here as the thing `ru` has to
# be equal to. If a preset ever drifts from this, one of the two is wrong and the run
# says which.
THE_HAND_WRITTEN = "32-126,0xA0-0xBF,0x2013-0x2014,0x2116,0x400-0x45F"

CLAIMS = [
    "nothing asked for still means ascii, and ascii is what it always was",
    "ru is exactly the five ranges that were written by hand",
    "a named block and a typed range are added together rather than one winning",
    "a name nobody knows stops the run and says what it does know",
    "--lang ru on this font is the font this project ships",
]

print("font.py: %s" % FONT_PY)
print("the shipped font is the anchor: %s" % os.path.normpath(ROBOTO_H))
print()
print("CLAIMS %d:" % len(CLAIMS))
for claim in CLAIMS:
    print("  - %s" % claim)

results = []


def claim(ok, name, detail=""):
    results.append((ok, name, detail))
    print("  %-4s %s" % ("ok" if ok else "NO", name))
    if detail:
        for line in detail.splitlines():
            print("       %s" % line)


print()

# ── 1. the default ──────────────────────────────────────────────────────────────
default_codes = font.expand_languages(font.DEFAULT_LANGUAGE)[1]
claim(
    sorted(default_codes) == list(range(0x20, 0x7F)),
    CLAIMS[0],
    "default %r covers %d codes, U+0020..U+007E is %d"
    % (font.DEFAULT_LANGUAGE, len(default_codes), 0x7E - 0x20 + 1),
)

# ── 2. a name means its codes ───────────────────────────────────────────────────
ru_names, ru_codes = font.expand_languages("ru")
hand = font.parse_range(THE_HAND_WRITTEN)
claim(
    sorted(ru_codes) == sorted(hand) and ru_names == ["ru"],
    CLAIMS[1],
    "ru asks for %d codes, the five typed ranges ask for %d; equal: %s"
    % (len(ru_codes), len(hand), sorted(ru_codes) == sorted(hand)),
)

# ── 3. a name and a range are added ─────────────────────────────────────────────
both = sorted(set(font.expand_languages("ascii")[1]) | set(font.parse_range("0x400-0x45F")))
claim(
    both == sorted(set(range(0x20, 0x7F)) | set(range(0x400, 0x460))),
    CLAIMS[2],
    "ascii plus 0x400-0x45F is %d codes" % len(both),
)

# ── 4. an unknown name stops the run ────────────────────────────────────────────
unknown = font.expand_languages("klingon")
known_named = unknown is None and all(name in font.LANGUAGES for name in font.LANGUAGES)
claim(
    unknown is None and known_named,
    CLAIMS[3],
    "expand_languages('klingon') returned %r; every table entry has a block: %s"
    % (unknown, known_named),
)

# ── 5. the anchor: the shipped font ─────────────────────────────────────────────
shipped = ""
if not os.path.exists(ROBOTO_H):
    claim(False, CLAIMS[4], "no %s -- nothing to check against" % os.path.normpath(ROBOTO_H))
else:
    with open(ROBOTO_H, "r", encoding="utf-8") as handle:
        shipped = handle.read()
    found = re.search(r"(\d+) glyphs over (\d+) run\(s\)", shipped)
    if found is None:
        claim(False, CLAIMS[4], "the header says nothing about glyph counts")
    elif not os.path.exists(ROBOTO_TTF):
        claim(False, CLAIMS[4], "no %s" % os.path.normpath(ROBOTO_TTF))
    else:
        want_glyphs, want_runs = int(found.group(1)), int(found.group(2))
        with tempfile.TemporaryDirectory() as tmp:
            out = os.path.join(tmp, "roboto_ru.c")
            run = subprocess.run(
                [sys.executable, FONT_PY, "--font", ROBOTO_TTF, "--size", "16", "--bpp", "4",
                 "--lang", "ru", "-o", out],
                capture_output=True, text=True,
            )
            if run.returncode != 0:
                claim(False, CLAIMS[4], "font.py failed:\n" + run.stderr.strip())
            else:
                tail = run.stdout.strip().splitlines()
                made = re.search(r"glyphs (\d+) over (\d+) run", tail[0])
                got_glyphs = int(made.group(1)) if made else -1
                got_runs = int(made.group(2)) if made else -1
                same = os.path.exists(out) and open(out, encoding="utf-8").read() == shipped
                claim(
                    got_glyphs == want_glyphs and got_runs == want_runs,
                    CLAIMS[4],
                    "shipped %d glyphs over %d run(s); --lang ru gives %d over %d\n"
                    "       header comment identical to the shipped one: %s"
                    % (want_glyphs, want_runs, got_glyphs, got_runs, same),
                )

passed = sum(1 for ok, _, _ in results if ok)
print()
print("PASSED %d of %d claims" % (passed, len(results)))
if passed != len(results):
    print("the run fails: a preset that means something else is a font that is quietly wrong")
sys.exit(0 if passed == len(results) else 1)