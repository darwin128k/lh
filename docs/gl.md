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
  z-order: first = back, last = front. The root has no parent and is attached
  to one surface. A scene is an ordinary child layer of the root; no separate
  scene type is needed until it has its own behaviour. A future separate scene
  manager handles swipes and the back stack by moving or selecting these layers.
  Windows and applications belong to the platform, outside `lh/gl`.
- **surface** `lh_gl_surface_t`: owns the display size and a fixed dirty-region
  array, and borrows a root layer. Attach and resize set the root rect to the
  surface size and invalidate the whole surface; detach keeps the root rect.
  Layers forward clipped areas, translated through their origins, to the surface.
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
  draw. Its origin and clip state are implemented; drawing primitives are later.
- **renderer as a server**: the UI sends it packets (dirty areas, draw
  requests), possibly via rail mailbox/queue. Rail (per-device HAL: bare
  base, mailbox, queues, loop) is the HAL. Must run on bare STM32. Later.

## Done

- point, size, rect, entity, layer, fixed-storage surface dirty regions, layer z-order, entity coordinate conversion, and clipped layer drawing.
- Known pre-existing warnings: `check-fields` on `self->cls` in
  `entity.c` get/set_class; `entity.c` function order vs levels.
- `docs/roadmap.md` section 5: a general allocator with a real free over a
  static buffer (TLSF-like, as LVGL's builtin heap), plugged via
  `lh_memory_allocator_t` / `*_init_by`. LH has no allocator of its own today.

## Next, in order

1. Renderer: skip children fully covered by an opaque sibling, draw only
   intersecting children, and support partial buffers and deferred redraw.
2. Canvas drawing primitives; renderer server and packets over rail; scene, input,
   scroll and layout managers; try on the panel.

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

- Surface storage has `LH_LIBRARY_OPTION_GL_SURFACE_DIRTY_MAX` slots (default 8).
- A surface clips incoming areas to its size, drops empty and contained areas,
  merges when the bounding area is no larger than the sum, and collapses to one
  bounding rectangle on overflow. Layers forward only and own no dirty storage.
- Attach and resize synchronize the root rectangle and invalidate the whole
  surface; detach clears the link without changing the rectangle. Fixed-size
  displays do not need resize. The root does not change its own size.
- Entity geometry changes and layer add, remove and deinit invalidate affected
  areas; try_remove checks ownership and unlinks in O(1).
