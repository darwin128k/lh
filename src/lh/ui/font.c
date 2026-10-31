/**
 * @file font.c
 * @brief Implementation of `lh/ui/font.h`.
 */
#include <lh/assert.h>

#include <lh/config.h>
#include <lh/ui/font.h>
#include <lh/util/return.h>

#if LH_LIBRARY_OPTION_UI_FONT_ROBOTO
#    include <lh/ui/font/roboto.h>
#endif

const lh_ui_font_t *
lh_ui_font_get_default(lh_void)
{
#if LH_LIBRARY_OPTION_UI_FONT_ROBOTO
    return lh_addr_of(lh_ui_font_roboto);
#else
    return lh_null;
#endif
}

lh_bool_t
lh_ui_font_runs_are_a_runs(const lh_ui_font_range_t *ranges, lh_u32_t count)
{
    lh_u32_t i;

    for (i = 0; i < count; ++i)
    {
        if (lh_ui_font_range_get_length(lh_addr_of(ranges[i])) == 0U)
        {
            return lh_bool_false;
        }
        if (i == 0U)
        {
            continue;
        }
        if (lh_ui_font_range_get_last(lh_addr_of(ranges[i - 1U])) >=
            lh_ui_font_range_get_first(lh_addr_of(ranges[i])))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}

lh_void
lh_ui_font_init(lh_ui_font_t *self, const lh_ui_mask_t *glyphs, lh_byte_cptr advances,
                const lh_s32_t *tops, const lh_ui_font_range_t *ranges, lh_s32_t line_height,
                lh_s32_t ascent, lh_s32_t cap_height, lh_u32_t range_count)
{
    lh_assert_runtime_ref(glyphs);
    lh_assert_runtime_ref(advances);
    lh_assert_runtime_ref(tops);
    lh_assert_runtime_ref(ranges);
    lh_assert_runtime_ifn(range_count > 0U && line_height >= 0 && ascent >= 0 &&
                              ascent <= line_height && cap_height >= 0 && cap_height <= ascent &&
                              lh_ui_font_runs_are_a_runs(ranges, range_count),
                          lh_runtime_error_code_invalid_argument);
    lh_ui_font_set_glyphs(self, glyphs);
    lh_ui_font_set_advances(self, advances);
    lh_ui_font_set_tops(self, tops);
    lh_ui_font_set_ranges(self, ranges);
    lh_ui_font_set_line_height(self, line_height);
    lh_ui_font_set_ascent(self, ascent);
    lh_ui_font_set_cap_height(self, cap_height);
    lh_ui_font_set_range_count(self, range_count);
}

lh_s32_t
lh_ui_font_get_ascent(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ascent;
}

lh_s32_t
lh_ui_font_get_cap_height(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->cap_height;
}

lh_s32_t
lh_ui_font_get_line_height(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->line_height;
}

lh_u32_t
lh_ui_font_get_range_count(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->range_count;
}

const lh_ui_font_range_t *
lh_ui_font_get_range(const lh_ui_font_t *self, lh_u32_t index)
{
    lh_assert_runtime_ifn(index < lh_ui_font_get_range_count(self),
                          lh_runtime_error_code_out_of_range);
    return lh_addr_of(lh_ui_font_get_ranges(self)[index]);
}

lh_u32_t
lh_ui_font_get_first(const lh_ui_font_t *self)
{
    return lh_ui_font_get_range_count(self) > 0U
               ? lh_ui_font_range_get_first(lh_addr_of(lh_ui_font_get_ranges(self)[0]))
               : 0U;
}

lh_u32_t
lh_ui_font_get_glyph_count(const lh_ui_font_t *self)
{
    lh_u32_t total = 0U;
    lh_u32_t i;

    for (i = 0; i < lh_ui_font_get_range_count(self); ++i)
    {
        total += lh_ui_font_range_get_length(lh_addr_of(lh_ui_font_get_ranges(self)[i]));
    }
    return total;
}

/** Which run holds @p code, or `lh_null`. The one place a code is looked up, because
   ::lh_ui_font_has_code and ::lh_ui_font_get_index asking it two different ways is how
   they come to answer two different things -- the whole defect this format exists to
   allow for is a code that is in one run and not the other.

   The runs are sorted, so a run that starts past @p code ends the search: everything
   after it starts higher still. */
const lh_ui_font_range_t *
lh_ui_font_get_range_of(const lh_ui_font_t *self, lh_u32_t code)
{
    lh_u32_t i;

    for (i = 0; i < lh_ui_font_get_range_count(self); ++i)
    {
        if (lh_ui_font_range_has(lh_addr_of(lh_ui_font_get_ranges(self)[i]), code))
        {
            return lh_addr_of(lh_ui_font_get_ranges(self)[i]);
        }
        if (lh_ui_font_range_get_first(lh_addr_of(lh_ui_font_get_ranges(self)[i])) > code)
        {
            break;
        }
    }
    return lh_null;
}

lh_bool_t
lh_ui_font_find_index(const lh_ui_font_t *self, lh_u32_t code, lh_u32_t *index)
{
    const lh_ui_font_range_t *run;

    lh_assert_runtime_ref(index);
    run = lh_ui_font_get_range_of(self, code);
    lh_return_if(lh_null_eq(run), lh_bool_false);
    lh_ptr_deref(index) = lh_ui_font_range_get_base(run) + (code - lh_ui_font_range_get_first(run));
    return lh_bool_true;
}

lh_s32_t
lh_ui_font_get_advance_at(const lh_ui_font_t *self, lh_u32_t index)
{
    lh_assert_runtime_ref(self);
    return self->advances[index];
}

lh_s32_t
lh_ui_font_get_top_at(const lh_ui_font_t *self, lh_u32_t index)
{
    lh_assert_runtime_ref(self);
    return self->tops[index];
}

const lh_ui_mask_t *
lh_ui_font_get_glyph_at(const lh_ui_font_t *self, lh_u32_t index)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->glyphs[index]);
}

lh_bool_t
lh_ui_font_has_code(const lh_ui_font_t *self, lh_u32_t code)
{
    return lh_null_ne(lh_ui_font_get_range_of(self, code)) ? lh_bool_true : lh_bool_false;
}

lh_u32_t
lh_ui_font_get_index(const lh_ui_font_t *self, lh_u32_t code)
{
    const lh_ui_font_range_t *run;

    run = lh_ui_font_get_range_of(self, code);
    lh_assert_runtime_ifn(lh_null_ne(run), lh_runtime_error_code_out_of_range);
    return lh_null_eq(run)
               ? 0U
               : lh_ui_font_range_get_base(run) + (code - lh_ui_font_range_get_first(run));
}

lh_s32_t
lh_ui_font_get_advance(const lh_ui_font_t *self, lh_u32_t code)
{
    const lh_ui_font_range_t *run;

    /* One lookup. ::lh_ui_font_has_code and then ::lh_ui_font_get_index walk the
       same runs again, and a line of text asks this once per letter. */
    run = lh_ui_font_get_range_of(self, code);
    lh_return_if(lh_null_eq(run), 0);
    return lh_ui_font_get_advances(
        self)[lh_ui_font_range_get_base(run) + (code - lh_ui_font_range_get_first(run))];
}

lh_s32_t
lh_ui_font_get_top(const lh_ui_font_t *self, lh_u32_t code)
{
    const lh_ui_font_range_t *run;

    run = lh_ui_font_get_range_of(self, code);
    lh_return_if(lh_null_eq(run), 0);
    return lh_ui_font_get_tops(
        self)[lh_ui_font_range_get_base(run) + (code - lh_ui_font_range_get_first(run))];
}

lh_s32_t
lh_ui_font_get_ink_top(const lh_ui_font_t *self, lh_u32_t code)
{
    return lh_ui_font_get_ascent(self) + lh_ui_font_get_top(self, code);
}

lh_bool_t
lh_ui_font_get_glyph(const lh_ui_font_t *self, lh_u32_t code, lh_ui_mask_t *mask)
{
    const lh_ui_font_range_t *run;

    lh_assert_runtime_ref(mask);
    run = lh_ui_font_get_range_of(self, code);
    lh_return_if(lh_null_eq(run), lh_bool_false);
    lh_ptr_deref(mask) = lh_ui_font_get_glyphs(
        self)[lh_ui_font_range_get_base(run) + (code - lh_ui_font_range_get_first(run))];
    return lh_bool_true;
}

lh_s32_t
lh_ui_font_get_ink_bottom(const lh_ui_font_t *self, lh_u32_t code)
{
    lh_ui_mask_t mask;

    lh_return_if(!lh_ui_font_get_glyph(self, code, lh_addr_of(mask)), 0);
    return lh_ui_font_get_ink_top(self, code) +
           lh_cast_static(lh_s32_t, lh_ui_mask_get_height(lh_addr_of(mask)));
}

const lh_ui_mask_t *
lh_ui_font_get_glyphs(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->glyphs;
}

lh_void
lh_ui_font_set_glyphs(lh_ui_font_t *self, const lh_ui_mask_t *glyphs)
{
    lh_assert_runtime_ref(self);
    self->glyphs = glyphs;
}

const lh_byte_t *
lh_ui_font_get_advances(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->advances;
}

lh_void
lh_ui_font_set_advances(lh_ui_font_t *self, const lh_byte_t *advances)
{
    lh_assert_runtime_ref(self);
    self->advances = advances;
}

const lh_s32_t *
lh_ui_font_get_tops(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->tops;
}

lh_void
lh_ui_font_set_tops(lh_ui_font_t *self, const lh_s32_t *tops)
{
    lh_assert_runtime_ref(self);
    self->tops = tops;
}

const lh_ui_font_range_t *
lh_ui_font_get_ranges(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ranges;
}

lh_void
lh_ui_font_set_ranges(lh_ui_font_t *self, const lh_ui_font_range_t *ranges)
{
    lh_assert_runtime_ref(self);
    self->ranges = ranges;
}

lh_void
lh_ui_font_set_line_height(lh_ui_font_t *self, lh_s32_t line_height)
{
    lh_assert_runtime_ref(self);
    self->line_height = line_height;
}

lh_void
lh_ui_font_set_ascent(lh_ui_font_t *self, lh_s32_t ascent)
{
    lh_assert_runtime_ref(self);
    self->ascent = ascent;
}

lh_void
lh_ui_font_set_cap_height(lh_ui_font_t *self, lh_s32_t cap_height)
{
    lh_assert_runtime_ref(self);
    self->cap_height = cap_height;
}

lh_void
lh_ui_font_set_range_count(lh_ui_font_t *self, lh_u32_t range_count)
{
    lh_assert_runtime_ref(self);
    self->range_count = range_count;
}
