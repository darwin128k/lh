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

- **scalar** `lh_gl_scalar_t`: the integer screen coordinate. It is
  `lh_math_scalar_t` and does not follow the FPU option. 3D stays on
  `lh_math_fscalar_t`.
- **point** `lh_gl_point_t`: two fields x, y (`lh_gl_scalar_t`), via
  `lh_math_point_fields`. 2D only.
- **size** `lh_gl_size_t`: `typedef` of point, own methods (width/height).
- **rect**: origin (point) + size, via `lh_math_rect_fields`.
- **entity** `lh_gl_entity_t`: rect (relative to parent) + parent + class
  table (kind, draw callback, opaque flag) + intrusive `lh_list_node_t`. It knows nothing
  about drivers, input, styles or events. Opaque means the rectangle covers every pixel.
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
- **Composition, not LVGL's fat lv_obj:** input, scroll, container and layout
  are separate objects. Layout moves rectangles. A scroll axis is one number,
  and a container borrows a layer plus up to two axes; it places children at
  the window origin minus those numbers and does not write the rectangles.
  Widgets are composed (label + image = button).
- **canvas**: portable drawing abstraction (pen, rect, circle, rounded rect,
  border, fill), a half driver that knows how to draw but has no output.
  The native target is an opaque handle plus a call table
  (`lh_gl_canvas_backend_t`). The canvas adds its origin, drops a shape that
  misses the clip, and forwards the rest. It does not name a device.
- **renderer as a server**: the UI sends it packets (dirty areas, draw
  requests), possibly via rail mailbox/queue. Rail (per-device HAL: bare
  base, mailbox, queues, loop) is the HAL. Must run on bare STM32. Later.

## Done

- point, size, rect, entity, layer, fixed-storage surface dirty regions, layer z-order, entity coordinate conversion, and clipped layer drawing.
- Renderer: one deferred pass over the surface dirty list. A positive band slices each rectangle into horizontal stripes. Children that miss the clip are skipped, and so is a child whose visible area is fully inside a later opaque sibling. The class flag `opaque` says so. `lh_gl_rect_contains` is the cover test.
- Canvas primitives: fill and stroke of a rectangle, a circle and a rounded rectangle, through `lh_gl_color_t`, `lh_gl_pen_t` and `lh_gl_canvas_backend_t`.
- Pixmap: a borrowed grid of those colors. One canvas call fills a rectangle by blending (`lh_gl_color_over`). Alpha 255 replaces the pixel. No other shapes. `lh_gl_bmp_try_store` writes that grid as a 32-bit bitmap a viewer opens. Alpha is copied.
- Point hit: `lh_gl_layer_find_entity` walks front to back and returns the deepest entity whose rectangle contains the point. No child means the layer itself. Outside the layer is null. `opaque` is draw occlusion only.
- Input hold: `lh_gl_input_t` stores one borrowed entity. Press uses the point hit. Move does not search again. Release clears the hold. No keys and no device.
- Scroll axis: one offset, not stored on an entity or a layer. Container: borrowed layer plus a horizontal axis, a vertical axis, or neither. Placement is the window origin minus the offsets. Rectangles stay put. `lh_gl_container_invalidate` marks the window; changing an axis does not.
- Layout: `lh_gl_layout_t` is one gap. Pack writes direct children's origins along one axis from zero, in list order. The other axis and the sizes stay. It does not draw, search or scroll.
- Fill: `lh_gl_fill_t` starts with an entity and borrows one color. The shared draw call reads that pointer. A layer's own draw call stays null; a background is a fill added first. The fill class is not opaque.
- Gradient: two colors and an angle on the fill. `180` runs down the box, `135` runs down-right. `radial` puts the first color at the middle and the second toward the corners; the angle is then unused. A radius clips that paint to the rounded rectangle; `0` leaves the rectangle.
- Cover: a pixel center is inside a rounded rectangle, a circle or a triangle, or it is outside. The pixmap fills those three shapes and strokes the first two plus a plain rectangle, outside the fill, one pen thick. The edge is hard. A triangle has no stroke.
- Shadow: a peak color, a spread and a shift. The box interior stays clear. The fade crosses the shifted edge. Corners are square.
- Known pre-existing warnings: `check-fields` on `self->cls` in
  `entity.c` get/set_class; `entity.c` function order vs levels.
- `docs/roadmap.md` section 5: a general allocator with a real free over a
  static buffer (TLSF-like, as LVGL's builtin heap), plugged via
  `lh_memory_allocator_t` / `*_init_by`. LH has no allocator of its own today.

## Next, in order

1. The same 480×320 card scene still needs rounded shadow corners and text.
   A linear gradient takes an angle: `180` down, `135` down-right. A radial
   gradient puts the first color at the middle.
2. Time that scene beside the existing `bench/lvgl` numbers.
3. A scene stays an ordinary child layer until a swipe and a back stack need
   their own behaviour. Renderer packets over rail when another loop draws.
   Try on the panel after that.

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
