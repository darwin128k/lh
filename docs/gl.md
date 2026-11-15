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
  to one surface. A screen is an ordinary child layer of the root
  (`lh_gl_screen_open`, `lh_gl_screen_back`): the last child is the one in
  front, and back unlinks it. No separate scene type. A gesture is not part
  of that list. Windows and applications belong to the platform, outside `lh/gl`.
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
- **canvas**: the drawing tool. It keeps the target and the backend and
  nothing else. Origin, rectangular clip and mask arrive on `lh_gl_place_t`
  for that call. A fill, a stroke, a circle, a rounded rectangle and a
  triangle scale each covered pixel by the mask.
  It does not name a device.
- **renderer as a server**: the UI sends it packets (dirty areas, draw
  requests), possibly via rail mailbox/queue. Rail (per-device HAL: bare
  base, mailbox, queues, loop) is the HAL. Must run on bare STM32. Later.

## Done

- point, size, rect, entity, layer, fixed-storage surface dirty regions, layer z-order, entity coordinate conversion, and clipped layer drawing.
- Renderer: one deferred pass over the surface dirty list. A positive band slices each rectangle into horizontal stripes. Children that miss the clip are skipped, and so is a child whose visible area is fully inside a later opaque sibling. The class flag `opaque` says so. `lh_gl_rect_contains` is the cover test. `lh_gl_renderer_packet_post` hands that frame to a rail executor: the packet keeps the dirty rectangles, the surface list is cleared, and a full mailbox leaves the list.
- Canvas primitives: fill and stroke of a rectangle, a circle and a rounded rectangle, a straight line and a sector, through `lh_gl_color_t`, `lh_gl_pen_t` and `lh_gl_canvas_backend_t`. The sector's 0 degrees points right and a positive sweep turns toward +y.
- Pixmap: a borrowed grid of those colors. One canvas call fills a rectangle by blending (`lh_gl_color_over`). Alpha 255 replaces the pixel. No other shapes. `lh_gl_bmp_try_store` writes that grid as a 32-bit bitmap a viewer opens. Alpha is copied.
- Point hit: `lh_gl_layer_find_entity` walks front to back and returns the deepest entity that accepts the point. A null class pick means the rectangle. A pick that declines continues to whoever is behind. `lh_gl_layer_find_entity_slop` grows that test by one number. No child means the layer itself. Outside the layer is null. `opaque` is draw occlusion only.
- Input hold: `lh_gl_input_t` stores one borrowed entity and one finger thickness. Press uses the point hit. Move does not search again. Release clears the hold. `lh_gl_input_apply` follows one pointer event: the first pointer down owns the hold, another pointer does not take it, and a key does not either. `lh_gl_input_controller_handle_event_is_handled` is the handler a controller calls itself. A click, a drag or a swipe counts when something is already held.
- Scroll axis: one offset, not stored on an entity or a layer. Container: borrowed layer plus a horizontal axis, a vertical axis, or neither. Placement is the window origin minus the offsets. Rectangles stay put. `lh_gl_container_invalidate` marks the window; changing an axis does not.
- Layout: `lh_gl_layout_t` is one gap. Pack writes direct children's origins along one axis from zero, in list order. The other axis and the sizes stay. It does not draw, search or scroll.
- Fill: `lh_gl_fill_t` starts with an entity and borrows one color. The shared draw call reads that pointer. A layer's own draw call stays null; a background is a fill added first. The fill class is not opaque.
- Blur and glass: `lh_gl_blur_rect` averages a pixmap rectangle in place, two box passes, caller scratch, no allocation. The sums of a row stay inside `lh_gl_blur_window_slide`. An opaque window sums the channels as they are and divides by one factor for that run. `lh_gl_glass` writes that blur only inside the rounded panel, one slide of the covered rows, and paints a tint over it; a corner the arc misses stays. A mask scales that frost and leaves a pixel it misses. A null mask does not. With no blur the tint is a row fill. `lh_gl_glass_t` is the panel. The canvas target is the pixmap it reads. A short scratch draws nothing. The class is not opaque.
- Gradient: two colors and an angle on the fill. `180` runs down the box, `135` runs down-right. `radial` puts the first color at the middle and the second toward the corners; the angle is then unused. A radius clips that paint to the rounded rectangle; `0` leaves the rectangle.
- Cover: a pixel center is inside a rounded rectangle, a circle or a triangle, or it is outside. `lh_gl_cover_round_rect_band` is the same arc with coverage 1–254 on the edge. The pixmap fills those three shapes and strokes the first two plus a plain rectangle, outside the fill, one pen thick. A triangle has no stroke.
- Mask: `lh_gl_mask_t` is one rounded rectangle or a combination of two masks. Intersection is the lesser coverage, union the greater, exclusion the left scaled by `(255 - right) / 255`. A null side covers 0. The layer stores one radius. On draw that radius is a mask in root coordinates, intersected with the mask from outside, and handed to each child on the place. `lh_gl_card_set_radius` sets that radius and clears the face radius, so the fill and the line are rectangles and the mask cuts them. A box shadow stays a sibling outside the mask.
- Shadow: a peak color, a spread and a shift. The box interior stays clear. The fade crosses the shifted edge. A radius on the fill rounds that edge; `0` is square.
- Text: `lh_gl_text_t` borrows a font, a color and a string. `lh_gl_font_t` is one run of `lh_gl_glyph_t`. One byte is one code. A code outside the run is skipped. `lh_gl_text_shadow_t` is a separate entity: the same glyphs, shifted and blurred by a borrowed shadow. A spread of `0` leaves a hard edge. The scratch is the caller's. A press passes through.
- Known pre-existing warnings: `check-fields` on `self->cls` in
  `entity.c` get/set_class; `entity.c` function order vs levels.
- `docs/roadmap.md` section 5: a general allocator with a real free over a
  static buffer (TLSF-like, as LVGL's builtin heap), plugged via
  `lh_memory_allocator_t` / `*_init_by`. LH has no allocator of its own today.

## Window

The window is the one `lh/os` already opens: Win32, X11, or AppKit. `lh/gl` does
not open it. Paint draws the layer into the pixmap, `lh_gl_pixmap_store_words`
writes `0xAARRGGBB` into the existing off-screen surface, and that surface's
present copies it into the paint. Input is the window's controller with
`lh_gl_input_controller_handle_event_is_handled`. There is no tick and no slide.
A resize does not grow the pixmap yet: the frame stays the size it was drawn.

## Next, in order

1. The 24-card scene is in the old range. Release, this machine, 8 frames:
   cards 285 µs, partial 9 (one card on an already drawn frame), vertical 283,
   radial 1249, shadow 1335. Before the row fills those were 5877, 326, 5923,
   8275, 11767. The i7-13700K table is cards 200 µs, partial 9.3, vertical 322,
   radial 1030, shadow 1024. Radial and shadow are still about a third over
   that table. Captions in the scene are `lh_gl_font_caption` (8×12), not
   Roboto 16 px.
2. Screens are the child layers of the root (`lh_gl_screen_open`,
   `lh_gl_screen_back`). The front child is the one drawn on top. A swipe
   stays a gesture on the input and does not move a layer. A frame on another
   loop is `lh_gl_renderer_packet_post`. A picture of the 24-card scene stands
   in for a live frame. Release, this machine, 32 frames after the layer mask:
   9.9 and 10.5 ms a frame. The same scene before the mask was 4.2 and 4.5 ms.
   The flat cross of that mask is now one rectangle, and a flat color run is
   one row: the same 32 frames are 5.3 and 4.6 ms.
   AppKit is written and is not built on this machine.

## Held: a press and a one-line screen

Agreed 2026-11-15. Built the same day. The scene test builds each card with
`lh_gl_card_init`, then a box shadow and a glass panel as siblings.

The entity stays four fields. No `visible`, no clickable, no alpha and no
hit radius on it. `opaque` stays draw occlusion only.

The walk goes front to back. A rectangle miss
skips. A hit asks the class. A decline continues to whoever is behind.
`lh_gl_entity_hit` is that answer for a radius: 0 keeps the rectangle, and a
positive radius asks the rounded shape, so an unpainted corner does not take
the press. A fill, a gradient fill and a card call it. Finger thickness is
one number on the input for the whole screen: the point grows by it, then
the shape is asked again. A small picture with a large target is a layer
whose rectangle is the target and whose ink is a smaller child.

Hide has two strengths. Remove the entity from the layer and it is neither
drawn nor found. Leave it in, and a flag on the painter (beside the color,
not on the entity) makes the draw return and the pick decline.
`lh_gl_card_set_shown` is that one call. A fill, a text and a gradient fill
each store `shown` and `target`. A shadow is built to draw and to decline,
with no call from the application.

The application does not assemble a fill, a radius and a text. Those stay
inside the library. One thing on a screen is `lh_gl_card_init`, then
`lh_gl_layer_add`. The line color is `lh_gl_card_set_ink`. The radius is one
number on the card and becomes the layer mask; the face stays a rectangle.
A box shadow and a glass panel stay separate entities, added around the
card. The pick is one more call in the search that already exists.

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
