/**
 * @file clip.c
 * @brief Implementation of `lh/ui/canvas/clip.h`.
 */
#include <lh/assert.h>

#include <lh/math.h>
#include <lh/ui/canvas/clip.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/radius.h>

/* -- One rounded cut ------------------------------------------------------- */

/* -- The clip -------------------------------------------------------------- */

lh_void
lh_ui_canvas_clip_init(lh_ui_canvas_clip_t *self, const lh_ui_rect_t *rect,
                       const lh_ui_canvas_clip_round_t *rounds, lh_u32_t count)
{
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ifn(count == 0U || lh_null_ne(rounds),
                          lh_runtime_error_code_invalid_argument);
    lh_ui_canvas_clip_set_rect(self, lh_ptr_deref(rect));
    lh_ui_canvas_clip_set_rounds(self, rounds);
    lh_ui_canvas_clip_set_round_count(self, count);
}

lh_void
lh_ui_canvas_clip_init_empty(lh_ui_canvas_clip_t *self)
{
    lh_ui_rect_t empty;

    lh_ui_rect_init_empty(lh_addr_of(empty));
    lh_ui_canvas_clip_init(self, lh_addr_of(empty), lh_null, 0U);
}

const lh_ui_rect_t *
lh_ui_canvas_clip_get_rect(const lh_ui_canvas_clip_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->rect);
}

lh_u32_t
lh_ui_canvas_clip_get_round_count(const lh_ui_canvas_clip_t *self)
{
    lh_assert_runtime_ref(self);
    return self->round_count;
}

const lh_ui_canvas_clip_round_t *
lh_ui_canvas_clip_get_round(const lh_ui_canvas_clip_t *self, lh_u32_t index)
{
    lh_assert_runtime_ifn(index < lh_ui_canvas_clip_get_round_count(self),
                          lh_runtime_error_code_out_of_range);
    return lh_ui_canvas_clip_get_rounds(self) + index;
}

lh_byte_t
lh_ui_canvas_clip_coverage(const lh_ui_canvas_clip_t *self, lh_s32_t x, lh_s32_t y)
{
    lh_byte_t kept = 255U;

    for (lh_u32_t i = 0U; i < lh_ui_canvas_clip_get_round_count(self) && kept != 0U; ++i)
    {
        kept = lh_ui_radius_scale(
            kept, lh_ui_canvas_clip_round_coverage(lh_ui_canvas_clip_get_rounds(self) + i, x, y));
    }
    return kept;
}

lh_void
lh_ui_canvas_clip_split_row(const lh_ui_canvas_clip_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                            lh_s32_t *mid0, lh_s32_t *mid1)
{
    lh_s32_t lo = x0;
    lh_s32_t hi = x1;

    for (lh_u32_t i = 0U; i < lh_ui_canvas_clip_get_round_count(self); ++i)
    {
        lh_ui_canvas_clip_round_narrow_row(lh_ui_canvas_clip_get_rounds(self) + i, y,
                                           lh_addr_of(lo), lh_addr_of(hi));
    }
    lh_ptr_deref(mid0) = lh_math_clamp(lo, x0, x1);
    lh_ptr_deref(mid1) = lh_math_clamp(hi, lh_ptr_deref(mid0), x1);
}

lh_ui_rect_t *
lh_ui_canvas_clip_get_rect_mut(lh_ui_canvas_clip_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->rect);
}

lh_void
lh_ui_canvas_clip_set_rect(lh_ui_canvas_clip_t *self, lh_ui_rect_t rect)
{
    lh_assert_runtime_ref(self);
    self->rect = rect;
}

const lh_ui_canvas_clip_round_t *
lh_ui_canvas_clip_get_rounds(const lh_ui_canvas_clip_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rounds;
}

lh_void
lh_ui_canvas_clip_set_rounds(lh_ui_canvas_clip_t *self, const lh_ui_canvas_clip_round_t *rounds)
{
    lh_assert_runtime_ref(self);
    self->rounds = rounds;
}

lh_void
lh_ui_canvas_clip_set_round_count(lh_ui_canvas_clip_t *self, lh_u32_t round_count)
{
    lh_assert_runtime_ref(self);
    self->round_count = round_count;
}
