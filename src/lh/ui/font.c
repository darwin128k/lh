/**
 * @file font.c
 * @brief Implementation of `lh/ui/font.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/config.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/font.h>
#include <lh/util/addr.h>
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

/* Whether the runs are in order and none of them overlaps the one before.

   The lookup walks them in order and takes the first run that holds the code, so an
   overlap is not a wrong answer but **two**: the glyph drawn is one and the advance
   measured can be the other, and which of the two a test sees depends on which table
   the test read. That is the same shape as two functions that agree with each other
   and disagree with the protocol, and it is caught here where the table is given over
   rather than on a screen. */
static lh_bool_t
font_runs_are_a_runs(const lh_ui_font_range_t *ranges, lh_u32_t count)
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
lh_ui_font_init(lh_ui_font_t *self, const lh_ui_mask_t *glyphs, const lh_byte_t *advances,
                const lh_s32_t *tops, const lh_ui_font_range_t *ranges, lh_s32_t line_height,
                lh_s32_t ascent, lh_s32_t cap_height, lh_u32_t range_count)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(glyphs);
    lh_assert_runtime_ref(advances);
    lh_assert_runtime_ref(tops);
    lh_assert_runtime_ref(ranges);
    lh_assert_runtime_ifn(range_count > 0U && line_height >= 0 && ascent >= 0 &&
                              ascent <= line_height && cap_height >= 0 && cap_height <= ascent &&
                              font_runs_are_a_runs(ranges, range_count),
                          lh_runtime_error_code_invalid_argument);
    self->glyphs = glyphs;
    self->advances = advances;
    self->tops = tops;
    self->ranges = ranges;
    self->line_height = line_height;
    self->ascent = ascent;
    self->cap_height = cap_height;
    self->range_count = range_count;
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
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ifn(index < lh_ui_font_get_range_count(self),
                          lh_runtime_error_code_out_of_range);
    return lh_addr_of(self->ranges[index]);
}

lh_u32_t
lh_ui_font_get_first(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->range_count > 0U ? lh_ui_font_range_get_first(lh_addr_of(self->ranges[0])) : 0U;
}

lh_u32_t
lh_ui_font_get_glyph_count(const lh_ui_font_t *self)
{
    lh_u32_t total = 0U;
    lh_u32_t i;

    lh_assert_runtime_ref(self);
    for (i = 0; i < self->range_count; ++i)
    {
        total += lh_ui_font_range_get_length(lh_addr_of(self->ranges[i]));
    }
    return total;
}

/* Which run holds @p code, or `lh_null`. The one place a code is looked up, because
   ::lh_ui_font_has_code and ::lh_ui_font_get_index asking it two different ways is how
   they come to answer two different things -- the whole defect this format exists to
   allow for is a code that is in one run and not the other.

   The runs are sorted, so a run that starts past @p code ends the search: everything
   after it starts higher still. */
static const lh_ui_font_range_t *
font_run_of(const lh_ui_font_t *self, lh_u32_t code)
{
    lh_u32_t i;

    for (i = 0; i < self->range_count; ++i)
    {
        if (lh_ui_font_range_has(lh_addr_of(self->ranges[i]), code))
        {
            return lh_addr_of(self->ranges[i]);
        }
        if (lh_ui_font_range_get_first(lh_addr_of(self->ranges[i])) > code)
        {
            break;
        }
    }
    return lh_null;
}

lh_bool_t
lh_ui_font_has_code(const lh_ui_font_t *self, lh_u32_t code)
{
    lh_assert_runtime_ref(self);
    return lh_null_ne(font_run_of(self, code)) ? lh_bool_true : lh_bool_false;
}

lh_u32_t
lh_ui_font_get_index(const lh_ui_font_t *self, lh_u32_t code)
{
    const lh_ui_font_range_t *run;

    lh_assert_runtime_ref(self);
    run = font_run_of(self, code);
    lh_assert_runtime_ifn(lh_null_ne(run), lh_runtime_error_code_out_of_range);
    return lh_null_eq(run) ? 0U
                           : lh_ui_font_range_get_base(run) +
                                 (code - lh_ui_font_range_get_first(run));
}

lh_s32_t
lh_ui_font_get_advance(const lh_ui_font_t *self, lh_u32_t code)
{
    const lh_ui_font_range_t *run;

    lh_assert_runtime_ref(self);
    /* One lookup. ::lh_ui_font_has_code and then ::lh_ui_font_get_index walk the
       same runs again, and a line of text asks this once per letter. */
    run = font_run_of(self, code);
    lh_return_if(lh_null_eq(run), 0);
    return self->advances[lh_ui_font_range_get_base(run) + (code - lh_ui_font_range_get_first(run))];
}

lh_s32_t
lh_ui_font_get_top(const lh_ui_font_t *self, lh_u32_t code)
{
    const lh_ui_font_range_t *run;

    lh_assert_runtime_ref(self);
    run = font_run_of(self, code);
    lh_return_if(lh_null_eq(run), 0);
    return self->tops[lh_ui_font_range_get_base(run) + (code - lh_ui_font_range_get_first(run))];
}

lh_s32_t
lh_ui_font_get_ink_top(const lh_ui_font_t *self, lh_u32_t code)
{
    return self->ascent + lh_ui_font_get_top(self, code);
}

lh_s32_t
lh_ui_font_get_ink_bottom(const lh_ui_font_t *self, lh_u32_t code)
{
    lh_ui_mask_t mask;

    lh_return_if(!lh_ui_font_get_glyph(self, code, lh_addr_of(mask)), 0);
    return lh_ui_font_get_ink_top(self, code) + lh_cast_static(lh_s32_t, lh_ui_mask_get_height(lh_addr_of(mask)));
}

lh_bool_t
lh_ui_font_get_glyph(const lh_ui_font_t *self, lh_u32_t code, lh_ui_mask_t *mask)
{
    const lh_ui_font_range_t *run;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(mask);
    run = font_run_of(self, code);
    lh_return_if(lh_null_eq(run), lh_bool_false);
    *mask = self->glyphs[lh_ui_font_range_get_base(run) + (code - lh_ui_font_range_get_first(run))];
    return lh_bool_true;
}