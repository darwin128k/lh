# Cleanup — the debt of lh

What the rules of `AGENTS.md` ask for that the code does not yet do, and what
the owner has decided to do about it. A debt ends here when it is paid, and
the entry is removed with it.

## The `gl`/`gx` transition

**The debt.** The `lh/gx` part is written beside `lh/gl` and replaces it. Until
the swap, the tree holds both, and the names `lh_gl_point_t`, `lh_gl_size_t`
and `lh_gl_rect_t` exist side by side with `lh_gx_point_t`, `lh_gx_size_t` and
`lh_gx_rect_t` over one math brick of `lh/core/math/`. Rule 3 ("no parallel
implementations, no detours") is bent here on purpose: the two sets of names
are aliases of the same core bricks, not a second implementation, and only one
of them survives.

**The plan.** `docs/design/gx.md`, phase 7: delete `lh/gl/`, rename `lh/gx/`
to `lh/gl/` and `lh_gx_*` to `lh_gl_*`, then rebuild every backend and run
every test. When that is done, this entry goes away with the old names.

**The date.** The owner decided on 2026-11-17 that `gx` replaces `gl`.

## The 3D renderer

**The debt.** `src/lh/gl/renderer3d.c` is a 3D renderer, not a user
interface, and does not belong under `lh/gl/` once that name means the
interface layer.

**The plan.** The 3D renderer gets its own part on the same draw list, moved
while `gl` is still there to test against — `docs/design/gx.md`, section 6 and
decision 4 of phase 1. (The web host, the other half of this debt, moved to
`lh/os/web/` on 2026-11-18, and the WebSocket frames to `lh/core/web/ws.h`.)

## The built-in font lives in gl

**The debt.** The baked Roboto (`lh/gl/font/roboto.h`) and the call that hands
it out (`lh_gl_font_get_default`) are gl's, though the table type is the
core's (`lh_font_t`). gx takes the table through `lh_gx_font_baked_init`, so a
gx program that wants the built-in font includes `lh/gl/font.h`
(`example/gx_web_button.c`). A declaration `lh_font_get_default` in
`lh/core/font.h` promised the same from the core but had no definition; it was
removed on 2026-11-18.

**The plan.** The table and its getter move to the core's font with the swap
(`docs/design/gx.md`, phase 8).

## The window throws the damage away

**The debt.** `src/lh/gl/window.c`, the paint path of `lh_gl_window_t`: it
takes the damage into a local variable and then calls `lh_gl_view_paint`,
which draws the whole frame — the damage is computed and dropped. The
partial path exists (`lh_gl_view_draw_strips`, and `lh_gl_view_draw` with a
damage), but the window does not take it. So the desktop redraws everything
on every paint.

The bench hides this: `bench/lvgl/scene_lh.c` measures the partial path
(`lh_gl_view_draw(&view, &partial)`), which the window never takes. The
"partial" rows of `bench/lvgl/RESULTS.md` therefore describe a path the
product does not use.

**Why it is not a three-line fix.** The damage has to be invalidated at the
moment it appears, and it has to stay pending until the part that changed has
been painted. The window cannot do that with the view's damage alone: it also
has to know what it has invalidated and not yet painted — a second list, in the
window — and the view has no way to reach the OS window to invalidate from
there. That is the work of the part that owns the pixels, and it is why the
first attempt at the fix was taken back: taking the damage at the invalidation
loses whatever changed between the update and the paint.

**The plan.** In gx the canvas owns the damage and the invalidation
(`docs/design/gx.md`, section 5.1): the entity reports, the canvas remembers,
and the canvas is what tells the platform what to repaint. The gl window is
replaced by that same canvas in phase 7, so the fix arrives with it.

**The date.** The owner named it on 2026-11-17.

## The render backend named after GDI

**The debt.** `lh/os/render/backend/gdi.h` describes itself as "the same
screen on every backend that has one — the Win32 one and the X11 one": it
draws into a surface and presents through `lh/os/system/surface.h`, and only
its present is GDI's. The name says Windows where the thing is portable, and
the directory `render/backend/` looks like a family with one member.

**The plan.** Rename it to what it is (`lh_os_render_backend_screen` or
similar) and move the `BitBlt` into `src/lh/os/system/win/`, where the
platform's own calls belong (rule 0.2). The public `lh_gl_window_t` then stops
naming GDI in its own fields.
