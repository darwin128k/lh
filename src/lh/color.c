#include <lh/color.h>
#include <lh/numeric/types.h>
#include <lh/cast/static.h>

lh_color_t
lh_color_make(lh_uchar_t r, lh_uchar_t g, lh_uchar_t b, lh_uchar_t a)
{
    lh_color_t c = { r, g, b, a };
    return c;
}

lh_color_t
lh_color_from_argb(lh_uint_t argb)
{
    lh_color_t c;
    c.a = lh_cast_static(lh_uchar_t, (argb >> 24) & 0xFFu);
    c.r = lh_cast_static(lh_uchar_t, (argb >> 16) & 0xFFu);
    c.g = lh_cast_static(lh_uchar_t, (argb >> 8) & 0xFFu);
    c.b = lh_cast_static(lh_uchar_t, argb & 0xFFu);
    return c;
}

lh_uint_t
lh_color_to_argb(lh_color_t self)
{
    return ((lh_uint_t)self.a << 24)
        | ((lh_uint_t)self.r << 16)
        | ((lh_uint_t)self.g << 8)
        | ((lh_uint_t)self.b);
}
