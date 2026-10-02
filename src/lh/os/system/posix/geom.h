/**
 * @file geom.h
 * @brief Backend-private: lh::geom ↔ X11 native conversions.
 *
 * Used by `src/lh/os/system/posix/window.c` and the future paint / input
 * backends. Not installed.
 *
 * X11 doesn't have a single `Rect` type — every XEvent variant exposes its
 * own `(x, y, width, height)` quartet as plain ints. The helpers here
 * pack / unpack those into `lh_rect_t` at the boundary.
 *
 * Xlib pixels are 24-bit unsigned (no alpha in the basic XImage /
 * `XCreateGC` path; alpha requires `XRender` which we don't use here).
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_GEOM_H
#define LH_SRC_OS_SYSTEM_POSIX_GEOM_H

#include <lh/color.h>
#include <lh/geom.h>
#include <lh/numeric/types.h>
#include <lh/os/system/posix/x11.h>

/**
 * @brief Convert `lh_rect_t` to Xlib's `(x, y, width, height)` quartet.
 */
void
lh_os_system_posix_rect_from_lh(const lh_rect_t *self,
                                 lh_int_t *out_x, lh_int_t *out_y,
                                 lh_uint_t *out_width, lh_uint_t *out_height);

/**
 * @brief Convert Xlib's `(x, y, width, height)` quartet to `lh_rect_t`.
 */
lh_rect_t
lh_os_system_posix_rect_to_lh(lh_int_t x, lh_int_t y,
                               lh_uint_t width, lh_uint_t height);

/**
 * @brief Convert `lh_color_t` to a 24-bit Xlib pixel value (`0xRRGGBB`).
 *        Alpha is dropped — Xlib core has no alpha in `XSetForeground`.
 */
lh_ulong_t
lh_os_system_posix_color_to_lh(lh_color_t self);

#endif /* LH_SRC_OS_SYSTEM_POSIX_GEOM_H */