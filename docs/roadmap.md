# LH roadmap

What the owner wants next, and in what order. Every item says what exists
today, what the target is, and what it costs. A rule that changes how the code
is written goes into `AGENTS.md`; a debt goes into the parent project's
`docs/cleanup.md`. This file is the target, not the debt list.

The order below is the owner's: the web host first, the controller after it.

## 1. The web host: nothing baked in

**Today.** The transport accepts an application supplied by its caller. The
Canvas2D player is a separate application: it renders the same LH view as a
desktop backend and sends drawing commands over WebSocket. It owns its page and
input endpoints. Applications attach their own route tables; the host does
not install a REST API. The player page is built as an XML tree written in
the HTML dialect and a JavaScript tree (`lh/core/web/xml/`, `lh/core/web/js/node.h`) and
written by their writers. The host parses request heads and Content-Length or
chunked bodies with the HTTP parser and framing bricks; response and WebSocket headers
are serialized from HTTP objects. Its generic response sender accepts caller-
supplied fields, while the player reply helper supplies its usual fields.
Streaming routes read chunked and gzip bodies incrementally with separate
encoded-body, decoded-body, metadata and request-time limits. Buffered routes
still require the complete wire request and decoded content to fit scratch.

**Target.** Build application UI once from LH entities and use it with desktop
and browser backends. Construct HTML and JavaScript from public objects and
serialize them at the browser boundary. The host composes the shared HTTP
parser, request and response objects and serializers; known methods and status
codes have named values while extension methods and headers remain possible.
REST is a separate facility attached by the application when wanted.

**Work.** Extend HTTP codings beyond gzip and chunked as needed, and extend the
HTML and JavaScript trees as needed. Exercise the same
view and input on desktop and web; test each public brick and their composition.
A JavaScript interpreter or bytecode VM is a separate future feature.

## 2. XML and HTML as nodes

**Today.** `lh_web_xml_entity_t` is the data of a tree node; the node API
operates on that same object to own and manage its attributes and children.
The entity and node are available to XML and HTML alike. One escaper and one
writer serve both, with the dialect as a flag. `lh_web_xml_reader_t` reads XML;
`lh/core/web/html/` adds HTML rules and builds on the XML reader's steps.

**Target.** Keep the shared tree and its operations as the common bricks, with
HTML-specific parsing and serialization rules above them. HTML syntax has its
own parsing and tree-construction rules; it is not XML with only different tag
spelling. The current HTML reader handles the documented well-formed subset
used by LH, not the browser's full HTML parsing algorithm.

The other side of the same idea: a C object serialized to and from text. JSON
already is (`lh/core/web/json/`); XML is the other form, and it belongs to the same
layer.

**Work.** Extend the HTML-specific rules as concrete uses require, keeping the
shared tree and low-level operations in use. Full browser-compatible parsing,
if needed, is a separate scope.

## 3. A JavaScript engine of our own

**Today.** The one script lh ships, the web player's, is built as a tree
(`lh_web_js_node_t`) and written by `lh_web_js_writer_t`; lh has a JavaScript
lexer but no parser and no interpreter.

**The decision.** The engine is **lh's own code**, on lh's bricks: the lexer,
the parser, the tree, the interpreter. Nothing foreign is vendored into the
tree — the same rule the rest of lh follows (SQLite is the exception because
it is a database, not a language we are building; Monocypher's Ed25519 has been
ported into lh's own bricks). The QuickJS implementation (MIT) is the
**reference for the algorithms** — the way to read a number, scan a string
literal, parse expressions, and compile a function — not a dependency and not
a library we link.

**Architecture to reconcile.** QuickJS parses source directly into bytecode;
it does not construct the general syntax tree proposed in the slices below.
The tree is therefore an LH design choice, not a QuickJS structure already
mapped to LH. A faithful port must account for QuickJS's bytecode emitter,
function and scope state, and virtual machine; the tree-based slices below
remain a proposal until those pieces are mapped to LH bricks.

**Why the port is a port.** The engine is written after QuickJS's, so the
hard parts (the number scanner, the string escapes, the operator precedence,
the garbage collector's roots) have a working answer to look at. What changes
is everything around them: lh's strings, lh's memory kernels, lh's arrays and
hashes, lh's error handling, lh's naming, lh's layering, and a test per brick.

**The slices.** Each is a checkpoint of its own:

1. **The lexer** — the tokens of a script: names, numbers, strings, the
   punctuation, the comments, the line and the column of an error.
2. **The tree** — the nodes a script is: the expressions, the statements, the
   functions, the scopes.
3. **The parser** — the text into the tree.
4. **The values** — what a name holds: a number, a string, a boolean, null,
   undefined, an object, a function.
5. **The interpreter** — the tree running.
6. **The runtime** — the objects, the prototypes, the garbage collector.

**Cost.** The whole engine is weeks, not days; the slices are days. The first
slice (the lexer) is the foundation of all the others and stands on its own:
a script can be tokenized, and the tokens are worth having.

**What it is for.** The media player and the web page's script stop being
C strings: they become scripts the engine runs. What crosses the boundary
between the script and the application are lh values — strings, numbers,
booleans — and not text.

## 4. The gamepad driver

**Today.** Mouse, keyboard, touch and pen have real backends. The gamepad does
not: `lh_os_input_kind_controller_button_down`, `_controller_axis` and the
device kind `controller` exist, and no backend produces them — there is no
XInput on Windows, no evdev on Linux, no device enumeration and no hot-plug.

**Target.** The same shape as the touch driver: a Win32 backend that loads
XInput dynamically, a Linux backend that reads evdev, and the device events
the kinds already promise.

**Cost.** About two days: XInput through `GetProcAddress` (about 300 lines and
a test against a fake backend), evdev (about 200 lines, and it brings the
device events with it).

## Not a game layer

The gamepad belongs to the applications that use the desktop stack — the 3D
editor with its DLL modules. The Authum game modules are server side, so the
input never crosses the Authum facade: there is no `authum_input_*` to write.
