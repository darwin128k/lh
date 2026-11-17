# gx — the new graphics part of LH

Status: in design. The owner and mavis, 2026-11-17. The unit is done and
confirmed — the scalar, the point, the size, the rectangle (`lh/gx/`) — and
everything below it is the plan. The order of that plan is the point of this
page.

`gx` is written beside `gl` and replaces it. At the end of the road `gl` is
deleted and `gx` takes its name: `lh/gx/…` becomes `lh/gl/…`, `lh_gx_*`
becomes `lh_gl_*`.

## 1. Why gl goes away

Not because it is broken — it works, and it is tested. Because of its shape.

| | gl today |
|---|---|
| public functions | 2088 |
| structs | 72 |
| headers / sources | 208 (952 KB) / 97 (1.15 MB) |
| test files | 58 |
| one title and one OK button | 20 calls and 6 objects the application must keep alive |
| the web player | 3329 lines that repeat the renderer in JavaScript |

Three numbers carry the rest:

- **2088 public functions** for a user interface. `get_*` and `set_*` are
  1239 of them — rule 0.0 paid in full on a model made of many small
  structs. `color → paint → style → entity` is three structs and a dozen
  accessors before a single word is on the screen.
- **The renderer exists twice.** `lh/gl/canvas/web.c` writes drawing commands
  as JSON, and `lh/gl/web/player/page.c` writes the browser side of the same
  semantics by hand, as a tree of JavaScript nodes 20 levels deep. Two
  renderers, one of them in another language, kept in step by hand.
- **The engine grew first.** gl was built from the pixel up: pixmap, canvas,
  blending, style, then the widget. The application surface was whatever was
  left at the end. That order is why its top is so wide and its bottom is
  hidden.

## 2. The rules of gx

The owner's decisions, 2026-11-17:

1. **No allocation.** gx never allocates. Every byte it uses is memory the
   application provided.
2. **Ownership.** Where nothing is allocated, ownership is the only thing
   left to say — so every gx header says who owns what, and for how long.
3. **No operating system.** gx runs on a board with no OS — on
   `lh/rail/bare.h`, one core and its interrupts — as well as on a desktop.
   Every table in it has a fixed size, nothing in it waits, nothing in it
   reads a clock, and nothing in it calls the platform. The window, the
   caption, the size and the flush belong to `lh/os/`; gx is above them and
   below the application.

**The model of the screen is not decided yet.** The first attempt at one — an
entity with a class, a place, a text and a set of links — was a half model
and was removed on 2026-11-17. What the model must decide is in section 4.1;
the bricks below it do not depend on the answer.

And the unit. The chain, as the owner set it on 2026-11-17:

- **`lh_gx_scalar_t`** (`lh/gx/scalar.h`) — the number of the gx plane.
- **`lh_gx_point_t`** (`lh/gx/point.h`) — two of them: `x` and `y`.
- **`lh_gx_size_t`** (`lh/gx/size.h`) — a point as well, with its own method
  names: `width` and `height`.
- **`lh_gx_rect_t`** (`lh/gx/rect.h`) — joins a point and a size.

These four are the math bricks of `lh/core/math/` under the name of this part:
gx adds no geometry of its own. Every function of the part is a thin layer over
the chosen math brick — it calls that brick and adds nothing — so there is one
point on the plane, one size, one rectangle, and one place where their rules
live.

## 2.1 The place of gx among the natures

gx is a composition, as `lh/gl/` is: data (the tree, the layers) and the
passes over it. It is neither execution nor the world — but the world reaches
it, and the execution layer can carry it.

That layer is `lh/rail/`: tasks, executors, loops, mailboxes, queues, and a
base of platform calls (`lh/rail/base.h`) that the OS fills
(`lh/os/rail/base.h`) or a bare board fills (`lh/rail/bare.h`). Its design
page is `docs/design/execution.md`, and it already names the user interface as
an owner: a mailbox belongs to "a game's frame, a UI tick".

So gx fits rail without being built on it:

| what gx needs | what rail has |
|---|---|
| a thread that draws and sleeps between frames | `lh_rail_loop_t` — a step, then sleep; it never spins |
| the timers of the animations | the loop's timer group |
| input events from another thread | `lh_rail_queue_t` — many producers, one consumer, no lock, allocates nothing |
| results back to the interface thread | `lh_rail_mailbox_t` |
| slow work off the drawing thread | `lh_rail_executor_t`, `lh_rail_thread_t` |
| no operating system at all | `lh/rail/bare.h` — one core, interrupts, a loop driven from `main` |

**Three things are decided now, so that gx fits rail later.** They cost
nothing now and are expensive to change later.

1. **The time comes in, not from inside.** `lh_gx_view_tick(view, now_ms)`,
   and the tick advances the animations by the time that really passed. gl's
   tick assumes 16 ms (`LH_GL_VIEW_FRAME_MS`) and the application that knows
   better calls `lh_gl_view_advance` instead — so gl's animations run at the
   wrong speed on a slow frame, and a view driven by a rail loop cannot be
   driven at all. The clock belongs to the platform (rail's base has
   `now_ms`), never to gx.
2. **The view is a record the application owns.** No allocation, `user`
   borrowed — as every rail record is — so a view can live inside the state of
   a rail loop.
3. **gx stays single-threaded.** It is fed events and told when to draw; it
   never starts a thread, never reads the operating system, never reads a
   clock. The concurrency belongs to rail: the owner drains rail's queue into
   the view, on the view's thread.

The dependency stays one way and stays optional: gx does not include rail, and
a thin `lh/gx/rail.h` — in the spirit of `lh/os/rail/base.h` — can run a view
on a rail loop when the application wants that.

## 2.2 Two kinds of ownership

The rule "no allocation, only ownership" is two rules, and they must not be
mixed.

- **Who owns the bytes.** In gx: the application. The entities live in memory
  the application provided — an array, or a block from a pool. gx never frees
  an entity, because gx never allocated one. The footprint is known at link
  time and there is no fragmentation inside the interface.
- **Who owns the tree.** In gx: the tree. The parent holds the place of its
  children — the order, the parent link, the responsibility to keep them
  consistent. It does not own their memory.

LVGL makes both the parent's: every object is `lv_free`d by its parent, and
the array of children is `lv_realloc`ed on every add and every remove
(`lv_obj_add_child`, `lv_obj_remove_child`), and the deletion is recursive
(`obj_delete_core`: "Recursively delete the children"). For a library whose
contract is a dynamic interface — rows and dialogs created and destroyed at
run time — that is the right trade, and LVGL gives the way out: its own
allocator (`LV_MEM_CUSTOM`), a pool over a static buffer
(`lv_mem_add_pool`), and a monitor that reports fragmentation and the peak
(`lv_mem_monitor`: `frag_pct`, `max_used`).

What LH takes from it is the ownership, not the heap: the allocator over a
static buffer is already in the roadmap (`docs/roadmap.md`, section 5), and
there it is LH's own brick over the application's buffer, plugged into
`lh_runtime_allocator`. So an application that wants a hundred rows at run
time asks that pool for them; gx still allocates nothing and still frees
nothing.

## 3. The method: from the application down

**The application program is the specification.** We write the program an
application author writes, in gx, before any engine exists. Then every brick
below is derived to make that program expressible — and every sentence the
program cannot say is a hole we can see, named, and filled.

The other order is what produced gl, and it is the order this project does
not repeat: the pixel, the canvas, the style chain, the widget, and the
application fitted into whatever was left. Building from the top means the
engine is measured by the application, not the other way round.

## 4. The application program

This is the spec. It is a complete program: an application that shows a
title and an OK button on a 480×320 screen.

```c
/**
 * The application owns everything. gx never allocates: the tree is an array
 * the application provides, and every entity is a value inside it.
 */

#define MY_ENTITIES 64

static lh_gx_entity_t s_entities[MY_ENTITIES];
static lh_gx_tree_t s_tree;
static lh_bool_t s_ok_pressed;

static void
on_ok(lh_gx_entity_t *entity, lh_ptr context)
{
    (void)entity;
    *((lh_bool_t *)context) = lh_bool_true;
}

lh_bool_t
my_ui_build(lh_void)
{
    lh_gx_rect_t r;
    lh_gx_entity_t *root;
    lh_gx_entity_t *title;
    lh_gx_entity_t *ok;

    lh_gx_tree_init(lh_addr_of(s_tree), s_entities, MY_ENTITIES);

    root = lh_gx_tree_try_add(lh_addr_of(s_tree), lh_gx_entity_kind_layer);
    lh_gx_rect_init(lh_addr_of(r), 0, 0, 480, 320);
    lh_gx_entity_set_rect(root, lh_addr_of(r));

    title = lh_gx_tree_try_add(lh_addr_of(s_tree), lh_gx_entity_kind_label);
    lh_gx_rect_init(lh_addr_of(r), 24, 24, 400, 28);
    lh_gx_entity_set_rect(title, lh_addr_of(r));
    lh_gx_entity_set_text(title, "Hello, lh");
    lh_gx_tree_add_child(lh_addr_of(s_tree), root, title);

    ok = lh_gx_tree_try_add(lh_addr_of(s_tree), lh_gx_entity_kind_button);
    lh_gx_rect_init(lh_addr_of(r), 24, 72, 120, 36);
    lh_gx_entity_set_rect(ok, lh_addr_of(r));
    lh_gx_entity_set_text(ok, "OK");
    lh_gx_entity_set_on_click(ok, on_ok, lh_addr_of(s_ok_pressed));
    lh_gx_tree_add_child(lh_addr_of(s_tree), root, ok);

    return lh_bool_true;
}
```

The reactions — `lh_gx_entity_set_on_click` and what else a class can answer —
come with the classes, which are the next brick after the tree.

Twelve calls and two objects, against gl's twenty calls and six objects
(`docs/subsystems/gl.md`, section 4.1) — and the difference is not brevity:
it is that the program says *what is on the screen*, while gl's says *how
its style objects are wired to each other*.

What the program already demands of the engine, before a line of it exists:

- a **tree** the application fills from its own array — so the tree is an
  index into memory the application owns, not a heap of nodes;
- an **entity** with a place, a text layer and a click layer — so the layers
  are facets of the entity, not objects the application must wire up;
- a **click layer** with a function and a context pointer — the context is
  the application's, the callback is the application's, and gx only calls it;
- the **screen** (480×320) and the **place of every entity** — so the layout
  pass has a job, and it has one input: the tree.

## 4.1 The model of the screen — not decided yet

The first attempt was a node with a class, a place, a way of looking, a text
and a set of links. It was a half model: it guessed what a node holds before
anyone had said what a node *is*, and the owner removed it on 2026-11-17
("a half model — we think separately about which one to take, what models
exist").

So: what models exist, and what each one costs on a board with no GPU.

### The models that exist

**1. Immediate mode** — Dear ImGui, Nuklear, microui. The application draws
the whole interface every frame, from code; the framework keeps only transient
state (what is hot, the text being typed).

- For: no tree to keep, the application reads like ordinary code, and it is
  fast on a GPU — one vertex buffer, few state changes.
- Against: **every frame is the whole screen.** On a microcontroller with no
  GPU that is the one thing we cannot afford; and the state lives in the
  application's code, not in anything the framework can inspect.
- For LH: a source of ideas for the API, not the model. Its speed comes from
  redrawing everything, which is the opposite of what the damage machinery is
  for.

**2. Retained, a type per widget** — LVGL, FLTK, Qt Widgets. A tree of
objects, each with its own type, style and state; the renderer walks it.

- For: the state is in the tree; windows and modality come with it.
- Against: **many objects per widget** (`color → paint → style → entity` in
  gl), and the kind of a widget decides how it is drawn and how it reacts —
  so changing what something looks like means touching its type.

**3. Retained plus damage** — Windows, Android, macOS, and LVGL's partial
mode. The system or the framework knows what changed and repaints only that.

- For: exactly what a small display needs; the work is proportional to the
  change, not to the screen.
- Against: someone has to say what changed, honestly. That is the hard part,
  and it is the part the first attempt got wrong.

**4. Declarative plus a diff** — React, Flutter, SwiftUI, Elm, Slint. The
application describes what should be on screen; the framework compares that
with what was and repaints the difference.

- For: the application only says *what should be*; nobody forgets to mark
  something invalid.
- Against: a diff engine and a second tree to compare against; on a
  microcontroller that is expensive, and the repaint is still driven by a
  comparison rather than by a place that knows it changed.

**5. A description that becomes a command list** — what a GPU renderer
consumes: the scene is turned into commands and replayed.

- For: one contract with the platform; the browser and the screen take the
  same list.
- Against: it says nothing about *what* is on screen — it is the output
  side, not the model.

### What the choice is actually about

Four questions decide it:

1. **Where the state lives** — in the application's code (immediate), in a
   tree the framework owns (retained), or in a description the framework
   diffs (declarative).
2. **Who decides what to repaint** — the application (it marks it), the
   framework (it compares), or the part that owns the pixels (the canvas,
   which is what the damage machinery says).
3. **Where the memory is** — with the application, always (LH's rule), or in
   a pool the framework manages.
4. **How the layout is computed** — by hand in the application (immediate),
   by a pass over the tree (retained), or by the diff (declarative).

### Where LH points, and why

Model 3 — **retained plus damage** — with the choices made explicit:

- the state is in a tree the **application** owns (rule: no allocation), so
  the memory is the application's and the footprint is known at link time;
- **the canvas owns the damage** (section 5.1), so nobody has to remember to
  mark anything: the part that changed says so, and the part that owns the
  pixels remembers;
- the layout is a **pass** over the tree, not the application's problem;
- the renderer is a pass too, and it produces primitives for the canvas — so
  a 3D renderer later is a second renderer into the same canvas, not a
  special case.

The tree is not what was wrong. The first attempt guessed what a node holds
(a text, a style index, a set of links) before anyone had said what a node
is. That guessing is what was removed.

What still needs deciding, and what this page does not yet answer:

- **What is a node?** A node of one type with a class, or a node per kind?
  The owner leans to one type — a layer is the same entity that holds many —
  but the class that says what a node *is* has not been written down.
- **Where the content of a node lives** — in the node, in a layer beside it,
  or in a table the node names.
- **How a node looks** — a theme entry by index, a style the node owns, or
  both.
- **How the reactions are stored** — slots in the node, or a table of
  reactions the node names.

The bricks that do not depend on the answer are already there: the unit and
the geometry (`scalar`, `point`, `size`, `rect`), the damage
(`lh_gx_damage_t`) and the target (`lh_gx_pixmap_t`).

## 5. What the engine under it must be

Derived from the program above, top down. Each item is a brick with its own
header, its own implementation and its own test.

1. **The tree** (`lh/gx/tree.h`) — the application's array, the root, the
   parent and child links, and the ownership rules written down.
2. **The entity** (`lh/gx/entity.h`) — one struct: its class, where it is,
   how it looks, what it says, its callbacks, its flags and its links.
3. **The classes** (`lh/gx/entity/kind.h`) — the kinds a node may have:
   `label`, `button`, `layer`, `image`, `scroll`, … one enum, and what each
   kind means for input and for painting.
4. **The reactions** (`lh/gx/entity/event/`) — the kinds of thing that can
   happen to a node (`pressed`, `released`, `clicked`, `shaken`, `focused`,
   `scrolled`) and the callback type that receives them.
5. **The theme** (`lh/gx/theme.h`) — the look: colours, font, radius, one
   entry per class and the named entries the application wants.
6. **The passes** (`lh/gx/measure/`, `lh/gx/layout/`, `lh/gx/paint/`,
   `lh/gx/hit/`) — four passes over the tree, each a separate brick, each
   writing into storage the application owns.
7. **The renderer** (`lh/gx/render/`) — walks the tree and produces drawing
   primitives. This is where the 3D renderer will sit later: a second renderer
   into the same canvas, not a special case inside the first one.
8. **The canvas** (`lh/gx/canvas.h`) — the pure drawing abstraction: pixels,
   colours, primitives, clipping. It knows nothing about entities and nothing
   about layers, and it remembers nothing about the scene.
9. **The backends** (`lh/gx/backend/`) — `sw` paints into pixels, `web` writes
   the list as JSON. The browser replays the list; it does not re-implement
   the renderer. This is what kills the 3329 lines.

## 5.1 The canvas owns the damage

The owner's decision, 2026-11-17: **a layer never remembers that it is
invalid.** The canvas does.

So the chain is: the tree says what is on screen; the renderer turns it into
primitives; the canvas draws them and **remembers what changed**; the window
asks the canvas what to repaint and presents that.

Nothing in the scene carries a "dirty" flag. A change is reported to the
canvas, and the canvas is the one that remembers it — which is what makes
::lh_gx_damage_t a canvas brick rather than a tree one.

**How the report travels.** The entity holds a call and the listener's state
(`lh_gx_entity_damage_fn` and its context); when something of it changes, it
calls that with where it was and where it is now. The tree installs the call
when it takes the entity, and the canvas is what listens. So the entity
depends on nothing of the drawing part: it only knows that something is
listening, and the tree is what connects the two.

The canvas stays a half-driver: it knows how to fill, to blend, to clip and
to move a line, and it knows nothing about what a button is.

## 5.2 What makes drawing fast

The engine has to be fast on a small, slow device: the target is a remote
control's display, and the yardstick is LVGL, which the remote runs today.
Three costs are separate and must be measured apart — LVGL's own rule is
"useful when the system monitor says so": **render ms high, flush ms low** means
the CPU is the bottleneck (styles, blend modes, a buffer in slow RAM); **flush
ms high** means the transfer is (a slow SPI clock, one buffer, a blocking
flush). A renderer that is fast and a display that is slow need different
fixes, so the bench measures them separately.

**What to redraw.**

- *Dirty rectangles* — the region that changed, as a list of rects. This is
  ::lh_gx_damage_t, and its one rule is that a merge never covers a pixel that
  did not change.
- *Trivial reject* — before any expensive work, compare bounding boxes. An
  entity that does not meet the clip is skipped; one that meets it entirely is
  taken whole. This is the cheapest large win and it is a rule, not a structure.
- *Clipping* — set the clip to the damaged area and draw only what is inside.
- *A hash grid* — a grid of cells, each holding a hash of what it shows; the
  cells whose hash changed are the dirty ones. Fixed memory, no allocation,
  and the work is proportional to the screen, not to the scene. The kind of
  thing that fits a microcontroller.
- *Tiles* — divide the screen into fixed tiles (32×32, 64×64, 128×128) and
  mark the tiles dirty. Tiles make the dirty set a bitmap, which is far
  cheaper to carry than a list of rects.
- *Cached slices* — the scene is cut into slices (content, controls, a
  background), each made of tiles that keep their rasterised content; a tile
  that did not change is not drawn again.
- *Coalescing and a frame budget* — take every event into the state at once,
  but redraw at most once per frame window. Many small changes are one frame.
- *Virtualisation* — draw only the part of a long list that fits, plus a small
  overscan.
- *Fast redraw* — an entity with a solid background behind it can be erased by
  filling a rectangle, with no intersection tests at all.

**How to draw it.**

- *A command buffer* — the draw list the paint pass produces. It is the one
  contract with the platform, and the web backend writes it as JSON while the
  software backend replays it into pixels.
- *A glyph atlas* — glyphs are rasterised once and then blitted.
- *An image atlas of fixed-size tiles* — the image goes in as tiles of one
  size, so giving one back is "a tile is free" and nothing fragments. This is
  the answer to the fragmentation the remote suffers from today: the atlas is
  fixed from the start, and the application owns its memory.
- *Blending once per span* — mix a whole run of pixels under one coverage
  instead of per pixel.
- *A table instead of a computation* — a gradient is a table of colours built
  once per fill, not a division per pixel.
- *Caching the mask, not the pixels* — identical shadows share one alpha mask.

**How it reaches the screen.**

- *Partial, direct or full* — LVGL v9 has three render modes: partial renders
  the screen in pieces through buffers smaller than the screen, direct renders
  into a screen-sized buffer and copies only what changed, full redraws
  everything every time. gx needs the first two.
- *Two buffers, or three* — with two, the CPU draws into one while the other
  is transferred; the third removes the idle time between the two.
- *The buffer in fast memory* — internal RAM, not external SRAM.
- *No blocking* — the swap waits on a signal, not on a spin.

**The window is the operating system's job.** The caption, the size, the
surface, the flush and the wait belong to `lh/os/`; gx never calls the
platform. What gx owes the window is: a tree, a place, a list of commands to
draw, and the damage that says which of them matter now. The window owns the
pixels and asks for the damage. The shape of that seam — and how the same
window works on a board with no operating system — is `docs/design/app-window.md`.

**The measure.** `bench/lvgl/` already compares lh with LVGL scene by scene:
eight scenes at 480×320 ARGB8888, each drawn whole and one card at a time
(`bench/lvgl/RESULTS.md`). After the gradient work of 2026-10-11 no scene is
slower than LVGL, and the partial redraws — where the work is small and the
difference survives any noise — win by up to 2.7x. On that machine the
measurement noise is 9–15%, so only a difference beyond ~1.2x means anything.
The bench is native and the numbers are a lower bound: the browser and the
remote are the real targets, and the multiplier of a wasm build is still not
measured. gx needs the same bench, with the same scenes, so that the two
runtimes can be put side by side.

## 5.2 What else could be faster

The drawing scheme in this plan — retained, damage, the canvas remembers — is
not the fastest thing that exists. It is the fastest thing that also answers
the remote's shape. The mechanisms that beat it, and what each one buys:

**On the wire (the remote's real cost).** A 480×320 RGB565 frame is 307 200
bytes. Sending only the changed rectangles sends the difference: a 40×40
button is 3 200 bytes, **a hundred times less**. That is the single largest
win on a board, and it is not the pixel loop — it is deciding *what to send*.

**In the scene.** The tree is walked once per frame even when one pixel
changed. Three ways around it:

- **A retained command list** — the scene is kept as the list of primitives it
  drew last time; a frame filters the list by the damage and replays the
  survivors. No tree walk at all. The list is rebuilt when the scene changes.
- **A hash grid** (Lite XL) — a grid of cells, each holding a hash of what
  was drawn in it; a frame compares the hashes with the previous frame and
  re-renders only the cells that changed. The tree is still walked once, but
  the *drawing* is limited to the changed cells, which are finer than the
  damaged rectangles.
- **A picture cache** (Webrender) — the static part of the screen is kept as
  tiles that were rasterised once; only the parts that move are drawn again.
  The biggest win when the screen is mostly still, which a remote's screen
  usually is.

**In the pixels.** Blend a whole span under one coverage instead of per pixel
(`lh_gl_pixmap_blend_const_32` does this now); blit instead of blend where the
source is opaque; one blend mode per pass, so the mode is set once per format;
never a full-screen translucent overlay, because every pixel under it has to be
blended; the glyph atlas, so a glyph is a copy.

**What the bench does not see.** `bench/lvgl/` times the renderer into memory
— "nothing on screen and no driver". On the remote the flush dominates, and
LVGL's own rule says to look at the two separately: *render ms high, flush ms
low* means the CPU is the bottleneck, *flush ms high* means the wire is. The
bench has to grow a second number — **bytes over the wire** — before any of
the schemes above can be compared honestly.

## 6. What leaves gl before the swap

Two things in `lh/gl/` are not user interface and should not wait for gx to
be finished:

- **The web host** (`src/lh/gl/web/host*`) is transport: HTTP, WebSocket,
  gzip. It belongs in `lh/os/web/`, and it does not wait for gx.
- **The 3D renderer** (`src/lh/gl/renderer3d.c`) is not a widget either. It
  draws through the same canvas, so it deserves its own part on the same
  draw list.

The CSS builder (`lh/gl/decl.*`) becomes gx's own builder: it fills a gx tree
instead of gl entities.

## 7. The phases

| Phase | What | Who |
|---|---|---|
| 0 | The unit: scalar, point, size, rect; the damage; the target | **done** |
| 1 | The model of the screen — the survey, and the choice | together |
| 2 | The tree and the node, in the model that was chosen | me |
| 3 | The passes: measure, layout, paint, hit | me |
| 4 | The canvas and the backends: sw, web | me |
| 5 | The window, the MCU path, the examples, the benches | me |
| 6 | The swap: delete gl, rename gx to gl, rebuild every backend | together |

The application program in section 4 was written against the first model and
is kept as a sketch of how short a program should be — it will be written
again in the model that is chosen.

From phase 2 on I work alone: every brick with its own test, the build green
after every brick, `CHANGELOG.md` updated as the public surface grows, and
the numbers of any performance work measured and written down.

## 8. The decisions of phase 1

Each one is a choice the owner makes; the recommendation is mine and the
reason is given, so that a disagreement is a disagreement about the reason.

**0. The path.**
*Decided by the owner, 2026-11-17: a fresh gx, then the swap.* The new part is
written beside gl, with no debt to gl's shape, and at the end gl is deleted
and gx takes its name (`lh/gx/…` → `lh/gl/…`, `lh_gx_*` → `lh_gl_*`).

The reason it is not a change of gl in place: the new model wants to **delete**
a part of gl's API — the `color → paint → style → entity` chain and the entity
as a widget — and gl is a public library, so that API cannot go without
breaking its users. Built in place, the new model would live beside the old
one, the API would grow before it shrinks, and every new decision would be
checked against 2088 old functions — the model would stay "whatever gl
allows". It already happened once: the first node written here was gl's shape
under a new name.

What the swap costs, counted: gl's API is used outside gl in **236 distinct
symbols** in about twenty places — the CSS generator, the bench, the GDI
backend, the os layer, the examples, the test helpers. The web host moves to
`lh/os/web/`, the CSS builder becomes gx's own, the 3D renderer gets its own
part, the bench is rewritten for gx (which is the point: an honest comparison),
and the docs are rewritten with it.

**1. The model of the screen.**
*Open. The first attempt was removed on 2026-11-17 as a half model.* The
models that exist, what each costs, and the four questions the choice turns
on are in section 4.1. What is already settled around it:

- **One node type** — the owner, 2026-11-17: a layer is the same node that
  holds many, so there is no second type for it; the class says what a node
  is.
- **The memory is the application's** — the owner, 2026-11-17: an array the
  application provides; the tree indexes into it and hands out slots.
- **The damage belongs to the canvas** — the owner, 2026-11-17: a layer never
  remembers that it is invalid.

Still open: what a node holds (the content, the reactions), and how it looks.

**2. Where the colours live.**
*Recommendation: a theme, a table the application owns, and the node names
a style by index.*
One place to change how the screen looks, and the entity stays small. The
alternative is per-entity colours (simple, but fifty entities later the
palette is fifty copies of one palette) or the gl chain (three structs per
entity).

**3. How the tree is stored.**
*Decided by the owner, 2026-11-17: the application provides an array of
entities; the tree indexes into it and hands out slots.*
No allocation, the size is known at build time, an entity is addressable and
stable, and the application can keep a pointer to any of its own entities.
The alternative is the tree owning an arena of its own (also no allocation,
but then the application cannot hold an entity pointer).

So `lh_gx_tree_t` is the application's array and the links between the
entities in it: `lh_gx_tree_try_add` takes a free slot and gives it the
tree's own call, so a change of an entity is reported through the tree to
whoever listens — the canvas. The links are indices, and every entity knows
its own index, so the application works with pointers while the tree stays a
flat array.

**4. What leaves gl before the swap.**
*Recommendation: yes — the web host to `lh/os/web/`, the 3D renderer to its
own part.*
Neither is a user interface, and both are easier to move while gl is still
there to test against. The cost is a bigger diff in the middle of the work.

## 9. The debt this page creates

Until the swap, the tree holds both `lh/gl/` and `lh/gx/`, and the names
`lh_gl_point_t` and `lh_gx_point_t` exist side by side over one math brick.
That is the transitional debt of rule 3; it is listed in `docs/cleanup.md`
and it ends with the swap in phase 7.
