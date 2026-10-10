#!/usr/bin/env python3
"""Prove that css2decl.py can still say no.

A parser for a language is only worth having if a wrong stylesheet is caught rather than
half-applied. The one this project cannot afford is the quiet kind: a `margin` that is
read and dropped leaves the row where the stylesheet did not put it, and the picture
still looks finished. So every refusal below is a case that must turn the tool red, and
the denominator is printed before anything is compared.

Each case is a whole stylesheet, built around the same two rows, with exactly one thing
wrong with it. Nothing here writes to the project: `read_sheet` / `measure` are called
directly and the result is never emitted.
"""

import importlib.util
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
CSS2DECL = os.path.join(HERE, "css2decl.py")
# **A path on the command line, and nothing else.** This script used to reach up out of the
# application it shipped with and read `ui/configurator.css` from two directories over --
# which worked exactly as long as the file stayed where it was, and the day it moved was
# the day the generator's own self-test stopped running. A generator that belongs to the
# library has no stylesheet of its own to check, so the check that a **real** sheet still
# goes through is worth keeping and is worth being told which sheet:
#
#     python3 scripts/css2decl-selftest.py ../some-app/ui/window.css
#
# Without an argument it is skipped and the denominator says so, rather than reporting a
# row it did not run.
REAL_CSS = sys.argv[1] if len(sys.argv) > 1 else ""

spec = importlib.util.spec_from_file_location("css2decl", CSS2DECL)
css = importlib.util.module_from_spec(spec)
spec.loader.exec_module(css)

OK = """
:root { --ink: #D2D8E2; }
box#root {
    display: flex; flex-direction: column;
    width: 400; height: 300;
    label#title { width: 400; height: 40; color: var(--ink); text: "Hi"; }
    box#body { display: flex; flex-direction: row; width: 400; height: 260;
        label#a { width: 200; height: 260; }
        label#b { width: 200; height: 260; }
    }
}
"""

#: name -> (stylesheet, a word the refusal must contain)
CASES = [
    ("a stylesheet nobody wrote an obvious way round is refused", OK.replace(
        "height: 300;", "margin: 0;"), "margin"),
    ("a font size is refused, because a font is the app's", OK.replace(
        "height: 40;", "font-size: 16;"), "font-size"),
    ("a box with no cross size is refused", OK.replace(
        "label#a { width: 200; height: 260; }", "label#a { height: 260; }"), "width"),
    ("a box with no other size is refused", OK.replace(
        "label#a { width: 200; height: 260; }", "label#a { width: 200; }"), "height"),
    ("a percentage is refused: a place takes a scalar", OK.replace(
        "width: 200; height: 260; }", "width: 50%; height: 260; }"), "50%"),
    ("a widget nobody has is refused", OK.replace("label#a", "gauge#a"), "gauge"),
    ("a label that holds children is refused", OK.replace(
        "label#a { width: 200; height: 260; }",
        "label#a { width: 200; height: 260; label#z { width: 10; height: 10; } }"),
     "does not place"),
    ("`display: none` is refused: showing is the app's business", OK.replace(
        "label#a {", "display: none; label#a {"), "none"),
    ("a colour nobody named is refused", OK.replace("var(--ink)", "var(--nope)"), "--nope"),
    ("a colour that is not a colour is refused", OK.replace("#D2D8E2", "cornflower"),
     "cornflower"),
    ("an alignment with no meaning here is refused, and says what is", OK.replace(
        "label#a { width: 200; height: 260; }",
        "text-align: justify; label#a { width: 200; height: 260; }"), "text-align"),
    ("two rows with one id are refused", OK.replace(
        "label#a { width: 200; height: 260; }",
        "label#a { width: 200; height: 260; } label#a { width: 10; height: 10; }"),
     "twice"),
    ("a second screen is refused", OK + "\nbox#other { width: 10; height: 10; }", "one root"),
    ("a scroll bar that drives nothing is refused", OK.replace(
        "label#b { width: 200; height: 260; }",
        "scrollbar#bar { width: 12; height: 260; axis: vertical; }"), "scrolls"),
    ("a scroll bar naming nothing is refused", OK.replace(
        "label#b { width: 200; height: 260; }",
        "scrollbar#bar { width: 12; height: 260; axis: vertical; scrolls: #ghost; }"),
     "ghost"),
    ("an `axis` on something that is not a bar is refused", OK.replace(
        "label#b { width: 200; height: 260; }",
        "axis: vertical; label#b { width: 200; height: 260; }"), "axis"),
    ("an unclosed brace is refused", OK.replace("}", "", 1), "never closed"),
    ("a `}` that closes nothing is refused", OK + "\n}\n", "closes nothing"),
    ("a property nobody reads is refused, and names the one it looks like", OK.replace(
        "height: 300;", "widht: 300;"), "widht"),
    ("an alignment value nobody wrote is refused", OK.replace(
        "label#a { width: 200; height: 260; }",
        "align: middel; label#a { width: 200; height: 260; }"), "middel"),
    ("a row placed by hand with half its coordinates is refused", OK.replace(
        "label#a { width: 200; height: 260; }",
        "position: absolute; left: 0; label#a { width: 200; height: 260; }"), "top"),
    ("a flow property on a container with no flow is refused", OK.replace(
        "box#body { display: flex; flex-direction: row; width: 400; height: 260;",
        "box#body { flex-direction: row; width: 400; height: 260;"), "display: flex"),
]

#: the sheets that must be **accepted**, because an instrument where every sheet turns
#: red proves nothing at all -- it cannot tell a refusal from a crash from a bug. The
#: good one first, and then two that differ from it by exactly one legal thing.
ACCEPT = [
    ("a whole good stylesheet is accepted", OK),
    ("a stylesheet with no `:root` at all is accepted", "\n".join(
        line for line in OK.split("\n") if "--ink" not in line).replace(
        "color: var(--ink); ", "")),
    # `padx` and `pady` are the two halves of `padding`, and they exist because
    # **`padding` cannot centre anything**: a label's vertical alignment happens inside
    # the box its padding leaves, so 14 rows of padding on a 28-row row leaves a content
    # box zero rows high and the middle of nothing is its top edge. Measured on the Add
    # device button: the caption sat at ink rows 14..25 of a box 0..28, six rows below
    # the middle. With `padx` the same row is 8/12/8.
    ("`padx` and `pady` are accepted", OK.replace("padding: 14;", "padx: 14; pady: 2;")),
]

#: Claims about what is **written**, which `accepts` above cannot see at all: it reads and
#: measures and never emits. A `:root` variable that the generator resolved into the
#: styles and then forgot to hand to the application therefore passed every case in this
#: file, and that is the whole theme: the application had its own eight colours in C for
#: exactly as long as this was true, and `--ink` in the stylesheet meant the title's colour
#: and nothing else. The project shipped one stylesheet and one `palette[8]`, which is two
#: themes that have to be edited together.
CLAIMS = [
    ("every `:root` colour is handed to the application by name", OK,
     "extern lh_ui_color_t lh_cfg_ui_colour_ink;"),
    ("a `--` in a name becomes a `_`, the other being no C identifier at all",
     OK.replace("--ink: #D2D8E2;", "--accent-dim: #D2D8E2;").replace("var(--ink)",
                                                                    "var(--accent-dim)"),
     "extern lh_ui_color_t lh_cfg_ui_colour_accent_dim;"),
]

print("css2decl.py: %s" % CSS2DECL)
print("every case is one wrong thing in an otherwise good stylesheet")
print()
print("CASES %d to refuse, %d to accept, and the project's own stylesheet"
      % (len(CASES), len(ACCEPT)))
for name, _sheet, _word in CASES:
    print("  - %s" % name)
for name, _sheet in ACCEPT:
    print("  + %s" % name)
print()


def accepts(sheet):
    """(True, message) when the sheet goes through, (False, '') when it does not."""
    try:
        variables, root = css.read_sheet(sheet)
        css.measure(root, variables)
        css.index_rows(root)
    except Exception as bad:                          # noqa: BLE001 -- a crash is a refusal
        return False, "%s: %s" % (type(bad).__name__, bad)
    return True, ""


def refuses(sheet):
    """(True, message) when the sheet is turned down, (False, '') when it is not.

    @p variables goes into `measure` because a colour check with no `:root` in hand
    refuses *every* sheet: the first version of this instrument called `measure(root)`
    and all nineteen cases turned red on the same `var(--ink)` -- an instrument that
    agrees with itself.
    """
    try:
        variables, root = css.read_sheet(sheet)
        css.measure(root, variables)
        css.index_rows(root)
    except css.Failure as bad:
        return True, str(bad)
    except Exception as bad:                       # noqa: BLE001 -- a crash is not a refusal
        return False, "%s: %s" % (type(bad).__name__, bad)
    return False, ""


def writes(sheet, needle):
    """(True, '') when the generated **header** says @p needle, (False, why) when not.

    `accepts` above never emits, and this is the one claim that cannot be made without
    emitting: the generator resolved `--ink` into the title's style perfectly well, and
    what was missing was a line of C the application could name. Both halves have to be
    asked for -- the sheet has to go through *and* the header has to say it -- or a
    generator that dropped the variables entirely still looks right.
    """
    try:
        variables, root = css.read_sheet(sheet)
        css.measure(root, variables)
        count = css.index_rows(root)
        head = css.header(count, variables)
    except Exception as bad:                       # noqa: BLE001 -- a crash is not a claim
        return False, "%s: %s" % (type(bad).__name__, bad)
    if needle in head:
        return True, ""
    return False, "the header does not say %r" % needle


passed = 0
total = len(CASES) + len(ACCEPT) + len(CLAIMS) + (1 if REAL_CSS else 0)

# --- a real stylesheet, which has to keep going through
if REAL_CSS:
    try:
        with open(REAL_CSS, encoding="utf-8") as fh:
            sheet = fh.read()
        variables, root = css.read_sheet(sheet)
        ids = css.measure(root, variables)
        count = css.index_rows(root)
        print("[ok] %s generates" % os.path.basename(REAL_CSS))
        print("      rows: %d, ids: %d" % (count, len(ids)))
        passed += 1
    except Exception as bad:                          # noqa: BLE001
        print("[NO] %s generates" % os.path.basename(REAL_CSS))
        print("      said: %s: %s" % (type(bad).__name__, bad))
else:
    print("[--] a real stylesheet: skipped, pass one as the only argument to run it")

# --- sheets that must go through
for name, sheet in ACCEPT:
    took, said = accepts(sheet)
    print("[%s] %s" % ("ok" if took else "NO", name))
    if took:
        print("      accepted")
    else:
        print("      refused a good sheet: %s" % said.replace("\n", " ")[:150])
    passed += took

for name, sheet, needle in CLAIMS:
    said_it, said = writes(sheet, needle)
    print("[%s] %s" % ("ok" if said_it else "NO", name))
    print("      wrote %r: %s" % (needle, said_it))
    if not said_it:
        print("      %s" % said.replace("\n", " ")[:150])
    passed += said_it

for name, sheet, word in CASES:
    refused, said = refuses(sheet)
    named = word in said
    good = refused and named
    print("[%s] %s" % ("ok" if good else "NO", name))
    print("      refused: %-5s   named %r: %s" % (refused, word, named))
    if refused:
        print("      said: %s" % said.replace("\n", " ")[:150])
    else:
        print("      the sheet was accepted, so nothing was refused")
    passed += good

print()
print("PASSED %d of %d cases" % (passed, total))
sys.exit(0 if passed == total else 1)