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

## The web host and the 3D renderer

**The debt.** `src/lh/gl/web/host*` is transport (HTTP, WebSocket, gzip) and
`src/lh/gl/renderer3d.c` is a 3D renderer; neither is a user interface, and
neither belongs under `lh/gl/` once that name means the interface layer.

**The plan.** The web host moves to `lh/os/web/`; the 3D renderer gets its own
part on the same draw list. Both move while `gl` is still there to test
against — `docs/design/gx.md`, section 6 and decision 4 of phase 1.

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
