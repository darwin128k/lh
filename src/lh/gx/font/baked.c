/**
 * @file baked.c
 * @brief Implementation of `lh/gx/font/baked.h`.
 */

#include <lh/gx/font/baked.h>

#include <lh/core/cast/const.h>
#include <lh/core/util/return.h>

lh_bool_t
lh_gx_font_baked_get_glyph(lh_ptr context, lh_u32_t code, lh_gx_glyph_t *out)
{
    const lh_font_t *table = lh_ptr_rcast(const lh_font_t, context);
    lh_u32_t index = 0U;

    lh_return_if(!lh_font_find_index(table, code, lh_addr_of(index)), lh_bool_false);
    lh_gx_glyph_init(out, lh_ptr_deref(lh_font_get_glyph_at(table, index)),
                     lh_font_get_advance_at(table, index), lh_font_get_top_at(table, index));
    return lh_bool_true;
}

lh_s32_t
lh_gx_font_baked_get_ascent(lh_ptr context)
{
    return lh_font_get_ascent(lh_ptr_rcast(const lh_font_t, context));
}

lh_s32_t
lh_gx_font_baked_get_line_height(lh_ptr context)
{
    return lh_font_get_line_height(lh_ptr_rcast(const lh_font_t, context));
}

lh_void
lh_gx_font_baked_init(lh_gx_font_t *self, const lh_font_t *table)
{
    lh_gx_font_init(self, lh_gx_font_baked_get_glyph, lh_gx_font_baked_get_ascent,
                    lh_gx_font_baked_get_line_height, lh_cast_const(lh_font_t *, table));
}
