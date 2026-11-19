# gx — the new graphics part of LH

Status: in design. The owner and mavis, 2026-11-17 and 2026-11-18. The unit,
the damage and the target are done (`lh/gx/`); the model of the screen is
decided (section 4.1) and the pipeline under it is set (section 5). Everything
below the unit is the plan, and the order of that plan is the point of this
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

**The model of the screen** is in section 4.1, decided on 2026-11-18. The
first attempt at one — an entity with a kind enum, a place, a text and a set of
links — was a half model and was removed on 2026-11-17.

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

### Where the fragmentation comes from, and the two ways out

The remote suffers from it today, and the cause is the shape above: LVGL
**owns and stores** every object — each one is a `malloc` of its own size, the
children of each parent are an array that is `realloc`ed on every add and
remove, styles and event lists are more small blocks, and a screen that is
built and torn down again and again leaves holes of every size in the heap.
After an hour there is enough free memory and no block big enough.

gx does not do that, and it has **one** rule for it, not two modes:

**gx never stores an entity.** The tree owns the order, not the bytes — it is
*intrusive*, as `lh_list_t` is, and it is built on `lh_list_t`. The links (the
owner, the list of children, the entity's own place in its owner's list) live
**inside** the entity, and the entity lives wherever the application put it:

- a field of the application's screen struct — the common case, its size known
  at link time;
- a plain static array, `lh_gx_entity_t rows[32]`;
- an `lh_array_t`, when the number of rows is known only at start-up.

For gx the three are the same thing — an address. Attaching is linking a node,
detaching is unlinking it; gx never allocates, never frees and never moves
anything, so there is nothing in it to fragment, and the memory of the
interface after an hour is what it was in the first second.

What follows from it:

- **Running out of memory is the allocator's answer, and the allocator is the
  application's.** gx asks for nothing, so it has nothing to report; an
  `lh_array_t` that cannot reserve says so through `lh_array_try_reserve`, and
  the application decides — fewer rows, another screen freed, a message.
- **An `lh_array_t` of entities is reserved once, before its entities are
  attached, and not grown afterwards.** Growing reallocates and moves the
  entities, and every link and every pointer the application holds is left at
  the old place.
- **An entity is detached before its memory goes** — the rule of every
  intrusive list, `lh_list_t`'s included. The validating forms (`_v`) check what
  they can.
- **A pool of one-size slots is not gx's.** For rows that are made and dropped
  all the time it would be the answer — slots of one size cannot fragment — but
  it is not graphics. If it is needed, it is a brick of the core
  (`lh/core/memory/`), added when there is a need for it, not before.

The price: the node is a pointer to the owner, a list head and a list node —
five pointers (20 bytes on a 32-bit board) where indices into one array would
have been 6 bytes. It is the price of LH's own list instead of a second one,
and of an entity that can live outside any array.

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
title and an OK button on a 480×320 screen. It is written in the model of
section 4.1; the names are a sketch, and the bricks below are derived from what
it has to say.

```c
/**
 * The application owns everything. gx never allocates: the entities are fields
 * of the application's own screen, the look is a table of styles in flash, and
 * what a button does when it is clicked is the application's own record.
 */

enum my_style { my_style_screen, my_style_title, my_style_button, my_style_count };

static const lh_gx_style_t s_theme[my_style_count] = { /* colours, radius, font… */ };

/* One screen is one struct: its memory is its size, known at link time. */
struct my_screen
{
    lh_gx_entity_t root;
    lh_gx_entity_t title;
    lh_gx_button_t ok;               /* a control: its box, its caption, its reaction */
    lh_bool_t ok_pressed;
};

static struct my_screen s_screen;

static lh_void
on_ok(lh_gx_button_t *button, lh_ptr context)
{
    (void)button;
    *((lh_bool_t *)context) = lh_bool_true;
}

lh_void
my_ui_build(struct my_screen *self)
{
    lh_gx_entity_init(&self->root, &lh_gx_box_class, &s_theme[my_style_screen],
                      0, 0, 480, 320);

    lh_gx_entity_init(&self->title, &lh_gx_text_class, &s_theme[my_style_title],
                      24, 24, 400, 28);
    lh_gx_entity_set_content(&self->title, "Hello, lh");
    lh_gx_entity_attach(&self->root, &self->title);

    /* The button is a composition: a box that takes input, owning a caption. */
    lh_gx_button_init(&self->ok, &s_theme[my_style_button], 24, 72, 120, 36, "OK",
                      on_ok, &self->ok_pressed);
    lh_gx_entity_attach(&self->root, lh_gx_button_get_box_mut(&self->ok));
}
```

What the program demands of the engine, before a line of it exists:

- a **tree** that keeps the links and never allocates — the entities here are
  fields of the application's screen, and they could as well be a static array
  or an `lh_array_t` (section 2.2);
- **one entity type** — the screen, the title, the button and its caption are
  the same struct; what differs is the **class** (a constant table: how it
  paints, whether it takes input) and the **style** (a constant record: how it
  looks);
- a button that is a **composition**: a box that takes input, owning a text.
  There is no "button with a label inside it" — there are two entities, and
  the owner link is what makes them one control;
- the class's own data — the text, the reaction of the button — is the
  application's, and the entity only points at it (`content`).

## 4.1 The model of the screen

*Decided by the owner, 2026-11-18.* Retained plus damage (model 3 below), with
**one entity type, classes as tables, styles as shared records**, and a
pipeline in which every part does one job (section 5).

### The models that exist

**1. Immediate mode** — Dear ImGui, Nuklear, microui. The application draws
the whole interface every frame, from code; the framework keeps only transient
state.

- For: no tree to keep, the application reads like ordinary code, and it is
  fast on a GPU — one vertex buffer, few state changes.
- Against: **every frame is the whole screen.** On a microcontroller with no
  GPU that is the one thing we cannot afford.

**2. Retained, a type per widget** — LVGL, FLTK, Qt Widgets. A tree of
objects, each with its own type, style and state; the renderer walks it.

**3. Retained plus damage** — Windows, Android, macOS, and LVGL's partial
mode. The framework knows what changed and repaints only that. The work is
proportional to the change, not to the screen — exactly what a small display
needs.

**4. Declarative plus a diff** — React, Flutter, SwiftUI, Elm, Slint. A diff
engine and a second tree to compare against; on a microcontroller that is
expensive, and the repaint is still driven by a comparison rather than by the
place that knows it changed.

**5. A command list** — what a GPU consumes. It is the output side, not a
model: it says nothing about *what* is on screen.

### How LVGL does it, and why gx does not copy it

LVGL 8/9 is model 2 with partial redraw. Its object (`lv_obj_t`) is not
"everything in one" as it first looks — it has three parts:

- **the object**: the coordinates, the parent, about thirty flags (`HIDDEN`,
  `CLICKABLE`, `SCROLLABLE`, `CHECKABLE`, …), the state (pressed, focused,
  checked), a pointer to its styles, and a lazily allocated block of rarer
  fields (`spec_attr`: the children, the scroll, the event list, the focus
  group);
- **the class** (`lv_obj_class_t`) — one table per kind of widget: the
  constructor, the destructor, the event handler, the size of an instance. The
  drawing is an event too: the class answers `LV_EVENT_DRAW_MAIN` with
  `lv_draw_rect` and `lv_draw_label`;
- **the styles**, separate objects the object only points at.

So the class and the look are already separate in LVGL. What is piled together
is something else: **the base object knows about everything any widget might
need** — the scroll, the focus, the click, the events. That is a deliberate
trade, for three reasons: C has no inheritance, and `lv_obj_set_x()` and
`lv_obj_add_flag()` must work on every widget, so the common part goes into the
base; ten years of features were cheaper to add to the base than to the
architecture, and `spec_attr` was the answer to the memory that cost; and every
object is a `malloc`, so one object should be one block. LVGL 9 did split the
drawing the way gx does: the tree produces *draw tasks*, and *draw units* (SW,
VG-Lite, a GPU) execute them.

gx takes the class and the shared style, and leaves the rest:

- **Many bricks in the code, one record in memory.** Every concern is a brick
  with its own header, its own API and its own test, and the entity **embeds
  them by value**. On a board that is one block with no pointer to follow; in
  the code every piece has one job. This is also what gl got wrong: it split
  the look into `color → paint → style → entity` **by pointers at run time**,
  so the application had to keep every piece alive — twenty calls and six
  objects for one title and one button.
- **No flags for what a class can answer.** An entity whose class has no
  `input` slot cannot be clicked, so there is no `CLICKABLE`. Thirty flags
  become "which slots does the class fill".
- **The scroll, the focus and the events are not in the base.** The scroll is
  a class of entity that offsets its children; the focus is a brick over the
  tree; the reactions are the class's data, which the application owns.
- **No heap.** The entities are the application's array, the classes and the
  styles are constants in flash.

### The entity

| brick | its job | where it lives |
|---|---|---|
| **node** | the tree: the owner, the children, its own link — intrusive, on `lh_list_t` | embedded |
| **place** | `x`, `y`, `width`, `height` relative to the owner (`lh_gx_rect_t`) | embedded |
| **visibility** | whether it is drawn, hit and laid out at all | a flag in the entity |
| **class** | *what I am and how I behave*: a table of slots — `paint`, `covers`, `hit`, `input`, `measure` | a constant, one per kind, in flash |
| **style** | *how I look*: colours, border, radius, font — flat data the `paint` reads | a constant record of a theme, shared |
| **content** | the class's own data: the text of a label, the reaction of a button | the application's, the entity points at it |
| **state** | pressed, focused, checked | a small brick, only in the classes that need it |

Consequences:

- **A layer and a card are not two types.** A *layer* is a class with no
  `paint` of its own that only holds children; a *card* is a class whose
  `paint` draws a box; a *label* is a class whose `paint` draws a run of
  glyphs. One struct, many tables.
- **Every control is a composition of entities.** A button is a box that
  takes input, owning a text. A spinbox, a roller, a dropdown, a checkbox, an
  arc, a QR code, a line, an icon — each is a class, or a few entities under
  one owner. Nothing below the entity knows what a button is.
- **An effect is an entity too**, beside its owner: a shadow is an entity
  under the card that reads its owner's place and radius. A blur or a glass
  has to read the pixels already drawn under it, so it needs an optional slot
  of the canvas (section 5); a backend without it cannot draw the effect, and
  says so instead of drawing something that only looks like it.
- **The text is not inside the entity or the canvas.** The glyphs come from a
  font, which is its own contract: it gives a mask, and the canvas draws the
  mask (`fill_mask`).

The size, for comparison: LVGL's base object is about 40–60 bytes on a 32-bit
MCU, plus `spec_attr` and the style list, all on the heap. A gx entity is the
node (five pointers, on `lh_list_t`), the place (four scalars), a flags byte
and three pointers (class, style, content) — about 50 bytes on a 32-bit board,
in memory the application chose, with the classes and the styles in flash.

## 5. The pipeline

Every part does one job, and no part remembers what is not its own.

```
 entity ──"I was here, I am there"──▶ owner ──▶ … ──▶ root ──▶ RENDERER
                                                            (keeps the damage)
 a frame:
 RENDERER ── takes the damage ── walks the tree ── for every entity:
              skips the hidden and the untouched,
              computes the absolute rectangle and the clip
                │
                ▼
           class.paint(entity, …) ── "I am a box of this colour and radius"
                │
                ▼
           CANVAS (a contract): fill_rect(clip, rect, colour), fill_round_rect,
                │               fill_mask (a glyph), blit (an image), line, arc …
                ▼
           BACKEND: sw → into a pixmap, web → into JSON, DMA2D, GDI …
                │
 RENDERER ── hands only the changed rectangles ──▶ PRESENT (the OS window, or the panel)
```

| part | knows | neither knows nor remembers |
|---|---|---|
| **entity** | its place, its owner, its children; reports that it changed | what a pixel or a frame is |
| **class** (`paint`) | what an entity of its kind looks like: a box, a text, an arc | where it is drawn, what else is on screen |
| **renderer** | what changed (the damage), the order, the clip, what to skip | how a rounded corner is drawn |
| **canvas** | how to draw a primitive inside a clip | the scene, the damage, the entities, the last frame |
| **backend** | where: a pixmap, a JSON stream, the hardware | anything above it |
| **present** | how to put rectangles on a screen | what is in them |

The bricks, from the bottom:

1. **The node** (`lh/gx/node.h`) — the intrusive links on `lh_list_t` (the
   owner, the children, the entity's own link), `attach`, `detach`, the walk,
   and the ownership rules written down.
2. **The entity** (`lh/gx/entity.h`) — the node, the place, the visibility,
   the class, the style, the content, embedded by value.
3. **The class** (`lh/gx/class.h`, one `fn.h` per slot) — the table of slots,
   each optional with its fallback written down: `paint`, `covers` (am I
   opaque over this rectangle), `hit`, `input`, `measure`.
4. **The style** (`lh/gx/style.h`) — one flat record; a theme is the
   application's array of them.
5. **The canvas** (`lh/gx/canvas.h`, one `fn.h` per slot) — a contract, as
   `lh_os_window_backend_t` is: a table of slots and the backend's context.
   Required: `fill_rect`. Optional, each with its fallback: `fill_round_rect`
   (else the renderer covers it with `fill_rect` and a mask), `fill_mask`,
   `blit`, `line`, `arc`, and the read-back slots that blur and glass need
   (without them, those effects are reported as not drawable).
6. **The backends** (`lh/gx/backend/`) — `sw` paints into a `lh_gx_pixmap_t`,
   with the pixel kernels of `lh/gl/` moved in (the span blend, the masks, the
   shadow cache, the gradients, the glyph atlas — measured on the i.MX RT, and
   not thrown away with gl's model); `web` writes the calls as JSON, and the
   browser replays them instead of re-implementing the renderer — this is what
   kills the 3329 lines.
7. **The renderer** (`lh/gx/render.h`) — the damage, the walk, the clip, the
   cover check, the strips, and the hand-over to the present.
8. **The font** (`lh/gx/font.h`) — a contract: a glyph in, a mask out.
9. **The classes** — `box`, `text`, then the controls as compositions.

## 5.1 The renderer keeps the damage; the canvas keeps nothing

*Decided by the owner, 2026-11-18*, replacing the decision of 2026-11-17 that
gave the damage to the canvas.

- **An entity never remembers that it is invalid.** When something of it
  changes — the place, the visibility, the style, the content — it reports
  where it was and where it is now. The report goes up through the owners,
  which turn it into screen coordinates (and drop it when an owner is hidden),
  to the root, and the root hands it to the one listener the tree has: the
  renderer.
- **The renderer keeps it** in a `lh_gx_damage_t` — up to 16 rectangles in a
  fixed table, merged only when the union is exactly a rectangle. The change
  is known before anything is drawn, and deciding what to redraw is the
  renderer's job, so the damage is the renderer's.
- **The canvas keeps nothing.** The target and the clip come with every call,
  or once per frame as the backend's context — so there is no `save` /
  `restore` stack in the canvas, as there is in gl: the state belongs to the
  renderer, which holds it anyway while it walks the tree. A canvas without
  memory can be swapped (sw, web, DMA2D), used for two targets at once (the
  screen and the browser) and tested with no scene at all.
- **The backend owns the pixels**, and the present shows only the rectangles
  the renderer hands it.

### What makes it fast

None of this is a trick on the side; it follows from the split.

1. **The work is proportional to the change.** A blinking cursor redraws the
   cursor's rectangle, not the screen. The renderer enters only the subtrees
   that meet the damage: an owner that is not touched skips all its children
   in one test.
2. **An opaque entity hides what is under it.** If a card is opaque and covers
   the damaged rectangle whole (its class's `covers`), the renderer starts the
   frame from that card and never draws what lies under it — LVGL's cover
   check.
3. **Strips on a board.** The buffer is not the screen but, say, 480×40; the
   renderer goes over the damage strip by strip by moving the clip, and the
   canvas never knows.
4. **Only the changed rectangles cross the wire.** A 40×40 button in RGB565
   is 3 200 bytes, not the 307 200 of a frame — the largest win on the remote.
5. **No heap and no pointer chasing** — the entities are one array, the
   classes and the styles are constants.
6. **The pixel kernels of gl**, which are measured and fast already.

The measure is the bench against LVGL, with two numbers: the render time and
the bytes sent to the screen.

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
platform. What gx owes the window is: the pixels drawn into the target it was
given, and the rectangles that changed — the renderer hands them to the
present. The shape of that seam — and how the same
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

## 5.3 What else could be faster

The drawing scheme in this plan — retained, damage, the renderer remembers — is
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
  gzip. It belongs in `lh/os/web/`, and it does not wait for gx. **Moved**
  (2026-11-18): the host and the application to `lh/os/web/`, the WebSocket
  frames to `lh/core/web/ws.h`; the gl player is one application of it.
- **The 3D renderer** (`src/lh/gl/renderer3d.c`) is not a widget either. It
  draws through the same canvas, so it deserves its own part on the same
  draw list.

The CSS builder (`lh/gl/decl.*`) becomes gx's own builder: it fills a gx tree
instead of gl entities.

## 7. The phases

| Phase | What | Who |
|---|---|---|
| 0 | The unit: scalar, point, size, rect; the damage; the target | **done** |
| 1 | The model of the screen — the survey, and the choice | **done** (2026-11-18) |
| 2 | The node (on `lh_list_t`) and the entity | **done** (2026-11-18) |
| 3 | The canvas contract and the `sw` backend, with gl's pixel kernels moved in | **done** for `fill_rect` and `fill_mask` (2026-11-18); the kernels are shared in `lh/core/pixel/`, gl calls them; gl's SIMD alpha spans move when a slot needs them |
| 4 | The renderer: the damage, the walk, the clip, the cover check, the strips | **done** (2026-11-18), with the class (`paint`, `covers`, `report`) and the paint; the first check passes |
| 5 | The first classes: `box`, then the font contract and `text` | **done** (2026-11-18): the style, `box`, the font contract over the core's baked font, `text` |
| 6 | The controls as compositions; input and `hit`; the `web` backend | **started** (2026-11-18): the walk, the `hit` and `input` slots, the event, the pointer, the button — every name the program of section 4 uses exists; the `web` backend writes the canvas calls as JSON and its player replays them in the browser (37 lines of script); the web host moved to `lh/os/web/` and knows nothing of gl; the gx application of the host (`lh/gx/web.h`) serves a screen to browsers, frames out and the pointer in, with a runnable example (`example/gx_web_button.c`); the tap gesture (`lh/gx/tap.h`) shared by the button, the checkbox (`lh/gx/checkbox.h`) and the radio (`lh/gx/radio.h`); the drag gesture (`lh/gx/drag.h`) and the slider (`lh/gx/slider.h`); the keyboard and the focus (`lh/gx/focus.h`, the `key` slot, the key and text lines of the page); the text field (`lh/gx/field.h`); every control so far — button, checkbox, radio, slider, field — runs together in a browser (`example/gx_web_button.c`); next: phase 7 |
| 7 | The window, the MCU path, the examples, the benches against LVGL | **started** (2026-11-18): the bench has a gx scene (`bench/lvgl/scene_gx.c`); on `cards` gx was 1.65x slower than gl; with a corner and a glyph handed to the canvas whole it is level with gl (188 against 192 microseconds a frame) and ahead of LVGL (233), and 1.2x faster than gl on a partial frame (`bench/lvgl/RESULTS.md`); the other scenes wait for gradients, shadows, wrapped text, a layout and a radius per corner; the present contract (`lh/gx/present.h`) and the desktop window (`lh/gx/window.h`): the same screen of controls runs in a browser and in a window (`example/gx_controls.c`), built and tested on Windows and Linux; the path with no operating system: gx builds and its tests pass with the OS off, a frame goes through a band smaller than the screen (`lh/gx/strip.h`), and a button screen cross-built for a Cortex-M4 is 29 KB of flash (17 KB the font) and 13 KB of RAM (12.5 KB the band; `bench/mcu/RESULTS.md`); the examples on the three outputs: the same screen of controls on a panel through a band (`example/gx_panel.c`, built with the OS off as well), in a browser and in a window; a radius per corner (`lh/gx/corners.h`): the `corners` scene of the bench in 212 microseconds against gl's 253; next: what the swap still lacks — gradients, shadows, wrapped text, a layout |
| 8 | The swap: delete gl, rename gx to gl, rebuild every backend | together |

The first check of phases 2–5: a card that changes its colour, and only its
rectangle goes to the screen.

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
*Decided by the owner, 2026-11-18* (section 4.1):

- **One entity type** — a layer, a card, a label and an effect are the same
  struct; the **class**, a constant table of slots, says what an entity is and
  how it behaves. No kind enum, no type per widget.
- **The memory is the application's** — the owner, 2026-11-17 and
  2026-11-18: wherever the application put the entity; gx never stores one
  (decision 3, section 2.2).
- **Many bricks in the code, one record in memory** — every concern is its own
  brick, embedded in the entity by value, never wired by pointers at run time.
- **Every control is a composition of entities**, and an effect is an entity
  beside its owner.

**2. Where the colours live.**
*Decided by the owner, 2026-11-18: a theme — an array of flat style records the
application owns, usually in flash — and the entity points at one.*
One place to change how the screen looks, and the entity stays small. The
alternatives were per-entity colours (fifty entities later the palette is fifty
copies of one palette) or the gl chain (three structs per entity).

**2.1. Who keeps the damage.**
*Decided by the owner, 2026-11-18: the renderer* (section 5.1), replacing the
decision of 2026-11-17 that gave it to the canvas. The canvas is a contract
with no memory: it knows how to draw, not where and not what was drawn
before.

**3. How the tree is stored.**
*Decided by the owner, 2026-11-18: gx never stores an entity* (section 2.2).
It replaces the decision of 2026-11-17 (an array the tree indexes into).

1. The tree only links: the links are inside the entity, on `lh_list_t`, and
   the tree owns the order, never the bytes.
2. Where the entity lives is the application's: a field of its struct, a
   static array, or an `lh_array_t` — the same to gx.
3. Running out of memory is answered by whoever allocates — the application,
   through `lh_array_try_reserve` and the like; gx asks for nothing.
4. An `lh_array_t` of entities is reserved once, before they are attached, and
   never grown afterwards.
5. No store in gx. A pool of one-size slots, if one is ever needed, is a core
   brick of its own.

**4. What leaves gl before the swap.**
*Recommendation: yes — the web host to `lh/os/web/`, the 3D renderer to its
own part.*
Neither is a user interface, and both are easier to move while gl is still
there to test against. The cost is a bigger diff in the middle of the work.

## 9. The debt this page creates

Until the swap, the tree holds both `lh/gl/` and `lh/gx/`, and the names
`lh_gl_point_t` and `lh_gx_point_t` exist side by side over one math brick.
That is the transitional debt of rule 3; it is listed in `docs/cleanup.md`
and it ends with the swap in phase 8.
