#!/usr/bin/env python3
"""Turn a CSS file into an `lh_ui_decl_t` table.

`include/lh/ui/decl.h` says there is no parser and no markup language, and for a while
that was right: the table was small, and a second language to learn was a second thing
to keep in step with the first. It stops being right at a thousand rows, because the
part that is painful to write by hand is not a call, it is **where a widget sits and
what it looks like** -- and that is exactly what a stylesheet is.

So this reads CSS and writes the table: as much of CSS as `lh` can draw, and a refusal
that says why for the rest.

The sheet
---------
One top-level rule with a tag and an `#id` is the screen, and the rules nested in it are
its rows: nesting is the tree. Every other top-level rule styles rows the screen already
has, the way a stylesheet styles a page -- `.btn`, `button`, `#ok`, `.bar > button`,
`a, b`, `.btn:hover` -- with the CSS cascade worked out here, once: specificity, then
order, the row's own block above every rule, `!important` above that, and the
inherited properties (`color`, `font-*`, `text-align`, ...) flowing down. Rows that end
up looking the same share one style.

Tags are the widgets (`box`, `label`, `button`, `toggle`, `image`, `input`,
`scrollbar`) or the names a page uses for them (`div`, `p`, `h1`, `span`, `img`,
`checkbox`, ...), with a small user-agent sheet under everything (::UA_SHEET).

What it reads
-------------
* layout: `display: flex | block | grid | none`, `flex-direction`, `flex-wrap`,
  `justify-content` (with
  `space-between / -around / -evenly`), `align-items`, `align-self`, `gap`, `flex`
  (weights, `flex-basis`), `width` / `height` in `px`, `rem`, `em` or `%`, `margin`
  (`auto` either way), `min-` / `max-` sizes, `padding`, `aspect-ratio`,
  `position: absolute | fixed` with any two of `left` / `right` / `width` per axis,
  `z-index`;
* look: `background` (a colour, `linear-gradient()` or `radial-gradient()`), `color`,
  `border` (inside the box, as `box-sizing: border-box`), `border-radius` (each corner), `box-shadow`, `opacity`, `visibility`, `font` (a resource the
  application defines -- see the header), `text-align`, `vertical-align`,
  `text-transform`, `text-overflow: ellipsis`, `white-space` (`normal` wraps; a label is one line
  unless it says so -- the one default that is not CSS's); any colour
  CSS writes, `var()` with fallbacks, `inherit` / `initial`;
* states: `:hover`, `:active`, `:checked`, `:checked:hover`, as rules or nested blocks,
  and `transition` between them (a time, a curve CSS names, the properties).

What lh has no widget for is built out of what it has (::compose): a button with `text`,
a checkbox with a caption, a progress bar, `overflow: auto` (a frame and a scroll bar),
and rows placed by hand over a flow.

What it refuses, and says so: anything that would not look on screen the way the sheet
says -- a grid, a transform, a transition, a second shadow, a row placed where its
parent clips it. A parser that quietly drops a property is worse than no parser: the
stylesheet says the row is 8px further down and the row is not.

The rects in the table are worked out with the engine's own arithmetic, so the first
layout moves nothing; the tests check that against `lh_ui_layout_apply`.

Usage
-----
    python3 scripts/css2decl.py <input.css> <output-dir> [--prefix <name>_]
        [--preview <shot.png> --build <lh-build-dir> [--cc <compiler>] [--watch]]

Writes `<output-dir>/ui_generated.c` and `<output-dir>/ui_generated.h`, both UTF-8
without a BOM and LF, so they are generated files and are not edited by hand.
`--preview` also paints the screen into a PNG with lh's software canvas; `--watch`
paints it again every time the sheet is saved.
"""

import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile
import time
import zlib

"""What every generated C symbol is named after. Set once, from `--prefix`.

The placeholder the generated text is **written** with, and what the default is, are the
same word on purpose: an application that passes nothing gets the names this script was
written with, and one that passes something gets its own. See ::main."""
PREFIX_PLACEHOLDER = "lh_cfg_ui_"
PREFIX = PREFIX_PLACEHOLDER

# -- The vocabulary ------------------------------------------------------------

#: tag -> `lh_ui_decl_kind_t`. A tag is the widget; nothing else decides it.
KINDS = {
    "entity": "lh_ui_decl_kind_entity",
    "box": "lh_ui_decl_kind_container",
    "container": "lh_ui_decl_kind_container",
    "label": "lh_ui_decl_kind_label",
    "button": "lh_ui_decl_kind_button",
    "toggle": "lh_ui_decl_kind_toggle",
    "image": "lh_ui_decl_kind_image",
    "scrollbar": "lh_ui_decl_kind_scrollbar",
    "input": "lh_ui_decl_kind_input",
}

#: The names a page writes, and the widget each one is. A sheet copied from the web
#: says `div` and `p` and `img`; the row is the widget, and the name is kept for the
#: selectors, so `div { }` styles the divs and not every box.
TAG_ALIASES = {
    "div": "box", "section": "box", "header": "box", "footer": "box", "nav": "box",
    "main": "box", "aside": "box", "article": "box", "form": "box", "ul": "box",
    "ol": "box", "li": "box", "fieldset": "box", "progress": "box",
    "span": "label", "p": "label", "h1": "label", "h2": "label", "h3": "label",
    "h4": "label", "h5": "label", "h6": "label", "a": "label", "strong": "label",
    "em": "label", "b": "label", "i": "label", "small": "label", "legend": "label",
    "caption": "label", "text": "label",
    "img": "image", "icon": "image",
    "hr": "entity", "spacer": "entity",
    "checkbox": "toggle", "radio": "toggle", "switch": "toggle",
}

#: What a browser draws before any sheet says a word -- the user-agent sheet -- as far
#: as it says anything this parser draws. It is the lowest layer of the cascade, so
#: every author rule is above it, and a rule of it that selects nothing is not a typo.
UA_SHEET = """
div, section, header, footer, nav, main, aside, article, form, ul, ol, li, fieldset {
    display: block;
}
hr { height: 1px; background: #808080; }
spacer { flex: 1; }
checkbox, radio {
    width: 16px; height: 16px; border: 1px solid #8a8f98; border-radius: 3px;
    background: #ffffff;
}
radio { border-radius: 8px; }
switch { width: 36px; height: 20px; border-radius: 10px; background: #c4c8cf; }
checkbox:checked, radio:checked, switch:checked {
    background: #2d6cdf; border-color: #2d6cdf;
}
progress { height: 8px; border-radius: 4px; background: #e3e6ea; }
input {
    height: 24px; padding: 0 6px; border: 1px solid #8a8f98; border-radius: 3px;
    background: #ffffff; color: #1b1f24; vertical-align: middle;
}
"""

#: tags that may hold children, because only a container places them.
HOLDERS = {"box", "container", "button", "toggle"}

#: A look a row takes on in a state, written as a nested block -- `button#ok { :hover {
#: ... } }` -- and the tags that have that state. Each is a whole style, and it starts
#: from the row's own look the way a CSS pseudo-class does: `:hover { background: ... }`
#: changes the fill and keeps the radius, rather than drawing a square button whenever
#: the pointer is on it.
#:
#: `:active` is the pressed look (::lh_ui_style_set_pressed), and it is put on **every**
#: look the row has, built on top of each: a toggle pressed while it is on has to look
#: pressed and on, not pressed and off.
STATES = {
    ":hover": ("button", "toggle"),
    ":checked": ("toggle",),
    ":checked:hover": ("toggle",),
    ":active": ("button", "toggle"),
}

#: What a state may change: its look, and not where anything is. A `width` under
#: `:hover` would be a button that grows under the pointer, and nothing here resizes a
#: row because of the pointer.
LOOK_PROPS = ("background", "color", "padding", "padx", "pady", "radius-tl", "radius-tr",
              "radius-br", "radius-bl", "shadow",
              "text-align", "valign", "padding-top", "padding-right", "padding-bottom",
              "padding-left", "border-width", "border-top-width", "border-right-width",
              "border-bottom-width", "border-left-width", "border-style", "border-color",
              "box-shadow", "opacity", "visibility", "font-family", "font-size",
              "font-weight", "font-style", "text-overflow", "white-space")

#: Narrower again for `:active`, because ::lh_ui_style_get_pressed reads only the paint
#: of a pressed style: "padding, font and alignment are read from the entity's own
#: style, so nothing moves under the pointer". A `padding` under `:active` would be read
#: and ignored.
PRESSED_PROPS = ("background", "color", "radius-tl", "radius-tr", "radius-br", "radius-bl",
                 "shadow", "box-shadow", "border-color",
                 "opacity")

TEXT_ALIGN = {"left": "lh_ui_text_align_h_left",
              "center": "lh_ui_text_align_h_center",
              "right": "lh_ui_text_align_h_right"}
VALIGN = {"top": "lh_ui_text_align_v_top",
          "center": "lh_ui_text_align_v_center",
          "bottom": "lh_ui_text_align_v_bottom"}

#: Properties that name the right side of a border box, with what to write instead.
#: Listed because "it did not take" is the worst answer a stylesheet author can get.
#:
#: `position`, `left` and `top` are **not** here: they were, until the application had to
#: say where a row goes when the row's parent has no flow of its own -- a title bar, a
#: panel floating over the window, a bar pinned to the right edge. A row that says
#: `position: absolute` with `left` and `top` is put exactly there and takes no room
#: from its parent's cursor; a row that says nothing is placed by the flow, as before.
NOT_SUPPORTED = {
    "float": "a child is a child, at the next place in the flow",
    "transform": "a row is drawn where its rect is; there is no matrix between them",
    "animation": "there are no keyframes here, only a time between two looks: that is "
                 "`transition`",
    "filter": "a style's paint is solid, and nothing is drawn through a filter",
    "backdrop-filter": "same as `filter`",
    "cursor": "the pointer's shape is the window's, not a row's",
    "outline": "use `border`: an outline is a border that takes no room, and a border "
               "here is inside the row's rect anyway",
    "text-shadow": "a shadow belongs to a box, not to its letters",
    "letter-spacing": "a font's advances are baked into it",
    "word-spacing": "same as `letter-spacing`",
    "line-height": "a label is one line, and its height is the font's",
    "text-decoration": "a font draws its letters and nothing under them",
    "pointer-events": "every row that can be clicked is clicked; say `display: none`",
    "user-select": "a label's text is not selectable",
    "content": "there is no `::before` and no `::after`: a caption is a `label` row "
               "with `text`",
    "grid-template-areas": "a grid here is placed in order, a cell after a cell: name the "
                           "rows in the order they go instead",
    "grid-area": "same as `grid-template-areas`",
    "grid-row": "a grid here is placed in order, a row after a row; a cell may `span` columns",
    "grid-auto-flow": "a grid here fills a row before the next, which is `row`",
    "grid-auto-columns": "a grid here has the columns `grid-template-columns` names",
    "order": "the order of the rows is the order in the sheet",
}

DISPLAY_VALUES = {"flex", "block", "none", "grid"}

MARGIN_PROPS = ("margin", "margin-top", "margin-right", "margin-bottom", "margin-left")
BOUND_PROPS = ("min-width", "max-width", "min-height", "max-height")

#: The properties CSS hands from a parent to its children when they say nothing.
INHERITED = ("color", "text-align", "font-family", "font-size", "font-weight", "font-style",
             "text-transform", "visibility", "accent-color", "white-space")
DIRECTIONS = {"row": "lh_ui_axis_horizontal", "column": "lh_ui_axis_vertical"}
AXES = {"horizontal": "lh_ui_axis_horizontal", "vertical": "lh_ui_axis_vertical"}
JUSTIFY = {"start": "lh_ui_justify_start", "flex-start": "lh_ui_justify_start",
           "left": "lh_ui_justify_start", "normal": "lh_ui_justify_start",
           "center": "lh_ui_justify_center",
           "end": "lh_ui_justify_end", "flex-end": "lh_ui_justify_end",
           "right": "lh_ui_justify_end",
           "space-between": "lh_ui_justify_space_between",
           "space-around": "lh_ui_justify_space_around",
           "space-evenly": "lh_ui_justify_space_evenly"}
ALIGN = {"start": "lh_ui_place_align_start", "flex-start": "lh_ui_place_align_start",
         "self-start": "lh_ui_place_align_start",
         "center": "lh_ui_place_align_center",
         "end": "lh_ui_place_align_end", "flex-end": "lh_ui_place_align_end",
         "self-end": "lh_ui_place_align_end",
         "stretch": "lh_ui_place_align_fill", "normal": "lh_ui_place_align_fill",
         "baseline": "lh_ui_place_align_baseline"}
SIZE_MODE = {"fixed": "lh_ui_place_size_fixed", "fill": "lh_ui_place_size_fill",
             "wrap": "lh_ui_place_size_wrap"}

#: Every property, and how its value is read. One table, because a property that is
#: forgotten here is a property nothing checks: the first version listed the names in
#: `read_declarations` (which refuses `margin`) and read the *values* in `emit` -- and
#: `emit` only runs when a file is written, so `text-align: justify` was accepted by
#: every reader and refused by the only writer.
VALUE_TABLES = {
    "text-align": TEXT_ALIGN,
    "valign": VALIGN,
    "align": ALIGN,
    "align-items": ALIGN,
    "justify-content": JUSTIFY,
    "flex-direction": DIRECTIONS,
    "axis": AXES,
    "position": {"absolute": "absolute", "fixed": "fixed", "relative": "relative",
                 "static": "static"},
    "checked": {"true": "lh_bool_true", "false": "lh_bool_false"},
    "text-transform": {"none": "none", "uppercase": "uppercase", "lowercase": "lowercase",
                       "capitalize": "capitalize"},
    "visibility": {"visible": "visible", "hidden": "hidden", "collapse": "hidden"},
    # The only model there is: a size is the whole box, border and padding included.
    "box-sizing": {"border-box": "border-box"},
    # A container clips its children by itself, so `visible` would be a promise this
    # parser cannot keep. `auto` and `scroll` are a scroll bar, put beside the box
    # (::compose).
    "overflow-x": {"hidden": "hidden", "clip": "hidden", "auto": "auto", "scroll": "scroll"},
    "overflow-y": {"hidden": "hidden", "clip": "hidden", "auto": "auto", "scroll": "scroll"},
    "scrollbar-width": {"auto": "auto", "thin": "thin", "none": "none"},
    "border-style": {"solid": "solid", "none": "none", "hidden": "none"},
    "font-style": {"normal": "normal", "italic": "italic"},
    "text-overflow": {"clip": "lh_ui_text_overflow_clip", "ellipsis": "lh_ui_text_overflow_ellipsis"},
    # `wrap-reverse` would put the lines the other way across, which no flow here does.
    "flex-wrap": {"nowrap": "lh_bool_false", "wrap": "lh_bool_true"},
    # `nowrap` and `pre` are what a label does unless told otherwise: its lines end
    # where the text breaks them and nowhere else. `normal` and `pre-wrap` also break
    # before a word that would not fit the box (::lh_ui_style_set_wrap). This is the one
    # default that is not CSS's: a label here was always one line, and a sheet written
    # for that would see its captions fold onto a second line where they were clipped.
    "white-space": {"nowrap": "lh_ui_text_wrap_none", "pre": "lh_ui_text_wrap_none",
                    "pre-line": "lh_ui_text_wrap_none", "normal": "lh_ui_text_wrap_words",
                    "pre-wrap": "lh_ui_text_wrap_words"},
}
NUMERIC_PROPS = ("width", "height", "flex", "row-gap", "column-gap", "padding", "padx",
                 "pady", "radius-tl", "radius-tr", "radius-br", "radius-bl", "left", "top", "right", "bottom", "shadow", "padding-top",
                 "padding-right", "padding-bottom", "padding-left", "flex-basis",
                 "border-width", "border-top-width", "border-right-width",
                 "border-bottom-width", "border-left-width", "font-size")

#: The lengths that may be a percentage, because a parent has a size to be a share of.
SIZED = ("width", "height", "left", "top", "right", "bottom", "flex-basis")
#: Colours; `background` may also be a gradient (::read_paint).
COLOUR_PROPS = ("color", "border-color")

#: `display` is the only thing that gives a container a flow. Everything else about a
#: row's place comes from where the row sits, so a stylesheet that says `gap` without
#: saying `display: flex` is describing a flow that does not exist -- the number would
#: be read by nothing and the rows would sit where the flow put them.
FLOW_PROPS = ("flex-direction", "justify-content", "row-gap", "column-gap", "align-items",
              "flex-wrap")

#: What a declaration may name: the properties this parser keeps, and the CSS ones that
#: are written differently and turned into them (::expand) once variables are in.
CSS_ALIASES = ("flex-flow", "background-color", "background-image", "border-radius", "radius",
               "border-top-left-radius", "border-top-right-radius",
               "border-bottom-right-radius", "border-bottom-left-radius", "vertical-align", "align-self",
               "flex-grow", "flex-shrink", "gap", "border", "border-top", "border-right",
               "border-bottom", "border-left", "inset", "font", "font-family",
               "font-weight", "box-shadow", "opacity", "aspect-ratio", "overflow")

KNOWN_PROPS = (set(VALUE_TABLES) | set(NUMERIC_PROPS) | set(COLOUR_PROPS) | set(CSS_ALIASES)
               | set(MARGIN_PROPS) | set(BOUND_PROPS)
               | {"grid-template-columns", "grid-template-rows", "grid-auto-rows", "grid-column",
                  "justify-items", "justify-self"}
               | {"background", "display", "scrolls", "text", "value", "maxlength", "max",
                  "accent-color",
                  "transition", "transition-duration", "transition-timing-function",
                  "transition-property", "transition-delay",
                  "z-index"})

COLOUR = re.compile(r"^#([0-9a-fA-F]{3}|[0-9a-fA-F]{4}|[0-9a-fA-F]{6}|[0-9a-fA-F]{8})$")
NUMBER = re.compile(r"^-?\d+$")
IDENT = re.compile(r"^[A-Za-z_][A-Za-z0-9_-]*$")


class Failure(Exception):
    """A stylesheet this parser will not guess at, with the line that caused it."""


# -- Lexing --------------------------------------------------------------------

def strip_comments(text):
    """Blank out `/* ... */`, keeping every byte's position so line numbers survive.

    A comment in the middle of a declaration is rare and not worth a lexer for; what
    matters is that an error on line 90 of the file is line 90 of the file.
    """
    out = []
    i = 0
    line = 1
    while i < len(text):
        if text.startswith("/*", i):
            end = text.find("*/", i + 2)
            if end < 0:
                raise Failure("a comment opened and never closed")
            chunk = text[i:end + 2]
            out.append("".join(c if c == "\n" else " " for c in chunk))
            line += chunk.count("\n")
            i = end + 2
        else:
            ch = text[i]
            out.append(ch)
            line += 1 if ch == "\n" else 0
            i += 1
    return "".join(out), line


def mask_strings(text):
    """@p text with every character inside `"..."` replaced by `x`, same length.

    Braces and semicolons are structure and a caption is not, so everything that looks
    for structure looks at this and everything that reads a value reads the original:
    the offsets are the same in both. Scanning the original for `{` read
    `text: "a { b }"` as the start of a child rule and refused the sheet for having two
    tag names in a selector that does not exist.
    """
    out = list(text)
    i = 0
    while i < len(text):
        if text[i] != '"':
            i += 1
            continue
        start = i
        i += 1
        while i < len(text) and text[i] != '"':
            # `\"` is a quote inside the string, and `\\` is a backslash that does not
            # escape the quote after it.
            i += 2 if text[i] == "\\" else 1
        if i >= len(text):
            raise Failure("a `\"` at line %d that is never closed"
                          % (text.count("\n", 0, start) + 1))
        for j in range(start + 1, i):
            if out[j] != "\n":
                out[j] = "x"
        i += 1
    return "".join(out)


def split_top_level(body):
    """Split a block body on `;` that are not inside `( ... )` or `"..."`."""
    parts = []
    depth = 0
    quoted = False
    escaped = False
    cur = []
    for ch in body:
        if escaped:
            escaped = False
        elif quoted and ch == "\\":
            escaped = True
        elif ch == '"':
            quoted = not quoted
        if not quoted:
            if ch == "(":
                depth += 1
            elif ch == ")":
                depth -= 1
                if depth < 0:
                    raise Failure("a `)` with nothing open in front of it")
            elif ch == ";" and depth == 0:
                parts.append("".join(cur))
                cur = []
                continue
        cur.append(ch)
    if depth != 0:
        raise Failure("a `(` that was never closed")
    if quoted:
        raise Failure("a `\"` that was never closed")
    parts.append("".join(cur))
    return parts


# -- The tree ------------------------------------------------------------------

class Node(object):
    def __init__(self, tag, name, line):
        self.tag = tag
        self.name = name            # the `#id`, without the hash; may be ""
        self.classes = set()        # the `.class`es, without the dot
        self.html = tag             # the name the sheet wrote: `div` for a `box`
        self.selector = ""          # set on a style rule: its selector as written
        self.line = line
        self.props = {}             # property -> (value, line)
        self.states = {}            # `:hover` and the like -> {property -> (value, line)}
        self.children = []
        self.parent = None
        # filled in by measure()
        self.rect = (0, 0, 0, 0)
        self.place = None
        self.layout = None
        self.index = -1
        self.hidden = False
        self.margin = (0, 0, 0, 0)
        self.bounds = (0, 0, 0, 0)

    def cname(self):
        """A C name: the `#id` if there is one, else `tag` plus its line.

        Two un-named siblings of the same tag are two different styles, so the line
        number is part of the name rather than a counter that resets per file.
        """
        if self.name:
            return self.name.replace("-", "_")
        return "%s_line%d" % (self.tag, self.line)


def read_selector(sel, line):
    """`label#tree-head.big`, `.row`, `box` -> (tag, id, classes), for a row of the tree.

    A row with no tag is a label, the way a row with only an `#id` always was.
    """
    sel = sel.strip()
    if not sel:
        raise Failure("an empty selector at line %d" % line)
    compound = parse_compound(sel, line)
    if compound["pseudo"]:
        raise Failure("`%s` at line %d is a row of the tree, and a row is not a state. A "
                      "state is written inside its row: `button#ok { :hover { ... } }`"
                      % (sel, line))
    if compound["tag"] == "*":
        raise Failure("`*` at line %d names every widget, and a row of the tree is one"
                      % line)
    html = compound["tag"] or "label"
    tag = TAG_ALIASES.get(html, html)
    if tag not in KINDS:
        raise Failure("`%s` at line %d is not a widget this parser knows. Known: %s"
                      % (html, line, ", ".join(sorted(set(KINDS) | set(TAG_ALIASES)))))
    return html, compound["id"], set(compound["classes"])


# -- Selectors and the cascade -------------------------------------------------

COMPOUND_TOKEN = re.compile(r"(\*|[A-Za-z_][A-Za-z0-9_-]*|#[A-Za-z_][A-Za-z0-9_-]*|"
                            r"\.[A-Za-z_][A-Za-z0-9_-]*|:[a-z-]+)")


def parse_compound(text, line):
    """`button#ok.primary:hover` -> {tag, id, classes, pseudo}. Refuses what it does
    not read rather than reading part of it: `a[href]` is not `a`."""
    out = {"tag": "", "id": "", "classes": [], "pseudo": ""}
    pos = 0
    while pos < len(text):
        m = COMPOUND_TOKEN.match(text, pos)
        if not m:
            raise Failure("`%s` at line %d: `%s` is not something a selector here can say. "
                          "A selector is tags, `#id`s, `.class`es and states, joined by "
                          "spaces, `>` and `,`" % (text, line, text[pos:]))
        token = m.group(0)
        if token[0] == "#":
            if out["id"]:
                raise Failure("two `#id`s in one selector at line %d: %r" % (line, text))
            out["id"] = token[1:]
        elif token[0] == ".":
            out["classes"].append(token[1:])
        elif token[0] == ":":
            out["pseudo"] += token
        else:
            if pos != 0:
                raise Failure("`%s` at line %d: a tag comes first in a selector, and `%s` "
                              "comes after something" % (text, line, token))
            out["tag"] = token
        if out["pseudo"] and token[0] != ":":
            raise Failure("`%s` at line %d: a state comes last, after the tag, the id and "
                          "the classes" % (text, line))
        pos = m.end()
    if out["pseudo"]:
        if out["pseudo"] not in STATES:
            raise Failure("`%s` at line %d is not a state this parser reads. Known: %s"
                          % (out["pseudo"], line, ", ".join(sorted(STATES))))
    if (out["tag"] and out["tag"] != "*" and out["tag"] not in KINDS
            and out["tag"] not in TAG_ALIASES):
        raise Failure("`%s` at line %d is not a widget this parser knows. Known: %s"
                      % (out["tag"], line, ", ".join(sorted(set(KINDS) | set(TAG_ALIASES)))))
    if out["id"] and not IDENT.match(out["id"]):
        raise Failure("`#%s` is not a C identifier at line %d" % (out["id"], line))
    if not (out["tag"] or out["id"] or out["classes"] or out["pseudo"]):
        raise Failure("an empty selector at line %d" % line)
    return out


def parse_selector_list(text, line):
    """`a, b > c d` -> [[(combinator, compound), ...], ...], the first combinator ''."""
    out = []
    for part in text.split(","):
        part = part.strip()
        if not part:
            raise Failure("an empty selector in a list at line %d: %r" % (line, text))
        steps = []
        for chunk in re.split(r"\s*(>)\s*|\s+", part):
            if chunk is None or chunk == "":
                continue
            if chunk == ">":
                if not steps or steps[-1][0] == ">":
                    raise Failure("a `>` with nothing on its left at line %d: %r"
                                  % (line, part))
                steps.append((">", None))
                continue
            compound = parse_compound(chunk, line)
            if steps and steps[-1][0] == ">" and steps[-1][1] is None:
                steps[-1] = (">", compound)
            else:
                steps.append((" " if steps else "", compound))
        if steps and steps[-1][1] is None:
            raise Failure("a `>` with nothing on its right at line %d: %r" % (line, part))
        for combinator, compound in steps[:-1]:
            if compound["pseudo"]:
                raise Failure("`%s` at line %d: a state belongs to the row the rule styles, "
                              "the last one in the selector" % (part, line))
        out.append(steps)
    return out


def specificity(steps):
    """(ids, classes and states, tags), the CSS order of how much a selector says."""
    ids = sum(1 for _c, comp in steps if comp["id"])
    classes = sum(len(comp["classes"]) + (comp["pseudo"].count(":")) for _c, comp in steps)
    tags = sum(1 for _c, comp in steps if comp["tag"] and comp["tag"] != "*")
    return ids, classes, tags


def compound_matches(compound, node):
    if compound["tag"] and compound["tag"] != "*" and compound["tag"] != node.html:
        return False
    if compound["id"] and compound["id"] != node.name:
        return False
    return all(c in node.classes for c in compound["classes"])


def selector_matches(steps, node, at=None):
    """Does the selector whose compounds are @p steps select @p node? Right to left,
    with a real search for ` ` so that `a b > c` finds the `a` that works, not only the
    nearest one."""
    at = len(steps) - 1 if at is None else at
    combinator, compound = steps[at]
    if not compound_matches(compound, node):
        return False
    if at == 0:
        return True
    if combinator == ">":
        return node.parent is not None and selector_matches(steps, node.parent, at - 1)
    up = node.parent
    while up is not None:
        if selector_matches(steps, up, at - 1):
            return True
        up = up.parent
    return False


def apply_rules(root, rules):
    """Fold every style rule into the rows it selects, the way CSS cascades.

    Lowest first: by specificity, then by where the rule is in the sheet; the row's own
    block is last of all, as an inline style is. A state is folded the same way into
    the row's state of that name, so `.btn:hover` and `button#ok { :hover {} }` are one
    look with the more specific one on top.

    A rule that selects nothing is refused. One sheet is one screen, so a rule nothing
    on that screen answers to is a typo -- `.primay` -- and the button it was written
    for is the one that stays grey.
    """
    nodes = []
    stack = [root]
    while stack:
        node = stack.pop()
        nodes.append(node)
        stack.extend(node.children)

    hits = dict((id(n), []) for n in nodes)
    ua = ua_rules()
    for order, rule in enumerate(ua + rules):
        from_ua = order < len(ua)
        used = False
        for steps in rule.selectors:
            pseudo = pseudo_set(steps[-1][1]["pseudo"])
            weight = specificity(steps)
            for node in nodes:
                if selector_matches(steps, node):
                    used = True
                    # The UA sheet is below every author rule whatever its selector:
                    # a weight that starts lower than any rule of the sheet's own.
                    lift = (-1,) if from_ua else (0,)
                    hits[id(node)].append((lift + weight, order, pseudo, rule.props, rule.line))
                    for selector, (state_props, line) in rule.states.items():
                        hits[id(node)].append((lift + (weight[0], weight[1] + selector.count(":"),
                                                       weight[2]), order, pseudo_set(selector),
                                               state_props, line))
        if not used and not from_ua:
            raise Failure("the rule `%s` at line %d selects nothing on this screen. Is "
                          "it a typo of a class or an `#id` that is here?"
                          % (rule.selector, rule.line))

    # The row's own block is an inline style: above every rule, and its own states above
    # it by how many states they name, so `:checked:hover` is over `:checked`.
    inline = (0, 1 << 30, 0, 0)
    for node in nodes:
        layers = list(hits[id(node)])
        layers.append((inline, -1, frozenset(), node.props, node.line))
        for selector, (state_props, line) in node.states.items():
            layers.append(((0, inline[1], selector.count(":"), 0), -1, pseudo_set(selector),
                           state_props, line))
        layers.sort(key=lambda layer: (layer[0], layer[1]))
        node.layers = [(pseudo, props) for _w, _o, pseudo, props, _l in layers]
        # What CSS inherits comes from the parent's resting look, under everything the
        # row itself is given: a `color` on a panel is the colour of every caption in it.
        if node.parent is not None:
            inherited = dict((key, node.parent.props[key]) for key in INHERITED
                             if key in node.parent.props)
            if inherited:
                node.layers.insert(0, (frozenset(), inherited))

        # What the rest of the parser reads: the resting look and sizes, and every state
        # anything said something about -- the latter to be checked, and to know which
        # looks a row has.
        node.props = resolve(node, frozenset())
        states = {}
        for _w, _o, pseudo, props, line in layers:
            if pseudo:
                key = ":" + ":".join(sorted(pseudo, key=lambda x: (x != "checked", x)))
                merged = states.setdefault(key, ({}, line))[0]
                for name, (value, at) in props.items():
                    value = value.strip()
                    if value.endswith("!important"):
                        value = value[:-len("!important")].strip()
                    merged[name] = (value, at)
        node.states = states


_UA_RULES = []


def ua_rules():
    """The UA sheet, read once, as rules."""
    if not _UA_RULES:
        text, _ = strip_comments(UA_SHEET)
        shape = mask_strings(text)
        pos = 0
        while True:
            open_at = shape.find("{", pos)
            if open_at < 0:
                break
            close_at = match_brace(shape, open_at, len(shape))
            header = text[pos:open_at].strip()
            rule = Node("rule", "", 0)
            rule.selector = re.sub(r"\s+", " ", header)
            rule.selectors = parse_selector_list(header, 0)
            read_body(rule, text, shape, open_at, close_at, {}, rows=False)
            rule.props = normalize(rule.props, "the UA sheet")
            _UA_RULES.append(rule)
            pos = close_at + 1
    return _UA_RULES


def pseudo_set(text):
    """`:checked:hover` -> {checked, hover}."""
    return frozenset(part for part in text.split(":") if part)


def resolve(node, state):
    """Every property of @p node while it is in @p state (a set of state names), by
    the cascade: each layer that applies in that state, lowest first.

    This is why a state is not "the resting look plus the state's block": `#danger` is
    more specific than `.btn:hover`, so a hovered danger button stays red, exactly as in
    a browser. Building the hover look as base-then-hover turned it the hover colour of
    every other button."""
    out = {}
    important = {}
    for pseudo, props in node.layers:
        if pseudo <= state:
            for key, (value, line) in props.items():
                text = value.strip()
                if text.endswith("!important"):
                    important[key] = (text[:-len("!important")].strip(), line)
                else:
                    out[key] = (text, line)
    # `!important` is above every ordinary declaration, whatever selected it.
    out.update(important)
    for key, (value, line) in list(out.items()):
        if value in ("initial", "unset"):
            del out[key]
        elif value == "inherit":
            parent = node.parent
            if parent is not None and key in parent.props:
                out[key] = parent.props[key]
            else:
                del out[key]
    return out


# -- Colour and variables ------------------------------------------------------

#: The CSS named colours, all 148 of them: a stylesheet copied from the web says `white`
#: and `tomato`, and a parser that knew six of them would be one more thing to translate.
NAMED_COLOURS = {
    "aliceblue": "f0f8ff", "antiquewhite": "faebd7", "aqua": "00ffff", "aquamarine": "7fffd4",
    "azure": "f0ffff", "beige": "f5f5dc", "bisque": "ffe4c4", "black": "000000",
    "blanchedalmond": "ffebcd", "blue": "0000ff", "blueviolet": "8a2be2", "brown": "a52a2a",
    "burlywood": "deb887", "cadetblue": "5f9ea0", "chartreuse": "7fff00", "chocolate": "d2691e",
    "coral": "ff7f50", "cornflowerblue": "6495ed", "cornsilk": "fff8dc", "crimson": "dc143c",
    "cyan": "00ffff", "darkblue": "00008b", "darkcyan": "008b8b", "darkgoldenrod": "b8860b",
    "darkgray": "a9a9a9", "darkgreen": "006400", "darkgrey": "a9a9a9", "darkkhaki": "bdb76b",
    "darkmagenta": "8b008b", "darkolivegreen": "556b2f", "darkorange": "ff8c00", "darkorchid": "9932cc",
    "darkred": "8b0000", "darksalmon": "e9967a", "darkseagreen": "8fbc8f", "darkslateblue": "483d8b",
    "darkslategray": "2f4f4f", "darkslategrey": "2f4f4f", "darkturquoise": "00ced1", "darkviolet": "9400d3",
    "deeppink": "ff1493", "deepskyblue": "00bfff", "dimgray": "696969", "dimgrey": "696969",
    "dodgerblue": "1e90ff", "firebrick": "b22222", "floralwhite": "fffaf0", "forestgreen": "228b22",
    "fuchsia": "ff00ff", "gainsboro": "dcdcdc", "ghostwhite": "f8f8ff", "gold": "ffd700",
    "goldenrod": "daa520", "gray": "808080", "green": "008000", "greenyellow": "adff2f",
    "grey": "808080", "honeydew": "f0fff0", "hotpink": "ff69b4", "indianred": "cd5c5c",
    "indigo": "4b0082", "ivory": "fffff0", "khaki": "f0e68c", "lavender": "e6e6fa",
    "lavenderblush": "fff0f5", "lawngreen": "7cfc00", "lemonchiffon": "fffacd", "lightblue": "add8e6",
    "lightcoral": "f08080", "lightcyan": "e0ffff", "lightgoldenrodyellow": "fafad2", "lightgray": "d3d3d3",
    "lightgreen": "90ee90", "lightgrey": "d3d3d3", "lightpink": "ffb6c1", "lightsalmon": "ffa07a",
    "lightseagreen": "20b2aa", "lightskyblue": "87cefa", "lightslategray": "778899", "lightslategrey": "778899",
    "lightsteelblue": "b0c4de", "lightyellow": "ffffe0", "lime": "00ff00", "limegreen": "32cd32",
    "linen": "faf0e6", "magenta": "ff00ff", "maroon": "800000", "mediumaquamarine": "66cdaa",
    "mediumblue": "0000cd", "mediumorchid": "ba55d3", "mediumpurple": "9370db", "mediumseagreen": "3cb371",
    "mediumslateblue": "7b68ee", "mediumspringgreen": "00fa9a", "mediumturquoise": "48d1cc", "mediumvioletred": "c71585",
    "midnightblue": "191970", "mintcream": "f5fffa", "mistyrose": "ffe4e1", "moccasin": "ffe4b5",
    "navajowhite": "ffdead", "navy": "000080", "oldlace": "fdf5e6", "olive": "808000",
    "olivedrab": "6b8e23", "orange": "ffa500", "orangered": "ff4500", "orchid": "da70d6",
    "palegoldenrod": "eee8aa", "palegreen": "98fb98", "paleturquoise": "afeeee", "palevioletred": "db7093",
    "papayawhip": "ffefd5", "peachpuff": "ffdab9", "peru": "cd853f", "pink": "ffc0cb",
    "plum": "dda0dd", "powderblue": "b0e0e6", "purple": "800080", "rebeccapurple": "663399",
    "red": "ff0000", "rosybrown": "bc8f8f", "royalblue": "4169e1", "saddlebrown": "8b4513",
    "salmon": "fa8072", "sandybrown": "f4a460", "seagreen": "2e8b57", "seashell": "fff5ee",
    "sienna": "a0522d", "silver": "c0c0c0", "skyblue": "87ceeb", "slateblue": "6a5acd",
    "slategray": "708090", "slategrey": "708090", "snow": "fffafa", "springgreen": "00ff7f",
    "steelblue": "4682b4", "tan": "d2b48c", "teal": "008080", "thistle": "d8bfd8",
    "tomato": "ff6347", "turquoise": "40e0d0", "violet": "ee82ee", "wheat": "f5deb3",
    "white": "ffffff", "whitesmoke": "f5f5f5", "yellow": "ffff00", "yellowgreen": "9acd32",
}


COLOUR_FUNCTION = re.compile(r"^(rgba?|hsla?)\(\s*(.*?)\s*\)$")


def _channel(text, line, where, scale):
    """One `rgb()` / `hsl()` argument: a number, or a percentage of @p scale."""
    text = text.strip()
    try:
        if text.endswith("%"):
            return float(text[:-1]) * scale / 100.0
        return float(text)
    except ValueError:
        raise Failure("%s at line %d has %r where a number belongs" % (where, line, text))


def _hsl(h, sat, light):
    """HSL (degrees, 0..1, 0..1) to 0..255 RGB, the CSS Color 4 formula."""
    def f(n):
        k = (n + h / 30.0) % 12
        a = sat * min(light, 1 - light)
        return light - a * max(-1, min(k - 3, 9 - k, 1))
    return tuple(int(round(f(n) * 255)) for n in (0, 8, 4))


def read_colour(value, line, variables, where):
    """`(r, g, b, a)` from any colour CSS writes as a value: `#rgb`, `#rgba`, `#rrggbb`,
    `#rrggbbaa`, a named colour, `transparent`, `rgb()`/`rgba()` and `hsl()`/`hsla()`,
    with commas or spaces and `/ alpha`.

    `var()` is resolved before this is asked (::substitute_vars); it is read here only
    for a value that was built outside a sheet."""
    value = value.strip()
    m = re.match(r"^var\(\s*--([A-Za-z0-9_-]+)\s*\)$", value)
    if m:
        key = m.group(1)
        if key not in variables:
            raise Failure("%s at line %d uses var(--%s), which :root never defines. "
                          "Defined: %s" % (where, line, key,
                                           ", ".join(sorted(variables)) or "nothing"))
        return variables[key]
    lower = value.lower()
    if lower == "transparent":
        return (0, 0, 0, 0)
    if lower in NAMED_COLOURS:
        value = "#" + NAMED_COLOURS[lower]
    m = COLOUR_FUNCTION.match(lower)
    if m:
        kind, body = m.group(1), m.group(2)
        alpha_text = None
        if "/" in body:
            body, alpha_text = body.split("/", 1)
        parts = [part for part in re.split(r"[\s,]+", body.strip()) if part]
        if len(parts) == 4 and alpha_text is None:
            alpha_text = parts.pop()
        if len(parts) != 3:
            raise Failure("%s at line %d is %r: a colour function takes three numbers and "
                          "an optional alpha" % (where, line, value))
        alpha = 1.0 if alpha_text is None else _channel(alpha_text, line, where, 1.0)
        if kind.startswith("rgb"):
            rgb = tuple(_channel(part, line, where, 255.0) for part in parts)
        else:
            hue = _channel(parts[0].replace("deg", ""), line, where, 360.0)
            rgb = _hsl(hue, _channel(parts[1], line, where, 1.0),
                       _channel(parts[2], line, where, 1.0))
        clamp = lambda v: max(0, min(255, int(round(v))))
        return (clamp(rgb[0]), clamp(rgb[1]), clamp(rgb[2]), clamp(alpha * 255))
    if not COLOUR.match(value):
        raise Failure("%s at line %d is %r. A colour is `#rgb`, `#rrggbb` (with or without "
                      "an alpha), a CSS colour name, `transparent`, `rgb()`, `rgba()`, "
                      "`hsl()` or `hsla()`, or var(--name) from :root. A gradient is not a "
                      "colour: a style's paint is solid" % (where, line, value))
    digits = value[1:]
    if len(digits) in (3, 4):
        digits = "".join(c * 2 for c in digits)
    red, green, blue = (int(digits[0:2], 16), int(digits[2:4], 16), int(digits[4:6], 16))
    alpha = int(digits[6:8], 16) if len(digits) == 8 else 255
    return (red, green, blue, alpha)


def split_commas(text):
    """@p text split on the commas that are not inside `( ... )`."""
    parts = []
    depth = 0
    cur = []
    for ch in text:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append("".join(cur).strip())
            cur = []
            continue
        cur.append(ch)
    parts.append("".join(cur).strip())
    return parts


ANGLE = re.compile(r"^(-?\d+(?:\.\d+)?)(deg|turn|rad|grad)$")
SIDES = {"to top": 0, "to right": 90, "to bottom": 180, "to left": 270}
CORNER_WORDS = {"to top right": "tr", "to right top": "tr", "to bottom right": "br",
                "to right bottom": "br", "to bottom left": "bl", "to left bottom": "bl",
                "to top left": "tl", "to left top": "tl"}


def gradient_angle(text, rect):
    """Degrees of a gradient direction, or None when @p text is not one. A corner
    (`to top right`) is the angle whose line meets that corner of a box of @p rect's
    size, as CSS draws it: perpendicular to the diagonal between the other two."""
    import math
    text = re.sub(r"\s+", " ", text.strip())
    m = ANGLE.match(text)
    if m:
        value = float(m.group(1))
        unit = m.group(2)
        degrees = {"deg": value, "turn": value * 360, "rad": math.degrees(value),
                   "grad": value * 0.9}[unit]
        return int(round(degrees)) % 360
    if text in SIDES:
        return SIDES[text]
    if text in CORNER_WORDS:
        w, h = (rect[2], rect[3]) if rect is not None and rect[2] > 0 and rect[3] > 0 else (1, 1)
        across = math.degrees(math.atan2(h, w))
        return int(round({"tr": across, "br": 180 - across, "bl": 180 + across,
                          "tl": 360 - across}[CORNER_WORDS[text]])) % 360
    return None


def read_stops(parts, line, variables, where):
    """[(rgb, frac 0..255)] for the colour stops of a gradient, with the positions CSS
    leaves out filled in as CSS fills them: the first at 0, the last at 100%, and the
    ones between spread evenly between the stops either side that say."""
    stops = []
    for part in parts:
        tokens = re.findall(r"[a-zA-Z-]+\([^)]*\)|\S+", part)
        positions = []
        while tokens and re.match(r"^-?\d+(?:\.\d+)?%$|^0$", tokens[-1]):
            positions.insert(0, tokens.pop())
        if not tokens:
            raise Failure("%s at line %d has a stop with no colour: %r" % (where, line, part))
        if tokens and re.match(r"^-?\d+(?:\.\d+)?(px|rem|em)$", tokens[-1]):
            raise Failure("%s at line %d places a stop at %s. A stop here is a share of the "
                          "gradient, in `%%`: the length of the line is the box's"
                          % (where, line, tokens[-1]))
        rgb = read_colour(" ".join(tokens), line, variables, where)
        if len(positions) > 2:
            raise Failure("%s at line %d: a stop has one or two positions" % (where, line))
        if not positions:
            stops.append([rgb, None])
        for pos in positions:
            stops.append([rgb, 0.0 if pos == "0" else float(pos[:-1])])
    if len(stops) < 2:
        raise Failure("%s at line %d: a gradient has at least two colours" % (where, line))
    if stops[0][1] is None:
        stops[0][1] = 0.0
    if stops[-1][1] is None:
        stops[-1][1] = 100.0
    i = 0
    while i < len(stops):
        if stops[i][1] is not None:
            i += 1
            continue
        j = i
        while stops[j][1] is None:
            j += 1
        start, end = stops[i - 1][1], stops[j][1]
        for k in range(i, j):
            stops[k][1] = start + (end - start) * (k - i + 1) / (j - i + 1)
        i = j
    out = []
    last = 0.0
    for rgb, pos in stops:
        # A stop before the one ahead of it is moved up to it, as CSS does.
        pos = max(last, min(100.0, max(0.0, pos)))
        last = pos
        out.append((rgb, int(round(pos * 255 / 100))))
    return out


def read_paint(value, line, variables, where, rect=None):
    """`("solid", rgb)` or `("linear" | "radial", angle, [(rgb, frac)])` for a
    `background`."""
    text = value.strip()
    m = re.match(r"^(repeating-)?(linear|radial|conic)-gradient\((.*)\)$", text, re.S)
    if not m:
        return ("solid", read_colour(text, line, variables, where))
    if m.group(1):
        raise Failure("%s at line %d is a repeating gradient, and a gradient here runs once "
                      "across the box" % (where, line))
    kind = m.group(2)
    if kind == "conic":
        raise Failure("%s at line %d is a conic gradient: a gradient here runs along a "
                      "line or out from the middle" % (where, line))
    parts = split_commas(m.group(3))
    if kind == "linear":
        angle = gradient_angle(parts[0], rect)
        if angle is not None:
            parts = parts[1:]
        elif parts[0].strip().startswith("to "):
            raise Failure("%s at line %d: `%s` is not a direction" % (where, line, parts[0]))
        else:
            angle = 180
        return ("linear", angle, read_stops(parts, line, variables, where))
    first = re.sub(r"\s+", " ", parts[0].strip())
    if re.match(r"^(circle|ellipse|closest|farthest|at )|^(ellipse|circle)? ?(closest|farthest)", first) or \
            first.startswith("at "):
        if first not in ("ellipse", "farthest-corner", "ellipse farthest-corner", "at center",
                         "ellipse at center", "farthest-corner at center",
                         "ellipse farthest-corner at center"):
            raise Failure("%s at line %d: `%s`. A radial gradient here is the CSS default -- "
                          "an ellipse the shape of the box, from the middle to the farthest "
                          "corner -- and says nothing else" % (where, line, parts[0]))
        parts = parts[1:]
    return ("radial", 0, read_stops(parts, line, variables, where))


def read_number(value, line, where):
    """A plain whole number, for what is a count and not a length (`flex`)."""
    value = value.strip()
    if not NUMBER.match(value):
        raise Failure("%s at line %d is %r. Only a whole number here"
                      % (where, line, value))
    return int(value)


LENGTH = re.compile(r"^(-?(?:\d+\.?\d*|\.\d+))(px|rem|em|%)?$")

#: What `1rem` is: the browser default, because a font size is the app's and there is no
#: other number a `rem` could be a multiple of. `em` is taken as the same, for the same
#: reason -- there is no `font-size` here for an `em` to be relative to.
REM = 16


def read_length(value, line, where, base=None):
    """A length in whole pixels: `12`, `12px`, `1.5rem`, `2em`, or a `%` of @p base.

    @p base is what a percentage is a share of; where nothing is (a gap, a radius) a
    percentage is refused rather than guessed. Fractions are rounded: the table is in
    whole pixels.
    """
    text = value.strip()
    m = LENGTH.match(text)
    if not m:
        raise Failure("%s at line %d is %r. A length is a number with `px`, `rem`, `em` "
                      "or `%%`, and nothing else: there is no `vh`, `vw` or `calc()` here"
                      % (where, line, text))
    number = float(m.group(1))
    unit = m.group(2) or "px"
    if unit in ("rem", "em"):
        number *= REM
    elif unit == "%":
        if base is None:
            raise Failure("%s at line %d is %r, and there is nothing here for a %% to be "
                          "a share of" % (where, line, text))
        number = number * base / 100.0
    return int(round(number))


def is_auto(node, key):
    return key in node.props and node.props[key][0].strip() == "auto"


#: `shadow: <spread> [<offset down>]`. One number or two, never three: the engine has a
#: colour, a fade distance and a shift on each axis, and a stylesheet that could spell
#: all four would have a way to write a shadow with no spread and one with a spread and
#: no fade, which are the same thing drawn twice.
SHADOW = re.compile(r"^(\d+)(?:px)?(?:\s+(\d+)(?:px)?)?$")


def read_shadow(value, line, where):
    """(spread, offset down) from `shadow: 18` or `shadow: 18 4`, or a refusal."""
    found = SHADOW.match(value.strip())
    if not found:
        raise Failure("%s at line %d is %r. A shadow is a fade distance and how far "
                      "down it is cast: `shadow: 18` or `shadow: 18 4`. Its colour is "
                      "`--shadow` in `:root`, and not a fourth number here"
                      % (where, line, value.strip()))
    return int(found.group(1)), int(found.group(2) or 0)


#: A time, as CSS writes one: `150ms`, `0.2s`. A bare number is milliseconds, the way a
#: bare number is pixels everywhere else this parser reads one.
TIME = re.compile(r"^(\d+(?:\.\d+)?|\.\d+)(ms|s)?$")

#: The longest a transition may be: `lh_ui_style_set_transition` takes an `lh_u16_t`, and
#: a time that does not fit in one is not rounded into it.
MAX_TRANSITION = 65535

#: The curves CSS names, and the engine's names for them (::lh_ui_easing_t).
EASINGS = {"ease": "lh_ui_easing_ease", "linear": "lh_ui_easing_linear",
           "ease-in": "lh_ui_easing_ease_in", "ease-out": "lh_ui_easing_ease_out",
           "ease-in-out": "lh_ui_easing_ease_in_out"}

#: What a transition can be asked to move. The engine mixes everything a look has an
#: amount of -- fills, the text and border paints, corners, the shadow -- and these are
#: the CSS names of those; `all` is all of them.
TRANSITION_PROPERTIES = ("all", "background", "background-color", "color", "border-color",
                         "border-radius", "border-top-left-radius", "border-top-right-radius",
                         "border-bottom-right-radius", "border-bottom-left-radius", "box-shadow",
                         "opacity", "none")


def read_time(text, line, where):
    """A time in whole milliseconds, or a refusal that says what is wrong."""
    text = text.strip()
    if text.startswith("-"):
        raise Failure("%s at line %d is %s, and a look that changes in less than no time "
                      "changes at once. Write `0`, or the time it should take"
                      % (where, line, text))
    found = TIME.match(text)
    if not found:
        raise Failure("%s at line %d: `%s` is not a time. A time is `150ms`, or `0.2s`, or "
                      "a bare number of milliseconds" % (where, line, text))
    number = float(found.group(1))
    ms = int(round(number * 1000)) if found.group(2) == "s" else int(round(number))
    if ms > MAX_TRANSITION:
        raise Failure("%s at line %d is %d ms, and a style keeps this time in an "
                      "`lh_u16_t`: at most %d" % (where, line, ms, MAX_TRANSITION))
    return ms


def _expand_transition(text, line, where):
    """`transition: <property> <time> <curve> [<delay>], ...` into its three parts.

    One time and one curve a look: the engine moves a whole look at once, so every
    item's time is the longest of them and the items must agree on a curve -- a look
    whose fill eased in while its text eased out is two transitions, and a style here
    is one. A delay other than 0 is refused: a look changes the moment its state does.
    The properties are kept for the check that something they name is changed by a
    state; the engine itself mixes every amount the look has.
    """
    if text == "none":
        return [("transition-duration", "0"), ("transition-property", "none")]
    longest = 0
    curves = set()
    names = []
    for item in split_commas(text):
        times = []
        for token in item.split():
            if token in EASINGS:
                curves.add(token)
            elif re.match(r"^(cubic-bezier|steps)\(", token) or token in ("step-start", "step-end"):
                raise Failure("%s at line %d: `%s`. A curve here is one CSS names -- `ease`, "
                              "`linear`, `ease-in`, `ease-out`, `ease-in-out` -- and not a "
                              "Bezier or steps of its own" % (where, line, token))
            elif TIME.match(token) or token.startswith("-"):
                times.append(token)
            elif token in TRANSITION_PROPERTIES:
                names.append(token)
            else:
                raise Failure("%s at line %d: `%s` is not a property a look moves. Here: %s"
                              % (where, line, token, ", ".join(TRANSITION_PROPERTIES)))
        if len(times) > 2:
            raise Failure("%s at line %d: `%s` has more times than a duration and a delay"
                          % (where, line, item))
        if len(times) == 2 and read_time(times[1], line, "the delay in " + where) != 0:
            raise Failure("%s at line %d has a delay of %s. A look changes the moment its "
                          "state does; there is no clock to wait on" % (where, line, times[1]))
        if times:
            longest = max(longest, read_time(times[0], line, where))
    if len(curves) > 1:
        raise Failure("%s at line %d asks for %s. A look moves on one curve: say one"
                      % (where, line, " and ".join(sorted(curves))))
    out = [("transition-duration", "%d" % longest)]
    if curves:
        out.append(("transition-timing-function", curves.pop()))
    out.append(("transition-property", ",".join(names) or "all"))
    return out


def read_text(value, line, where):
    """The caption a `text:` spells, as the string it means.

    Quoted or bare; inside quotes `\\"` is a quote, `\\\\` a backslash, and `\\` with one
    to six hex digits is that code point, as in CSS -- so `\\A` is a line break. Any
    other backslash is refused: a `\\n` that meant a new line is told so rather than
    drawn as an `n`.
    """
    value = value.strip()
    if not (len(value) >= 2 and value.startswith('"') and value.endswith('"')):
        return value
    out = []
    i = 1
    while i < len(value) - 1:
        ch = value[i]
        if ch == "\\":
            nxt = value[i + 1] if i + 1 < len(value) - 1 else ""
            hexa = re.match(r"[0-9A-Fa-f]{1,6}", value[i + 1:len(value) - 1])
            if hexa:
                # A code point, as CSS writes one: `\A` is a line break. One space
                # after the digits ends them and is not part of the text.
                out.append(chr(int(hexa.group(0), 16)))
                i += 1 + len(hexa.group(0))
                if i < len(value) - 1 and value[i] == " ":
                    i += 1
                continue
            if nxt not in ('"', "\\"):
                raise Failure("%s at line %d has `\\%s`. Inside a caption `\\\"` is a "
                              "quote and `\\\\` a backslash, and nothing else is an escape"
                              % (where, line, nxt))
            out.append(nxt)
            i += 2
            continue
        out.append(ch)
        i += 1
    return "".join(out)


def c_string(text):
    """@p text as a C string literal, UTF-8, every byte that is not plain ASCII octal.

    Octal and not hex, and always three digits: `\\x` takes every hex digit after it, so
    `\\xC3` followed by an `A` is one byte too wide, while `\\303A` is two characters.
    And escaped rather than written as is, because a compiler that does not read its
    source as UTF-8 (MSVC without `/utf-8`) would read a raw `é` as two other bytes.
    """
    out = ['"']
    for byte in text.encode("utf-8"):
        ch = chr(byte)
        if ch in ('"', "\\"):
            out.append("\\" + ch)
        elif 0x20 <= byte < 0x7F and ch != "?":
            # `?` is escaped too, because `??/` is a trigraph and a caption that says
            # "Really??/" would otherwise close a line of C nobody wrote.
            out.append(ch)
        else:
            out.append("\\%03o" % byte)
    out.append('"')
    return "".join(out)


# -- Reading a stylesheet ------------------------------------------------------

def read_sheet(text):
    """A stylesheet is `:root { --x: ...; }` and then nested rules. Returns
    (variables, root node).

    Nesting is the tree: a block inside a block is a child of it. That is the only
    place the two languages differ, and it is the difference that makes a tree
    readable as a stylesheet instead of as a list of rows.

    One recursive walk does the reading, because the three things a block has -- its
    own declarations, its children, and where its closing brace is -- have to be found
    in the same pass or they drift apart. The first version found the closing brace
    with `find("}")` and stopped at the end of the first child, then read the whole
    body as declarations and complained that a `}` "is not a property".
    """
    text, _ = strip_comments(text)
    shape = mask_strings(text)

    variables = {}
    pos = 0
    root = None
    rules = []

    while True:
        open_at = shape.find("{", pos)
        if open_at < 0:
            break
        close_at = match_brace(shape, open_at, len(shape))
        header = text[pos:open_at].strip()
        line = text.count("\n", 0, open_at) + 1
        if not header:
            raise Failure("a block with no selector at line %d" % line)

        if header == ":root":
            variables.update(read_variables(text[open_at + 1:close_at], line_at(text, open_at + 1)))
            pos = close_at + 1
            continue

        if is_tree_root(header, line):
            if root is not None:
                raise Failure("a second top-level rule at line %d (`%s`). One stylesheet "
                              "is one screen: the tree has exactly one root, and a second "
                              "one is a screen nobody asked for. To style a row that is "
                              "already in the tree, select it without a tag: `#%s { ... }`"
                              % (line, header, parse_compound(header, line)["id"]))
            root = read_block(text, shape, open_at, close_at, line, variables,
                              header_cut=pos - 1)
        else:
            rule = Node("rule", "", line)
            rule.selector = re.sub(r"\s+", " ", header)
            rule.selectors = parse_selector_list(header, line)
            read_body(rule, text, shape, open_at, close_at, variables, rows=False)
            for steps in rule.selectors:
                if steps[-1][1]["pseudo"] and rule.states:
                    raise Failure("the rule `%s` at line %d is a state and has states "
                                  "inside it. Write each state once, as its own rule"
                                  % (rule.selector, line))
            rules.append(rule)
        pos = close_at + 1

    if root is None:
        raise Failure("the stylesheet has no rule in it")

    # Whatever is left after the last `}` is a `}` that closes nothing, or a stray word.
    # The first version of this walked off the end of the sheet and called it a good one:
    # a half-finished rule at the bottom of the file -- a `}` typed on its own, a name
    # with no block -- produced exactly the same screen as a sheet that ended cleanly.
    tail = shape[pos:].strip()
    if tail:
        at = text.count("\n", 0, pos + (len(text[pos:]) - len(text[pos:].lstrip()))) + 1
        raise Failure("a `%s` at line %d closes nothing: the sheet ends after the last "
                      "block and this text is left over. `match_brace` counts what it "
                      "opened, so a brace with no `{` in front of it is a typo, not a "
                      "screen"
                      % (tail[0], at))
    raw = variables
    substitute_all(root, raw)
    for rule in rules:
        substitute_all(rule, raw)
    link_parents(root)
    apply_rules(root, rules)
    root.variables = colour_variables(raw)
    compose(root)
    return root.variables, root


#: What a row says about its place in its parent, as opposed to how it looks or what it
#: holds: the part of a row a wrapper takes over when the row is put inside one.
PLACE_KEYS = ("flex", "flex-basis", "align", "margin", "margin-top", "margin-right",
              "margin-bottom", "margin-left", "position", "left", "top", "right", "bottom",
              "min-width", "max-width", "min-height", "max-height", "aspect-ratio")

#: What makes a flow, and moves with it.
FLOW_KEYS = ("display", "flex-direction", "justify-content", "align-items", "row-gap",
             "column-gap", "flex-wrap")

#: How wide a scroll bar is, by `scrollbar-width`.
SCROLLBAR_WIDTH = {"auto": 12, "thin": 8}


def new_row(tag, name, line, props, html=None):
    """A row the sheet did not write, made out of one it did (::compose)."""
    node = Node(tag, name, line)
    node.html = html or tag
    node.props = dict(props)
    node.layers = [(frozenset(), node.props)]
    node.states = {}
    return node


def take(node, keys):
    """The properties of @p node named in @p keys, taken out of it."""
    out = {}
    for key in keys:
        if key in node.props:
            out[key] = node.props.pop(key)
    return out


def named(node):
    """@p node's `#id`, giving it one if it has none: a row that is composed into others
    is referred to by name, by them and by the application."""
    if not node.name:
        node.name = node.cname()
    return node.name


def put_in_place_of(old, new):
    parent = old.parent
    if parent is not None:
        parent.children[parent.children.index(old)] = new
    new.parent = parent
    old.parent = new


def compose(node):
    """Build the widgets lh does not have out of the ones it does, after the cascade.

    Each is a row the sheet wrote, taken apart into rows the engine draws, the way a
    browser draws a checkbox with a caption as two boxes:

    - a `button` with `text` holds a caption that fills it;
    - a `checkbox`, `radio` or `switch` with `text` is a row of the control and its
      caption;
    - a `progress` is a track holding a bar `value / max` of it long;
    - a box that is `overflow: auto` or `scroll` is put in a frame with a scroll bar
      beside it;
    - a flow with rows placed by hand is a frame holding the flow and, over it, those
      rows -- because a flow places every row it holds, and a row placed by hand has to
      be somewhere a flow is not;
    - an `auto` margin along a flow is a spacer that takes a share of what is left;
    - `z-index` is the order rows placed by hand are drawn in.

    The row keeps its `#id`, so the application asks for the toggle, the bar or the box
    it wrote; the rows made around it are named after it, `#remember__text`.
    """
    if node.props.get("display", ("", 0))[0].strip() == "grid":
        grid_rows(node)
    for child in list(node.children):
        compose_one(child)
    if node.tag in HOLDERS:
        auto_margins(node)
        lift_placed_rows(node)
        stack(node)


def compose_one(node):
    if node.tag == "button" and "text" in node.props:
        caption_button(node)
    elif node.tag == "toggle" and "text" in node.props:
        caption_toggle(node)
    if node.html == "progress":
        progress_bar(node)
    if node.tag in HOLDERS and any(
            node.props.get(key, ("", 0))[0].strip() in ("auto", "scroll")
            for key in ("overflow-x", "overflow-y")):
        scroll_frame(node)
    compose(node)


def caption_props(node):
    """What a caption made for @p node starts with: its text, and everything about
    text the row itself would have handed down."""
    props = dict((key, node.props[key]) for key in INHERITED if key in node.props)
    props["text"] = node.props.pop("text")
    return props


def caption_button(node):
    """`button { text: "OK" }`: a caption that fills the button, centred in it."""
    props = caption_props(node)
    props.setdefault("text-align", ("center", node.line))
    props.setdefault("valign", ("center", node.line))
    if "display" in node.props and node.props["display"][0].strip() in ("flex", "block"):
        props["flex"] = ("1", node.line)
    else:
        # A button that places nothing holds its caption by hand, over all of it.
        for key in ("left", "top", "right", "bottom"):
            props[key] = ("0", node.line)
        props["position"] = ("absolute", node.line)
    caption = new_row("label", named(node) + "__text", node.line, props, "label")
    node.children.insert(0, caption)
    caption.parent = node


def caption_toggle(node):
    """`checkbox { text: "Remember me" }`: a row of the control and its caption."""
    props = caption_props(node)
    caption = new_row("label", named(node) + "__text", node.line, props, "label")
    outer = take(node, PLACE_KEYS)
    if node.props.get("display", ("", 0))[0].strip() == "none":
        outer["display"] = node.props.pop("display")
    else:
        outer["display"] = ("flex", node.line)
    outer.setdefault("align-items", ("center", node.line))
    outer.setdefault("column-gap", ("6px", node.line))
    row = new_row("box", node.name + "__row", node.line, outer)
    put_in_place_of(node, row)
    row.children = [node, caption]
    caption.parent = row


def progress_bar(node):
    """`progress { value: 40; max: 100 }`: the track, and a bar 40% of it long."""
    if node.children:
        raise Failure("%s at line %d holds rows, and a progress bar holds its bar"
                      % (describe(node), node.line))
    value, line = node.props.pop("value", ("0", node.line))
    most, most_line = node.props.pop("max", ("100", node.line))
    try:
        share = float(value) / float(most)
    except (ValueError, ZeroDivisionError):
        raise Failure("%s at line %d: `value: %s` of `max: %s` is not a share"
                      % (describe(node), line, value, most))
    share = max(0.0, min(1.0, share))
    props = {"width": ("%g%%" % (share * 100), line), "height": ("100%", line),
             "background": node.props.get("accent-color", ("#2d6cdf", line))}
    for key in ("radius-tl", "radius-tr", "radius-br", "radius-bl"):
        if key in node.props:
            props[key] = node.props[key]
    bar = new_row("entity", named(node) + "__bar", line, props)
    node.children = [bar]
    bar.parent = node


def scroll_frame(node):
    """`overflow: auto` on a box: a frame the size the box was, the box in it less a
    scroll bar, and the bar beside it driving the box."""
    modes = (node.props.pop("overflow-x", ("hidden", 0))[0].strip(),
             node.props.pop("overflow-y", ("hidden", 0))[0].strip())
    across, down = (mode in ("auto", "scroll") for mode in modes)
    thickness = node.props.pop("scrollbar-width", ("auto", 0))[0].strip()
    if thickness == "none" or not (across or down):
        return
    bar = SCROLLBAR_WIDTH[thickness]
    outer = take(node, PLACE_KEYS + ("width", "height"))
    if node.props.get("display", ("", 0))[0].strip() == "none":
        outer["display"] = node.props.pop("display")
    frame = new_row("box", named(node) + "__frame", node.line, outer)
    put_in_place_of(node, frame)
    node.props.update({
        "position": ("absolute", node.line), "left": ("0", node.line),
        "top": ("0", node.line),
        "right": ("%dpx" % (bar if down else 0), node.line),
        "bottom": ("%dpx" % (bar if across else 0), node.line),
    })
    frame.children = [node]
    for axis, on in (("vertical", down), ("horizontal", across)):
        if not on:
            continue
        props = {"position": ("absolute", node.line), "axis": (axis, node.line),
                 "scrolls": ("#" + node.name, node.line),
                 # `scroll` shows the bar whether or not there is anything to scroll.
                 "scrollbar-mode": ("always" if modes[0 if axis == "horizontal" else 1] == "scroll"
                                    else "auto", node.line)}
        if axis == "vertical":
            props.update({"top": ("0", node.line), "right": ("0", node.line),
                          "bottom": ("%dpx" % (bar if across else 0), node.line),
                          "width": ("%dpx" % bar, node.line)})
        else:
            props.update({"left": ("0", node.line), "bottom": ("0", node.line),
                          "right": ("%dpx" % (bar if down else 0), node.line),
                          "height": ("%dpx" % bar, node.line)})
        scrollbar = new_row("scrollbar", node.name + ("__vbar" if axis == "vertical" else "__hbar"),
                            node.line, props)
        frame.children.append(scrollbar)
        scrollbar.parent = frame


def grid_tracks(node, key, line):
    """The tracks a `grid-template-*` names: `[("length", text) | ("fr", weight)]`."""
    value = node.props[key][0].strip()
    text = value
    # repeat(N, tracks) -> the tracks N times.
    while True:
        m = re.search(r"repeat\(\s*(\d+)\s*,\s*([^()]*)\)", text)
        if not m:
            break
        text = text[:m.start()] + " ".join([m.group(2).strip()] * int(m.group(1))) + text[m.end():]
    if re.search(r"repeat\(|minmax\(|fit-content\(|\[", text):
        raise Failure("`%s: %s` at line %d in %s. A track here is a length or a share (`fr`), "
                      "and `repeat()` of those; `minmax()`, `auto-fill` and named lines are "
                      "sizes a grid here does not work out" % (key, value, line, describe(node)))
    tracks = []
    for token in text.split():
        m = re.match(r"^(\d+(?:\.\d+)?)fr$", token)
        if m:
            weight = float(m.group(1))
            if weight != int(weight) or weight <= 0:
                raise Failure("`%s` at line %d in %s: a share is a whole number of `fr`"
                              % (token, line, describe(node)))
            tracks.append(("fr", int(weight)))
        elif token in ("auto", "min-content", "max-content"):
            if key == "grid-template-columns":
                raise Failure("`%s` in `%s` at line %d in %s: a column as wide as the widest "
                              "thing in it is a size a grid here cannot work out; give it a "
                              "length or a share (`1fr`)" % (token, key, line, describe(node)))
            # A row that says `auto` shares the room down with the others, as CSS
            # stretches auto rows to a grid that is taller than they are.
            tracks.append(("fr", 1))
        else:
            read_length(token, line, "`%s` of %s" % (key, describe(node)), 100)
            tracks.append(("length", token))
    if not tracks:
        raise Failure("`%s` at line %d in %s names no tracks" % (key, line, describe(node)))
    return tracks


def grid_span(item):
    """How many columns @p item spans: `grid-column: span 2`, else 1."""
    if "grid-column" not in item.props:
        return 1
    value, line = item.props.pop("grid-column")
    text = re.sub(r"\s+", " ", value.strip())
    m = re.match(r"^(?:auto ?/ ?)?span (\d+)$", text)
    if m and int(m.group(1)) >= 1:
        return int(m.group(1))
    if text == "auto":
        return 1
    raise Failure("`grid-column: %s` at line %d in %s. A grid here is placed in order, a "
                  "cell after a cell; a cell may say `span N` and nothing else"
                  % (value, line, describe(item)))


def grid_rows(node):
    """`display: grid` as the engine draws it: a column of rows, each a row of cells.

    A column whose track is a length is that wide in every row, and one that is a
    share (`fr`) has that weight in every row -- so the columns line up, row under row,
    as long as every row has every column. The last row is filled up with empty cells
    for that, and a cell that spans columns of shares takes their weights together,
    which lines up only when there is no gap between columns to lose; one that spans
    lengths takes their lengths and the gaps between them. A row's track is its
    height, or its share of the height.
    """
    line = node.props["display"][1]
    if "grid-template-columns" not in node.props:
        raise Failure("%s at line %d is a grid with no `grid-template-columns`. Name the "
                      "columns -- `repeat(3, 1fr)` is three equal ones" % (describe(node), line))
    columns = grid_tracks(node, "grid-template-columns", node.props["grid-template-columns"][1])
    rows = grid_tracks(node, "grid-template-rows", node.props["grid-template-rows"][1]) \
        if "grid-template-rows" in node.props else []
    auto_row = grid_tracks(node, "grid-auto-rows", node.props["grid-auto-rows"][1])[0] \
        if "grid-auto-rows" in node.props else ("fr", 1)
    column_gap = node.props.get("column-gap", ("0", line))[0]
    row_gap = node.props.get("row-gap", ("0", line))
    justify = node.props.pop("justify-items", ("stretch", line))[0].strip()
    across = node.props.pop("align-items", None)
    for key in ("grid-template-columns", "grid-template-rows", "grid-auto-rows"):
        node.props.pop(key, None)

    items = [child for child in node.children if in_flow(child)]
    others = [child for child in node.children if not in_flow(child)]
    placed = []
    row = []
    used = 0
    for item in items:
        span = grid_span(item)
        if span > len(columns):
            raise Failure("%s spans %d columns of a grid that has %d" % (describe(item), span, len(columns)))
        if used + span > len(columns):
            placed.append(row)
            row, used = [], 0
        row.append((item, used, span))
        used += span
    if row:
        placed.append(row)

    def track_props(start, span, at_line):
        tracks = columns[start:start + span]
        if span == len(columns):
            # Every column: the whole row, whatever the columns are -- a heading
            # across a grid is the span a sheet writes most.
            return {"flex": ("1", at_line)}
        if all(kind == "length" for kind, _v in tracks):
            if span == 1:
                return {"width": (tracks[0][1], at_line)}
            total = sum(read_length(v, at_line, "a column of %s" % describe(node), 100) for _k, v in tracks)
            gap = read_length(column_gap, at_line, "`column-gap` of %s" % describe(node))
            return {"width": ("%dpx" % (total + gap * (span - 1)), at_line)}
        if all(kind == "fr" for kind, _v in tracks):
            if span > 1 and read_length(column_gap, at_line, "`column-gap`") != 0:
                raise Failure("a cell at line %d in %s spans %d shared columns with a "
                              "`column-gap` between them. Its share would be the columns' and "
                              "not the gap's, and the cells under it would not line up: drop "
                              "the gap or give the columns lengths" % (at_line, describe(node), span))
            return {"flex": ("%d" % sum(v for _k, v in tracks), at_line)}
        raise Failure("a cell at line %d in %s spans a column of length and one of a share; a "
                      "span here is all lengths or all shares" % (at_line, describe(node)))

    name = named(node)
    row_nodes = []
    for index, cells in enumerate(placed):
        kind, value = rows[index] if index < len(rows) else auto_row
        props = {"display": ("flex", line), "column-gap": (column_gap, line)}
        if kind == "length":
            props["height"] = (value, line)
        else:
            props["flex"] = ("%d" % value, line)
        if across is not None:
            props["align-items"] = across
        row_box = new_row("box", "%s__row%d" % (name, index), line, props)
        row_box.parent = node
        filled = 0
        for item, start, span in cells:
            cell = track_props(start, span, item.line)
            self_justify = item.props.pop("justify-self", (justify, item.line))[0].strip()
            if self_justify in ("start", "end", "center") or "width" in item.props:
                # A cell narrower than its column is put in a box the column's width,
                # at the start, the end or the middle of it, as `justify-self` says.
                holder_props = dict(cell)
                holder_props.update({"display": ("flex", item.line),
                                     "justify-content": ({"start": "start", "end": "end",
                                                          "center": "center"}.get(self_justify, "start"),
                                                         item.line)})
                holder = new_row("box", "%s__cell" % named(item), item.line, holder_props)
                holder.children = [item]
                item.parent = holder
                holder.parent = row_box
                row_box.children.append(holder)
            else:
                for key in ("width", "flex", "flex-basis"):
                    item.props.pop(key, None)
                item.props.update(cell)
                item.parent = row_box
                row_box.children.append(item)
            filled = start + span
        # The empty cells of the last row, so its columns are the others' columns.
        for column in range(filled, len(columns)):
            spacer = new_row("entity", "%s__fill%d_%d" % (name, index, column), line,
                             track_props(column, 1, line), "spacer")
            spacer.parent = row_box
            row_box.children.append(spacer)
        row_nodes.append(row_box)
    node.props["display"] = ("flex", line)
    node.props["flex-direction"] = ("column", line)
    node.props["row-gap"] = row_gap
    node.props.pop("column-gap", None)
    node.children = row_nodes + others


def flow_axis(node):
    """`"vertical"` or `"horizontal"` for a row that holds a flow, else None."""
    display = node.props.get("display", ("", 0))[0].strip()
    if display == "block":
        return "vertical"
    if display == "flex":
        direction = node.props.get("flex-direction", ("row", 0))[0].strip()
        return "vertical" if direction == "column" else "horizontal"
    return None


def in_flow(node):
    return (node.props.get("position", ("", 0))[0].strip() not in ("absolute", "fixed")
            and node.props.get("display", ("", 0))[0].strip() != "none")


def margin_texts(node):
    """The four sides of @p node's margin as written: `{side: (text, line)}`."""
    sides = dict((side, ("0", node.line)) for side in ("top", "right", "bottom", "left"))
    if "margin" in node.props:
        value, line = node.props["margin"]
        for side, text in zip(("top", "right", "bottom", "left"),
                              box_sides_text(value, line, "`margin` of %s" % describe(node))):
            sides[side] = (text.strip(), line)
    for side in ("top", "right", "bottom", "left"):
        if "margin-" + side in node.props:
            value, line = node.props["margin-" + side]
            sides[side] = (value.strip(), line)
    return sides


def auto_margins(node):
    """`margin-left: auto` in a row: a spacer that takes a share of what is left.

    CSS gives the free room of a flow to its `auto` margins, equally, once the rows
    that grow have had none of it to take -- which is exactly what a filling row with a
    weight of one is. So each `auto` margin along the flow becomes an empty row that
    fills, beside the row that said it. A spacer is one more row in the flow and so one
    more gap; it is given a margin of minus one gap to take that gap back, and the
    arithmetic is CSS's to the pixel. When a row of the flow grows (`flex`), the `auto`
    margins get nothing, as in CSS, and are 0.
    """
    axis = flow_axis(node)
    if axis is None:
        return
    vertical = axis == "vertical"
    near, far = ("top", "bottom") if vertical else ("left", "right")
    gap = node.props.get("row-gap" if vertical else "column-gap", ("0", node.line))[0].strip()
    flowing = [child for child in node.children if in_flow(child)]
    growing = False
    for child in flowing:
        if "flex" in child.props and NUMBER.match(child.props["flex"][0].strip()):
            growing = growing or int(child.props["flex"][0]) > 0
    rows = []
    for child in node.children:
        if child not in flowing:
            rows.append(child)
            continue
        sides = margin_texts(child)
        autos = [side for side in (near, far) if sides[side][0] == "auto"]
        for side in autos:
            child.props["margin-" + side] = ("0", sides[side][1])
        if not autos or growing:
            rows.append(child)
            continue
        for side in (near, far):
            if side == far:
                rows.append(child)
            if side in autos:
                spacer = new_row("entity", "%s__%s" % (named(child), side), sides[side][1],
                                 {"flex": ("1", sides[side][1]),
                                  "margin-" + near: (gap if gap.startswith("-") else "-" + gap,
                                                     sides[side][1])}, "spacer")
                spacer.parent = node
                rows.append(spacer)
    node.children = rows


def stack(node):
    """`z-index` on the rows placed by hand: the order they are drawn in.

    Only those: a row in a flow is placed in its order, so its order is not the sheet's
    to change, and in a flow nothing overlaps for a `z-index` to decide. A negative one
    goes under the flow, as in CSS, and equals keep the order they were written in.
    """
    def depth(child):
        if child.props.get("position", ("", 0))[0].strip() not in ("absolute", "fixed"):
            return 0
        if "z-index" not in child.props:
            return 0
        value, line = child.props["z-index"]
        value = value.strip()
        if value == "auto":
            return 0
        if not NUMBER.match(value):
            raise Failure("`z-index: %s` at line %d in %s is not a whole number"
                          % (value, line, describe(child)))
        return int(value)

    node.children = sorted(node.children, key=depth)


def lift_placed_rows(node):
    """A flow holding rows placed by hand: the flow is moved into a box of its own,
    laid inside @p node's border and padding, and the placed rows sit over it."""
    display = node.props.get("display", ("", 0))[0].strip()
    if display not in ("flex", "block"):
        return
    placed = [child for child in node.children
              if child.props.get("position", ("", 0))[0].strip() in ("absolute", "fixed")]
    if not placed:
        return
    flow = take(node, FLOW_KEYS)
    sides = inner_sides(node.props, describe(node)) or (0, 0, 0, 0)
    for key, value in zip(("left", "top", "right", "bottom"), sides):
        flow[key] = ("%dpx" % value, node.line)
    flow["position"] = ("absolute", node.line)
    inner = new_row("box", named(node) + "__flow", node.line, flow)
    inner.children = [child for child in node.children if child not in placed]
    for child in inner.children:
        child.parent = inner
    node.children = [inner] + placed
    inner.parent = node


def link_parents(node):
    for child in node.children:
        child.parent = node
        link_parents(child)


def is_tree_root(header, line):
    """Is this top-level rule the screen, rather than a rule that styles it?

    The screen is the one rule that names one widget by tag **and** `#id` and nothing
    else -- `box#root`. Everything else at the top -- `.btn`, `button`, `#ok`,
    `.bar > button`, `a, b`, `.btn:hover` -- styles rows the screen already has.
    """
    if "," in header or ">" in header or re.search(r"\s", header.strip()):
        return False
    compound = parse_compound(header.strip(), line)
    return bool(compound["tag"] and compound["tag"] != "*" and compound["id"]
                and not compound["pseudo"])


def line_at(text, offset):
    """The 1-based line @p offset of @p text is on."""
    return text.count("\n", 0, offset) + 1


def each_declaration(body, line):
    """`(declaration, its line)` for every `a: b` in @p body, which starts on @p line.

    The line is the declaration's own and not its block's: a sheet that misspells the
    fifth property of a rule has to be told about line five. Every property used to
    carry the line of the `{` it sat under, so a typo four lines down was reported at
    the selector, which is the one line that was certainly right.
    """
    for part in split_top_level(body):
        lead = len(part) - len(part.lstrip())
        decl = part.strip()
        if decl:
            yield decl, line + part.count("\n", 0, lead)
        line += part.count("\n")


def read_variables(body, line):
    out = {}
    for decl, line in each_declaration(body, line):
        if ":" not in decl:
            raise Failure("`%s` at line %d is not a property" % (decl, line))
        key, _, val = decl.partition(":")
        key = key.strip()
        if not key.startswith("--"):
            raise Failure("`:root` holds variables and %r at line %d is not one"
                          % (key, line))
        out[key[2:]] = (val.strip(), line)
    return out


VAR = re.compile(r"var\(\s*--([A-Za-z0-9_-]+)\s*(?:,\s*([^()]*(?:\([^()]*\))?[^()]*))?\)")


def substitute_vars(value, line, raw, where, depth=0):
    """@p value with every `var(--x)` / `var(--x, fallback)` replaced by what `:root`
    says, recursively -- a variable may be built from another."""
    if depth > 16:
        raise Failure("%s at line %d: the variables refer to one another in a circle"
                      % (where, line))

    def one(m):
        key, fallback = m.group(1), m.group(2)
        if key in raw:
            return substitute_vars(raw[key][0], raw[key][1], raw, "`--%s`" % key, depth + 1)
        if fallback is not None:
            return fallback.strip()
        raise Failure("%s at line %d uses var(--%s), which :root never defines. "
                      "Defined: %s" % (where, line, key, ", ".join(sorted(raw)) or "nothing"))
    return VAR.sub(one, value)


def substitute_all(node, raw):
    """Every value of @p node and of its states, with the variables in it and every
    CSS spelling turned into this parser's (::normalize)."""
    for key, (value, line) in list(node.props.items()):
        node.props[key] = (substitute_vars(value, line, raw, "`%s` of %s"
                                           % (key, describe(node))), line)
    node.props = normalize(node.props, describe(node))
    for selector, (props, line0) in list(node.states.items()):
        for key, (value, line) in list(props.items()):
            props[key] = (substitute_vars(value, line, raw, "`%s` of %s"
                                          % (key, describe(node))), line)
        node.states[selector] = (normalize(props, describe(node)), line0)
    for child in node.children:
        substitute_all(child, raw)


BORDER_STYLES = ("solid", "none", "hidden", "dashed", "dotted", "double", "groove", "ridge",
                 "inset", "outset")
FONT_WEIGHTS = {"normal": "", "bold": "bold", "bolder": "bold", "lighter": "",
                "100": "100", "200": "200", "300": "300", "400": "", "500": "500",
                "600": "600", "700": "bold", "800": "800", "900": "900"}


def _is_length(token):
    return bool(LENGTH.match(token)) or token == "0"


def expand(key, value, line, where):
    """`(key, value)` pairs this parser keeps, for one CSS declaration.

    The way a browser turns `border: 1px solid red` into a width, a style and a colour:
    once, here, so that everything downstream reads one spelling of each thing. Runs
    after `var()` is substituted, because `border: var(--line)` has to be split as what
    the variable says, not as the word `var`.
    """
    important = ""
    text = value.strip()
    if text.endswith("!important"):
        text = text[:-len("!important")].strip()
        important = " !important"
    out = _expand(key, text, line, where)
    return [(k, v + important) for k, v in out]


def _expand(key, text, line, where):
    if key == "background-color":
        return [("background", text)]
    if key == "background":
        return [("background", "transparent" if text == "none" else text)]
    if key == "background-image":
        if text == "none":
            return []
        if not re.match(r"^(linear|radial)-gradient\(", text):
            raise Failure("`background-image: %s` at line %d in %s: a gradient is a paint, "
                          "and a picture is an `image` row" % (text, line, where))
        return [("background", text)]
    if key in ("border-radius", "radius"):
        if "/" in text:
            raise Failure("`border-radius: %s` at line %d in %s: an elliptical corner -- two "
                          "radii either side of a `/` -- is not a corner a style rounds"
                          % (text, line, where))
        top_left, top_right, bottom_right, bottom_left = box_sides_text(
            text, line, "`border-radius` of %s" % where)
        return [("radius-tl", top_left), ("radius-tr", top_right),
                ("radius-br", bottom_right), ("radius-bl", bottom_left)]
    corner = {"border-top-left-radius": "radius-tl", "border-top-right-radius": "radius-tr",
              "border-bottom-right-radius": "radius-br",
              "border-bottom-left-radius": "radius-bl"}.get(key)
    if corner is not None:
        if len(text.split()) != 1:
            raise Failure("`%s: %s` at line %d in %s: an elliptical corner is not one a style "
                          "rounds" % (key, text, line, where))
        return [(corner, text)]
    if key == "vertical-align":
        table = {"top": "top", "middle": "center", "center": "center", "bottom": "bottom"}
        if text not in table:
            raise Failure("`vertical-align: %s` at line %d in %s. Legal here: %s"
                          % (text, line, where, ", ".join(sorted(table))))
        return [("valign", table[text])]
    if key == "align-self":
        return [] if text == "auto" else [("align", text)]
    if key == "flex-grow":
        return [("flex", text)]
    if key == "transition":
        return _expand_transition(text, line, where)
    if key == "transition-duration":
        return [("transition-duration", "%d" % max(read_time(t, line, where) for t in split_commas(text)))]
    if key == "transition-timing-function":
        curves = set(c.strip() for c in split_commas(text))
        if len(curves) != 1 or next(iter(curves)) not in EASINGS:
            raise Failure("`transition-timing-function: %s` at line %d in %s. One curve a "
                          "look, of: %s" % (text, line, where, ", ".join(sorted(EASINGS))))
        return [("transition-timing-function", curves.pop())]
    if key == "transition-property":
        for name in split_commas(text):
            if name.strip() not in TRANSITION_PROPERTIES:
                raise Failure("`transition-property: %s` at line %d in %s: `%s` is not a "
                              "property a look moves" % (text, line, where, name.strip()))
        return [("transition-property", ",".join(n.strip() for n in split_commas(text)))]
    if key == "transition-delay":
        if any(read_time(t, line, where) != 0 for t in split_commas(text)):
            raise Failure("`transition-delay: %s` at line %d in %s. A look changes the moment "
                          "its state does; there is no clock to wait on" % (text, line, where))
        return []
    if key == "flex-flow":
        out = []
        for word in text.split():
            if word in DIRECTIONS or word in ("row-reverse", "column-reverse"):
                out.append(("flex-direction", word))
            else:
                out.append(("flex-wrap", word))
        return out
    if key == "flex-shrink":
        # A flow here never shrinks a row: one that does not fit runs past the end and
        # is clipped. `flex-shrink: 0` is what that is, and `1` is the CSS default a
        # sheet copied from the web says everywhere; neither has anything to change.
        read_number(text, line, "`flex-shrink` of %s" % where)
        return []
    if key == "flex":
        return _expand_flex(text, line, where)
    if key == "gap":
        parts = text.split()
        if len(parts) == 1:
            return [("row-gap", parts[0]), ("column-gap", parts[0])]
        if len(parts) == 2:
            return [("row-gap", parts[0]), ("column-gap", parts[1])]
        raise Failure("`gap: %s` at line %d in %s: one length, or a row gap and a "
                      "column gap" % (text, line, where))
    if key == "inset":
        top, right, bottom, left = box_sides_text(text, line, "`inset` of %s" % where)
        return [("top", top), ("right", right), ("bottom", bottom), ("left", left)]
    if key == "border" or key.startswith("border-") and key[7:] in ("top", "right", "bottom", "left"):
        return _expand_border(key, text, line, where)
    if key == "font":
        return _expand_font(text, line, where)
    if key == "font-family":
        return [("font-family", font_family(text, line, where))]
    if key == "font-weight":
        if text not in FONT_WEIGHTS:
            raise Failure("`font-weight: %s` at line %d in %s. Legal here: %s"
                          % (text, line, where, ", ".join(sorted(FONT_WEIGHTS))))
        return [("font-weight", FONT_WEIGHTS[text] or "normal")]
    if key == "overflow":
        parts = text.split()
        if len(parts) not in (1, 2):
            raise Failure("`overflow: %s` at line %d in %s: one value, or one across and "
                          "one down" % (text, line, where))
        return [("overflow-x", parts[0]), ("overflow-y", parts[-1])]
    if key == "display":
        if text == "block":
            # A block stacks its children down and stretches them across, which is a
            # column -- but it stays `block` here rather than becoming `flex` and
            # `column`, or a `display: flex` over a block (a `div` the UA sheet made
            # one) would keep the column and lay a row out downwards.
            return [("display", "block")]
        if text == "inline-flex":
            return [("display", "flex")]
        if text == "inline-grid":
            return [("display", "grid")]
        return [("display", text)]
    return [(key, text)]


def box_sides_text(text, line, where):
    """`a [b [c [d]]]` -> the four texts, top right bottom left, unread."""
    parts = text.split()
    if not 1 <= len(parts) <= 4:
        raise Failure("%s at line %d is %r: one to four values" % (where, line, text))
    if len(parts) == 1:
        parts = parts * 4
    elif len(parts) == 2:
        parts = [parts[0], parts[1], parts[0], parts[1]]
    elif len(parts) == 3:
        parts = [parts[0], parts[1], parts[2], parts[1]]
    return parts


def _expand_flex(text, line, where):
    """`flex: none | auto | <grow> [<shrink>] [<basis>]`."""
    if text == "none":
        return [("flex", "0")]
    if text in ("auto", "initial"):
        return [("flex", "1" if text == "auto" else "0")]
    parts = text.split()
    numbers = [part for part in parts if NUMBER.match(part)]
    lengths = [part for part in parts if not NUMBER.match(part)]
    if len(parts) > 3 or len(lengths) > 1 or (lengths and not (_is_length(lengths[0])
                                                                or lengths[0] == "auto")):
        raise Failure("`flex: %s` at line %d in %s is not a flex this parser reads: "
                      "`none`, `auto`, or a grow, a shrink and a basis" % (text, line, where))
    grow = numbers[0] if numbers else "0"
    basis = lengths[0] if lengths else None
    out = [("flex", grow)]
    if basis is not None and basis not in ("auto",):
        if int(grow) != 0 and read_length(basis, line, "`flex` of %s" % where, 0) != 0:
            raise Failure("`flex: %s` at line %d in %s grows from a basis of %s. A filling "
                          "row here starts from nothing and takes a share of what is "
                          "left: write `flex: %s` or `flex: 0 0 %s`"
                          % (text, line, where, basis, grow, basis))
        if int(grow) == 0:
            out.append(("flex-basis", basis))
    return out


def _expand_border(key, text, line, where):
    """`border[-side]: <width> || <style> || <colour>`, in any order."""
    if text == "none" or text == "0":
        widths = [("border-width", "0")] if key == "border" else [("%s-width" % key, "0")]
        return widths
    width = style = None
    colour = []
    for token in re.findall(r"[a-z]+\([^)]*\)|\S+", text):
        if style is None and token in BORDER_STYLES:
            style = token
        elif width is None and _is_length(token):
            width = token
        elif width is None and token in ("thin", "medium", "thick"):
            width = {"thin": "1", "medium": "3", "thick": "5"}[token]
        else:
            colour.append(token)
    out = []
    side = "" if key == "border" else key[6:]
    out.append(("border%s-width" % side, width if width is not None else "3"))
    out.append(("border-style", style if style is not None else "none"))
    if colour:
        out.append(("border-color", " ".join(colour)))
    return out


GENERIC_FAMILIES = ("serif", "sans-serif", "monospace", "cursive", "fantasy", "system-ui",
                    "ui-sans-serif", "ui-serif", "ui-monospace", "default")


def font_family(text, line, where):
    """The one family a font is looked up by: the first in the list, as a C name, or
    `default` for a generic one."""
    first = text.split(",")[0].strip().strip("\"'").strip()
    if not first:
        raise Failure("an empty `font-family` at line %d in %s" % (line, where))
    if first.lower() in GENERIC_FAMILIES:
        return "default"
    name = re.sub(r"[^A-Za-z0-9_]+", "_", first).strip("_").lower()
    if not name or not IDENT.match(name):
        raise Failure("`font-family: %s` at line %d in %s is not a name a C symbol can "
                      "carry" % (text, line, where))
    return name


def _expand_font(text, line, where):
    """`font: [style] [weight] <size>[/<line-height>] <family>`."""
    m = re.match(r"^((?:(?:italic|normal|bold|bolder|lighter|[1-9]00)\s+)*)"
                 r"(\S+?)(?:/(\S+))?\s+(.+)$", text)
    if not m:
        raise Failure("`font: %s` at line %d in %s is not a font this parser reads: "
                      "`[italic] [bold] <size> <family>`" % (text, line, where))
    out = []
    for word in m.group(1).split():
        if word == "italic":
            out.append(("font-style", "italic"))
        elif word != "normal":
            out.append(("font-weight", FONT_WEIGHTS[word] or "normal"))
    out.append(("font-size", m.group(2)))
    if m.group(3) is not None:
        raise Failure("`font: %s` at line %d in %s has a line height, and a label here is "
                      "one line as high as its font" % (text, line, where))
    out.append(("font-family", font_family(m.group(4), line, where)))
    return out


def normalize(props, where):
    """@p props with every CSS spelling turned into this parser's (::expand)."""
    out = {}
    for key, (value, line) in props.items():
        for name, text in expand(key, value, line, where):
            out[name] = (text, line)
    return out


def colour_variables(raw):
    """The variables that are colours, as colours: the theme the header hands out."""
    out = {}
    for name, (value, line) in raw.items():
        try:
            out[name] = read_colour(substitute_vars(value, line, raw, "`--%s`" % name),
                                    line, {}, "`--%s`" % name)
        except Failure:
            continue
    return out


def read_block(text, shape, open_at, close_at, line, variables, header_cut):
    """One `{ ... }`: its own declarations and its children.

    @p header_cut is where this block's **selector** starts: just after the `}` or `;`
    that ended whatever came before it. It is passed in because the caller already
    found it, and finding it twice is how the two copies drift apart.

    @p shape is @p text with its strings masked (::mask_strings): every brace and
    semicolon is looked for there, and every value is read from @p text.
    """
    header = text[header_cut + 1:open_at].strip()
    html, name, classes = read_selector(header, line)
    node = Node(TAG_ALIASES.get(html, html), name, line)
    node.html = html
    node.classes = classes
    read_body(node, text, shape, open_at, close_at, variables, rows=True)
    return node


def read_body(node, text, shape, open_at, close_at, variables, rows):
    """The declarations, states and (when @p rows) child rows of one `{ ... }`."""
    body_start = open_at + 1
    i = body_start
    chunk_start = body_start
    while i < close_at:
        if shape[i] == "{":
            # The declarations before this block belong to this block; the block is a
            # child. The chunk ends where the child's selector begins, which is after
            # the last `}` or `;` before the `{` -- not merely at the last newline: a
            # stylesheet written one rule per line is normal, and cutting at a newline
            # handed `label#a { width: 200; } label#z` to the selector reader, which
            # then refused the whole sheet for the wrong reason.
            #
            # The search starts one character **before** the chunk, because the `}` that
            # ended the previous child sits exactly there. Searching from `chunk_start`
            # missed it, and two rules in a row with nothing between them left `cut` at
            # -1: the selector was then read from the very top of the file, and the
            # whole sheet was refused for having two tag names in one of its rules.
            search_from = chunk_start - 1 if chunk_start > 0 else 0
            cut = max(shape.rfind("}", search_from, i), shape.rfind(";", search_from, i))
            read_declarations(node, text[chunk_start:cut + 1] if cut >= chunk_start else "",
                              line_at(text, chunk_start), variables)
            inner_close = match_brace(shape, i, close_at)
            inner_line = text.count("\n", 0, i) + 1
            inner_header = text[cut + 1:i].strip()
            if inner_header == ":root":
                raise Failure("`:root` nested inside a rule at line %d" % inner_line)
            if inner_header.startswith(":"):
                read_state(node, inner_header, text, shape, i, inner_close, inner_line,
                           variables)
                i = inner_close + 1
                chunk_start = i
                continue
            if not rows:
                raise Failure("`%s` at line %d inside the rule %s. A rule styles rows that "
                              "are already on the screen and does not add any: write rows "
                              "inside the tree" % (inner_header, inner_line, describe(node)))
            child = read_block(text, shape, i, inner_close, inner_line, variables, cut)
            child.parent = node
            node.children.append(child)
            i = inner_close + 1
            chunk_start = i
            continue
        i += 1
    read_declarations(node, text[chunk_start:close_at], line_at(text, chunk_start), variables)


def read_state(node, selector, text, shape, open_at, close_at, line, variables):
    """`:hover { ... }` inside @p node: a look, not a child.

    Which states a tag has, and what a state may say, is checked with everything else
    in ::measure; here it is only read, and refused if it is not a state at all or holds
    a block of its own.
    """
    selector = re.sub(r"\s+", "", selector)
    if selector not in STATES:
        raise Failure("`%s` at line %d in %s is not a state this parser reads. Known: %s"
                      % (selector, line, describe(node), ", ".join(sorted(STATES))))
    if selector in node.states:
        raise Failure("`%s` at line %d in %s is written twice. One look per state: "
                      "say it once" % (selector, line, describe(node)))
    if "{" in shape[open_at + 1:close_at]:
        raise Failure("a block inside `%s` at line %d in %s. A state is a look and "
                      "holds no rows" % (selector, line, describe(node)))
    props = {}
    read_declarations(node, text[open_at + 1:close_at], line_at(text, open_at + 1),
                      variables, into=props)
    # Kept even when empty: `:hover {}` is a request for a hover look that is the same
    # as the resting one, and dropping it would turn that into "no hover look".
    node.states[selector] = (props, line)


def read_declarations(node, text, line, variables, into=None):
    """Every `a: b;` in @p text, which starts on @p line, into @p node (or into the
    state's own @p into), refusing anything unknown."""
    target = node.props if into is None else into
    if not text.strip():
        return
    for decl, line in each_declaration(text, line):
        if ":" not in decl:
            raise Failure("`%s` at line %d in %s is not a property"
                          % (decl, line, describe(node)))
        key, _, val = decl.partition(":")
        key = key.strip().lower()
        if key in NOT_SUPPORTED:
            raise Failure("`%s` at line %d in %s is not something this parser can "
                          "express: %s" % (key, line, describe(node), NOT_SUPPORTED[key]))
        if key not in KNOWN_PROPS:
            raise Failure("`%s` at line %d in %s is not a property this parser reads, "
                          "and a property nothing reads is one the row quietly does "
                          "not have. Known: %s%s"
                          % (key, line, describe(node), ", ".join(sorted(KNOWN_PROPS)),
                             _did_you_mean(key)))
        target[key] = (val.strip(), line)


def _did_you_mean(key):
    """The one legal property a misspelling is most likely to have meant, or ``."""
    close = [k for k in sorted(KNOWN_PROPS)
             if len(k) > 2 and (k.startswith(key[:3]) or key.startswith(k[:4]))]
    return (" -- did you mean %s?" % close[0]) if len(close) == 1 else ""


def match_brace(text, open_at, limit):
    """The `}` that closes the `{` at @p open_at."""
    depth = 0
    i = open_at
    while i < limit:
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return i
        i += 1
    raise Failure("a `{` at line %d that is never closed"
                  % (text.count("\n", 0, open_at) + 1))


# -- Measure: CSS box -> lh place ----------------------------------------------

def measure(root, variables=None):
    """Fill in every node's rect and place, and check what the stylesheet forgot.

    Two rules, both from what `lh_ui_place_t` can answer: a child asks for room **along**
    the flow (fixed, a share, or what it measures) and sits **across** it. So in a
    column a child's height is its along size and in a row its width is, and the other
    dimension is its own.

    @p variables is what `:root` defined, so that a colour can be checked here rather
    than at write time. The first version checked colours while emitting, and the
    self-test -- which reads and measures but never writes -- reported a `var(--nope)`
    as accepted. A check that runs only on the way out is a check that does not run.
    """
    ids = set()
    root.index = 0
    root.variables = variables if variables is not None else {}
    check_display(root)
    w, h = stated_size(root, "the root: it is the window, and nothing outside it says "
                             "how big that is")
    _measure(root, 0, 0, w, h, ids)
    index_rows(root)
    check_values(root)
    check_colours(root)
    check_scrollbars(root, ids)
    return ids


def all_nodes(node, out=None):
    out = [] if out is None else out
    out.append(node)
    for child in node.children:
        all_nodes(child, out)
    return out


def prop_sets(node):
    """`(props, where)` for the row's own properties and then for each of its states.

    One walk for both, so that a check written for a row is a check on its `:hover` as
    well: a `var(--nope)` under `:hover` is as wrong as one on the row.
    """
    out = [(node.props, describe(node))]
    for selector, (props, _line) in sorted(node.states.items()):
        out.append((props, "`%s` of %s" % (selector, describe(node))))
    return out


def check_values(root):
    """Every value this parser reads, read now -- not when the file is written.

    `lookup` is the one place a named value is turned into a constant, and it only
    ran from `emit`. `emit` runs when a file is written, so a stylesheet nobody had
    written yet was never checked for `align: middel`, and the refusal arrived at the
    one moment it could no longer be acted on.
    """
    for node in all_nodes(root):
        check_states(node)
        check_layout_values(node)
        check_transition(node)
        if "maxlength" in node.props:
            if node.tag != "input":
                raise Failure("`maxlength` at line %d on %s, which is not an `input`"
                              % (node.props["maxlength"][1], describe(node)))
            value, line = node.props["maxlength"]
            if not 1 <= read_number(value, line, "`maxlength` of %s" % describe(node)) <= 65534:
                raise Failure("`maxlength: %s` at line %d in %s: 1 to 65534 bytes"
                              % (value, line, describe(node)))
        if "value" in node.props:
            if node.tag != "input":
                raise Failure("`value` at line %d on %s: a value is an `input`'s or a "
                              "`progress`'s" % (node.props["value"][1], describe(node)))
            value, line = node.props["value"]
            read_text(value, line, "`value` of %s" % describe(node))
        if "accent-color" in node.props:
            value, line = node.props["accent-color"]
            read_colour(value, line, root.variables, "`accent-color` of %s" % describe(node))
        if "checked" in node.props and node.tag != "toggle":
            raise Failure("`checked` at line %d on %s, which is a `%s`. Only a `toggle` "
                          "is on or off" % (node.props["checked"][1], describe(node), node.tag))
        for props, where in prop_sets(node):
            for key, (value, line) in props.items():
                if key in VALUE_TABLES:
                    lookup(VALUE_TABLES[key], node, key, props)
                elif key == "text":
                    # Only a label row carries a caption (`lh_ui_decl_t::as.label`); on a
                    # button or a box the text was read and written nowhere.
                    if node.tag != "label":
                        raise Failure("`text` at line %d on %s, which is a `%s`. Only a "
                                      "`label` has a caption: put one inside it"
                                      % (line, describe(node), node.tag))
                    read_text(value, line, "`text` of %s" % describe(node))
                elif key in NUMERIC_PROPS:
                    if key == "shadow":
                        read_shadow(value, line, "`shadow` of %s" % where)
                    elif key == "flex":
                        read_number(value, line, "`flex` of %s" % where)
                    elif key in PADDING_PROPS:
                        padding_sides({key: (value, line)}, where)
                    elif key in SIZED and value.strip() == "auto":
                        pass
                    else:
                        # A size and a position may be a share of the parent; the
                        # parent is measured later, so here only the spelling is read.
                        read_length(value, line, "`%s` of %s" % (key, where),
                                    100 if key in SIZED else None)


def check_layout_values(node):
    """Margins and bounds, which are read where the parent's size is known; here only
    their spelling, so that a typo is told while the sheet is being read."""
    for key in MARGIN_PROPS:
        if key in node.props:
            margin_sides(node, 100)
            break
    for key in BOUND_PROPS:
        if key in node.props:
            bounds_of(node, 100, 100)
            break


#: What the engine mixes between two looks, and the whole of it: a fill and a radius.
#: `border-radius` and `radius` are turned into the four corners by ::expand before
#: they get here, so the corners are what a sheet's spelling has become.
ANIMATED = ("background", "color", "border-color", "radius-tl", "radius-tr", "radius-br",
            "radius-bl", "box-shadow", "shadow", "opacity", "visibility")


def check_transition(node):
    """A `transition` has to have something to move: a state of the row that changes a
    fill, a paint, a corner or a shadow. A transition on a row whose looks are all the
    same in those would be a clock running over a picture that never moves."""
    for props, where in prop_sets(node):
        if "transition-duration" not in props:
            continue
        value, line = props["transition-duration"]
        if not int(value):
            continue
        if any(key in other for other, _where in prop_sets(node) for key in ANIMATED
               if other is not props or key in props):
            continue
        raise Failure("`transition` at line %d in %s, and there is nothing for it to move: "
                      "no state of %s changes a fill, a colour, a corner or a shadow"
                      % (line, where, describe(node)))


def check_states(node):
    """A state belongs to a tag that has it, and says only what a state can change."""
    for selector, (props, line) in sorted(node.states.items()):
        if node.tag not in STATES[selector]:
            raise Failure("`%s` at line %d in %s, and a `%s` has no such state. `%s` is "
                          "for: %s" % (selector, line, describe(node), node.tag, selector,
                                       ", ".join(STATES[selector])))
        legal = PRESSED_PROPS if selector == ":active" else LOOK_PROPS
        for key, (_value, at) in sorted(props.items()):
            if key not in legal:
                raise Failure("`%s` at line %d in `%s` of %s. A state changes how a row "
                              "looks, not where it is or what it holds%s. Here: %s"
                              % (key, at, selector, describe(node),
                                 " -- and a pressed look is only paint"
                                 if selector == ":active" else "",
                                 ", ".join(legal)))


def check_colours(root):
    for node in all_nodes(root):
        for props, where in prop_sets(node):
            if "box-shadow" in props:
                value, line = props["box-shadow"]
                read_box_shadow(value, line, root.variables, "`box-shadow` of %s" % where)
            if "opacity" in props:
                read_opacity(props["opacity"][0], props["opacity"][1], where)
            if "aspect-ratio" in props:
                read_ratio(node) if props is node.props else None
            if any(key in props for key in ("font-family", "font-size", "font-weight",
                                            "font-style")):
                font_name(props, where)
            border_sides(props, where)
            # A row that casts a shadow is asking for `--shadow` without saying so, so
            # the check is here rather than in the emitter: the message has to arrive
            # while the stylesheet is being read, and by the time a file is written the
            # person has walked away from it.
            if "shadow" in props and "shadow" not in root.variables:
                raise Failure("`shadow` on %s needs `--shadow` in `:root`: a shadow's "
                              "colour is a theme's to change, and a theme is a set of "
                              "variables" % where)
            for key in COLOUR_PROPS:
                if key not in props:
                    continue
                value, line = props[key]
                read_colour(value, line, root.variables, "`%s` of %s" % (key, where))
            if "background" in props:
                value, line = props["background"]
                read_paint(value, line, root.variables, "`background` of %s" % where)


def check_scrollbars(root, ids):
    """A bar must name a container that exists, and one that comes before it.

    Both were checked while emitting in the first version, which is the same mistake
    the colour check was: the self-test never emits, and so never saw either.
    """
    for node in all_nodes(root):
        if node.tag != "scrollbar":
            continue
        wanted = prop_str(node, "scrolls").lstrip("#")
        if wanted not in ids:
            raise Failure("%s at line %d scrolls `#%s`, which no rule declares. "
                          "Declared ids: %s"
                          % (describe(node), node.line, wanted,
                             ", ".join(sorted(n for n in ids if n))))
        target = [n for n in all_nodes(root) if n.name == wanted][0]
        if target.index >= node.index:
            raise Failure("%s at line %d scrolls `#%s`, which is row %d while the bar "
                          "itself is row %d. `lh_ui_decl_build` wants the thing scrolled "
                          "to exist first, so put the bar after it"
                          % (describe(node), node.line, wanted, target.index, node.index))


def check_display(node):
    """`display` says whether a node places children, and it is the one property that
    is **checked** rather than read: a `label` with `display: flex` and children would
    silently place nothing, and `display: none` would quietly put the dialog on screen.
    """
    if "display" in node.props:
        value, line = node.props["display"]
        value = value.strip()
        if value not in DISPLAY_VALUES:
            raise Failure("`display: %s` at line %d on %s. Here a row is `flex` (it places "
                          "its children), `block` (a column), or `none` (built hidden)"
                          % (value, line, describe(node)))
        if value in ("flex", "block") and node.tag not in HOLDERS:
            raise Failure("`display: flex` at line %d on %s, and a `%s` is not a "
                          "container: the flow would have nowhere to belong"
                          % (line, describe(node), node.tag))
    for child in node.children:
        check_display(child)


def stated_size(node, why, base=(None, None)):
    """`(width, height)` of a row whose size nothing but the sheet can give, or a refusal
    that says @p why nothing else can."""
    w = prop(node, "width", base[0])
    h = prop(node, "height", base[1])
    if "aspect-ratio" in node.props and (w is None) != (h is None):
        ratio = read_ratio(node)
        if w is None:
            w = int(round(h * ratio))
        else:
            h = int(round(w / ratio))
    for key, value in (("width", w), ("height", h)):
        if value is None:
            raise Failure("%s at line %d has no `%s`, and it needs one: it is %s"
                          % (describe(node), node.line, key, why))
    return w, h


PADDING_PROPS = ("padding", "padding-top", "padding-right", "padding-bottom", "padding-left",
                 "padx", "pady")


def box_sides(text, line, where):
    """`a`, `a b`, `a b c` or `a b c d` -> (top, right, bottom, left), as CSS spells a
    shorthand for four sides."""
    parts = text.split()
    if not 1 <= len(parts) <= 4:
        raise Failure("%s at line %d is %r: one to four lengths" % (where, line, text))
    values = [read_length(part, line, where) for part in parts]
    if len(values) == 1:
        values = values * 4
    elif len(values) == 2:
        values = [values[0], values[1], values[0], values[1]]
    elif len(values) == 3:
        values = [values[0], values[1], values[2], values[1]]
    return tuple(values)


def padding_sides(props, where):
    """(left, top, right, bottom) of the padding @p props describe, or None for none.

    The shorthand first, then each side, then `padx` / `pady` -- the order in which a
    later, narrower declaration overrides a broader one in CSS. This is the one reading
    of padding: the style is written from it and the layout is worked out from it, so
    the two cannot disagree about where a child starts.
    """
    if not any(key in props for key in PADDING_PROPS):
        return None
    top = right = bottom = left = 0
    if "padding" in props:
        value, line = props["padding"]
        top, right, bottom, left = box_sides(value, line, "`padding` of %s" % where)
    sides = {"padding-top": "top", "padding-right": "right",
             "padding-bottom": "bottom", "padding-left": "left"}
    found = {"top": top, "right": right, "bottom": bottom, "left": left}
    for key, side in sides.items():
        if key in props:
            value, line = props[key]
            found[side] = read_length(value, line, "`%s` of %s" % (key, where))
    if "padx" in props:
        value, line = props["padx"]
        found["left"] = found["right"] = read_length(value, line, "`padx` of %s" % where)
    if "pady" in props:
        value, line = props["pady"]
        found["top"] = found["bottom"] = read_length(value, line, "`pady` of %s" % where)
    return found["left"], found["top"], found["right"], found["bottom"]


BORDER_SIDE_KEYS = (("border-left-width", 0), ("border-top-width", 1),
                    ("border-right-width", 2), ("border-bottom-width", 3))


def border_sides(props, where):
    """(left, top, right, bottom) of the border @p props draw, or None when they draw
    none -- which is what CSS draws until a `border-style` says otherwise."""
    style = props.get("border-style", ("none", 0))[0].strip()
    if style == "none":
        return None
    sides = [3, 3, 3, 3]     # `medium`, the CSS default width
    if "border-width" in props:
        value, line = props["border-width"]
        top, right, bottom, left = box_sides(value, line, "`border-width` of %s" % where)
        sides = [left, top, right, bottom]
    for key, index in BORDER_SIDE_KEYS:
        if key in props:
            value, line = props[key]
            sides[index] = read_length(value, line, "`%s` of %s" % (key, where))
    return tuple(sides) if any(sides) else None


def inner_sides(props, where):
    """(left, top, right, bottom) from the edge of a row to where its content starts:
    its border and its padding. One answer for the style and for the layout."""
    pad = padding_sides(props, where)
    border = border_sides(props, where)
    if pad is None and border is None:
        return None
    pad = pad or (0, 0, 0, 0)
    border = border or (0, 0, 0, 0)
    return tuple(a + b for a, b in zip(pad, border))


def content_box(node, x, y, w, h):
    """The rect @p node places its children in: its own, less its padding.

    Read with ::padding_sides, the same reading the style is written with. Leaving
    padding out of this started every flow at the parent's corner, and the first layout
    moved every row of a padded panel by its padding.
    """
    # A border is inside the rect here, so the content starts inside it, as in CSS.
    sides = inner_sides(node.props, describe(node)) or (0, 0, 0, 0)
    left, top, right, bottom = sides
    return x + left, y + top, max(w - left - right, 0), max(h - top - bottom, 0)


def margin_sides(node, base):
    """(top, right, bottom, left) of @p node's margin, each a length or `"auto"`; a
    percentage is of @p base, the width of the room it is in, as in CSS."""
    found = ["0", "0", "0", "0"]
    lines = [node.line] * 4
    if "margin" in node.props:
        value, line = node.props["margin"]
        found = box_sides_text(value, line, "`margin` of %s" % describe(node))
        lines = [line] * 4
    for index, side in enumerate(("top", "right", "bottom", "left")):
        key = "margin-" + side
        if key in node.props:
            found[index], lines[index] = node.props[key]
    out = []
    for text, line in zip(found, lines):
        text = text.strip()
        out.append("auto" if text == "auto" else
                   read_length(text, line, "`margin` of %s" % describe(node), base))
    return tuple(out)


def bounds_of(node, base_w, base_h):
    """(min width, min height, max width, max height); 0 is none."""
    out = []
    for key, base in (("min-width", base_w), ("min-height", base_h),
                      ("max-width", base_w), ("max-height", base_h)):
        if key in node.props and node.props[key][0].strip() not in ("none", "auto"):
            value, line = node.props[key]
            out.append(max(read_length(value, line, "`%s` of %s" % (key, describe(node)), base), 0))
        else:
            out.append(0)
    return tuple(out)


def clamp(length, least, most):
    """::lh_ui_place_clamp: the maximum first, then the minimum."""
    if most > 0 and length > most:
        length = most
    if length < least:
        length = least
    return length


def lay_out(node, flowing, axis, gap, content, ids):
    """::lay_out_line once, or -- for a flow that wraps -- once a line, the way
    `lh_ui_layout_apply` breaks the children into lines and shares out what is left
    across them (CSS's `align-content: stretch`)."""
    wraps = node.props.get("flex-wrap", ("nowrap", 0))[0].strip() == "wrap"
    if not wraps:
        lay_out_line(node, flowing, axis, gap, content, ids)
        return
    cx, cy, cw, ch = content
    vertical = axis == "lh_ui_axis_vertical"
    room_along, room_across = (ch, cw) if vertical else (cw, ch)
    line_gap = number_of(node, "column-gap" if vertical else "row-gap", 0)

    def size_of(child, mode, size, cross):
        left, top, right, bottom = child.margin
        ms, me, cs, ce = (top, bottom, left, right) if vertical else (left, right, top, bottom)
        minw, minh, maxw, maxh = child.bounds
        along_bounds = (minh, maxh) if vertical else (minw, maxw)
        across_bounds = (minw, maxw) if vertical else (minh, maxh)
        if mode == "fill":
            wants = 0
        else:
            wants = clamp(size if mode == "fixed" else 0, *along_bounds)
        return wants + ms + me

    def depth_of(child, cross, align):
        left, top, right, bottom = child.margin
        cs, ce = (left, right) if vertical else (top, bottom)
        minw, minh, maxw, maxh = child.bounds
        across_bounds = (minw, maxw) if vertical else (minh, maxh)
        own = clamp(0, *across_bounds) if align == "lh_ui_place_align_fill" else cross
        return own + cs + ce

    lines = []
    line = []
    used = 0
    count = 0
    depth = 0
    for item in flowing:
        child, mode, size, cross, align = item
        if child.hidden:
            line.append(item)
            continue
        length = size_of(child, mode, size, cross)
        if count > 0 and used + gap + length > room_along:
            lines.append((line, depth))
            line, used, count, depth = [], 0, 0, 0
        used += (gap if count > 0 else 0) + length
        count += 1
        depth = max(depth, depth_of(child, cross, align))
        line.append(item)
    if line:
        lines.append((line, depth))
    if not lines:
        return
    total = sum(d for _l, d in lines)
    free = room_across - total - line_gap * (len(lines) - 1)
    extra = free // len(lines) if free > 0 else 0
    at = 0
    for items, depth in lines:
        if vertical:
            band = (cx + at, cy, depth + extra, ch)
        else:
            band = (cx, cy + at, cw, depth + extra)
        lay_out_line(node, items, axis, gap, band, ids)
        at += depth + extra + line_gap


def lay_out_line(node, flowing, axis, gap, content, ids):
    """Place the flowing children of @p node the way `lh_ui_layout_apply` will.

    A line-for-line copy of that pass, for the cases it can know: fixed lengths,
    weighted shares of what is left, margins, bounds, `justify-content`, and where each
    row sits across. A `wrap` row asks for its minimum here, because its size is its
    text and the font is the app's. Shares are divided whole, as in a build without an
    FPU; a float build gives the last pixel elsewhere, and its first layout says so.
    """
    cx, cy, cw, ch = content
    vertical = axis == "lh_ui_axis_vertical"
    room_along, room_across = (ch, cw) if vertical else (cw, ch)
    origin_along, origin_across = (cy, cx) if vertical else (cx, cy)

    def sides(child):
        left, top, right, bottom = child.margin
        return ((top, bottom, left, right) if vertical else (left, right, top, bottom))

    def bound_along(child, length):
        minw, minh, maxw, maxh = child.bounds
        return clamp(length, *((minh, maxh) if vertical else (minw, maxw)))

    def bound_across(child, length):
        minw, minh, maxw, maxh = child.bounds
        return clamp(length, *((minw, maxw) if vertical else (minh, maxh)))

    def wants_of(mode, size, child):
        if mode == "fill":
            return 0
        if mode == "fixed":
            return bound_along(child, size)
        return bound_along(child, 0)

    used = 0
    shown = 0
    weights = 0
    for child, mode, size, _cross, _align in flowing:
        if child.hidden:
            continue
        ms, me, _cs, _ce = sides(child)
        if mode == "fill":
            weights += size if size > 0 else 1
        wants = wants_of(mode, size, child)
        if wants > 0 or mode == "fill" or ms + me > 0:
            used += wants + ms + me
            shown += 1
    if shown > 0:
        used += gap * (shown - 1)
    left = max(room_along - used, 0)
    spread = 0
    at = origin_along
    justify = (lookup(JUSTIFY, node, "justify-content") if "justify-content" in node.props
               else "lh_ui_justify_start")
    if weights == 0 and shown > 0:
        if justify == "lh_ui_justify_center":
            at += (left + 1) // 2
        elif justify == "lh_ui_justify_end":
            at += left
        elif justify == "lh_ui_justify_space_between":
            spread = left // (shown - 1) if shown > 1 else 0
        elif justify == "lh_ui_justify_space_around":
            spread = left // shown
            at += spread // 2
        elif justify == "lh_ui_justify_space_evenly":
            spread = left // (shown + 1)
            at += spread

    for child, mode, size, cross, align in flowing:
        ms, me, cs, ce = sides(child)
        if mode == "fill":
            wants = bound_along(child, left * (size if size > 0 else 1) // weights)
        else:
            wants = wants_of(mode, size, child)
        room = max(room_across - cs - ce, 0)
        if align == "lh_ui_place_align_fill":
            across, length = origin_across + cs, bound_across(child, room)
        else:
            offset = 0
            # A baseline is the font's, so the table starts a baseline row where the
            # engine puts one that has none -- in the middle -- and the first layout
            # lifts it onto the line.
            if align in ("lh_ui_place_align_center", "lh_ui_place_align_baseline"):
                offset = max((room - cross + 1) // 2, 0)
            elif align == "lh_ui_place_align_end":
                offset = room - cross
            across, length = max(origin_across + cs + max(offset, 0), 0), cross
        if vertical:
            _measure(child, across, at + ms, length, wants, ids)
        else:
            _measure(child, at + ms, across, wants, length, ids)
        # A hidden row is not moved by the engine and takes no room: it is measured
        # where it would have started, and nothing after it moves for it.
        if child.hidden:
            continue
        if wants > 0 or ms + me > 0:
            at += ms + wants + me + gap + spread


def _measure(node, x, y, w, h, ids):
    """Give @p node the rect its parent decided, check it, and place its children.

    The rects in the table are what the first layout will find, worked out here with
    the arithmetic of `lh_ui_layout_apply` (layout.c), so that a tree that has not
    been laid out yet already looks like one that has. The one size this cannot know
    is a `wrap` row's -- that is its text, and the text is measured by a font -- so it
    starts at 0 along the flow and the first layout gives it its size.
    """
    if node.name:
        if node.name in ids:
            raise Failure("`#%s` appears twice at line %d. Two rows with one name is a "
                          "tree the app cannot ask a question of"
                          % (node.name, node.line))
        ids.add(node.name)

    node.rect = (x, y, w, h)

    # A scrollbar is the one row that names something else rather than sitting in a
    # flow of its own: `scrolls: #rows` and `axis:` are read here and become the row
    # index and the axis in the table, and a scrollbar missing either is refused rather
    # than emitted as a bar that drives nothing.
    if "axis" in node.props and node.tag != "scrollbar":
        raise Failure("`axis` at line %d on %s, which is a `%s`. Only a scrollbar has an "
                      "axis to name" % (node.props["axis"][1], describe(node), node.tag))
    if "scrolls" in node.props and node.tag != "scrollbar":
        raise Failure("`scrolls` at line %d on %s, which is not a scrollbar"
                      % (node.props["scrolls"][1], describe(node)))
    if node.tag == "scrollbar":
        if "scrolls" not in node.props:
            raise Failure("%s at line %d is a scroll bar that drives nothing. Say which "
                          "container with `scrolls: #id`" % (describe(node), node.line))
        if "axis" not in node.props:
            raise Failure("%s at line %d scrolls something but names no `axis`"
                          % (describe(node), node.line))

    children = node.children
    if children and node.tag not in HOLDERS:
        raise Failure("`%s` at line %d holds %d children, and a `%s` does not place "
                      "children. Use `box` for a container"
                      % (describe(node), node.line, len(children), node.tag))

    # `display: flex` is the only thing that gives a container a flow, so it is the only
    # thing that makes `flex-direction`, `justify-content` and `gap` mean anything. The
    # first version gave every container a flow because it had children, which meant a
    # title bar silently became a column: the rows under it were laid out by
    # `lh_ui_container_place_children` on every frame, and the coordinates the
    # stylesheet wrote were overwritten before anybody could see whether they were right.
    display = node.props.get("display", ("", 0))[0].strip()
    flows = display in ("flex", "block")
    if display == "block" and "flex-direction" in node.props:
        raise Failure("`flex-direction` at line %d in %s, which is a block: a block runs "
                      "down, and `display: flex` is what runs either way"
                      % (node.props["flex-direction"][1], describe(node)))
    node.hidden = node.props.get("display", ("", 0))[0].strip() == "none"
    for key in FLOW_PROPS:
        if key in node.props and not flows:
            raise Failure("`%s` at line %d in %s, and %s says nothing about a flow. "
                          "Either write `display: flex` on %s or drop the property: a "
                          "flow property on a container that has no flow is read by "
                          "nothing"
                          % (key, node.props[key][1], describe(node), describe(node),
                             describe(node)))

    if display == "block":
        axis = "lh_ui_axis_vertical"
    else:
        axis = (lookup(DIRECTIONS, node, "flex-direction") if "flex-direction" in node.props
                else "lh_ui_axis_horizontal")
    # `gap` is two gaps in CSS, between rows and between columns; a flow runs one way
    # and uses the one between the things it lines up.
    gap = number_of(node, "row-gap" if axis == "lh_ui_axis_vertical" else "column-gap", 0)

    flowing = []
    cursor = 0
    inner = content_box(node, x, y, w, h)
    for child in children:
        placed = placed_absolute(child)
        if placed and flows:
            # The flow places **every** child it holds: an entity's place is part of
            # the entity, not a pointer that can be left empty, so there is no row a
            # flow skips. A bar written as `position: absolute` inside a column came
            # out where the stylesheet said, and then the column's first layout put it
            # under the last row at a height of 0 -- off the bottom of the window.
            raise Failure("`position: absolute` at line %d on %s, inside %s, which has "
                          "`display: flex`. A flow places every row it holds, so this "
                          "one would be moved into the flow on the first layout. Put it "
                          "in a box with no `display`, which places nothing"
                          % (child.props["position"][1], describe(child), describe(node)))
        if placed:
            # A row that says where it is takes no room from the cursor: the panel
            # floating over the window is not the next thing in a column, and counting
            # it as one would push everything under it down by its own height.
            #
            # `left` and `top` are measured **from the parent's corner**, as they are in
            # CSS, and the table wants absolute rects, so the parent's own origin is
            # added here. Reading them as absolute put the tree's heading at (0, 0) --
            # on top of the window title -- and every header in the window with it,
            # while the stylesheet looked perfectly reasonable.
            child.place = None
            bx, by, bw, bh = (0, 0) + window_size(node) if placed == "fixed" else (x, y, w, h)
            cx, cw = place_by_hand(child, bx, bw, "left", "right", "width")
            cy, ch = place_by_hand(child, by, bh, "top", "bottom", "height")
            # A margin moves a row placed by hand away from the side it is placed
            # from, and its bounds hold its size, as in CSS.
            top, right, bottom, left = [0 if m == "auto" else m for m in margin_sides(child, bw)]
            minw, minh, maxw, maxh = bounds_of(child, bw, bh)
            cw, ch = clamp(cw, minw, maxw), clamp(ch, minh, maxh)
            cx += left if "left" in child.props or "right" not in child.props else -right
            cy += top if "top" in child.props or "bottom" not in child.props else -bottom
            # A container here clips what it holds, where CSS lets it show unless told
            # `overflow: hidden`. A row placed outside its parent would be in the
            # sheet and nowhere on screen, so it is refused unless the parent says
            # clipping is what it wants.
            clips = any(node.props.get(key, ("", 0))[0].strip() in ("hidden", "auto", "scroll")
                        for key in ("overflow-x", "overflow-y"))
            if not clips and placed != "fixed" and (cx < x or cy < y or cx + cw > x + w
                                                    or cy + ch > y + h):
                raise Failure("%s at line %d is placed at (%d, %d) %dx%d, outside %s at "
                              "(%d, %d) %dx%d -- and a container clips what it holds, so it "
                              "would not be seen. Put it in a row it fits in, or say "
                              "`overflow: hidden` on %s if being cut is the point"
                              % (describe(child), child.line, cx, cy, cw, ch, describe(node),
                                 x, y, w, h, describe(node)))
            _measure(child, cx, cy, cw, ch, ids)
            continue

        if not flows:
            # A box with no `display` places nothing, so what is in it stays where the
            # table puts it: one after another across, from the box's corner, at the
            # size it says. Nothing will ever correct these, so nothing is guessed.
            child.place = None
            cw, ch = stated_size(child, "in %s, which has no `display` and so gives its "
                                        "rows no size; say `display: flex` on it, or "
                                        "size this one" % describe(node),
                                 (inner[2], inner[3]))
            top, right, bottom, left = [0 if m == "auto" else m
                                        for m in margin_sides(child, inner[2])]
            minw, minh, maxw, maxh = bounds_of(child, inner[2], inner[3])
            cw, ch = clamp(cw, minw, maxw), clamp(ch, minh, maxh)
            _measure(child, inner[0] + cursor + left, inner[1] + top, cw, ch, ids)
            cursor += left + cw + right + gap
            continue

        vertical = axis == "lh_ui_axis_vertical"
        along_key, cross_key = ("height", "width") if vertical else ("width", "height")
        share = prop(child, "flex")
        if share == 0:
            share = None
        # A percentage is a share of the room the flow has: its content box.
        room_along, room_cross = (inner[3], inner[2]) if vertical else (inner[2], inner[3])
        along = prop(child, along_key, room_along)
        if "flex-basis" in child.props and not is_auto(child, "flex-basis"):
            # The basis is the size along the flow, whichever way it runs, and it
            # outranks `width` / `height` there as it does in CSS.
            along = prop(child, "flex-basis", room_along)
        cross = prop(child, cross_key, room_cross)
        if along is None and share is None and "aspect-ratio" in child.props:
            across = cross if cross is not None else room_cross
            ratio = read_ratio(child)
            along = int(round(across / ratio)) if vertical else int(round(across * ratio))
        if share is not None:
            # The share is a weight: `flex: 2` beside `flex: 1` is two thirds and one,
            # and the place carries it where a fixed row carries its length.
            if share < 0:
                raise Failure("`flex: %d` at line %d in %s: a share of what is left is not "
                              "less than none" % (share, child.props["flex"][1], describe(child)))
            if along is not None:
                raise Failure("`flex: 1` and `%s` at line %d in %s: one says \"what is "
                              "left\" and the other a number, and only one of them can be "
                              "the size along %s" % (along_key, child.props["flex"][1],
                                                    describe(child), describe(node)))
            size_mode, size = "fill", share
        elif along is not None:
            # The size **along** the flow. A column runs down, so what a child asks
            # for down there is its height; a row runs across, so it is its width. The
            # first version had these the other way round and a column of full-width
            # rows advanced the cursor by 980 each time and put the scroll bar at
            # (1028, 2672).
            size_mode, size = "fixed", along
        else:
            # Nothing said: as big as what it holds, the way an `auto` size is. A label
            # is its text, a box is its children, and one with neither is nothing and
            # takes no room -- which is what makes an empty label collapse.
            size_mode, size = "wrap", 0

        # Across the flow, the CSS rule: a row with no size of its own is stretched to
        # the parent, and one that states a size keeps it. The parser used to stretch
        # every row whatever it said, so a `width: 100` in a 400 column was written into
        # the table as 100 and drawn as 400 after the first layout.
        # `align-self` on the row, else `align-items` on the flow.
        align_from = child if "align" in child.props else None
        if align_from is None and "align-items" in node.props:
            align_from = node
        if align_from is not None:
            align_key = "align" if align_from is child else "align-items"
            align = lookup(ALIGN, align_from, align_key)
            if (align not in ("lh_ui_place_align_fill",) and cross is None
                    and child.tag in ("label", "input")):
                # A caption with no size across sits in the middle of a row in CSS
                # because it is as tall as its text. A label here is as tall as it is
                # told, so it is stretched across instead and its **text** is put where
                # the row would have put the box: the same picture, and no number the
                # sheet did not write.
                text_key = "valign" if not vertical else "text-align"
                text_value = {"lh_ui_place_align_start": "top" if not vertical else "left",
                              "lh_ui_place_align_center": "center",
                              "lh_ui_place_align_end": "bottom" if not vertical else "right",
                              "lh_ui_place_align_baseline": "center"}[align]
                if text_key not in child.props:
                    tucked = {text_key: (text_value, child.line)}
                    child.layers.insert(0, (frozenset(), tucked))
                    child.props[text_key] = tucked[text_key]
                align = "lh_ui_place_align_fill"
            elif (align not in ("lh_ui_place_align_fill",) and cross is None
                    and child.tag in ("box", "container", "entity") and not paints_itself(child)
                    and (not child.children
                         or child.props.get("display", ("", 0))[0].strip() in ("flex", "block"))):
                # The same for a box that draws nothing of its own and holds a flow: in
                # CSS it is as tall as what it holds and sits in the middle; here it is
                # stretched, and what it holds is put in its middle instead. Nothing on
                # screen is the box, so nothing on screen can tell the difference.
                if "align-items" not in child.props and child.children:
                    tucked = {"align-items": ({"lh_ui_place_align_start": "start",
                                               "lh_ui_place_align_center": "center",
                                               "lh_ui_place_align_end": "end",
                                               "lh_ui_place_align_baseline": "baseline"}[align],
                                              child.line)}
                    child.props["align-items"] = tucked["align-items"]
                align = "lh_ui_place_align_fill"
            if align == "lh_ui_place_align_fill" and cross is not None:
                if align_from is node:
                    # `align-items: stretch` is the default every row of a flow gets;
                    # a row that states its size keeps it, as in CSS.
                    align = "lh_ui_place_align_start"
                else:
                    raise Failure("`align: stretch` and `%s` at line %d in %s say two "
                                  "sizes across %s. Drop one: with no `%s` the row is "
                                  "stretched anyway"
                                  % (cross_key, child.props["align"][1], describe(child),
                                     describe(node), cross_key))
            if align != "lh_ui_place_align_fill" and cross is None:
                raise Failure("`%s` at line %d in %s puts %s somewhere across %s, and it "
                              "has no `%s` to be put there with. Only `stretch` takes its "
                              "size from the parent"
                              % (align_key, align_from.props[align_key][1],
                                 describe(align_from), describe(child), describe(node),
                                 cross_key))
        elif cross is not None:
            align = "lh_ui_place_align_start"
        else:
            align = "lh_ui_place_align_fill"

        # The margin, and what an `auto` margin across means: the row is pushed to the
        # other side, or to the middle when both are -- `margin: 0 auto` is how CSS
        # centres a box, and here it is the row's place across.
        top, right, bottom, left = margin_sides(child, inner[2])
        before, after = (left, right) if vertical else (top, bottom)
        lead, trail = (top, bottom) if vertical else (left, right)
        if "auto" in (lead, trail):
            raise Failure("an `auto` margin along the flow at line %d in %s. The room left "
                          "over is the flow's to hand out: say `justify-content` on %s, or "
                          "`flex` on this row" % (child.props.get("margin", child.props.get(
                              "margin-%s" % ("top" if vertical else "left"),
                              ("", child.line)))[1], describe(child), describe(node)))
        if "auto" in (before, after):
            if cross is None:
                raise Failure("an `auto` margin across the flow at line %d in %s, which has "
                              "no `%s` to be pushed about with" % (child.line, describe(child),
                                                                 cross_key))
            align = {(True, True): "lh_ui_place_align_center",
                     (True, False): "lh_ui_place_align_end",
                     (False, True): "lh_ui_place_align_start"}[(before == "auto", after == "auto")]
            before = 0 if before == "auto" else before
            after = 0 if after == "auto" else after
            if vertical:
                left, right = before, after
            else:
                top, bottom = before, after
        child.margin = (left, top, right, bottom)
        child.hidden = child.props.get("display", ("", 0))[0].strip() == "none"
        minw, minh, maxw, maxh = bounds_of(child, inner[2], inner[3])
        child.bounds = (minw, minh, maxw, maxh)
        if size_mode == "fixed":
            size = clamp(size, *((minh, maxh) if vertical else (minw, maxw)))
        if cross is not None:
            cross = clamp(cross, *((minw, maxw) if vertical else (minh, maxh)))

        child.place = (SIZE_MODE[size_mode], size, align, child.margin, (minw, minh), (maxw, maxh))
        flowing.append((child, size_mode, size, cross, align))

    if flowing:
        lay_out(node, flowing, axis, gap, content_box(node, x, y, w, h), ids)

    # A flow belongs to `display: flex`, **not** to having children. `#rows` is a
    # container the stylesheet gives a flow and no children, because the children are
    # the 1408 rows of the parameter table and those are data the application fills;
    # tying the flow to the stylesheet's own children left that one container with no
    # flow at all, and 1408 rows with a place and nowhere to be placed by.
    node.layout = (axis, gap, lookup(JUSTIFY, node, "justify-content") if "justify-content" in node.props else "lh_ui_justify_start",
                   node.props.get("flex-wrap", ("nowrap", 0))[0].strip() == "wrap",
                   number_of(node, "column-gap" if axis == "lh_ui_axis_vertical" else "row-gap", 0)) \
        if flows else None


def paints_itself(node):
    """Does @p node draw anything of its own -- a fill, a border, a shadow -- in any of
    its looks? A box that does not is only a way of placing what it holds."""
    for pseudo, props in node.layers:
        if any(key in props for key in ("background", "box-shadow", "shadow")):
            return True
        if props.get("border-style", ("none", 0))[0].strip() not in ("none", ""):
            return True
    return False


def placed_absolute(node):
    """`"absolute"` or `"fixed"` for a row that says where it is, else False.

    `relative` and `static` are the flow's own place, and a row that says one of them
    with an offset is refused: there is no nudge after a layout here.
    """
    where = [key for key in ("left", "top", "right", "bottom") if key in node.props]
    has_position = "position" in node.props
    if not has_position and not where:
        return False
    if not has_position:
        # `lookup` reports the line of the property it was asked about, and there is
        # no `position` to have a line: this was a KeyError and a traceback.
        key = where[0]
        raise Failure("`%s` at line %d in %s without `position: absolute`. A coordinate "
                      "is read only for a row placed by hand, and this one is in the flow"
                      % (key, node.props[key][1], describe(node)))
    position = lookup(VALUE_TABLES["position"], node, "position")
    if position in ("relative", "static"):
        if where:
            raise Failure("`position: %s` and `%s` at line %d in %s. A row in a flow is "
                          "where the flow puts it, and nothing here moves it after: use "
                          "`position: absolute` in a box with no `display`"
                          % (position, where[0], node.props[where[0]][1], describe(node),
                             ))
        return False
    for pair in (("left", "right"), ("top", "bottom")):
        if not any(key in node.props for key in pair):
            raise Failure("`position: %s` at line %d in %s names neither `%s` nor `%s`. "
                          "A row placed by hand is placed by both of its coordinates, and "
                          "half of them is the top left corner of the window"
                          % (position, node.props["position"][1], describe(node), pair[0],
                             pair[1]))
    return position


def place_by_hand(node, start, room, near, far, size_key):
    """`(start, length)` of a row placed by hand along one axis, from any two of
    `left` / `right` / `width` (or `top` / `bottom` / `height`) -- or all three, with
    the size winning over the far side, as CSS has it."""
    lo = prop(node, near, room)
    hi = prop(node, far, room)
    length = prop(node, size_key, room)
    if length is None and "aspect-ratio" in node.props:
        other = "height" if size_key == "width" else "width"
        known = prop(node, other, room)
        if known is not None:
            ratio = read_ratio(node)
            length = int(round(known * ratio)) if size_key == "width" else int(round(known / ratio))
    if length is None:
        if lo is None or hi is None:
            raise Failure("%s at line %d has no `%s`, and it needs one: it is placed by "
                          "hand, and only `%s` with `%s` can stand for it"
                          % (describe(node), node.line, size_key, near, far))
        length = max(room - lo - hi, 0)
    if lo is None:
        lo = room - hi - length
    return start + lo, length


def window_size(node):
    """`(width, height)` of the window: the root's own size, which it states."""
    while node.parent is not None:
        node = node.parent
    return node.rect[2], node.rect[3]


def read_ratio(node):
    """`aspect-ratio: 16 / 9` or `1.5` -> width over height."""
    value, line = node.props["aspect-ratio"]
    m = re.match(r"^\s*(\d+(?:\.\d+)?)\s*(?:/\s*(\d+(?:\.\d+)?))?\s*$", value)
    if not m or float(m.group(2) or 1) == 0 or float(m.group(1)) == 0:
        raise Failure("`aspect-ratio: %s` at line %d in %s is not a ratio: `16 / 9` or "
                      "`1.5`" % (value, line, describe(node)))
    return float(m.group(1)) / float(m.group(2) or 1)


def prop(node, key, base=None):
    """A length @p node states for @p key, or None when it states none (or `auto`)."""
    if key not in node.props or is_auto(node, key):
        return None
    value, line = node.props[key]
    if key == "flex":
        return read_number(value, line, "`flex` of %s" % describe(node))
    return read_length(value, line, "`%s` of %s" % (key, describe(node)), base)


def number_of(node, key, default, base=None):
    if key not in node.props:
        return default
    value, line = node.props[key]
    return read_length(value, line, "`%s` of %s" % (key, describe(node)), base)


def prop_str(node, key, default="", props=None):
    props = node.props if props is None else props
    return props.get(key, (default, node.line))[0].strip()


def lookup(table, node, key, props=None):
    """One of the named values of @p key, or a message that says which are legal.

    A bare `table[value]` answers a typo with a Python traceback and a line number in
    this file, which is the least useful thing a stylesheet author can be handed.

    @p props is where @p key is read from: the row's own, or one of its states'.
    """
    props = node.props if props is None else props
    value = prop_str(node, key, props=props)
    if value not in table:
        raise Failure("`%s: %s` at line %d in %s. Legal here: %s"
                      % (key, value, props[key][1], describe(node),
                         ", ".join(sorted(table))))
    return table[value]


def describe(node):
    """`tag#id`, and nothing else.

    The line number is deliberately **not** in here. Every caller already has it and
    prints it, and the two together made every message say `at line 8` twice --
    which reads like two different problems and is the kind of thing that makes a
    tool's output worth ignoring.
    """
    if node.selector:
        return "`%s`" % node.selector
    return "`%s`" % (node.html + ("#" + node.name if node.name else ""))


# -- Writing -------------------------------------------------------------------

def walk(node, out):
    out.append(node)
    for child in node.children:
        walk(child, out)
    return out


#: The fonts the last ::emit named, for ::header to declare.
FONTS_USED = []


def opacity_of(node, props):
    """How opaque @p node is in the look @p props describe: its own `opacity` times
    every ancestor's, and 0 when it is `visibility: hidden`.

    There is no opacity in a style, so it is applied where it shows -- to the alpha of
    every colour the row and the rows under it paint. That is CSS's group opacity
    wherever the children do not overlap one another, which in a flow they do not.
    """
    fade = 1.0
    if props.get("visibility", ("visible", 0))[0].strip() == "hidden":
        return 0.0
    if "opacity" in props:
        fade = read_opacity(props["opacity"][0], props["opacity"][1], describe(node))
    up = node.parent
    while up is not None:
        if "opacity" in up.props:
            fade *= read_opacity(up.props["opacity"][0], up.props["opacity"][1], describe(up))
        up = up.parent
    return fade


def faded(rgb, fade):
    if fade >= 1.0:
        return rgb
    return (rgb[0], rgb[1], rgb[2], int(round((rgb[3] if len(rgb) > 3 else 255) * fade)))


def tint_of(node, variables):
    value, line = node.props["color"]
    return faded(read_colour(value, line, variables, "`color` of %s" % describe(node)),
                 opacity_of(node, node.props))


def read_opacity(value, line, where):
    text = value.strip()
    try:
        fade = float(text[:-1]) / 100.0 if text.endswith("%") else float(text)
    except ValueError:
        raise Failure("`opacity: %s` at line %d in %s is not a number" % (text, line, where))
    return max(0.0, min(1.0, fade))


def read_box_shadow(value, line, variables, where):
    """`<x> <y> [<blur> [<spread>]] <colour>` -> (x, y, blur, rgb), or None for `none`.

    The engine's shadow is a colour, a fade distance and a shift; CSS's blur is the
    fade. A spread -- the shadow grown before it fades -- has no place in it, nor has an
    `inset` shadow or a second one, so those are refused rather than drawn as another
    shadow.
    """
    text = value.strip()
    if text == "none":
        return None
    if re.search(r",(?![^()]*\))", text):
        raise Failure("%s at line %d has more than one shadow; a style has one" % (where, line))
    tokens = re.findall(r"[a-z]+\([^)]*\)|\S+", text)
    if "inset" in tokens:
        raise Failure("%s at line %d is an `inset` shadow, and a shadow here is cast "
                      "outside the row" % (where, line))
    lengths = [t for t in tokens if _is_length(t)]
    colour = [t for t in tokens if not _is_length(t)]
    if not 2 <= len(lengths) <= 4:
        raise Failure("%s at line %d is %r: two to four lengths and a colour"
                      % (where, line, text))
    numbers = [read_length(t, line, where) for t in lengths] + [0, 0]
    if numbers[3] != 0:
        raise Failure("%s at line %d has a spread of %d. A shadow here fades from the "
                      "row's own edge and is not grown first: drop the fourth length"
                      % (where, line, numbers[3]))
    rgb = read_colour(" ".join(colour), line, variables, where) if colour else (0, 0, 0, 255)
    return numbers[0], numbers[1], max(numbers[2], 0), rgb


def font_name(props, where):
    """The C name of the font @p props ask for -- `body_14_bold` -- or None for the
    default. A font is baked at one size and one weight, so each is its own resource."""
    family = props.get("font-family", ("default", 0))[0].strip()
    parts = [family]
    if "font-size" in props:
        value, line = props["font-size"]
        parts.append("%d" % read_length(value, line, "`font-size` of %s" % where))
    weight = props.get("font-weight", ("normal", 0))[0].strip()
    if weight != "normal":
        parts.append(weight)
    if props.get("font-style", ("normal", 0))[0].strip() == "italic":
        parts.append("italic")
    if parts == ["default"]:
        return None
    return "_".join(parts)


def transform_text(text, how):
    how = how.strip()
    if how == "uppercase":
        return text.upper()
    if how == "lowercase":
        return text.lower()
    if how == "capitalize":
        return re.sub(r"(^|\s)(\S)", lambda m: m.group(1) + m.group(2).upper(), text)
    return text


def input_capacity(node):
    """The bytes an `input` holds: its `maxlength`, else 64."""
    if "maxlength" in node.props:
        return read_number(node.props["maxlength"][0], node.props["maxlength"][1], "`maxlength`")
    return 64


def emit(nodes, variables, root, source=""):
    colours = {}
    styles = []
    layouts = []
    places = []

    def colour_of(rgb):
        key = "ui_c_%02x%02x%02x%02x" % (rgb[0], rgb[1], rgb[2], rgb[3] if len(rgb) > 3 else 255)
        if key not in colours:
            colours[key] = rgb
        return key

    def number_in(props, key):
        value, line = props[key]
        return read_length(value, line, "`%s`" % key)

    def look(sname, props, node, where):
        """The calls that make `ui_s_<sname>` look like @p props say. Empty when they
        say nothing, and then no style is written at all."""
        body = []
        fade = opacity_of(node, props)

        def paint(key):
            value, line = props[key]
            return colour_of(faded(read_colour(value, line, variables,
                                               "`%s` of %s" % (key, where)), fade))

        if "background" in props:
            value, line = props["background"]
            fill = read_paint(value, line, variables, "`background` of %s" % where, node.rect)
            if fill[0] == "solid":
                body.append("lh_ui_style_set_fill(lh_addr_of(ui_s_%s), lh_addr_of(%s_paints));"
                            % (sname, colour_of(faded(fill[1], fade))))
            else:
                # The stops are set on the gradient once, in this init, and the paint
                # points at it: a gradient is a table and a paint is copied by value.
                shape, angle, stops = fill
                gradients_used.append(len(stops))
                body.append("{")
                body.append("    lh_ui_color_t stops[%d];" % len(stops))
                body.append("    const lh_byte_t at[%d] = {%s};"
                            % (len(stops), ", ".join("%d" % frac for _rgb, frac in stops)))
                for index, (rgb, _frac) in enumerate(stops):
                    rgb = faded(rgb, fade)
                    body.append("    lh_ui_color_init(lh_addr_of(stops[%d]), %d, %d, %d, %d);"
                                % (index, rgb[0], rgb[1], rgb[2], rgb[3] if len(rgb) > 3 else 255))
                body.append("    lh_ui_gradient_init(lh_addr_of(ui_gr_%s));" % sname)
                body.append("    lh_ui_gradient_init_stops(lh_addr_of(ui_gr_%s), stops, at, %d);"
                            % (sname, len(stops)))
                if shape == "radial":
                    body.append("    lh_ui_gradient_set_radial(lh_addr_of(ui_gr_%s));" % sname)
                else:
                    body.append("    lh_ui_gradient_set_linear(lh_addr_of(ui_gr_%s), %d);" % (sname, angle))
                body.append("}")
                body.append("lh_ui_paint_init_gradient(lh_addr_of(ui_gp_%s), lh_addr_of(ui_gr_%s));"
                            % (sname, sname))
                body.append("lh_ui_style_set_fill(lh_addr_of(ui_s_%s), lh_addr_of(ui_gp_%s));"
                            % (sname, sname))
        # On an image `color` is the tint (`lh_ui_decl_t::as.image.tint`), which is
        # what a picture's colour is; a style's text paint is read by nothing there.
        if "color" in props and node.tag != "image":
            body.append("lh_ui_style_set_text(lh_addr_of(ui_s_%s), lh_addr_of(%s_paints));"
                        % (sname, paint("color")))
        border = border_sides(props, where)
        if border is not None:
            # Its colour is `currentColor` when it names none, as in CSS.
            key = "border-color" if "border-color" in props else (
                "color" if "color" in props else None)
            colour = paint(key) if key else colour_of(faded((0, 0, 0, 255), fade))
            body.append("lh_ui_insets_init(lh_addr_of(ui_bd_%s), lh_ui_scalar(%d), "
                        "lh_ui_scalar(%d), lh_ui_scalar(%d), lh_ui_scalar(%d));"
                        % ((sname,) + border))
            body.append("lh_ui_style_set_border(lh_addr_of(ui_s_%s), lh_addr_of(ui_bd_%s));"
                        % (sname, sname))
            body.append("lh_ui_style_set_border_fill(lh_addr_of(ui_s_%s), "
                        "lh_addr_of(%s_paints));" % (sname, colour))
        font = font_name(props, where)
        if font is not None:
            # A font is a resource the application loads; the sheet names it and the
            # header declares it, so a font the app forgot is a link error, not a
            # caption that draws in some other face.
            fonts_used.add(font)
            body.append("lh_ui_style_set_font(lh_addr_of(ui_s_%s), "
                        "lh_addr_of(lh_cfg_ui_font_%s));" % (sname, font))
        sides = inner_sides(props, where)
        if sides is not None:
            if len(set(sides)) == 1:
                body.append("lh_ui_style_set_padding(lh_addr_of(ui_s_%s), lh_ui_scalar(%d));"
                            % (sname, sides[0]))
            else:
                # Per side, because **`padding` on all four sides cannot centre
                # anything**: a label's vertical alignment happens inside the box its
                # padding leaves, so a 28-row row with 14 of padding has a content box
                # zero rows high and the middle of nothing is its top edge.
                body.append("lh_ui_insets_init(lh_addr_of(ui_pad_%s), lh_ui_scalar(%d), "
                            "lh_ui_scalar(%d), lh_ui_scalar(%d), lh_ui_scalar(%d));"
                            % ((sname,) + sides))
                body.append("lh_ui_style_set_padding_insets(lh_addr_of(ui_s_%s), "
                            "lh_addr_of(ui_pad_%s));" % (sname, sname))
        corners = [number_in(props, key) if key in props else None
                   for key in ("radius-tl", "radius-tr", "radius-br", "radius-bl")]
        if any(c is not None for c in corners):
            corners = [c or 0 for c in corners]
            if len(set(corners)) == 1:
                body.append("lh_ui_style_set_radius(lh_addr_of(ui_s_%s), lh_ui_scalar(%d));"
                            % (sname, corners[0]))
            else:
                body.append("lh_ui_radii_init(lh_addr_of(ui_rd_%s), lh_ui_scalar(%d), "
                            "lh_ui_scalar(%d), lh_ui_scalar(%d), lh_ui_scalar(%d));"
                            % ((sname,) + tuple(corners)))
                body.append("lh_ui_style_set_radii(lh_addr_of(ui_s_%s), lh_addr_of(ui_rd_%s));"
                            % (sname, sname))
        # A transition is set on **every** look of the row, not only the resting one.
        # The engine reads the time off the style it is switching *to*
        # (::lh_ui_style_get_transition), so a `:hover` that carried the time and a
        # resting style that did not would interpolate on the way in and snap on the
        # way out. `props` here is the whole cascade for this state, so a `transition`
        # written once on the row is already in every one of these calls.
        #
        # A time of zero is the style's own default and is not written: an
        # `lh_ui_style_set_transition(..., 0)` says nothing the `lh_ui_style_init` two
        # lines above it does not.
        if "transition-duration" in props and int(props["transition-duration"][0]):
            body.append("lh_ui_style_set_transition(lh_addr_of(ui_s_%s), %d);"
                        % (sname, int(props["transition-duration"][0])))
            curve = props.get("transition-timing-function", ("ease", 0))[0].strip()
            if curve != "ease":
                body.append("lh_ui_style_set_easing(lh_addr_of(ui_s_%s), %s);" % (sname, EASINGS[curve]))
        # `shadow` is the fade distance and how far down it is cast. Its **colour** is
        # `--shadow` in `:root`, because a colour is a thing a theme changes and a
        # distance is a thing a layout changes: one variable the reader can replace to
        # restyle every island at once, and a number per row that stays put when the
        # palette does not.
        shadow = None
        if "box-shadow" in props:
            value, line = props["box-shadow"]
            shadow = read_box_shadow(value, line, variables, "`box-shadow` of %s" % where)
        elif "shadow" in props:
            spread, drop = read_shadow(props["shadow"][0], props["shadow"][1],
                                       "`shadow` of %s" % where)
            shadow = (0, drop, spread, root.variables["shadow"])
        if shadow is not None:
            dx, dy, blur, rgb = shadow
            body.append("lh_ui_shadow_init(lh_addr_of(ui_shadow_%s));" % sname)
            body.append("lh_ui_shadow_set_color(lh_addr_of(ui_shadow_%s), %s);"
                        % (sname, colour_of(faded(rgb, fade))))
            body.append("lh_ui_shadow_set_spread(lh_addr_of(ui_shadow_%s), lh_ui_scalar(%d));"
                        % (sname, blur))
            body.append("lh_ui_shadow_set_offset(lh_addr_of(ui_shadow_%s), lh_ui_scalar(%d), "
                        "lh_ui_scalar(%d));" % (sname, dx, dy))
            body.append("lh_ui_style_set_shadow(lh_addr_of(ui_s_%s), "
                        "lh_addr_of(ui_shadow_%s));" % (sname, sname))
        if "text-align" in props:
            body.append("lh_ui_style_set_align_h(lh_addr_of(ui_s_%s), %s);"
                        % (sname, lookup(TEXT_ALIGN, node, "text-align", props)))
        if "valign" in props:
            body.append("lh_ui_style_set_align_v(lh_addr_of(ui_s_%s), %s);"
                        % (sname, lookup(VALIGN, node, "valign", props)))
        if "text-overflow" in props:
            body.append("lh_ui_style_set_overflow(lh_addr_of(ui_s_%s), %s);"
                        % (sname, lookup(VALUE_TABLES["text-overflow"], node, "text-overflow", props)))
        if "white-space" in props and node.tag in ("label", "input"):
            wrap = lookup(VALUE_TABLES["white-space"], node, "white-space", props)
            if wrap != "lh_ui_text_wrap_none":
                body.append("lh_ui_style_set_wrap(lh_addr_of(ui_s_%s), %s);" % (sname, wrap))
        return body

    #: sname -> whether a row in that look has a style at all, for the rows to point at.
    styled = {}
    del FONTS_USED[:]
    fonts_used = set()
    gradients_used = []
    #: sname -> the style it is written as (itself, or an identical one written first).
    canonical = {}
    #: the calls of a style -> the name it was first written under.
    shared = {}

    for node in nodes:
        # --- looks: one per state the row can be in, each the cascade for that state
        # (::resolve), and a pressed one for each of those when anything says `:active`.
        name = node.cname()
        said = set()
        for pseudo, _props in node.layers:
            said |= pseudo
        wanted = [("", frozenset())]
        if node.tag in ("button", "toggle") and "hover" in said:
            wanted.append(("__hover", frozenset(["hover"])))
        if node.tag == "toggle":
            if "checked" in said:
                wanted.append(("__checked", frozenset(["checked"])))
            if "hover" in said:
                wanted.append(("__checked_hover", frozenset(["checked", "hover"])))
        if node.tag in ("button", "toggle") and "active" in said:
            wanted = wanted + [(suffix + "__active", state | frozenset(["active"]))
                               for suffix, state in wanted]

        for suffix, state in sorted(wanted, key=lambda w: "active" not in w[1]):
            sname = name + suffix
            props = resolve(node, state)
            body = look("@@", props, node, "%s%s" % (
                describe(node), "" if not state else " in `:%s`" % ":".join(sorted(state))))
            if ("active" in said and "active" not in state and node.tag in ("button", "toggle")
                    and canonical[name + suffix + "__active"] is not None):
                body.append("lh_ui_style_set_pressed(lh_addr_of(ui_s_@@), lh_addr_of(ui_s_%s));"
                            % canonical[name + suffix + "__active"])
            # Two rows that look the same share one style: that is what a class is for,
            # and ten buttons of one class are one `lh_ui_style_t`, not ten copies.
            key = tuple(body)
            if not body:
                canonical[sname] = None
            elif key in shared:
                canonical[sname] = shared[key]
            else:
                shared[key] = sname
                canonical[sname] = sname
                body = [line.replace("@@", sname) for line in body]
                styles.append((sname, True, body))
            styled[sname] = canonical[sname] is not None

        # The tint is written into the table, after the colours are; it is named here
        # so that the colour it points at is one of them.
        if node.tag == "image" and "color" in node.props:
            colour_of(tint_of(node, variables))

        if node.layout:
            layouts.append((node,) + tuple(node.layout))
        if node.place:
            places.append((node,) + node.place)

    # --- rows
    rows = []
    for node in nodes:
        x, y, w, h = node.rect
        parent = LH_ROOT if node is root else str(nodes.index(node.parent))
        kind = KINDS[node.tag]
        text = ""
        if node.tag == "label" and "text" in node.props:
            value, line = node.props["text"]
            text = transform_text(read_text(value, line, "`text` of %s" % describe(node)),
                                  node.props.get("text-transform", ("none", 0))[0])
        rows.append((node, kind, x, y, w, h, parent, text))

    out = []
    add = out.append
    add("/**")
    add(" * @file ui_generated.c")
    add(" * @brief The window, written as a stylesheet. Generated. Do not edit.")
    add(" *")
    add(" * Written by `scripts/css2decl.py` of `lh` from `%s`, so that the picture on" % (source or "?"))
    add(" * screen comes from one file of CSS rather than from a thousand lines of calls.")
    add(" * Every colour, paint, style, flow and place below is what some rule said.")
    add(" *")
    add(" * @note `lh_ui_decl_build` still does the measuring: this table says where each row")
    add(" *       starts and what it wants, and the flow that gives a child its place is the")
    add(" *       one its parent carries.")
    add(" */")
    add("")
    add("#include \"ui_generated.h\"")
    add("")
    add("#include <string.h>")
    add("#include <lh/ui/gradient.h>")
    add("#include <lh/ui/insets.h>")
    add("#include <lh/ui/radii.h>")
    add("#include <lh/ui/shadow.h>")
    add("#include <lh/util/addr.h>")
    add("#include <lh/util/ptr.h>")
    add("")
    add("/* -- The theme -------------------------------------------------------- */")
    add("")
    add("/* What `:root` defines, handed to the application by name.")
    add("")
    add("   A stylesheet styles the **widgets**. It cannot style what the application")
    add("   draws itself -- the rows of a table it fills from data, say -- so the colours")
    add("   for those would have nowhere to come from but a second hand-written table of")
    add("   literals in C, and a theme would then be two files: change `--ink` here and")
    add("   that text stays the colour it was. Every `--name` in `:root` is therefore")
    add("   emitted as an `lh_ui_color_t` the application can point a style at, which is")
    add("   the whole of what an `lv_theme_t` is asked for here: a named set of colours the")
    add("   caller swaps by swapping a stylesheet. */")
    add("")
    for name in sorted(variables):
        add("lh_ui_color_t lh_cfg_ui_colour_%s;" % varname(name))
    add("")
    for name in sorted(variables):
        rgb = variables[name]
        r, g, b = rgb[0], rgb[1], rgb[2]
        alpha = rgb[3] if len(rgb) > 3 else 255
        add("/* --%s  #%02X%02X%02X%02X */" % (name, r, g, b, alpha))
        add("static void lh_cfg_ui_colour_%s_init(void)" % varname(name))
        add("{")
        add("    lh_ui_color_init(lh_addr_of(lh_cfg_ui_colour_%s), %d, %d, %d, %d);"
            % (varname(name), r, g, b, alpha))
        add("}")
        add("")
    add("/* -- Colours and paints --------------------------------------------- */")
    add("")
    for key, rgb in sorted(colours.items()):
        add("static lh_ui_color_t %s;" % key)
    add("")
    for key in sorted(colours):
        add("static lh_ui_paint_t %s_paints;" % key)
    add("")
    for key, rgb in sorted(colours.items()):
        r, g, b = rgb[0], rgb[1], rgb[2]
        alpha = rgb[3] if len(rgb) > 3 else 255
        add("/* %s  #%02X%02X%02X%02X */" % (key, r, g, b, alpha))
        add("static void %s_init(void)" % key)
        add("{")
        add("    lh_ui_color_init(lh_addr_of(%s), %d, %d, %d, %d);" % (key, r, g, b, alpha))
        add("    lh_ui_paint_init_color(lh_addr_of(%s_paints), lh_addr_of(%s));" % (key, key))
        add("}")
        add("")
    add("static void ui_colours_init(void)")
    add("{")
    for name in sorted(variables):
        add("    lh_cfg_ui_colour_%s_init();" % varname(name))
    for key in sorted(colours):
        add("    %s_init();" % key)
    add("}")
    add("")
    if gradients_used:
        # A gradient's stops are a fixed table in lh, as many as the build allows; a
        # sheet with more is told so by the compiler rather than by a gradient that
        # stops short.
        add("#if LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS < %d" % max(gradients_used))
        add("#error \"a gradient in the sheet has %d colour stops: build lh with "
            "-DLH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS=%d or more\"" % ((max(gradients_used),) * 2))
        add("#endif")
        add("")
    add("/* -- Styles --------------------------------------------------------- */")
    add("")
    for sname, has_look, body in styles:
        if has_look:
            add("static lh_ui_style_t ui_s_%s;" % sname)
    add("")
    # The per-side padding for the rows that ask for one. A separate object rather than
    # a fourth argument on every setter: it is four numbers, and a row that only has
    # `padx` would otherwise carry two zeroes it never uses.
    for sname, has_look, body in styles:
        if has_look and any("ui_pad_" in line for line in body):
            add("static lh_ui_insets_t ui_pad_%s;" % sname)
    add("")
    for sname, has_look, body in styles:
        if has_look and any("ui_bd_" in line for line in body):
            add("static lh_ui_insets_t ui_bd_%s;" % sname)
    for sname, has_look, body in styles:
        if has_look and any("ui_rd_" in line for line in body):
            add("static lh_ui_radii_t ui_rd_%s;" % sname)
    for sname, has_look, body in styles:
        if has_look and any("ui_gr_" in line for line in body):
            add("static lh_ui_gradient_t ui_gr_%s;" % sname)
            add("static lh_ui_paint_t ui_gp_%s;" % sname)
    add("")
    for sname, has_look, body in styles:
        if has_look and any("ui_shadow_" in line for line in body):
            add("static lh_ui_shadow_t ui_shadow_%s;" % sname)
    add("")
    for sname, has_look, body in styles:
        if not has_look:
            continue
        add("static void ui_style_%s_init(void)" % sname)
        add("{")
        # `lh_ui_style_init` is what puts the **default font** in a style, and the font
        # is where a label looks: `lh_ui_label_get_font` reads it out of the style, not
        # out of the label. A style that is only given a paint and a padding has a null
        # font, and a label with a null font draws nothing at all -- so the window came
        # up with its title bar, its headings and its status line all blank, and the one
        # thing on screen that proved the build worked was the Add button, which is
        # drawn as a fill and needs no font.
        add("    lh_ui_style_init(lh_addr_of(ui_s_%s));" % sname)
        for line in body:
            add("    " + line)
        add("}")
        add("")
    add("static void ui_styles_init(void)")
    add("{")
    for sname, has_look, _body in styles:
        if has_look:
            add("    ui_style_%s_init();" % sname)
    add("}")
    add("")
    add("/* -- Flows ---------------------------------------------------------- */")
    add("")
    for node, axis, gap, justify, wraps, line_gap in layouts:
        add("static lh_ui_layout_t ui_l_%s;" % node.cname())
        add("static void ui_l_%s_init(void)" % node.cname())
        add("{")
        add("    lh_ui_layout_init(lh_addr_of(ui_l_%s), %s, lh_ui_scalar(%d));" % (node.cname(), axis, gap))
        add("    lh_ui_layout_set_justify(lh_addr_of(ui_l_%s), %s);" % (node.cname(), justify))
        if wraps:
            add("    lh_ui_layout_set_wrap(lh_addr_of(ui_l_%s), lh_bool_true, lh_ui_scalar(%d));"
                % (node.cname(), line_gap))
        add("}")
        add("")
    add("static void ui_layouts_init(void)")
    add("{")
    for node, _a, _g, _j, _w, _lg in layouts:
        add("    ui_l_%s_init();" % node.cname())
    add("}")
    add("")
    add("/* -- Places --------------------------------------------------------- */")
    add("")
    for node, mode, size, align, margin, least, most in places:
        name = node.cname()
        if any(margin):
            add("static lh_ui_insets_t ui_mg_%s;" % name)
        add("static lh_ui_place_t ui_pl_%s;" % name)
        add("static void ui_pl_%s_init(void)" % name)
        add("{")
        add("    lh_ui_place_init(lh_addr_of(ui_pl_%s), %s, lh_ui_scalar(%d));" % (name, mode, size))
        add("    lh_ui_place_set_align(lh_addr_of(ui_pl_%s), %s);" % (name, align))
        if any(margin):
            add("    lh_ui_insets_init(lh_addr_of(ui_mg_%s), lh_ui_scalar(%d), lh_ui_scalar(%d), "
                "lh_ui_scalar(%d), lh_ui_scalar(%d));" % ((name,) + tuple(margin)))
            add("    lh_ui_place_set_margin(lh_addr_of(ui_pl_%s), lh_addr_of(ui_mg_%s));"
                % (name, name))
        if any(least):
            add("    lh_ui_place_set_min(lh_addr_of(ui_pl_%s), lh_ui_scalar(%d), lh_ui_scalar(%d));"
                % ((name,) + tuple(least)))
        if any(most):
            add("    lh_ui_place_set_max(lh_addr_of(ui_pl_%s), lh_ui_scalar(%d), lh_ui_scalar(%d));"
                % ((name,) + tuple(most)))
        add("}")
        add("")
    add("static void ui_places_init(void)")
    add("{")
    for place in places:
        add("    ui_pl_%s_init();" % place[0].cname())
    add("}")
    add("")
    add("/* -- The tree ------------------------------------------------------- */")
    add("")
    inputs = [n for n in nodes if n.tag == "input"]
    if inputs:
        add("/* -- Fields ------------------------------------------------------- */")
        add("")
        for node in inputs:
            add("static lh_char_t ui_buf_%s[%d + 1];" % (node.cname(), input_capacity(node)))
        add("")
    add("const lh_ui_decl_t lh_cfg_ui_decls[] = {")
    for node, kind, x, y, w, h, parent, text in rows:
        def style_of(sname):
            return ("lh_addr_of(ui_s_%s)" % canonical[sname]) if styled.get(sname) else "lh_null"

        style = style_of(node.cname())
        place = ("lh_addr_of(ui_pl_%s)" % node.cname()) if node.place else "lh_null"
        layout = ("lh_addr_of(ui_l_%s)" % node.cname()) if node.layout else "lh_null"
        add("    /* %3d  %s%s */" % (node.index, node.cname(), "  (hidden)" if node.hidden else ""))
        union_at = len(out)
        add("    {%s, %d, %d, %d, %d, %s," % (kind, x, y, w, h, parent))
        add("     %s, %s, %s," % (style, place, layout))
        if node.tag == "label":
            add("     {.label = {%s}}}," % (c_string(text) if text else "lh_null"))
        elif node.tag == "scrollbar":
            # The row of the container it drives. It has to be a **lower** index, and
            # the scrollbar has to come after the thing it scrolls for that to be true,
            # which is also why it is a sibling of the rows and not a child of them.
            wanted = prop_str(node, "scrolls")
            target = [n for n in nodes if n.name == wanted.lstrip("#")]
            if not target:
                raise Failure("%s at line %d scrolls `#%s`, which no rule declares. "
                              "Declared ids: %s"
                              % (describe(node), node.line, wanted.lstrip("#"),
                                 ", ".join(n.name for n in nodes if n.name)))
            if target[0].index >= node.index:
                raise Failure("%s at line %d scrolls `#%s`, which is **row %d** -- the "
                              "bar itself is row %d. `lh_ui_decl_build` wants the thing "
                              "scrolled to exist first; put the bar after it"
                              % (describe(node), node.line, wanted.lstrip("#"),
                                 target[0].index, node.index))
            add("     {.scrollbar = {%d, %s, %s}}},"
                % (target[0].index, lookup(AXES, node, "axis"),
                   "lh_ui_scrollbar_mode_always" if node.props.get(
                       "scrollbar-mode", ("auto", 0))[0] == "always"
                   else "lh_ui_scrollbar_mode_auto"))
        elif node.tag == "button":
            # The click is the application's: a callback is C, and a stylesheet that
            # named one would be a second place a function's name has to be spelled.
            # `on_click` stays null here and ::lh_cfg_ui_button is how the app sets it.
            add("     {.button = {%s, lh_null, lh_null}}}," % style_of(node.cname() + "__hover"))
        elif node.tag == "toggle":
            name = node.cname()
            on = name + "__checked" if styled.get(name + "__checked") else name
            if styled.get(name + "__checked_hover"):
                on_hot = name + "__checked_hover"
            elif ":checked" not in node.states:
                on_hot = name + "__hover"
            else:
                on_hot = ""
            add("     {.toggle = {%s, %s, %s, %s, %s}}},"
                % (style_of(name), style_of(name + "__hover"), style_of(on),
                   style_of(on_hot) if on_hot else "lh_null",
                   lookup(VALUE_TABLES["checked"], node, "checked")
                   if "checked" in node.props else "lh_bool_false"))
        elif node.tag == "input":
            # The buffer is the field's to edit, so it is not the table's `const`:
            # one static of `maxlength` and a terminator for every field the sheet has.
            add("     {.input = {ui_buf_%s, %d, %s}}},"
                % (node.cname(), input_capacity(node),
                   c_string(read_text(node.props["value"][0], node.props["value"][1],
                                      "`value` of %s" % describe(node))) if "value" in node.props
                   else "lh_null"))
        elif node.tag == "image":
            # The mask is the application's, the way a font is: a resource it loads.
            # The colour it is shown in is a theme's, so that one is here.
            tint = "lh_null"
            if "color" in node.props:
                tint = "lh_addr_of(%s)" % colour_of(tint_of(node, variables))
            add("     {.image = {lh_null, %s}}}," % tint)
        else:
            # Two braces: the union, and the first struct in it. One reads as "the
            # union is a scalar" to `-Wmissing-braces`, once per row.
            add("     {{0}}},")
        # The union line is the last of the row, and the flag goes after it -- said
        # for every row, so that `-Wextra` has no field of a row to ask about.
        last = len(out) - 1
        assert last >= union_at and out[last].endswith("},")
        out[last] = out[last][:-2] + (", lh_bool_true}," if node.hidden else ", lh_bool_false},")
    add("};")
    add("")
    add("const lh_u32_t lh_cfg_ui_decl_count = %d;" % len(rows))
    add("")
    add("const char *const lh_cfg_ui_names[] = {")
    for node, _k, _x, _y, _w, _h, _p, _t in rows:
        add('    "%s",' % (node.name if node.name else ""))
    add("};")
    add("")
    FONTS_USED.extend(sorted(fonts_used))
    add("lh_bool_t")
    add("lh_cfg_ui_init(void)")
    add("{")
    add("    ui_colours_init();")
    add("    ui_styles_init();")
    add("    ui_layouts_init();")
    add("    ui_places_init();")
    add("    return lh_bool_true;")
    add("}")
    add("")
    return "\n".join(out)


LH_ROOT = "LH_UI_DECL_ROOT"


def index_rows(root):
    counter = 0
    for node in walk(root, []):
        node.index = counter
        counter += 1
    return counter


def varname(name):
    """The C name a `:root` variable is handed to the application under.

    `--accent-dim` is a fine CSS name and `lh_cfg_ui_colour_accent-dim` is not a C one:
    the dash reads as a minus, so the header would not compile at all. One place does the
    translation rather than two, because a header that declares one spelling and a source
    that defines another is a link error at best.
    """
    return name.replace("-", "_")


def header(count, variables, source=""):
    theme = []
    if variables:
        theme.append("/**")
        theme.append(" * @brief The colours `:root` defines, by name -- `--ink` is `lh_cfg_ui_colour_ink`.")
        theme.append(" *")
        theme.append(" * A stylesheet styles the widgets; these are for the parts of the window the")
        theme.append(" * application draws itself, so that one `:root` is the whole theme and a")
        theme.append(" * second set of literals in C is not a second theme. Valid after")
        theme.append(" * ::lh_cfg_ui_init.")
        theme.append(" *")
        theme.append(" * A `--` becomes `_`: `--accent-dim` is `lh_cfg_ui_colour_accent_dim`.")
        theme.append(" */")
        for name in sorted(variables):
            theme.append("extern lh_ui_color_t lh_cfg_ui_colour_%s;" % varname(name))
        theme.append("")
    if FONTS_USED:
        theme.append("/**")
        theme.append(" * @brief The fonts the sheet names, which the **application** defines and")
        theme.append(" *        loads before the first paint: `font: 14px body` is `lh_cfg_ui_font_body_14`.")
        theme.append(" *")
        theme.append(" * A font is baked at one size and weight, so each one the sheet asks for is")
        theme.append(" * its own. One that is not defined is a link error, which is the point.")
        theme.append(" */")
        for name in FONTS_USED:
            theme.append("extern lh_ui_font_t lh_cfg_ui_font_%s;" % name)
        theme.append("")
    return """/**
 * @file ui_generated.h
 * @brief The window as a table. Generated from `""" + (source or "?") + """` by
 *        `scripts/css2decl.py` of `lh`. Do not edit.
 */

#ifndef LH_CFG_UI_GENERATED_H
#define LH_CFG_UI_GENERATED_H

#include <lh/ui/decl.h>

LH_COMPILER_EXTERN_C_BEGIN

""" + "\n".join(theme) + """/** Every row of the tree, in the order `parent` refers to. */
extern const lh_ui_decl_t lh_cfg_ui_decls[];

/** How many rows. */
extern const lh_u32_t lh_cfg_ui_decl_count;

/** The `#id` of each row, in the same order; "" for a row that has none. */
extern const char *const lh_cfg_ui_names[];

/**
 * @brief Fill in the colours, paints, styles, flows and places the table points at.
 *
 * A style, a flow and a place are **not copied** into a widget: the widget keeps the
 * address, so the things they point at have to be alive before the tree is built.
 * Calling this once, before `lh_ui_decl_build`, is that moment.
 */
lh_bool_t
lh_cfg_ui_init(void);

/**
 * @brief Row of the `#id`, or `LH_UI_DECL_ROOT` when there is no such row.
 *
 * A linear walk: the tree is tens of rows and this is called on a click, not on a
 * frame. An `#id` that is not in the stylesheet and a typo of one that is are the same
 * answer here, so check what comes back.
 */
lh_s32_t
lh_cfg_ui_find(const char *id);

/**
 * @brief Row of the `#id`, as one kind, or `lh_null` when it is another.
 *
 * The application asks for a row by name and uses it as the thing it knows it is:
 * `cfg_ui_label(self, "title")` is a `lh_ui_label_t *`. Nothing checks that
 * `#title` really is a label, so the first version of this did a plain cast and a
 * renamed row became a write of a label's fields into a scrollbar's memory -- with
 * no complaint from the compiler, because both are structs that begin with the same
 * entity. **These are the only functions in the program that know what a row is**,
 * so each one compares the kind and answers `lh_null` rather than reinterpreting.
 *
 * @param nodes The entities ::lh_ui_decl_build wrote, one per row, in row order.
 *
 * `lh_cfg_ui_entity` is the one that has **no** kind test, and that is not an oversight:
 * every widget begins with an entity, so every row can be read as one. The others do
 * compare, because reading a scrollbar as a label writes a label's fields into it.
 *
 * A click is set through these: a callback is C, and the stylesheet does not name one.
 */
""" + "\n".join("%s *\nlh_cfg_ui_%s(lh_ui_entity_t **nodes, const char *id);\n" % (ctype, kind)
                for kind, ctype in ACCESSORS) + """
LH_COMPILER_EXTERN_C_END

#endif /* LH_CFG_UI_GENERATED_H */
"""


def find_impl(nodes):
    out = []
    out.append("lh_s32_t")
    out.append("lh_cfg_ui_find(const char *id)")
    out.append("{")
    out.append("    lh_s32_t found = LH_UI_DECL_ROOT;")
    out.append("")
    out.append("    if (id == lh_null)")
    out.append("    {")
    out.append("        return found;")
    out.append("    }")
    out.append("    for (lh_u32_t i = 0; i < lh_cfg_ui_decl_count; ++i)")
    out.append("    {")
    out.append("        if (lh_cfg_ui_names[i][0] != '\\0' && strcmp(lh_cfg_ui_names[i], id) == 0)")
    out.append("        {")
    out.append("            found = (lh_s32_t)i;")
    out.append("            break;")
    out.append("        }")
    out.append("    }")
    out.append("    return found;")
    out.append("}")
    out.append("")
    return "\n".join(out)


#: kind -> C type. The accessors are generated from this, so the cast in the generated
#: code and the kind it checks are written down once.
ACCESSORS = [
    ("entity", "lh_ui_entity_t"),
    ("label", "lh_ui_label_t"),
    ("button", "lh_ui_button_t"),
    ("toggle", "lh_ui_toggle_t"),
    ("container", "lh_ui_container_t"),
    ("image", "lh_ui_image_t"),
    ("scrollbar", "lh_ui_scrollbar_t"),
    ("input", "lh_ui_input_t"),
]


def accessors():
    out = []
    for kind, ctype in ACCESSORS:
        out.append("%s *" % ctype)
        out.append("lh_cfg_ui_%s(lh_ui_entity_t **nodes, const char *id)" % kind)
        out.append("{")
        out.append("    lh_s32_t at = lh_cfg_ui_find(id);")
        out.append("")
        if kind == "entity":
            # **Every** row begins with an entity, so an entity is what every row can be
            # read as and there is nothing here to check. The first version compared the
            # kind like the other five do, and the answer for a container was `lh_null`:
            # the application's layout asked for `#tree`, was told "no such row", and
            # laid the devices out at (-16) pixels wide and 44 rows too high. A title
            # bar came out right anyway, because a zero-height label draws its text in
            # nearly the same place -- so a screenshot did not see it and only a row
            # with a size that mattered did.
            out.append("    if (at == LH_UI_DECL_ROOT)")
        else:
            out.append("    if (at == LH_UI_DECL_ROOT || lh_cfg_ui_decls[at].kind != "
                       "lh_ui_decl_kind_%s)" % kind)
        out.append("    {")
        # `lh_null` is `void *`, and a function returning `lh_ui_label_t *` will not take
        # it without saying so. The cast is written here rather than left to a caller.
        out.append("        return lh_ptr_cast(%s, lh_null);" % ctype)
        out.append("    }")
        out.append("    return lh_ptr_rcast(%s, nodes[at]);" % ctype)
        out.append("}")
        out.append("")
    return "\n".join(out)


# -- Preview -----------------------------------------------------------------

#: The program a preview is: the generated table, built, laid out and painted by the
#: software canvas into memory, and the pixels written out as they are. It is the same
#: code the application runs, so the picture is the application's picture -- the
#: point of a preview is that nothing in it is a second opinion.
PREVIEW_MAIN = r"""
#include "ui_generated.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lh/ui/canvas.h>
#include <lh/ui/canvas/sw.h>
#include <lh/ui/color.h>
#include <lh/ui/font.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/view.h>

@FONTS@

int
main(int argc, char **argv)
{
    static lh_byte_t pool[256 * LH_UI_DECL_NODE_MAX] __attribute__((aligned(16)));
    static lh_ui_entity_t *nodes[256];
    lh_ui_entity_t *root;
    lh_ui_decl_fault_t fault;
    lh_ui_pixmap_t pixmap;
    lh_ui_canvas_sw_t sw;
    lh_ui_canvas_t canvas;
    lh_ui_view_t view;
    lh_ui_color_t clear;
    lh_ui_size_t size;
    int width, height;
    unsigned char *bits;
    FILE *out;

    if (argc != 2 || lh_cfg_ui_decl_count > 256)
    {
        return 2;
    }
@FONT_INIT@
    lh_cfg_ui_init();
    if (!lh_ui_decl_build(lh_cfg_ui_decls, lh_cfg_ui_decl_count, nodes, pool, sizeof pool, &root, &fault))
    {
        fprintf(stderr, "preview: row %u refused, code %d\n", (unsigned)fault.index, (int)fault.code);
        return 1;
    }
    width = (int)lh_cfg_ui_decls[0].w;
    height = (int)lh_cfg_ui_decls[0].h;
    bits = (unsigned char *)calloc((size_t)width * (size_t)height, 4);
    lh_ui_pixmap_init(&pixmap, bits, width, height, width * 4, lh_ui_pixmap_format_argb8888);
    lh_ui_canvas_sw_init(&sw);
    lh_ui_canvas_sw_set_pixmap(&sw, &pixmap);
    lh_ui_canvas_init(&canvas, &lh_ui_canvas_backend_sw, &sw);
    lh_ui_size_init(&size, lh_ui_scalar(width), lh_ui_scalar(height));
    lh_ui_canvas_set_size(&canvas, size);
    lh_ui_color_init(&clear, 255, 255, 255, 255);
    lh_ui_view_init(&view);
    lh_ui_view_set_canvas(&view, &canvas);
    lh_ui_view_set_root(&view, root);
    lh_ui_view_set_clear(&view, &clear);
    lh_ui_view_paint(&view);

    out = fopen(argv[1], "wb");
    if (out == NULL)
    {
        return 1;
    }
    fwrite(&width, sizeof width, 1, out);
    fwrite(&height, sizeof height, 1, out);
    fwrite(bits, 4, (size_t)width * (size_t)height, out);
    fclose(out);
    return 0;
}
"""


def png_bytes(width, height, argb):
    """A PNG of @p argb, little-endian ARGB8888 rows, written with nothing but zlib."""
    rows = bytearray()
    for y in range(height):
        rows.append(0)
        line = argb[y * width * 4:(y + 1) * width * 4]
        # B G R A in memory -> R G B A in the file.
        pixels = bytearray(len(line))
        pixels[0::4] = line[2::4]
        pixels[1::4] = line[1::4]
        pixels[2::4] = line[0::4]
        pixels[3::4] = line[3::4]
        rows += pixels

    def chunk(kind, data):
        return (struct.pack(">I", len(data)) + kind + data
                + struct.pack(">I", zlib.crc32(kind + data) & 0xFFFFFFFF))

    return (b"\x89PNG\r\n\x1a\n"
            + chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0))
            + chunk(b"IDAT", zlib.compress(bytes(rows), 6))
            + chunk(b"IEND", b""))


def preview(outdir, png, build, cc):
    """Compile the generated table with lh, paint it, and write @p png.

    @p build is an lh build directory: the one with `liblh.a` and `include/lh/config.h`
    in it. The fonts the sheet names are given the default font here, so a preview of
    a sheet whose fonts the application has not baked yet still shows its text.
    """
    library = os.path.join(build, "liblh.a")
    if not os.path.isfile(library):
        raise Failure("--build %s has no liblh.a in it: build lh there first" % build)
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    work = tempfile.mkdtemp(prefix="css2decl-preview-")
    try:
        fonts = "\n".join("lh_ui_font_t lh_cfg_ui_font_%s;" % name for name in FONTS_USED)
        font_init = "\n".join("    lh_cfg_ui_font_%s = *lh_ui_font_get_default();" % name
                              for name in FONTS_USED)
        main_c = os.path.join(work, "preview.c")
        with open(main_c, "w", encoding="utf-8", newline="\n") as fh:
            fh.write(PREVIEW_MAIN.replace("@FONTS@", fonts).replace("@FONT_INIT@", font_init)
                     .replace(PREFIX_PLACEHOLDER, PREFIX))
        program = os.path.join(work, "preview.exe" if os.name == "nt" else "preview")
        command = [cc, "-std=gnu11", "-O1", "-I", outdir, "-I", os.path.join(build, "include"),
                   "-I", os.path.join(here, "include"), main_c,
                   os.path.join(outdir, "ui_generated.c"), library, "-lm", "-o", program]
        made = subprocess.run(command, capture_output=True, text=True)
        if made.returncode != 0:
            raise Failure("the preview did not compile:\n%s" % (made.stderr or made.stdout))
        raw = os.path.join(work, "pixels.raw")
        env = dict(os.environ)
        env["PATH"] = os.path.dirname(shutil.which(cc) or cc) + os.pathsep + env.get("PATH", "")
        ran = subprocess.run([program, raw], capture_output=True, text=True, env=env)
        if ran.returncode != 0:
            raise Failure("the preview did not run: %s" % (ran.stderr or ran.returncode))
        with open(raw, "rb") as fh:
            data = fh.read()
        width, height = struct.unpack("<ii", data[:8])
        with open(png, "wb") as fh:
            fh.write(png_bytes(width, height, data[8:]))
        return width, height
    finally:
        shutil.rmtree(work, ignore_errors=True)


def write(path, text):
    with open(path, "w", encoding="utf-8", newline="\n") as fh:
        # Lower case for the names, upper for the include guard: two applications with
        # two prefixes are two headers, and one guard would hide the second.
        fh.write(text.replace(PREFIX_PLACEHOLDER, PREFIX)
                 .replace(PREFIX_PLACEHOLDER.upper(), PREFIX.upper()))


def generate(src, outdir):
    """Read @p src and write the table and its header into @p outdir."""
    with open(src, encoding="utf-8") as fh:
        text = fh.read()

    variables, root = read_sheet(text)
    measure(root, variables)
    count = index_rows(root)
    nodes = walk(root, [])

    # Every `#id` the stylesheet mentions is used exactly once: an id that no rule
    # selects, or one two rules select, is a typo either way and this is where it shows.
    named_ids = [n.name for n in nodes if n.name]
    if len(set(named_ids)) != len(named_ids):
        raise Failure("two rows share an `#id`; see the message from the pass above")

    os.makedirs(outdir, exist_ok=True)
    source = os.path.basename(src)
    body = emit(nodes, variables, root, source) + "\n" + find_impl(nodes) + "\n" + accessors()
    write(os.path.join(outdir, "ui_generated.c"), body)
    write(os.path.join(outdir, "ui_generated.h"), header(count, variables, source))
    return count


def main(argv):
    """``css2decl.py <sheet.css> <output-dir> [--prefix <name>] [--preview <png> --build
    <lh-build-dir> [--cc <compiler>] [--watch]]``

    ``--prefix`` is the C name every generated symbol hangs off -- ``lh_cfg_ui_decls``,
    ``lh_cfg_ui_colour_ink``, ``lh_cfg_ui_init``. **It is an option and not a constant
    because this script belongs to `lh` and not to the application that happens to call it
    first.** Every one of those names is `lh_ui_*` machinery, but the `cfg` in them was
    one application's prefix written into a generator that any application on this library
    would otherwise have to copy. The generated text is written with
    ::PREFIX_PLACEHOLDER and substituted here, once, on the way out -- so a second
    application does not fork the generator, it passes a different word.

    ``--preview`` paints the screen into a PNG with lh's own software canvas, so a sheet
    can be looked at without building the application; ``--watch`` does it again every
    time the sheet is saved, which is the whole of what makes a stylesheet faster to
    write than C: the picture is a save away.
    """
    global PREFIX
    args = list(argv[1:])
    options = {"--prefix": None, "--preview": None, "--build": None, "--cc": None}
    watch = False
    positional = []
    while args:
        arg = args.pop(0)
        if arg == "--watch":
            watch = True
        elif arg in options:
            if not args:
                sys.stderr.write("css: %s needs a value\n" % arg)
                return 2
            options[arg] = args.pop(0)
        else:
            positional.append(arg)
    if len(positional) != 2:
        sys.stderr.write(__doc__)
        return 2
    src, outdir = positional
    if options["--prefix"] is not None:
        PREFIX = options["--prefix"]
        # The prefix ends in `_` and **not** in a letter or a digit: every generated name
        # is this word followed by a name that does not start with one, so `lh_cfg_ui_`
        # gives `lh_cfg_ui_decls` and a prefix without the underscore would give
        # `lh_cfguidecls`. The first version of this check demanded a trailing letter --
        # which is the opposite of the rule -- and the flag's own first use said so.
        if not PREFIX.endswith("_") or len(PREFIX) < 2:
            sys.stderr.write("css: --prefix has to end in '_', got %r\n" % PREFIX)
            return 2
    if (options["--preview"] is None) != (options["--build"] is None):
        sys.stderr.write("css: --preview and --build go together\n")
        return 2
    if watch and options["--preview"] is None:
        sys.stderr.write("css: --watch watches a --preview\n")
        return 2
    cc = options["--cc"] or os.environ.get("CC") or "cc"

    def once():
        count = generate(src, outdir)
        sys.stderr.write("ui: %d rows from %s -> %s\n" % (count, os.path.basename(src), outdir))
        if options["--preview"] is not None:
            width, height = preview(outdir, options["--preview"], options["--build"], cc)
            sys.stderr.write("ui: %dx%d -> %s\n" % (width, height, options["--preview"]))

    if not watch:
        once()
        return 0
    seen = None
    while True:
        try:
            stamp = os.stat(src).st_mtime
        except OSError:
            stamp = None
        if stamp != seen:
            seen = stamp
            try:
                once()
            except Failure as bad:
                # A sheet half-way through an edit is the normal case while watching:
                # say what is wrong and wait for the next save.
                sys.stderr.write("css: %s\n" % bad)
        time.sleep(0.3)


if __name__ == "__main__":
    try:
        sys.exit(main(sys.argv))
    except Failure as bad:
        sys.stderr.write("css: %s\n" % bad)
        sys.exit(1)