# lh/gl - handoff and roadmap

Read this first, then `AGENTS.md`. This is the state of the new graphics core
as of 2026-11-14 and the plan agreed with Egor.

## Where and why

- Repo: `H:\Projects\raspad-authum\lib\lh` (LH 0.5.0, submodule of raspad-authum;
  pult-lh and f4ve-bare also use LH).
- `lh/gl` (`include/lh/gl`, `src/lh/gl`, `test/src/lh/gl`, test target
  `lh_test_gl`) is a from-scratch rewrite of the UI engine. `lh/ui` is legacy:
  do not touch it, it will be deleted once gl replaces it.
- Targets: Windows/Linux desktop, the ventilation control panel (uClinux,
  no MMU, kernel 6.12, heap fragmentation is a real, painful problem) and
  bare-metal STM32F407 with no OS (H:\Projects\f4ve-bare, TFT).
- Goal: rebuild the work on the scheme below, then try it on the real panel
  and measure how it looks and what it costs.

## Concept (agreed)

- **point** `lh_gl_point_t`: two fields x, y (`lh_math_scalar_t`), via
  `lh_math_point_fields`. 2D only.
- **size** `lh_gl_size_t`: `typedef` of point, own methods (width/height).
- **rect**: origin (point) + size, via `lh_math_rect_fields`.
- **entity** `lh_gl_entity_t`: rect (relative to parent) + parent + class
  table (kind, draw callback) + intrusive `lh_list_node_t`. It knows nothing
  about drivers, input, styles or events.
- **layer** `lh_gl_layer_t`: an entity as its first field + an intrusive
  `lh_list_t` of children. Only a layer can own an entity; an entity's parent
  is `lh_gl_layer_t *`. Layers nest (a layer is an entity). List order is
  z-order: first = back, last = front. View, scene, window are layers; the
  root has no parent.
- **Memory (important, decided against the LVGL model):** a layer never
  allocates or frees children. It only links/unlinks nodes and sets/clears
  parent; `lh_gl_layer_deinit` unlinks children and clears their parent.
  Entity memory belongs to its creator: static, stack, a fixed-size pool, or
  one allocation per window holding all its widgets. Reason: no heap
  fragmentation, memory known at link time, add/remove O(1) and cannot fail.
  Debt: debug assert that an entity being deinit'ed has no parent.
- **Composition, not LVGL's fat lv_obj:** input, scroll, container, layout
  are separate managers that move entities' rects. Widgets are composed
  (label + image = button).
- **canvas**: portable drawing abstraction (pen, rect, circle, rounded rect,
  border, fill), a "half driver" that knows how to draw but has no output.
  The native target (HDC, X11/GL, STM32 framebuffer/TFT) is an opaque handle
  inside the canvas implementation. A layer is given a canvas when asked to
  draw. Only `canvas/decl.h` exists. Not now.
- **renderer as a server**: the UI sends it packets (dirty areas, draw
  requests), possibly via rail mailbox/queue. Rail (per-device HAL: bare
  base, mailbox, queues, loop) is the HAL. Must run on bare STM32. Later.

## Done (uncommitted unless Egor committed it)

- point, size, rect: headers, src, tests, CHANGELOG. Egor commits these.
- entity, entity class, layer on `lh_list_t`: staged by Egor, with unstaged
  edits on top (list switch). Checkers, clang-format 19, gcc -fsyntax-only
  pass. Tests NOT run since the list switch.
- Known pre-existing warnings: `check-fields` on `self->cls` in
  `entity.c` get/set_class; `entity.c` function order vs levels.
- `docs/roadmap.md` section 5: a general allocator with a real free over a
  static buffer (TLSF-like, as LVGL's builtin heap), plugged via
  `lh_memory_allocator_t` / `*_init_by`. LH has no allocator of its own today.

## Next, in order

1. Build and run `lh_test_gl` once, fix what fails (Egor: run tests only at
   checkpoints or before a commit, not on every change).
2. Dirty regions in the layer, on a fixed array (no allocation):
   - `lh_gl_entity_invalidate(self)` and rect setters report old and new
     area to `parent` in parent coordinates;
   - layer accumulates areas: clip to own rect, drop areas inside an
     existing one, merge overlapping/adjacent when the union is not much
     bigger, collapse to one bounding rect past N areas;
   - propagate up to the root, which hands the list to the renderer.
3. Later, with the renderer: skip children fully covered by an opaque
   sibling, draw only children intersecting the area, banded partial buffer
   + DMA on STM32, deferred redraw per frame.
4. canvas abstraction; renderer server and packets over rail; managers
   (input, scroll, layout); try on the panel.

## Rules that matter to Egor

- Follow `AGENTS.md`: bricks, fields private (level-1 accessors only), one
  file per entity, names from the path (`lh/gl` -> `lh_gl_`), only the
  includes actually used (no duplicates), `@file`/`@brief` docs, CHANGELOG
  `[Unreleased]`, LF endings, no parallel implementations.
- Don't invent concepts or API that were not asked for; if a method is
  missing, ask.
- A question is a question: answer, don't change code.
- Never `git add` or commit; Egor stages and commits himself.
- LH must not mention devices, the panel or stand addresses.
- Replies to Egor: Russian, brief, with numbers.
## Dirty regions (decided)

- `LH_LIBRARY_OPTION_GL_LAYER_DIRTY_MAX` is a CMake cache option, defaults to 8,
  and must be a positive integer.
  Each layer has fixed storage and a count; only the root accumulates regions.
- Areas are clipped to `(0, 0, width, height)` and empty areas are dropped.
  A non-root layer offsets the clipped area by its origin and forwards it.
- A bounding union is merged when its area is at most the sum of the two areas.
  On overflow all regions and the new area collapse to one bounding rectangle.
- `lh_gl_layer_invalidate`, `lh_gl_layer_get_dirty_count`,
  `lh_gl_layer_get_dirty`, and `lh_gl_layer_clear_dirty` expose the root list.
  `lh_gl_entity_invalidate` forwards the entity's rectangle to its parent;
  its rectangle, origin and size setters invalidate the old and new rectangles.
