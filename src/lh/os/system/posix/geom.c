/**
 * @file geom.c
 * @brief Xlib native ↔ lh::geom conversions for the Linux/X11 backend.
 *
 * Xlib core has no `Rect` type — every XEvent variant carries its own
 * `(x, y, width, height)` as plain `int`s. These helpers just pack /
 * unpack that quartet into `lh_rect_t` at the API boundary so the rest of
 * lh and pa only ever sees `lh_rect_t`.
 *
 * Pixels are 24-bit unsigned (`0xRRGGBB`) on every X server since the
 * 1990s; alpha requires XRender (which we explicitly don't use).
 */

#include <lh/color.h>
#include <lh/geom.h>
#include <lh/numeric/types.h>
#include <lh/os/system/posix/geom.h>

void
lh_os_system_posix_rect_from_lh(const lh_rect_t *self,
                                 lh_int_t *out_x, lh_int_t *out_y,
                                 lh_uint_t *out_width, lh_uint_t *out_height)
{
    if (out_x != 0) { *out_x = self->origin.x; }
    if (out_y != 0) { *out_y = self->origin.y; }
    if (out_width != 0) { *out_width = (lh_uint_t)self->size.width; }
    if (out_height != 0) { *out_height = (lh_uint_t)self->size.height; }
}

lh_rect_t
lh_os_system_posix_rect_to_lh(lh_int_t x, lh_int_t y,
                               lh_uint_t width, lh_uint_t height)
{
    return lh_rect_make(x, y, (lh_coord_t)width, (lh_coord_t)height);
}

lh_ulong_t
lh_os_system_posix_color_to_lh(lh_color_t self)
{
    /* `0xRRGGBB` — top byte zero, like every other 24-bit X server. */
    return ((lh_uint_t)self.r << 16)
        | ((lh_uint_t)self.g << 8)
        | ((lh_uint_t)self.b);
}