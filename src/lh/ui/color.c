#include <lh/ui/color.h>
#include <lh/numeric/types.h>
#include <lh/cast/static.h>
#include <lh/util/numeric.h>

lh_ui_color_t
lh_ui_color_make(lh_byte_t r, lh_byte_t g, lh_byte_t b, lh_byte_t a)
{
    lh_ui_color_t c = { r, g, b, a };
    return c;
}

lh_ui_color_t
lh_ui_color_from_argb(lh_uint_t argb)
{
    const lh_uint_t byte_max = lh_numeric_limit_umax(lh_byte_t);
    lh_ui_color_t c;
    c.a = lh_cast_static(lh_byte_t, (argb >> 24) & byte_max);
    c.r = lh_cast_static(lh_byte_t, (argb >> 16) & byte_max);
    c.g = lh_cast_static(lh_byte_t, (argb >> 8) & byte_max);
    c.b = lh_cast_static(lh_byte_t, argb & byte_max);
    return c;
}

lh_uint_t
lh_ui_color_to_argb(lh_ui_color_t self)
{
    return ((lh_uint_t)self.a << 24)
        | ((lh_uint_t)self.r << 16)
        | ((lh_uint_t)self.g << 8)
        | ((lh_uint_t)self.b);
}
