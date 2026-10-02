/**
 * @file geom.c
 * @brief Win32 native ↔ lh::geom conversions for the Win32 backend.
 *
 * Single-purpose converters used by the Win32 window / paint / input
 * backends. All three converters are `O(1)` value arithmetic — no
 * allocations.
 *
 * `RECT` uses left/top/right/bottom (right/bottom exclusive), the opposite
 * of `lh_rect_t`'s origin+size. We do the +1/-1 dance here so the public
 * `lh_rect_*` API keeps inclusive-at-origin / inclusive-at-bottom semantics.
 */

#include <lh/cast/static.h>
#include <lh/color.h>
#include <lh/geom.h>
#include <lh/null.h>
#include <lh/os/system/win/geom.h>
#include <lh/os/system/win/user32.h>

void
lh_os_system_win_rect_from_lh(const lh_rect_t *self, lh_os_system_win_rect_t *out)
{
    if (lh_null_eq(self) || lh_null_eq(out))
    {
        return;
    }
    out->left = self->origin.x;
    out->top = self->origin.y;
    /* Win32 `RECT` is half-open: `right`/`bottom` are first pixel OUTSIDE
       the rectangle. `lh_rect_t::size` is the pixel count, which is also
       half-open (column `[origin.x, origin.x + size.width)`), so the
       fields line up without any `+1`. */
    out->right = self->origin.x + self->size.width;
    out->bottom = self->origin.y + self->size.height;
}

lh_rect_t
lh_os_system_win_rect_to_lh(const lh_os_system_win_rect_t *self)
{
    if (lh_null_eq(self))
    {
        return lh_rect_zero();
    }
    lh_rect_t r;
    r.origin.x = self->left;
    r.origin.y = self->top;
    r.size.width = self->right - self->left;
    r.size.height = self->bottom - self->top;
    return r;
}

lh_os_system_win_colorref_t
lh_os_system_win_color_to_lh(lh_color_t self)
{
    /* Win32 `COLORREF` is `0x00BBGGRR`; the high byte is reserved and
       ignored. Alpha is dropped — caller is responsible for any alpha
       blending on top of the painted background. */
    return lh_cast_static(lh_os_system_win_colorref_t,
                          ((lh_uint_t)self.b << 16)
                              | ((lh_uint_t)self.g << 8)
                              | ((lh_uint_t)self.r));
}