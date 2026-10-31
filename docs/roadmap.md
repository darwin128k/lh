# LH roadmap

What the owner wants next, and in what order. Every item says what exists
today, what the target is, and what it costs. A rule that changes how the code
is written goes into `AGENTS.md`; a debt goes into the parent project's
`docs/cleanup.md`. This file is the target, not the debt list.

The order below is the owner's: the web host first, the controller after it.

## 1. The web host: nothing baked in

**Today.** `lh_ui_web_host_t` serves a hardcoded Canvas2D player at `/` and
answers a fixed set of routes: `/ui/press`, `/ui/release`, `/ui/move`,
`/ui/wheel`, `/ui/key`, `/ui/text`, `/ui/size`, `/ui.json`, `/ws`. The player
picks its own scale mode (`?scale=fit`, `layout` otherwise) and its script is
assembled from C strings (`lh/ui/web/js.h`, `src/lh/ui/web/page.c`, about 800
lines). The host, the application and the protocol are one thing.

**Target.** The host is the transport: HTTP, the websocket, the files. The
application belongs to the caller — its page, its routes, its scaling. A REST
surface is a brick of its own that the caller attaches when it wants one; a
desktop program has no reason to carry it, and the transport does not need it.
Nothing is removed: the player becomes one application built on the transport
instead of the transport's built-in default. No hidden, hardcoded behaviour.

**Cost.** About two days: the transport and the application split, the player
moves out of the host into an application of its own, and a route table
(`lh/web/route.h`) lets an application attach REST handlers.

## 2. XML and HTML as nodes

**Today.** `lh_ui_web_html_t` is already a node with children, but the
children are borrowed pointers in arrays the caller builds by hand — the page
builder declares about thirty locals to describe one page. There is no XML in
lh at all.

**Target.** `lh/xml/`: an **entity** is the data (kind, name, attributes,
children), a **node** is that same entity with the operations on it (add a
child, set an attribute, serialize). That is the pseudo-object shape: the
entity is the plain tag, the node is the same tag with the functions. HTML is
XML with different tagging rules — void elements, attribute quoting — so one
tree and one serializer serve both, with the dialect as a flag.

The other side of the same idea: a C object serialized to and from text. JSON
already is (`lh/json/`); XML is the other form, and it belongs to the same
layer.

**Cost.** About three to five days: the entity and node bricks, the owned
children, the serializer, the HTML dialect, and the migration of
`lh_ui_web_html_*` onto it.

## 3. A JavaScript engine in lh

**Today.** Every script lh ships is JavaScript written as C strings.

**There are JavaScript engines written in C.** QuickJS (MIT, no dependencies,
almost complete ES2025, last release 2026-06-04) is the one that fits: a few
files of C and about 350 KiB of code. MuJS (ISC) is smaller but ES5;
Duktape (MIT) is ES5 and no longer developed; JerryScript is ES5.1. V8 and
SpiderMonkey are C++ and far too heavy for this.

**Target.** A brick that loads an engine the way the Win32 backend already
looks up the pointer API at run time — dynamically, so lh never links an
engine and a program without one still starts. Scripts run in the engine, and
what crosses the boundary are C objects through our wrappers, not text. The
media player is then an application that runs a script, not a page with a
script pasted into it.

**Cost.** About two to three days: context, evaluation, calling in, calling
out, and the lifetime rules.

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
