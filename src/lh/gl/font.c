/**
 * @file font.c
 * @brief Implementation of `lh/gl/font.h`.
 */
#include <lh/core/assert.h>

#include <lh/config.h>
#include <lh/gl/font.h>
#include <lh/core/util/return.h>

#if LH_LIBRARY_OPTION_GL_FONT_ROBOTO
#    include <lh/gl/font/roboto.h>
#endif

const lh_gl_font_t *
lh_gl_font_get_default(lh_void)
{
#if LH_LIBRARY_OPTION_GL_FONT_ROBOTO
    return lh_addr_of(lh_gl_font_roboto);
#else
    return lh_null;
#endif
}

lh_bool_t
lh_gl_font_runs_are_a_runs(const lh_gl_font_range_t *ranges, lh_u32_t count)
{
    return lh_font_runs_are_a_runs(ranges, count);
}

lh_void
lh_gl_font_init(lh_gl_font_t *self, const lh_gl_mask_t *glyphs, lh_byte_cptr advances,
                const lh_s32_t *tops, const lh_gl_font_range_t *ranges, lh_s32_t line_height,
                lh_s32_t ascent, lh_s32_t cap_height, lh_u32_t range_count)
{
    lh_font_init(self, glyphs, advances, tops, ranges, line_height, ascent, cap_height,
                 range_count);
}

lh_s32_t
lh_gl_font_get_ascent(const lh_gl_font_t *self)
{
    return lh_font_get_ascent(self);
}

lh_s32_t
lh_gl_font_get_cap_height(const lh_gl_font_t *self)
{
    return lh_font_get_cap_height(self);
}

lh_s32_t
lh_gl_font_get_line_height(const lh_gl_font_t *self)
{
    return lh_font_get_line_height(self);
}

lh_u32_t
lh_gl_font_get_range_count(const lh_gl_font_t *self)
{
    return lh_font_get_range_count(self);
}

const lh_gl_font_range_t *
lh_gl_font_get_range(const lh_gl_font_t *self, lh_u32_t index)
{
    return lh_font_get_range(self, index);
}

lh_u32_t
lh_gl_font_get_first(const lh_gl_font_t *self)
{
    return lh_font_get_first(self);
}

lh_u32_t
lh_gl_font_get_glyph_count(const lh_gl_font_t *self)
{
    return lh_font_get_glyph_count(self);
}

/** Which run holds @p code, or `lh_null`. The one place a code is looked up, because
   ::lh_gl_font_has_code and ::lh_gl_font_get_index asking it two different ways is how
   they come to answer two different things -- the whole defect this format exists to
   allow for is a code that is in one run and not the other.

   The runs are sorted, so a run that starts past @p code ends the search: everything
   after it starts higher still. */
const lh_gl_font_range_t *
lh_gl_font_get_range_of(const lh_gl_font_t *self, lh_u32_t code)
{
    return lh_font_get_range_of(self, code);
}

lh_bool_t
lh_gl_font_find_index(const lh_gl_font_t *self, lh_u32_t code, lh_u32_t *index)
{
    return lh_font_find_index(self, code, index);
}

lh_s32_t
lh_gl_font_get_advance_at(const lh_gl_font_t *self, lh_u32_t index)
{
    return lh_font_get_advance_at(self, index);
}

lh_s32_t
lh_gl_font_get_top_at(const lh_gl_font_t *self, lh_u32_t index)
{
    return lh_font_get_top_at(self, index);
}

const lh_gl_mask_t *
lh_gl_font_get_glyph_at(const lh_gl_font_t *self, lh_u32_t index)
{
    return lh_font_get_glyph_at(self, index);
}

lh_bool_t
lh_gl_font_has_code(const lh_gl_font_t *self, lh_u32_t code)
{
    return lh_font_has_code(self, code);
}

lh_u32_t
lh_gl_font_get_index(const lh_gl_font_t *self, lh_u32_t code)
{
    return lh_font_get_index(self, code);
}

lh_s32_t
lh_gl_font_get_advance(const lh_gl_font_t *self, lh_u32_t code)
{
    return lh_font_get_advance(self, code);
}

lh_s32_t
lh_gl_font_get_top(const lh_gl_font_t *self, lh_u32_t code)
{
    return lh_font_get_top(self, code);
}

lh_s32_t
lh_gl_font_get_ink_top(const lh_gl_font_t *self, lh_u32_t code)
{
    return lh_font_get_ink_top(self, code);
}

lh_bool_t
lh_gl_font_get_glyph(const lh_gl_font_t *self, lh_u32_t code, lh_gl_mask_t *mask)
{
    return lh_font_get_glyph(self, code, mask);
}

lh_s32_t
lh_gl_font_get_ink_bottom(const lh_gl_font_t *self, lh_u32_t code)
{
    return lh_font_get_ink_bottom(self, code);
}

const lh_gl_mask_t *
lh_gl_font_get_glyphs(const lh_gl_font_t *self)
{
    return lh_font_get_glyphs(self);
}

lh_void
lh_gl_font_set_glyphs(lh_gl_font_t *self, const lh_gl_mask_t *glyphs)
{
    lh_font_set_glyphs(self, glyphs);
}

const lh_byte_t *
lh_gl_font_get_advances(const lh_gl_font_t *self)
{
    return lh_font_get_advances(self);
}

lh_void
lh_gl_font_set_advances(lh_gl_font_t *self, const lh_byte_t *advances)
{
    lh_font_set_advances(self, advances);
}

const lh_s32_t *
lh_gl_font_get_tops(const lh_gl_font_t *self)
{
    return lh_font_get_tops(self);
}

lh_void
lh_gl_font_set_tops(lh_gl_font_t *self, const lh_s32_t *tops)
{
    lh_font_set_tops(self, tops);
}

const lh_gl_font_range_t *
lh_gl_font_get_ranges(const lh_gl_font_t *self)
{
    return lh_font_get_ranges(self);
}

lh_void
lh_gl_font_set_ranges(lh_gl_font_t *self, const lh_gl_font_range_t *ranges)
{
    lh_font_set_ranges(self, ranges);
}

lh_void
lh_gl_font_set_line_height(lh_gl_font_t *self, lh_s32_t line_height)
{
    lh_font_set_line_height(self, line_height);
}

lh_void
lh_gl_font_set_ascent(lh_gl_font_t *self, lh_s32_t ascent)
{
    lh_font_set_ascent(self, ascent);
}

lh_void
lh_gl_font_set_cap_height(lh_gl_font_t *self, lh_s32_t cap_height)
{
    lh_font_set_cap_height(self, cap_height);
}

lh_void
lh_gl_font_set_range_count(lh_gl_font_t *self, lh_u32_t range_count)
{
    lh_font_set_range_count(self, range_count);
}
