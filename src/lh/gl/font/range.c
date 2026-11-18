/**
 * @file range.c
 * @brief Implementation of `lh/gl/font/range.h`.
 */

#include <lh/core/assert/runtime.h>
#include <lh/gl/font/range.h>

lh_void
lh_gl_font_range_init(lh_gl_font_range_t *self, lh_u32_t first, lh_u32_t length, lh_u32_t base)
{
    lh_font_range_init(self, first, length, base);
}

lh_u32_t
lh_gl_font_range_get_first(const lh_gl_font_range_t *self)
{
    return lh_font_range_get_first(self);
}

lh_u32_t
lh_gl_font_range_get_length(const lh_gl_font_range_t *self)
{
    return lh_font_range_get_length(self);
}

lh_u32_t
lh_gl_font_range_get_base(const lh_gl_font_range_t *self)
{
    return lh_font_range_get_base(self);
}

lh_u32_t
lh_gl_font_range_get_last(const lh_gl_font_range_t *self)
{
    return lh_font_range_get_last(self);
}

lh_bool_t
lh_gl_font_range_has(const lh_gl_font_range_t *self, lh_u32_t code)
{
    return lh_font_range_has(self, code);
}

lh_void
lh_gl_font_range_set_first(lh_gl_font_range_t *self, lh_u32_t first)
{
    lh_font_range_set_first(self, first);
}

lh_void
lh_gl_font_range_set_length(lh_gl_font_range_t *self, lh_u32_t length)
{
    lh_font_range_set_length(self, length);
}

lh_void
lh_gl_font_range_set_base(lh_gl_font_range_t *self, lh_u32_t base)
{
    lh_font_range_set_base(self, base);
}
