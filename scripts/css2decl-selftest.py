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
    ("a `z-index` that is not a number is refused", OK.replace(
        "box#body { display: flex; flex-direction: row; width: 400; height: 260;",
        "box#body { width: 400; height: 260;").replace(
        "label#a { width: 200; height: 260; }",
        "label#a { position: absolute; left: 0; top: 0; width: 1; height: 1; z-index: top; }"),
     "whole number"),
    ("a `display` nothing here lays out is refused", OK.replace(
        "box#body { display: flex;", "box#body { display: grid;"), "grid"),
    ("a property nothing here can draw is refused, and says why", OK.replace(
        "height: 40;", "height: 40; transform: rotate(5deg);"), "transform"),
    ("a font with a line height is refused: a label is one line", OK.replace(
        "height: 40;", "height: 40; font: 14px/20px body;"), "line height"),
    ("a box shadow with a spread is refused", OK.replace(
        "height: 40;", "height: 40; box-shadow: 0 2px 8px 3px #000;"), "spread"),
    ("two box shadows are refused: a style has one", OK.replace(
        "height: 40;", "height: 40; box-shadow: 0 1px 2px #000, 0 2px 4px #000;"),
     "more than one"),
    ("a corner each is refused: a style has one radius", OK.replace(
        "height: 40;", "height: 40; border-radius: 2px 4px;"), "one radius"),
    # A size is required only where nothing else can give one: the window, a row placed
    # by hand, and a row in a box that places nothing. Everywhere else the flow does.
    ("a root with no width is refused: it is the window", OK.replace(
        "    width: 400; height: 300;", "    height: 300;"), "the root"),
    ("a row placed by hand with no height is refused", OK.replace(
        "box#body { display: flex; flex-direction: row; width: 400; height: 260;",
        "box#body { width: 400; height: 260;").replace(
        "label#a { width: 200; height: 260; }",
        "label#a { width: 200; position: absolute; left: 0; top: 0; }"), "placed by hand"),
    ("a row with no size in a box that places nothing is refused", OK.replace(
        "box#body { display: flex; flex-direction: row; width: 400; height: 260;",
        "box#body { width: 400; height: 260;").replace(
        "label#a { width: 200; height: 260; }", "label#a { width: 200; }"), "no `display`"),
    ("`flex: 1` with a length along the flow is refused: two sizes", OK.replace(
        "label#b { width: 200; height: 260; }", "label#b { width: 200; flex: 1; }"),
     "what is"),
    ("`align: stretch` with a cross size is refused: two sizes", OK.replace(
        "label#b { width: 200; height: 260; }",
        "label#b { width: 200; height: 100; align: stretch; }"), "two sizes"),
    ("`align: center` on a box that paints, with no cross size, is refused", OK.replace(
        "label#b { width: 200; height: 260; }",
        "box#b { width: 200; align: center; background: #fff; }"),
     "Only `stretch`"),
    ("a percentage with nothing to be a share of is refused", OK.replace(
        "display: flex; flex-direction: row;", "display: flex; flex-direction: row; gap: 5%;"),
     "5%"),
    ("a unit nobody reads is refused", OK.replace(
        "label#a { width: 200; height: 260; }", "label#a { width: 20vw; }"), "vw"),
    ("a colour function with the wrong count is refused", OK.replace(
        "color: var(--ink);", "color: rgb(1, 2);"), "three numbers"),
    ("a variable nobody defined, and no fallback, is refused", OK.replace(
        "height: 40;", "height: var(--tall);"), "--tall"),
    ("a padding with five sides is refused", OK.replace(
        "height: 40;", "height: 40; padding: 1 2 3 4 5;"), "one to four"),
    ("a widget nobody has is refused", OK.replace("label#a", "gauge#a"), "gauge"),
    ("a label that holds children is refused", OK.replace(
        "label#a { width: 200; height: 260; }",
        "label#a { width: 200; height: 260; label#z { width: 10; height: 10; } }"),
     "does not place"),
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
    ("a caption on something that is not a label is refused", OK.replace(
        "box#body { display: flex;", 'box#body { text: "x"; display: flex;'), "caption"),
    ("an escape a caption does not read is refused", OK.replace(
        'text: "Hi";', r'text: "a\nb";'), r"`\n`"),
    ("a row placed outside a parent that clips is refused: it would not be seen", OK.replace(
        "box#body { display: flex; flex-direction: row; width: 400; height: 260;",
        "box#body { width: 400; height: 260;").replace(
        "label#a { width: 200; height: 260; }",
        "label#a { position: absolute; left: 0; top: -20px; width: 10; height: 10; }"),
     "would not be seen"),
    ("a `white-space` nothing here draws is refused", OK.replace(
        "height: 40;", "height: 40; white-space: break-spaces;"), "white-space"),
    ("a `maxlength` on something that is not an input is refused", OK.replace(
        "height: 40;", "height: 40; maxlength: 8;"), "not an `input`"),
    ("a coordinate without `position` is refused, not a traceback", OK.replace(
        "label#b { width: 200; height: 260; }", "label#b { width: 200; height: 260; left: 4; }"),
     "position: absolute"),
    ("a state on a tag that has none is refused", OK.replace(
        "label#a { width: 200; height: 260; }",
        "label#a { width: 200; height: 260; :hover { background: #fff; } }"), "no such state"),
    ("a state nobody reads is refused, and names the ones there are", OK.replace(
        "label#a { width: 200; height: 260; }",
        "button#a { width: 200; height: 260; :focus { background: #fff; } }"), ":focus"),
    ("a state that moves the row is refused", OK.replace(
        "label#a { width: 200; height: 260; }",
        "button#a { width: 200; height: 260; :hover { width: 210; } }"), "not where it is"),
    ("a pressed look that pads is refused: a pressed style is only paint", OK.replace(
        "label#a { width: 200; height: 260; }",
        "button#a { width: 200; height: 260; :active { padding: 2; } }"), "only paint"),
    ("a state written twice is refused", OK.replace(
        "label#a { width: 200; height: 260; }",
        "button#a { width: 200; height: 260; :hover { radius: 1; } :hover { radius: 2; } }"),
     "twice"),
    ("a row inside a state is refused", OK.replace(
        "label#a { width: 200; height: 260; }",
        "button#a { width: 200; height: 260; :hover { label#z { width: 1; height: 1; } } }"),
     "holds no rows"),
    ("`checked` on something that is not a toggle is refused", OK.replace(
        "label#a { width: 200; height: 260; }",
        "button#a { width: 200; height: 260; checked: true; }"), "toggle"),
    ("a colour nobody named under a state is refused", OK.replace(
        "label#a { width: 200; height: 260; }",
        "button#a { width: 200; height: 260; :hover { background: var(--nope); } }"), "--nope"),
    # -- the cascade
    ("a rule that selects nothing is refused: it is a typo of something here",
     ".primay { background: #fff; }\n" + OK, "selects nothing"),
    ("a rule that holds rows is refused: rules style, the tree adds",
     ".x { label#z { width: 1; height: 1; } }\n" + OK.replace("label#a {", "label#a.x {"),
     "does not add"),
    ("a selector this parser cannot read all of is refused, not read in part",
     "label[title] { color: #fff; }\n" + OK, "[title]"),
    ("a state from a rule on a row that has no states is refused",
     ".x:hover { background: #fff; }\n" + OK.replace("label#a {", "label#a.x {"),
     "no such state"),
    ("a second screen is refused and says how to style a row instead",
     OK + "\nlabel#a { color: #fff; }", "`#a {"),
    ("a caption that never closes is refused", OK.replace('text: "Hi";', 'text: "Hi;'),
     "never closed"),
    # The line is the declaration's, not its rule's: `widht` sits on line 5 of the sheet
    # while `box#root` opens on line 3, which is where every refusal used to point.
    ("a refusal names the line the property is on", OK.replace(
        "    width: 400; height: 300;", "    widht: 400; height: 300;"), "line 5"),
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
    #
    # It used to replace `padding: 14;`, which this sheet does not have, so the case
    # accepted `OK` itself and said nothing about `padx` at all.
    ("`padx` and `pady` are accepted", OK.replace(
        "height: 40;", "height: 40; padx: 14; pady: 2;")),
    # `flex` was read twice -- once as a number and then again as a string -- and every
    # sheet that used it died with a traceback. Nothing here said `flex`, so nothing saw.
    ("`flex: 1` is accepted", OK.replace(
        "label#b { width: 200; height: 260; }", "label#b { flex: 1; }")),
    ("a toggle with all four looks and a pressed one is accepted", OK.replace(
        "label#a { width: 200; height: 260; }",
        "toggle#a { width: 200; height: 260; checked: true; background: #111;"
        " :hover { background: #222; } :checked { background: #333; }"
        " :checked:hover { background: #444; } :active { radius: 3; } }")),
    ("a caption with braces, semicolons and escaped quotes in it is accepted", OK.replace(
        'text: "Hi";', r'text: "a { b }; \"c\" \\";')),
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

#: Claims about the generated **source**, for what only the table says: a caption is a C
#: string literal, and one with a quote, a backslash or a byte outside ASCII in it has
#: to come out as one. It went out between two `"` as it stood, so `\` reached the
#: compiler as the start of an escape nobody wrote.
EMITS = [
    ("`white-space: normal` breaks a caption into lines that fit its box",
     OK.replace("height: 40;", "height: 40; white-space: normal;"),
     "lh_ui_style_set_wrap(lh_addr_of(ui_s_title), lh_ui_text_wrap_words);"),
    ("...and is handed down, as in CSS",
     OK.replace("    width: 400; height: 300;", "    width: 400; height: 300; white-space: normal;"),
     "lh_ui_style_set_wrap(lh_addr_of(ui_s_title), lh_ui_text_wrap_words);"),
    ("`text-overflow: ellipsis` shortens a line that does not fit",
     OK.replace("height: 40;", "height: 40; white-space: nowrap; overflow: hidden; "
                               "text-overflow: ellipsis;"),
     "lh_ui_style_set_overflow(lh_addr_of(ui_s_title), lh_ui_text_overflow_ellipsis);"),
    ("a hex escape in a caption is that code point: backslash-A is a line break",
     OK.replace('text: "Hi";', r'text: "one\A two";'), r'{.label = {"one\012two"}}'),
    ("`margin-left: auto` pushes the row to the far end",
     OK.replace("label#a { width: 200; height: 260; }", "label#a { width: 100; }")
       .replace("label#b { width: 200; height: 260; }", "label#b { width: 100; margin-left: auto; }"),
     "{lh_ui_decl_kind_label, 300, 40, 100, 260, 2,"),
    ("...by a spacer that fills, named after it",
     OK.replace("label#b { width: 200; height: 260; }", "label#b { width: 100; margin-left: auto; }"),
     "/*   4  b__left */"),
    ("an `auto` margin gets nothing when a row of the flow grows, as in CSS",
     OK.replace("label#a { width: 200; height: 260; }", "label#a { flex: 1; }")
       .replace("label#b { width: 200; height: 260; }", "label#b { width: 100; margin-left: auto; }"),
     "!b__left"),
    ("`z-index` draws a row placed by hand later, whatever order it was written in",
     OK.replace("box#body { display: flex; flex-direction: row; width: 400; height: 260;",
                "box#body { width: 400; height: 260;")
       .replace("label#a { width: 200; height: 260; }",
                "label#a { position: absolute; left: 0; top: 0; width: 10; height: 10; z-index: 2; }")
       .replace("label#b { width: 200; height: 260; }",
                "label#b { position: absolute; left: 0; top: 0; width: 10; height: 10; }"),
     "/*   3  b */"),
    # -- widgets built out of others
    ("a row placed by hand in a flow is lifted out of it: the flow gets a box of its own",
     OK.replace("label#b { width: 200; height: 260; }",
                "label#b { width: 200; height: 260; position: absolute; left: 0; top: 0; }"),
     "/*   3  body__flow */"),
    ("...and the placed row is where it said, over the flow",
     OK.replace("label#b { width: 200; height: 260; }",
                "label#b { width: 200; height: 260; position: absolute; left: 0; top: 0; }"),
     "{lh_ui_decl_kind_label, 0, 40, 200, 260, 2,"),
    ("an `input` edits a buffer of its `maxlength` and starts with its `value`",
     OK.replace("label#a { width: 200; height: 260; }",
                'input#a { width: 200; maxlength: 12; value: "hi"; }'),
     '{.input = {ui_buf_a, 12, "hi"}}'),
    ("...and the buffer is there, with room for the terminator",
     OK.replace("label#a { width: 200; height: 260; }", 'input#a { width: 200; maxlength: 12; }'),
     "static lh_char_t ui_buf_a[12 + 1];"),
    ("a `button` with `text` holds a caption, centred",
     OK.replace("label#a { width: 200; height: 260; }", 'button#a { width: 200; text: "OK"; }'),
     "/*   4  a__text */"),
    ("a `checkbox` with `text` is a row of the box and its caption",
     OK.replace("label#a { width: 200; height: 260; }",
                'checkbox#a { text: "Remember me"; checked: true; }'),
     "/*   3  a__row */"),
    ("...and the box keeps its `#id` and its look when checked",
     OK.replace("label#a { width: 200; height: 260; }",
                'checkbox#a { text: "Remember me"; checked: true; }'),
     "lh_ui_style_set_fill(lh_addr_of(ui_s_a__checked), lh_addr_of(ui_c_2d6cdfff_paints));"),
    ("a `progress` is a track and a bar `value / max` of it long",
     OK.replace("label#a { width: 200; height: 260; }",
                "progress#a { width: 200; value: 30; max: 60; }"),
     "{lh_ui_decl_kind_entity, 0, 40, 100, 8, 3,"),
    ("`overflow-y: auto` is a frame, the box less a bar, and the bar driving it",
     OK.replace("label#a { width: 200; height: 260; }",
                "box#a { width: 200; display: flex; flex-direction: column; overflow-y: auto; }"),
     "{lh_ui_decl_kind_scrollbar, 188, 40, 12, 260, 3,"),
    # -- CSS properties, as the web spells them
    ("`background-color` and `border-radius` are the fill and the radius",
     OK.replace("height: 40;", "height: 40; background-color: #102030; border-radius: 6px;"),
     "lh_ui_style_set_radius(lh_addr_of(ui_s_title), lh_ui_scalar(6));"),
    ("`border` is a width, a style and a colour, in any order",
     OK.replace("height: 40;", "height: 40; border: solid #ff0000 2px;"),
     "lh_ui_style_set_border_fill(lh_addr_of(ui_s_title), lh_addr_of(ui_c_ff0000ff_paints));"),
    ("the content starts inside the border, as in CSS",
     OK.replace("height: 40;", "height: 40; border: 2px solid red; padding: 3px;"),
     "lh_ui_style_set_padding(lh_addr_of(ui_s_title), lh_ui_scalar(5));"),
    ("a border with no style is no border, as in CSS",
     OK.replace("height: 40;", "height: 40; border-width: 2px;"),
     "!lh_ui_style_set_border("),
    ("`box-shadow` is a shift, a fade and a colour",
     OK.replace("height: 40;", "height: 40; box-shadow: 1px 3px 12px rgba(0, 0, 0, 0.5);"),
     "lh_ui_shadow_set_offset(lh_addr_of(ui_shadow_title), lh_ui_scalar(1), lh_ui_scalar(3));"),
    ("`opacity` fades every colour the row paints",
     OK.replace("height: 40;", "height: 40; opacity: 0.5;").replace("color: var(--ink);",
                                                                "color: #ffffff;"),
     "ui_c_ffffff80"),
    ("`opacity` fades the rows under it too",
     OK.replace("    width: 400; height: 300;", "    width: 400; height: 300; opacity: 50%;")
       .replace("color: var(--ink);", "color: #ffffff;"),
     "ui_c_ffffff80"),
    ("`visibility: hidden` keeps the room and paints nothing",
     OK.replace("height: 40;", "height: 40; visibility: hidden;").replace(
         "color: var(--ink);", "color: #ffffff;"), "ui_c_ffffff00"),
    ("a font is a resource the application defines, named by family, size and weight",
     OK.replace("height: 40;", "height: 40; font: bold 14px 'Roboto Mono', monospace;"),
     "lh_ui_style_set_font(lh_addr_of(ui_s_title), lh_addr_of(lh_cfg_ui_font_roboto_mono_14_bold));"),
    ("`text-transform` is applied to the caption",
     OK.replace("height: 40;", "height: 40; text-transform: uppercase;"),
     '{.label = {"HI"}}'),
    ("`align-items: center` centres a caption with no height by its text",
     OK.replace("display: flex; flex-direction: row;",
                "display: flex; flex-direction: row; align-items: center;")
       .replace("label#a { width: 200; height: 260; }", "label#a { width: 200; }"),
     "lh_ui_style_set_align_v(lh_addr_of(ui_s_a), lh_ui_text_align_v_center);"),
    ("`align-items: stretch` leaves a row that states its size at that size",
     OK.replace("display: flex; flex-direction: row;",
                "display: flex; flex-direction: row; align-items: stretch;")
       .replace("label#a { width: 200; height: 260; }", "label#a { width: 200; height: 100; }"),
     "{lh_ui_decl_kind_label, 0, 40, 200, 100, 2,"),
    ("`flex: 0 0 120px` is a fixed length along the flow",
     OK.replace("label#a { width: 200; height: 260; }", "label#a { flex: 0 0 120px; }"),
     "{lh_ui_decl_kind_label, 0, 40, 120, 260, 2,"),
    ("`display: block` stacks its rows down",
     OK.replace("box#body { display: flex; flex-direction: row;", "box#body { display: block;")
       .replace("label#a { width: 200; height: 260; }", "label#a { height: 100; }")
       .replace("label#b { width: 200; height: 260; }", "label#b { height: 100; }"),
     "{lh_ui_decl_kind_label, 0, 140, 400, 100, 2,"),
    ("`right` and `bottom` place a row from the far edges",
     OK.replace("box#body { display: flex; flex-direction: row; width: 400; height: 260;",
                "box#body { width: 400; height: 260;")
       .replace("label#a { width: 200; height: 260; }",
                "label#a { position: absolute; right: 10px; bottom: 20px; width: 50; height: 30; }")
       .replace("label#b { width: 200; height: 260; }",
                "label#b { position: absolute; inset: 0; }"),
     "{lh_ui_decl_kind_label, 340, 250, 50, 30, 2,"),
    ("`aspect-ratio` gives the size along the flow from the size across",
     OK.replace("label#a { width: 200; height: 260; }",
                "label#a { height: 100; aspect-ratio: 16 / 9; }"),
     "{lh_ui_decl_kind_label, 0, 40, 178, 100, 2,"),
    # -- values written the way the web writes them
    ("a percentage is a share of the parent's room",
     OK.replace("label#a { width: 200; height: 260; }", "label#a { width: 50%; height: 260; }"),
     "{lh_ui_decl_kind_label, 0, 40, 200, 260, 2,"),
    ("`px` and `rem` are pixels, a rem being 16 of them",
     OK.replace("label#a { width: 200; height: 260; }", "label#a { width: 12.5rem; height: 260px; }"),
     "{lh_ui_decl_kind_label, 0, 40, 200, 260, 2,"),
    ("a CSS colour name is that colour",
     OK.replace("color: var(--ink);", "color: tomato;"), "lh_ui_color_init(lh_addr_of(ui_c_ff6347ff)"),
    ("`rgba()` carries its alpha",
     OK.replace("color: var(--ink);", "color: rgba(255, 0, 0, 0.5);"), "ui_c_ff000080"),
    ("`hsl()` is converted to the same colour in RGB",
     OK.replace("color: var(--ink);", "color: hsl(120deg 100% 50%);"), "ui_c_00ff00ff"),
    ("a variable can be a length, and its fallback is used when it is not defined",
     OK.replace("height: 40;", "height: var(--tall, 30px);"),
     "{lh_ui_decl_kind_label, 0, 0, 400, 30, 0,"),
    ("`padding` with two values is top and bottom, then left and right",
     OK.replace("height: 40;", "height: 40; padding: 4px 8px;"),
     "lh_ui_insets_init(lh_addr_of(ui_pad_title), lh_ui_scalar(8), lh_ui_scalar(4), "
     "lh_ui_scalar(8), lh_ui_scalar(4));"),
    ("`color` is inherited by every row under the one that says it",
     OK.replace("    width: 400; height: 300;", "    width: 400; height: 300; color: #123456;"),
     # `#a` then looks exactly like `#root`, so it is pointed at the root's style.
     "lh_addr_of(ui_s_root), lh_addr_of(ui_pl_a), lh_null,"),
    ("`!important` outranks the row's own block",
     ".x { color: #00ff00 !important; }\n" + OK.replace("label#title {", "label#title.x {"),
     "lh_ui_style_set_text(lh_addr_of(ui_s_title), lh_addr_of(ui_c_00ff00ff_paints));"),
    ("two rows of one class share one style",
     ".btn { background: #123456; radius: 4; }\n" + OK.replace("label#a { width: 200; height: 260; }", "button#a.btn { width: 200; }")
   .replace("label#b { width: 200; height: 260; }", "button#b.btn { width: 200; }"),
     "lh_addr_of(ui_s_a), lh_addr_of(ui_pl_b)"),
    ("an `#id` outranks a `.class:hover`, so the hover look is the row's own",
     ".btn { background: #123456; }\n.btn:hover { background: #00ff00; }\n"
     "#b { background: #aa0000; }\n" + OK.replace("label#a { width: 200; height: 260; }", "button#a.btn { width: 200; }")
   .replace("label#b { width: 200; height: 260; }", "button#b.btn { width: 200; }"),
     "{.button = {lh_addr_of(ui_s_b), lh_null, lh_null}}"),
    ("`>` is a child and a space is any descendant",
     # Same specificity and later, so if `>` reached a grandchild it would win; the
     # `#title` keeps the rule from selecting nothing at all.
     "#root .btn { radius: 3; }\n#root > .btn, #title { radius: 7; }\n" +OK.replace("label#a { width: 200; height: 260; }", "button#a.btn { width: 200; }")
   .replace("label#b { width: 200; height: 260; }", "button#b.btn { width: 200; }"),
     "lh_ui_style_set_radius(lh_addr_of(ui_s_a), lh_ui_scalar(3));"),
    ("the row's own block outranks every rule",
     "#a.btn { radius: 9; }\n" + OK.replace("label#a { width: 200; height: 260; }",
                                            "button#a.btn { width: 200; radius: 2; }"),
     "lh_ui_style_set_radius(lh_addr_of(ui_s_a), lh_ui_scalar(2));"),
    ("a caption's quotes, backslashes and UTF-8 are escaped for C",
     OK.replace('text: "Hi";', r'text: "\"q\" \\ é";'),
     r'{.label = {"\"q\" \\ \303\251"}}'),
    ("`flex: 1` is a fill place",
     OK.replace("label#b { width: 200; height: 260; }",
                "label#b { flex: 1; }"),
     "lh_ui_place_init(lh_addr_of(ui_pl_b), lh_ui_place_size_fill, lh_ui_scalar(1));"),
    # -- what the engine gained for CSS
    ("`flex: 2` beside `flex: 1` is two thirds and one third",
     OK.replace("label#a { width: 200; height: 260; }", "label#a { flex: 2; }")
       .replace("label#b { width: 200; height: 260; }", "label#b { flex: 1; }"),
     "{lh_ui_decl_kind_label, 266, 40, 133, 260, 2,"),
    ("`justify-content: space-between` puts the leftover between the rows",
     OK.replace("display: flex; flex-direction: row;",
                "display: flex; flex-direction: row; justify-content: space-between;")
       .replace("label#a { width: 200; height: 260; }", "label#a { width: 100; }")
       .replace("label#b { width: 200; height: 260; }", "label#b { width: 100; }"),
     "{lh_ui_decl_kind_label, 300, 40, 100, 260, 2,"),
    ("a margin is room around the row, and the place carries it",
     OK.replace("label#a { width: 200; height: 260; }", "label#a { width: 100; margin: 4px 8px; }"),
     "lh_ui_place_set_margin(lh_addr_of(ui_pl_a), lh_addr_of(ui_mg_a));"),
    ("the row after a margin starts after it",
     OK.replace("label#a { width: 200; height: 260; }", "label#a { width: 100; margin: 4px 8px; }")
       .replace("label#b { width: 200; height: 260; }", "label#b { width: 100; }"),
     "{lh_ui_decl_kind_label, 116, 40, 100, 260, 2,"),
    ("`margin: 0 auto` centres a row across a column",
     OK.replace("label#title {", "box#title2 { width: 100; height: 10; margin: 0 auto; } label#title {"),
     "{lh_ui_decl_kind_container, 150, 0, 100, 10, 0,"),
    ("`max-width` holds a fill",
     OK.replace("label#a { width: 200; height: 260; }", "label#a { flex: 1; max-width: 120px; }"),
     "lh_ui_place_set_max(lh_addr_of(ui_pl_a), lh_ui_scalar(120), lh_ui_scalar(0));"),
    ("`display: none` builds the row hidden and gives it no room",
     OK.replace("label#a { width: 200; height: 260; }", "label#a { width: 200; display: none; }"),
     ", lh_bool_true},"),
    ("a hidden row leaves the next one where it would have been without it",
     OK.replace("label#a { width: 200; height: 260; }", "label#a { width: 200; display: none; }"),
     "{lh_ui_decl_kind_label, 0, 40, 200, 260, 2,"),
]

print("css2decl.py: %s" % CSS2DECL)
print("every case is one wrong thing in an otherwise good stylesheet")
print()
print("CASES %d to refuse, %d to accept, %d about what is written, and the project's own "
      "stylesheet" % (len(CASES), len(ACCEPT), len(CLAIMS) + len(EMITS)))
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


def emits(sheet, needle):
    """(True, '') when the generated **source** says @p needle, (False, why) when not."""
    try:
        variables, root = css.read_sheet(sheet)
        css.measure(root, variables)
        css.index_rows(root)
        source = css.emit(css.walk(root, []), variables, root)
    except Exception as bad:                       # noqa: BLE001 -- a crash is not a claim
        return False, "%s: %s" % (type(bad).__name__, bad)
    # A needle that starts with `!` is something the source must **not** say.
    if needle.startswith("!"):
        if needle[1:] in source:
            return False, "the source says %r" % needle[1:]
        return True, ""
    if needle in source:
        return True, ""
    return False, "the source does not say %r" % needle


passed = 0
total = len(CASES) + len(ACCEPT) + len(CLAIMS) + len(EMITS) + (1 if REAL_CSS else 0)

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

for name, sheet, needle in EMITS:
    said_it, said = emits(sheet, needle)
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