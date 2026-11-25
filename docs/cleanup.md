# Cleanup — the debt of lh

What the rules of `AGENTS.md` ask for that the code does not yet do, and what
the owner has decided to do about it. A debt ends here when it is paid, and
the entry is removed with it.

## The 3D renderer

**The debt.** `src/lh/gl/renderer3d.c` is a 3D renderer, not a user
interface, and does not belong under `lh/gl/` once that name means the
interface layer.

**The plan.** The 3D renderer gets its own part beside `lh/gl/`. (The web host, the other half of this debt, moved to
`lh/os/web/` on 2026-11-18, and the WebSocket frames to `lh/core/web/ws.h`.)

## The built-in font lives in gl

**The debt.** The baked Roboto (`lh/gl/font/roboto.h`) and the call that hands
it out (`lh_gl_font_get_default`) are gl's, though the table type is the
core's (`lh_font_t`), so a program that wants only the font and the core's
mask still includes `lh/gl/font.h`. A declaration `lh_font_get_default` in
`lh/core/font.h` promised the same from the core but had no definition; it was
removed on 2026-11-18.

**The plan.** The table and its getter move to the core's font.

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

## The core gradient takes gl's option

**The debt.** The gradient moved to `lh/core/pixel/gradient.h` (2026-11-19),
but the number of stops it holds is still `LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS`:
a core type sized by an option named after gl.

**The plan.** Rename the option to `LH_LIBRARY_OPTION_GRADIENT_MAX_STOPS` and
keep the old name as an alias for one release.

## gl's 16-bit SIMD kernels

**The debt.** The 32-bit alpha kernels moved to the core
(`lh_pixel_span_blend_alphas_argb`, 2026-11-19), and gl calls them there. The
RGB565 ones (`lh_gl_pixmap_blend_alpha_16_sse2` and its octet) stay in gl with a
copy of `gather_alpha`, and the core's RGB565 rows are scalar.

**The plan.** Move them beside the 32-bit ones (`lh_pixel_span_cover_rgb565`
choosing them as `cover_argb` does), and gl calls the core.
