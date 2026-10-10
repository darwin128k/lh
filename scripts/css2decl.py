#!/usr/bin/env python3
"""Turn a CSS file into an `lh_ui_decl_t` table.

`include/lh/ui/decl.h` says there is no parser and no markup language, and for a while
that was right: the table was small, and a second language to learn was a second thing
to keep in step with the first. It stops being right at a thousand rows, because the
part that is painful to write by hand is not a call, it is **where a widget sits and
what it looks like** -- and that is exactly what a stylesheet is.

So this reads the subset of CSS that `lh` can actually express, and writes the table.

What that subset is, and why it is not "all of CSS"
---------------------------------------------------
`lh_ui_decl_t` places children by a **flow**: an axis, a gap and a justify, and then
each child answers two questions -- how much room along the flow it wants, and where it
sits across it. That is flexbox. So the language in is flexbox, and:

* `display: flex` on a node makes it a container and gives it a flow;
* a child's **height** is its size when the parent is a column and its size when the
  parent is a row -- the size along the flow, whichever way the parent runs;
* the other dimension is its cross size and comes from its own box;
* `flex: N` asks for a share of what is left instead, which is `lh_ui_place_size_fill`.

What this deliberately does **not** take, and says so out loud rather than dropping:
`margin`, `position`, `top` / `left`, `float`, `z-index`, `display: block | grid`,
`font-size`, `border`, `box-shadow`, `%`, `em`, `vh`. None of them have a place in
`lh_ui_place_t`, and a parser that quietly ignores a property is worse than no parser:
the stylesheet says the row is 8px further down and the row is not.

Fonts are the other gap and it is a deliberate one: a font is a resource the
application loads, not a number a stylesheet carries, so the generated styles leave
`font` alone and the app sets it. `font-size` is therefore rejected rather than
silently absorbed.

Usage
-----
    python3 scripts/css2decl.py <input.css> <output-dir>

Writes `<output-dir>/ui_generated.c` and `<output-dir>/ui_generated.h`, both UTF-8
without a BOM and LF, so they are generated files and are not edited by hand.
"""

import os
import re
import sys

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
    "image": "lh_ui_decl_kind_image",
    "scrollbar": "lh_ui_decl_kind_scrollbar",
}

#: tags that may hold children, because only a container places them.
HOLDERS = {"box", "container", "button"}

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
    "margin": "there is no margin in a flow: `gap` is the space between rows, and a "
              "row's own padding is inside it",
    "float": "a child is a child, at the next place in the flow",
    "z-index": "draw order is the order of the rows",
    "font-size": "a font is a resource the application loads, not a number a "
                 "stylesheet carries; the generated styles leave it to the app",
    "font": "same as `font-size`",
    "border": "there is no border in a style yet",
    "box-shadow": "a shadow is `shadow: <fade> [<down>]`, and its colour is "
                  "`--shadow` in `:root`",
    "overflow": "a container clips its children by itself",
    "opacity": "there is no opacity in a style yet",
    "min-width": "there is no minimum in a place",
    "max-width": "there is no maximum in a place",
    "right": "the application says what is stuck to the right edge, because a "
             "stylesheet draws one screen at one size and a window is not that size "
             "after a drag",
    "bottom": "same as `right`",
}

DISPLAY_VALUES = {"flex"}
DIRECTIONS = {"row": "lh_ui_axis_horizontal", "column": "lh_ui_axis_vertical"}
AXES = {"horizontal": "lh_ui_axis_horizontal", "vertical": "lh_ui_axis_vertical"}
JUSTIFY = {"start": "lh_ui_justify_start", "center": "lh_ui_justify_center", "end": "lh_ui_justify_end"}
ALIGN = {"start": "lh_ui_place_align_start", "center": "lh_ui_place_align_center",
         "end": "lh_ui_place_align_end", "stretch": "lh_ui_place_align_fill"}
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
    "justify-content": JUSTIFY,
    "flex-direction": DIRECTIONS,
    "axis": AXES,
    "position": {"absolute": "absolute"},
}
NUMERIC_PROPS = ("width", "height", "flex", "gap", "padding", "padx", "pady", "radius",
                 "left", "top", "shadow")
COLOUR_PROPS = ("background", "color")

#: `display` is the only thing that gives a container a flow. Everything else about a
#: row's place comes from where the row sits, so a stylesheet that says `gap` without
#: saying `display: flex` is describing a flow that does not exist -- the number would
#: be read by nothing and the rows would sit where the flow put them.
FLOW_PROPS = ("flex-direction", "justify-content", "gap")

KNOWN_PROPS = (set(VALUE_TABLES) | set(NUMERIC_PROPS) | set(COLOUR_PROPS)
               | {"display", "scrolls", "text"})

COLOUR = re.compile(r"^#([0-9a-fA-F]{3}|[0-9a-fA-F]{6}|[0-9a-fA-F]{8})$")
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


def split_top_level(body):
    """Split a block body on `;` that are not inside `( ... )`."""
    parts = []
    depth = 0
    quoted = False
    cur = []
    for ch in body:
        if ch == '"':
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
        self.line = line
        self.props = {}             # property -> (value, line)
        self.children = []
        self.parent = None
        # filled in by measure()
        self.rect = (0, 0, 0, 0)
        self.place = None
        self.layout = None
        self.index = -1

    def cname(self):
        """A C name: the `#id` if there is one, else `tag` plus its line.

        Two un-named siblings of the same tag are two different styles, so the line
        number is part of the name rather than a counter that resets per file.
        """
        if self.name:
            return self.name.replace("-", "_")
        return "%s_line%d" % (self.tag, self.line)


def read_selector(sel, line):
    """`label#tree-head`, `.row`, `box` -> (tag, id)."""
    sel = sel.strip()
    if not sel:
        raise Failure("an empty selector at line %d" % line)
    ident = None
    name = ""
    for m in re.finditer(r"[#.A-Za-z_][A-Za-z0-9_-]*", sel):
        token = m.group(0)
        if token == ".":
            continue
        if sel[m.start()] == "#":
            if name:
                raise Failure("two `#id`s in one selector at line %d: %r" % (line, sel))
            name = token[1:]
            if not IDENT.match(name):
                raise Failure("`#%s` is not a C identifier at line %d" % (name, line))
        else:
            if ident:
                raise Failure("two tag names in one selector at line %d: %r" % (line, sel))
            ident = token
    if ident is None and not name:
        raise Failure("a selector that names neither a tag nor an `#id` at line %d: %r"
                      % (line, sel))
    tag = ident if ident else "label"
    if tag not in KINDS:
        raise Failure("`%s` at line %d is not a widget this parser knows. Known: %s"
                      % (tag, line, ", ".join(sorted(KINDS))))
    return tag, name


# -- Colour and variables ------------------------------------------------------

def read_colour(value, line, variables, where):
    value = value.strip()
    m = re.match(r"^var\(\s*--([A-Za-z0-9_-]+)\s*\)$", value)
    if m:
        key = m.group(1)
        if key not in variables:
            raise Failure("%s at line %d uses var(--%s), which :root never defines. "
                          "Defined: %s" % (where, line, key,
                                           ", ".join(sorted(variables)) or "nothing"))
        return variables[key]
    if not COLOUR.match(value):
        raise Failure("%s at line %d is %r. Only #rgb, #rrggbb and #rrggbbaa, or "
                      "var(--name) from :root -- a named colour, a gradient and a "
                      "computed colour all need something this parser does not have"
                      % (where, line, value))
    digits = value[1:]
    if len(digits) == 3:
        digits = "".join(c * 2 for c in digits)
    red, green, blue = (int(digits[0:2], 16), int(digits[2:4], 16), int(digits[4:6], 16))
    # `#rrggbbaa` is here for the **shadow**, and for nothing else yet. A shadow is a
    # fade to nothing, so its peak colour has to carry an opacity; a solid grey under a
    # soft fade ends in a hard edge, and a hard edge under a card is a border wearing a
    # different hat. Opacity is written as a fourth number rather than a `rgba(...)` so
    # that a colour stays a colour here, the way it is everywhere else in this file.
    alpha = int(digits[6:8], 16) if len(digits) == 8 else 255
    return (red, green, blue, alpha)


def read_number(value, line, where):
    value = value.strip()
    if not NUMBER.match(value):
        raise Failure("%s at line %d is %r. Only whole pixels: there is no %%, no em "
                      "and no vh in a place that takes a scalar"
                      % (where, line, value))
    return int(value)


#: `shadow: <spread> [<offset down>]`. One number or two, never three: the engine has a
#: colour, a fade distance and a shift on each axis, and a stylesheet that could spell
#: all four would have a way to write a shadow with no spread and one with a spread and
#: no fade, which are the same thing drawn twice.
SHADOW = re.compile(r"^(\d+)(?:\s+(\d+))?$")


def read_shadow(value, line, where):
    """(spread, offset down) from `shadow: 18` or `shadow: 18 4`, or a refusal."""
    found = SHADOW.match(value.strip())
    if not found:
        raise Failure("%s at line %d is %r. A shadow is a fade distance and how far "
                      "down it is cast: `shadow: 18` or `shadow: 18 4`. Its colour is "
                      "`--shadow` in `:root`, and not a fourth number here"
                      % (where, line, value.strip()))
    return int(found.group(1)), int(found.group(2) or 0)


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

    variables = {}
    pos = 0
    root = None

    while True:
        open_at = text.find("{", pos)
        if open_at < 0:
            break
        close_at = match_brace(text, open_at, len(text))
        header = text[pos:open_at].strip()
        line = text.count("\n", 0, open_at) + 1
        if not header:
            raise Failure("a block with no selector at line %d" % line)

        if header == ":root":
            variables.update(read_variables(text[open_at + 1:close_at], line))
            pos = close_at + 1
            continue

        node = read_block(text, open_at, close_at, line, variables, header_cut=pos - 1)
        if root is None:
            root = node
        else:
            raise Failure("a second top-level rule at line %d (`%s`). One stylesheet is "
                          "one screen: the tree has exactly one root, and a second one "
                          "is a screen nobody asked for" % (line, header))
        pos = close_at + 1

    if root is None:
        raise Failure("the stylesheet has no rule in it")

    # Whatever is left after the last `}` is a `}` that closes nothing, or a stray word.
    # The first version of this walked off the end of the sheet and called it a good one:
    # a half-finished rule at the bottom of the file -- a `}` typed on its own, a name
    # with no block -- produced exactly the same screen as a sheet that ended cleanly.
    tail = text[pos:].strip()
    if tail:
        at = text.count("\n", 0, pos + (len(text[pos:]) - len(text[pos:].lstrip()))) + 1
        raise Failure("a `%s` at line %d closes nothing: the sheet ends after the last "
                      "block and this text is left over. `match_brace` counts what it "
                      "opened, so a brace with no `{` in front of it is a typo, not a "
                      "screen"
                      % (tail[0], at))
    return variables, root


def read_variables(body, line):
    out = {}
    for part in split_top_level(body):
        decl = part.strip()
        if not decl:
            continue
        if ":" not in decl:
            raise Failure("`%s` at line %d is not a property" % (decl, line))
        key, _, val = decl.partition(":")
        key = key.strip()
        if not key.startswith("--"):
            raise Failure("`:root` holds variables and %r at line %d is not one"
                          % (key, line))
        out[key[2:]] = read_colour(val, line, {}, "a variable")
    return out


def read_block(text, open_at, close_at, line, variables, header_cut):
    """One `{ ... }`: its own declarations and its children.

    @p header_cut is where this block's **selector** starts: just after the `}` or `;`
    that ended whatever came before it. It is passed in because the caller already
    found it, and finding it twice is how the two copies drift apart.
    """
    header = text[header_cut + 1:open_at].strip()
    tag, name = read_selector(header, line)
    node = Node(tag, name, line)

    body_start = open_at + 1
    i = body_start
    chunk_start = body_start
    while i < close_at:
        if text[i] == "{":
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
            cut = max(text.rfind("}", search_from, i), text.rfind(";", search_from, i))
            read_declarations(node, text[chunk_start:cut + 1] if cut >= chunk_start else "",
                              line, variables)
            inner_close = match_brace(text, i, close_at)
            inner_line = text.count("\n", 0, i) + 1
            inner_header = text[cut + 1:i].strip()
            if inner_header == ":root":
                raise Failure("`:root` nested inside a rule at line %d" % inner_line)
            child = read_block(text, i, inner_close, inner_line, variables, cut)
            child.parent = node
            node.children.append(child)
            i = inner_close + 1
            chunk_start = i
            continue
        i += 1
    read_declarations(node, text[chunk_start:close_at], line, variables)
    return node


def read_declarations(node, text, line, variables):
    """Every `a: b;` in @p text, into @p node, refusing anything unknown."""
    if not text.strip():
        return
    for part in split_top_level(text):
        decl = part.strip()
        if not decl:
            continue
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
        node.props[key] = (val.strip(), line)


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
    _measure(root, 0, 0, ids, parent_axis=None)
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


def check_values(root):
    """Every value this parser reads, read now -- not when the file is written.

    `lookup` is the one place a named value is turned into a constant, and it only
    ran from `emit`. `emit` runs when a file is written, so a stylesheet nobody had
    written yet was never checked for `align: middel`, and the refusal arrived at the
    one moment it could no longer be acted on.
    """
    for node in all_nodes(root):
        for key, (value, line) in node.props.items():
            if key in VALUE_TABLES:
                lookup(VALUE_TABLES[key], node, key)
            elif key in NUMERIC_PROPS:
                if key == "shadow":
                    read_shadow(value, line, "`shadow` of %s" % describe(node))
                else:
                    read_number(value, line, "`%s` of %s" % (key, describe(node)))


def check_colours(root):
    for node in all_nodes(root):
        # A row that casts a shadow is asking for `--shadow` without saying so, so the
        # check is here rather than in the emitter: the message has to arrive while the
        # stylesheet is being read, and by the time a file is written the person has
        # walked away from it.
        if "shadow" in node.props and "shadow" not in root.variables:
            raise Failure("`shadow` on %s needs `--shadow` in `:root`: a shadow's "
                          "colour is a theme's to change, and a theme is a set of "
                          "variables" % describe(node))
        for key in ("background", "color"):
            if key not in node.props:
                continue
            value, line = node.props[key]
            read_colour(value, line, root.variables, "`%s` of %s" % (key, describe(node)))


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
            raise Failure("`display: %s` at line %d on %s. Only `flex` places children "
                          "here, and a row that is not on screen is the application's "
                          "business, not the stylesheet's"
                          % (value, line, describe(node)))
        if node.tag not in HOLDERS:
            raise Failure("`display: flex` at line %d on %s, and a `%s` is not a "
                          "container: the flow would have nowhere to belong"
                          % (line, describe(node), node.tag))
    for child in node.children:
        check_display(child)


def _measure(node, x, y, ids, parent_axis):
    if node.name:
        if node.name in ids:
            raise Failure("`#%s` appears twice at line %d. Two rows with one name is a "
                          "tree the app cannot ask a question of"
                          % (node.name, node.line))
        ids.add(node.name)

    w = prop(node, "width")
    h = prop(node, "height")
    if w is None:
        raise Failure("`%s` at line %d has no `width`. Every box here states its cross "
                      "size outright: `flex` answers the size **along** the flow and "
                      "says nothing about the other one"
                      % (describe(node), node.line))
    if h is None:
        raise Failure("`%s` at line %d has no `height`. See `width`: the cross size is "
                      "the box's own" % (describe(node), node.line))

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
    for key in FLOW_PROPS:
        if key in node.props and "display" not in node.props:
            raise Failure("`%s` at line %d in %s, and %s says nothing about a flow. "
                          "Either write `display: flex` on %s or drop the property: a "
                          "flow property on a container that has no flow is read by "
                          "nothing"
                          % (key, node.props[key][1], describe(node), describe(node),
                             describe(node)))
    flows = "display" in node.props

    axis = lookup(DIRECTIONS, node, "flex-direction") if "flex-direction" in node.props else "lh_ui_axis_horizontal"
    gap = number_of(node, "gap", 0)

    cursor = 0
    for child in children:
        placed = placed_absolute(child)
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
            _measure(child, x + number_of(child, "left", 0), y + number_of(child, "top", 0),
                     ids, parent_axis=None)
            continue

        share = prop(child, "flex")
        if share is None:
            # The size **along** the flow. A column runs down, so what a child asks
            # for down there is its height; a row runs across, so it is its width. The
            # first version had these the other way round and a column of full-width
            # rows advanced the cursor by 980 each time and put the scroll bar at
            # (1028, 2672).
            along = prop(child, "height" if axis == "lh_ui_axis_vertical" else "width")
            size_mode, size = "fixed", along
        else:
            size_mode, size = "fill", read_number(share, child.props["flex"][1], "`flex`")
        align = lookup(ALIGN, child, "align") if "align" in child.props else "lh_ui_place_align_fill"

        child.place = (SIZE_MODE[size_mode], size, align)

        if axis == "lh_ui_axis_vertical":
            cx, cy = x, y + cursor
        else:
            cx, cy = x + cursor, y
        _measure(child, cx, cy, ids, parent_axis=axis)
        cursor += (size if size_mode == "fixed" else 0) + gap

    # A flow belongs to `display: flex`, **not** to having children. `#rows` is a
    # container the stylesheet gives a flow and no children, because the children are
    # the 1408 rows of the parameter table and those are data the application fills;
    # tying the flow to the stylesheet's own children left that one container with no
    # flow at all, and 1408 rows with a place and nowhere to be placed by.
    node.layout = (axis, gap, lookup(JUSTIFY, node, "justify-content") if "justify-content" in node.props else "lh_ui_justify_start") \
        if flows else None


def placed_absolute(node):
    """Does this row say where it is, rather than letting the flow do it?

    Both halves are required and neither is optional: `position: absolute` with no
    `left` would be a row at (0, 0), which is a real place and never the meant one, and
    `left` with no `position` is a number nothing reads.
    """
    has_position = "position" in node.props
    has_where = "left" in node.props or "top" in node.props
    if not has_position and not has_where:
        return False
    lookup(VALUE_TABLES["position"], node, "position")
    for key in ("left", "top"):
        if key not in node.props:
            raise Failure("`position: absolute` at line %d in %s names no `%s`. A row "
                          "placed by hand is placed by both of its coordinates, and "
                          "half of them is the top left corner of the window"
                          % (node.props["position"][1], describe(node), key))
    return True


def prop(node, key):
    if key not in node.props:
        return None
    value, line = node.props[key]
    return read_number(value, line, "`%s`" % key)


def number_of(node, key, default):
    if key not in node.props:
        return default
    value, line = node.props[key]
    return read_number(value, line, "`%s`" % key)


def prop_str(node, key, default=""):
    return node.props.get(key, (default, node.line))[0].strip()


def lookup(table, node, key):
    """One of the named values of @p key, or a message that says which are legal.

    A bare `table[value]` answers a typo with a Python traceback and a line number in
    this file, which is the least useful thing a stylesheet author can be handed.
    """
    value = prop_str(node, key)
    if value not in table:
        raise Failure("`%s: %s` at line %d in %s. Legal here: %s"
                      % (key, value, node.props[key][1], describe(node),
                         ", ".join(sorted(table))))
    return table[value]


def describe(node):
    """`tag#id`, and nothing else.

    The line number is deliberately **not** in here. Every caller already has it and
    prints it, and the two together made every message say `at line 8` twice --
    which reads like two different problems and is the kind of thing that makes a
    tool's output worth ignoring.
    """
    return "`%s`" % (node.tag + ("#" + node.name if node.name else ""))


# -- Writing -------------------------------------------------------------------

def walk(node, out):
    out.append(node)
    for child in node.children:
        walk(child, out)
    return out


def emit(nodes, variables, root):
    colours = {}
    styles = []
    layouts = []
    places = []

    def colour_of(rgb):
        key = "ui_c_%02x%02x%02x%02x" % (rgb[0], rgb[1], rgb[2], rgb[3] if len(rgb) > 3 else 255)
        if key not in colours:
            colours[key] = rgb
        return key

    for node in nodes:
        # --- look
        has_look = False
        body = []
        if "background" in node.props:
            value, line = node.props["background"]
            rgb = read_colour(value, line, variables, "`background` of %s" % describe(node))
            body.append("lh_ui_style_set_fill(lh_addr_of(ui_s_%s), lh_addr_of(%s_paints));"
                        % (node.cname(), colour_of(rgb)))
            has_look = True
        if "color" in node.props:
            value, line = node.props["color"]
            rgb = read_colour(value, line, variables, "`color` of %s" % describe(node))
            body.append("lh_ui_style_set_text(lh_addr_of(ui_s_%s), lh_addr_of(%s_paints));"
                        % (node.cname(), colour_of(rgb)))
            has_look = True
        pad = number_of(node, "padding", None) if "padding" in node.props else None
        if pad is not None:
            body.append("lh_ui_style_set_padding(lh_addr_of(ui_s_%s), lh_ui_scalar(%d));"
                        % (node.cname(), pad))
            has_look = True
        # `padx` and `pady` are the two halves of `padding`, and they exist because
        # **`padding` on all four sides cannot centre anything**: a label's vertical
        # alignment happens inside the box its padding leaves, so a 28-row row with 14
        # of padding has a content box **zero rows high**, and the middle of nothing is
        # its top edge -- the caption then sits at row 14 of 28, which is six rows below
        # the middle and reads as "the text is crooked". Measured on the Add device
        # button: ink rows 14..25 of a box 0..28.
        # The engine has had per-side insets all along (::lh_ui_style_set_padding_insets);
        # the stylesheet simply had no way to spell them.
        padx = number_of(node, "padx", None) if "padx" in node.props else None
        pady = number_of(node, "pady", None) if "pady" in node.props else None
        if padx is not None or pady is not None:
            left = padx if padx is not None else 0
            right = padx if padx is not None else 0
            top = pady if pady is not None else 0
            bottom = pady if pady is not None else 0
            body.append("lh_ui_insets_init(lh_addr_of(ui_pad_%s), lh_ui_scalar(%d), "
                        "lh_ui_scalar(%d), lh_ui_scalar(%d), lh_ui_scalar(%d));"
                        % (node.cname(), left, top, right, bottom))
            body.append("lh_ui_style_set_padding_insets(lh_addr_of(ui_s_%s), "
                        "lh_addr_of(ui_pad_%s));" % (node.cname(), node.cname()))
            has_look = True
        if "radius" in node.props:
            body.append("lh_ui_style_set_radius(lh_addr_of(ui_s_%s), lh_ui_scalar(%d));"
                        % (node.cname(), number_of(node, "radius", 0)))
            has_look = True
        # `shadow` is the fade distance and how far down it is cast. Its **colour** is
        # `--shadow` in `:root`, because a colour is a thing a theme changes and a
        # distance is a thing a layout changes: one variable the reader can replace to
        # restyle every island at once, and a number per row that stays put when the
        # palette does not.
        if "shadow" in node.props:
            spread, drop = read_shadow(node.props["shadow"][0], node.props["shadow"][1],
                                       "`shadow` of %s" % describe(node))
            body.append("lh_ui_shadow_init(lh_addr_of(ui_shadow_%s));"
                        % node.cname())
            body.append("lh_ui_shadow_set_color(lh_addr_of(ui_shadow_%s), %s);"
                        % (node.cname(), colour_of(root.variables["shadow"])))
            body.append("lh_ui_shadow_set_spread(lh_addr_of(ui_shadow_%s), lh_ui_scalar(%d));"
                        % (node.cname(), spread))
            body.append("lh_ui_shadow_set_offset(lh_addr_of(ui_shadow_%s), lh_ui_scalar(0), "
                        "lh_ui_scalar(%d));" % (node.cname(), drop))
            body.append("lh_ui_style_set_shadow(lh_addr_of(ui_s_%s), "
                        "lh_addr_of(ui_shadow_%s));" % (node.cname(), node.cname()))
            has_look = True
        if "text-align" in node.props:
            body.append("lh_ui_style_set_align_h(lh_addr_of(ui_s_%s), %s);"
                        % (node.cname(), lookup(TEXT_ALIGN, node, "text-align")))
            has_look = True
        if "valign" in node.props:
            body.append("lh_ui_style_set_align_v(lh_addr_of(ui_s_%s), %s);"
                        % (node.cname(), lookup(VALIGN, node, "valign")))
            has_look = True
        styles.append((node, has_look, body))

        if node.layout:
            axis, gap, justify = node.layout
            layouts.append((node, axis, gap, justify))
        if node.place:
            places.append((node,) + node.place)

    # --- rows
    rows = []
    for node in nodes:
        x, y, w, h = node.rect
        parent = LH_ROOT if node is root else str(nodes.index(node.parent))
        kind = KINDS[node.tag]
        text = ""
        if node.tag == "label":
            value = prop_str(node, "text", "").strip()
            if value.startswith('"') and value.endswith('"') and len(value) >= 2:
                value = value[1:-1]
            text = value
        rows.append((node, kind, x, y, w, h, parent, text))

    out = []
    add = out.append
    add("/**")
    add(" * @file ui_generated.c")
    add(" * @brief The window, written as a stylesheet. Generated. Do not edit.")
    add(" *")
    add(" * Written by `lib/lh/scripts/css2decl.py` from the stylesheet named below, so that the")
    add(" * picture on screen comes from one file of CSS rather than from a thousand lines of")
    add(" * calls. Every colour, paint, style, flow and place below is what some rule said.")
    add(" *")
    add(" * @note `lh_ui_decl_build` still does the measuring: this table says where each row")
    add(" *       starts and what it wants, and the flow that gives a child its place is the")
    add(" *       one its parent carries.")
    add(" */")
    add("")
    add("#include \"ui_generated.h\"")
    add("")
    add("#include <string.h>")
    add("#include <lh/ui/insets.h>")
    add("#include <lh/ui/shadow.h>")
    add("#include <lh/util/addr.h>")
    add("")
    add("/* -- The theme -------------------------------------------------------- */")
    add("")
    add("/* What `:root` defines, handed to the application by name.")
    add("")
    add("   A stylesheet styles the **widgets**. It cannot style the rows of a table the")
    add("   application draws itself -- here that is 1408 lines of parameter and value -- so")
    add("   the colours for those had nowhere to come from but a second hand-written table")
    add("   of literals in C, and a theme was then two files: change `--ink` here and the")
    add("   tree's text stayed the colour it was. Every `--name` in `:root` is therefore")
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
    add("/* -- Styles --------------------------------------------------------- */")
    add("")
    for node, has_look, body in styles:
        if has_look:
            add("static lh_ui_style_t ui_s_%s;" % node.cname())
    add("")
    # The per-side padding for the rows that ask for one. A separate object rather than
    # a fourth argument on every setter: it is four numbers, and a row that only has
    # `padx` would otherwise carry two zeroes it never uses.
    for node, has_look, body in styles:
        if has_look and any("ui_pad_" in line for line in body):
            add("static lh_ui_insets_t ui_pad_%s;" % node.cname())
    add("")
    for node, has_look, body in styles:
        if has_look and any("ui_shadow_" in line for line in body):
            add("static lh_ui_shadow_t ui_shadow_%s;" % node.cname())
    add("")
    for node, has_look, body in styles:
        if not has_look:
            continue
        add("static void ui_style_%s_init(void)" % node.cname())
        add("{")
        # `lh_ui_style_init` is what puts the **default font** in a style, and the font
        # is where a label looks: `lh_ui_label_get_font` reads it out of the style, not
        # out of the label. A style that is only given a paint and a padding has a null
        # font, and a label with a null font draws nothing at all -- so the window came
        # up with its title bar, its headings and its status line all blank, and the one
        # thing on screen that proved the build worked was the Add button, which is
        # drawn as a fill and needs no font.
        add("    lh_ui_style_init(lh_addr_of(ui_s_%s));" % node.cname())
        for line in body:
            add("    " + line)
        add("}")
        add("")
    add("static void ui_styles_init(void)")
    add("{")
    for node, has_look, _body in styles:
        if has_look:
            add("    ui_style_%s_init();" % node.cname())
    add("}")
    add("")
    add("/* -- Flows ---------------------------------------------------------- */")
    add("")
    for node, axis, gap, justify in layouts:
        add("static lh_ui_layout_t ui_l_%s;" % node.cname())
        add("static void ui_l_%s_init(void)" % node.cname())
        add("{")
        add("    lh_ui_layout_init(lh_addr_of(ui_l_%s), %s, lh_ui_scalar(%d));" % (node.cname(), axis, gap))
        add("    lh_ui_layout_set_justify(lh_addr_of(ui_l_%s), %s);" % (node.cname(), justify))
        add("}")
        add("")
    add("static void ui_layouts_init(void)")
    add("{")
    for node, _a, _g, _j in layouts:
        add("    ui_l_%s_init();" % node.cname())
    add("}")
    add("")
    add("/* -- Places --------------------------------------------------------- */")
    add("")
    for node, mode, size, align in places:
        add("static lh_ui_place_t ui_pl_%s;" % node.cname())
        add("static void ui_pl_%s_init(void)" % node.cname())
        add("{")
        add("    lh_ui_place_init(lh_addr_of(ui_pl_%s), %s, lh_ui_scalar(%d));" % (node.cname(), mode, size))
        add("    lh_ui_place_set_align(lh_addr_of(ui_pl_%s), %s);" % (node.cname(), align))
        add("}")
        add("")
    add("static void ui_places_init(void)")
    add("{")
    for node, _m, _s, _a in places:
        add("    ui_pl_%s_init();" % node.cname())
    add("}")
    add("")
    add("/* -- The tree ------------------------------------------------------- */")
    add("")
    add("const lh_ui_decl_t lh_cfg_ui_decls[] = {")
    for node, kind, x, y, w, h, parent, text in rows:
        style = ("lh_addr_of(ui_s_%s)" % node.cname()) if any(
            n is node and ok for n, ok, _b in styles) else "lh_null"
        place = ("lh_addr_of(ui_pl_%s)" % node.cname()) if node.place else "lh_null"
        layout = ("lh_addr_of(ui_l_%s)" % node.cname()) if node.layout else "lh_null"
        add("    /* %3d  %s */" % (node.index, node.cname()))
        add("    {%s, %d, %d, %d, %d, %s," % (kind, x, y, w, h, parent))
        add("     %s, %s, %s," % (style, place, layout))
        if node.tag == "label":
            add("     {.label = {%s}}}," % (('"%s"' % text) if text else "lh_null"))
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
            add("     {.scrollbar = {%d, %s, lh_ui_scrollbar_mode_auto}}},"
                % (target[0].index, lookup(AXES, node, "axis")))
        else:
            add("     {0}},")
    add("};")
    add("")
    add("const lh_u32_t lh_cfg_ui_decl_count = %d;" % len(rows))
    add("")
    add("const char *const lh_cfg_ui_names[] = {")
    for node, _k, _x, _y, _w, _h, _p, _t in rows:
        add('    "%s",' % (node.name if node.name else ""))
    add("};")
    add("")
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


def header(count, variables):
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
    return """/**
 * @file ui_generated.h
 * @brief The window as a table. Generated from CSS by `lib/lh/scripts/css2decl.py`.
 */

#ifndef CFG_UI_GENERATED_H
#define CFG_UI_GENERATED_H

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
 * answer here, which is why `css2decl.py` refuses a `#id` nothing selects.
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
 * every widget begins with an entity, so every row can be read as one. The other five
 * do compare, because reading a scrollbar as a label writes a label's fields into it.
 */
lh_ui_entity_t *
lh_cfg_ui_entity(lh_ui_entity_t **nodes, const char *id);

lh_ui_label_t *
lh_cfg_ui_label(lh_ui_entity_t **nodes, const char *id);

lh_ui_button_t *
lh_cfg_ui_button(lh_ui_entity_t **nodes, const char *id);

lh_ui_container_t *
lh_cfg_ui_container(lh_ui_entity_t **nodes, const char *id);

lh_ui_image_t *
lh_cfg_ui_image(lh_ui_entity_t **nodes, const char *id);

lh_ui_scrollbar_t *
lh_cfg_ui_scrollbar(lh_ui_entity_t **nodes, const char *id);

LH_COMPILER_EXTERN_C_END

#endif /* CFG_UI_GENERATED_H */
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
    ("container", "lh_ui_container_t"),
    ("image", "lh_ui_image_t"),
    ("scrollbar", "lh_ui_scrollbar_t"),
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
        out.append("        return (%s *)lh_null;" % ctype)
        out.append("    }")
        out.append("    return (%s *)nodes[at];" % ctype)
        out.append("}")
        out.append("")
    return "\n".join(out)


def write(path, text):
    with open(path, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(text.replace(PREFIX_PLACEHOLDER, PREFIX))


def main(argv):
    """``css2decl.py <sheet.css> <output-dir> [--prefix <name>]``

    ``--prefix`` is the C name every generated symbol hangs off -- ``lh_cfg_ui_decls``,
    ``lh_cfg_ui_colour_ink``, ``lh_cfg_ui_init``. **It is an option and not a constant
    because this script belongs to `lh` and not to the application that happens to call it
    first.** Every one of those names is `lh_ui_*` machinery, but the `cfg` in them was
    one application's prefix written into a generator that any application on this library
    would otherwise have to copy. The generated text is written with
    ::PREFIX_PLACEHOLDER and substituted here, once, on the way out -- so a second
    application does not fork the generator, it passes a different word.
    """
    if len(argv) not in (3, 5) or (len(argv) == 5 and argv[3] != "--prefix"):
        sys.stderr.write(__doc__)
        return 2
    src, outdir = argv[1], argv[2]
    if len(argv) == 5:
        global PREFIX
        PREFIX = argv[4]
        # The prefix ends in `_` and **not** in a letter or a digit: every generated name
        # is this word followed by a name that does not start with one, so `lh_cfg_ui_`
        # gives `lh_cfg_ui_decls` and a prefix without the underscore would give
        # `lh_cfguidecls`. The first version of this check demanded a trailing letter --
        # which is the opposite of the rule -- and the flag's own first use said so.
        if not PREFIX.endswith("_") or len(PREFIX) < 2:
            sys.stderr.write("css: --prefix has to end in '_', got %r\n" % PREFIX)
            return 2
    with open(src, encoding="utf-8") as fh:
        text = fh.read()

    variables, root = read_sheet(text)
    measure(root, variables)
    count = index_rows(root)
    nodes = walk(root, [])

    # Every `#id` the stylesheet mentions is used exactly once: an id that no rule
    # selects, or one two rules select, is a typo either way and this is where it shows.
    named = [n.name for n in nodes if n.name]
    if len(set(named)) != len(named):
        raise Failure("two rows share an `#id`; see the message from the pass above")

    os.makedirs(outdir, exist_ok=True)
    body = emit(nodes, variables, root) + "\n" + find_impl(nodes) + "\n" + accessors()
    write(os.path.join(outdir, "ui_generated.c"), body)
    write(os.path.join(outdir, "ui_generated.h"), header(count, variables))

    sys.stderr.write("ui: %d rows from %s -> %s\n" % (count, os.path.basename(src), outdir))
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main(sys.argv))
    except Failure as bad:
        sys.stderr.write("css: %s\n" % bad)
        sys.exit(1)