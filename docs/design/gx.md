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

1. **Entity.** One thing on the screen is one entity. Not a family of widget
   types with a type each: a label, a button, a container are entities that
   carry different layers.
2. **Layers.** A layer is an entity that holds entities — the same node, with
   children. There is no second type for it: the class says what a node is,
   and the tree holds both. What an entity shows and does is not a chain of
   style objects pointing at each other but its own fields: a theme entry, a
   text, a set of callbacks, a set of flags.
3. **No allocation.** gx never allocates. Every byte it uses is memory the
   application provided.
4. **Ownership.** Where nothing is allocated, ownership is the only thing
   left to say — so every gx header says who owns what, and for how long.
5. **No operating system.** gx runs on a board with no OS — on
   `lh/rail/bare.h`, one core and its interrupts — as well as on a desktop.
   Every table in it has a fixed size, nothing in it waits, nothing in it
   reads a clock, and nothing in it calls the platform. The window, the
   caption, the size and the flush belong to `lh/os/`; gx is above them and
   below the application.

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

    lh_gx_rect_init(lh_addr_of(r), 0, 0, 480, 320);
    root = lh_gx_tree_add(lh_addr_of(s_tree), lh_addr_of(r));

    lh_gx_rect_init(lh_addr_of(r), 24, 24, 400, 28);
    title = lh_gx_tree_add(lh_addr_of(s_tree), lh_addr_of(r));
    lh_gx_entity_set_text(title, "Hello, lh");
    lh_gx_tree_add_child(lh_addr_of(s_tree), root, title);

    lh_gx_rect_init(lh_addr_of(r), 24, 72, 120, 36);
    ok = lh_gx_tree_add(lh_addr_of(s_tree), lh_addr_of(r));
    lh_gx_entity_set_text(ok, "OK");
    lh_gx_entity_set_on_click(ok, on_ok, lh_addr_of(s_ok_pressed));
    lh_gx_tree_add_child(lh_addr_of(s_tree), root, ok);

    return lh_bool_true;
}
```

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

## 4.1 The node: what an entity is and where it lives

The owner's model, 2026-11-17: an entity has a class, a place, a way of
looking, callbacks that are called, and state flags. A layer is the same
entity — one that has children. The tree holds both, and the class says which
is which.

**One type, one array.** The application provides an array of entities; the
tree indexes into it. A child is an index, not a pointer: the node holds its
parent, its first child, the number of its children and its next sibling. So
there is no allocation, no pointer to chase, every node is the same size, and
a node keeps its address for as long as the tree lives. A layer is an entity
whose class may hold children — nothing more, and the tree does not care
which is which.

**The node** (`lh/gx/entity.h`):

| field | what | bytes |
|---|---|---|
| `kind` | the class: label, button, layer, … | 1 |
| `flags` | visible, enabled, invalid, hovered, pressed, focused, … | 4 |
| `rect` | where the entity is | 16 |
| `style` | how it looks: an index into the theme | 2 |
| `text` | what it says | 16 |
| `parent`, `first_child`, `child_count`, `next` | the tree links | 16 |
| `callbacks` | the reactions: kind, function, context, two slots | 48 |
| `user` | the application's own pointer to its own state | 8 |

That is **120 bytes** for a node with two callback slots, 96 with one, 72 with
none. Against gl: one entity is 136 bytes, and the title it needs — entity,
style, paint, colour — is **316 bytes in four objects**. So the node that
carries the class, the place, the look, the text, the flags and the callbacks
is less than a third of the size of gl's chain, and it is one object instead of
four.

The theme holds the look: colours, font, radius — one entry per kind, plus
the entries the application wants to name. The node says which entry is its,
so changing how the whole screen looks is changing the theme, not touching
fifty entities.

**The tree** (`lh/gx/tree.h`) owns the array and the links; the application
owns the bytes. The tree hands out slots, links children, and walks the array
in order — the same walk for an entity and for a layer.

The app program from section 4 stays as it is, and the callback is one call:

```c
lh_gx_entity_set_callback(ok, lh_gx_event_kind_clicked, on_ok, lh_addr_of(s_ok_pressed));
```

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
7. **The draw list** (`lh/gx/draw.h`) — what the paint pass produces: a flat
   array of commands with their own bounds. One contract with the platform,
   and the seam where the 3D renderer will draw later.
8. **The backends** (`lh/gx/backend/`) — `sw` paints the list into pixels,
   `web` writes the list as JSON. The browser replays the list; it does not
   re-implement the renderer. This is what kills the 3329 lines.

## 5.1 What makes drawing fast

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
pixels and asks for the damage.

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
| 0 | The unit: scalar, point, size, rect | **done** |
| 1 | The application program — the spec, and the four decisions below | together |
| 2 | The tree and the entity | me |
| 3 | The layers | me |
| 4 | The passes: measure, layout, paint, hit | me |
| 5 | The draw list and the backends: sw, web | me |
| 6 | The window, the MCU path, the examples, the benches | me |
| 7 | The swap: delete gl, rename gx to gl, rebuild every backend | together |

From phase 2 on I work alone: every brick with its own test, the build green
after every brick, `CHANGELOG.md` updated as the public surface grows, and
the numbers of any performance work measured and written down.

## 8. The four decisions of phase 1

Each one is a choice the owner makes; the recommendation is mine and the
reason is given, so that a disagreement is a disagreement about the reason.

**1. The node: one type, and where a layer lives.**
*Decided by the owner, 2026-11-17: one node type.* `lh_gx_entity_t` is the
node; there is no second type for a node that has children. A node with
children and a node without are the same thing — the children are links in the
tree, and a node without them has none. The cost of the children is four
numbers in the node (parent, first child, child count, next sibling), not an
array of pointers, so the leaf pays little for being a leaf.

*Still open: where a layer lives.*
*Recommendation: inside the entity, one slot per layer kind.*
The entity owns its layers, the application owns the entities: two levels of
ownership, both explicit, and nothing to keep alive separately. The cost is
that a layer cannot be shared between entities — a hundred cards share one
style only if the style is in the theme, not in the layer. The alternative is
a layer object the application owns and the entity borrows; that buys sharing
and costs a pointer and a lifetime rule per layer — and it is the shape gl
already has, with `color → paint → style → entity` in front of it.

**2. Where the colours live.**
*Recommendation: a theme, a table the application owns, and the entity names
a style by index.*
One place to change how the screen looks, and the entity stays small. The
alternative is per-entity colours (simple, but fifty entities later the
palette is fifty copies of one palette) or the gl chain (three structs per
entity).

**3. How the tree is stored.**
*Recommendation: the application provides an array of entities; the tree
indexes into it and hands out slots.*
No allocation, the size is known at build time, an entity is addressable and
stable, and the application can keep a pointer to any of its own entities.
The alternative is the tree owning an arena of its own (also no allocation,
but then the application cannot hold an entity pointer).

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
