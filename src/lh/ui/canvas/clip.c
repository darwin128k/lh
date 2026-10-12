/**
 * @file clip.c
 * @brief Implementation of `lh/ui/canvas/clip.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/math.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/canvas/clip.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/radius.h>
#include <lh/util/addr.h>

/* ── One rounded cut ─────────────────────────────────────────────────────── */

lh_void
lh_ui_canvas_clip_round_init(lh_ui_canvas_clip_round_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    self->rect = *rect;
    self->radius = lh_ui_radius_clamp(rect, radius);
}

lh_byte_t
lh_ui_canvas_clip_round_coverage(const lh_ui_canvas_clip_round_t *self, lh_s32_t x, lh_s32_t y)
{
    lh_assert_runtime_ref(self);
    return lh_ui_radius_coverage(lh_addr_of(self->rect), self->radius, x, y);
}

lh_void
lh_ui_canvas_clip_round_narrow_row(const lh_ui_canvas_clip_round_t *self, lh_s32_t y, lh_s32_t *lo, lh_s32_t *hi)
{
    lh_s32_t x0;
    lh_s32_t x1;

    lh_ui_canvas_round_full_span(lh_addr_of(self->rect), self->radius, y, lh_addr_of(x0), lh_addr_of(x1));
    *lo = lh_math_max(*lo, x0);
    *hi = lh_math_min(*hi, x1);
}

/* ── The clip ────────────────────────────────────────────────────────────── */

lh_void
lh_ui_canvas_clip_init(lh_ui_canvas_clip_t *self, const lh_ui_rect_t *rect, const lh_ui_canvas_clip_round_t *rounds,
                       lh_u32_t count)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ifn(count == 0U || lh_null_ne(rounds), lh_runtime_error_code_invalid_argument);
    self->rect = *rect;
    self->rounds = rounds;
    self->round_count = count;
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
    lh_assert_runtime_ifn(index < lh_ui_canvas_clip_get_round_count(self), lh_runtime_error_code_out_of_range);
    return self->rounds + index;
}

lh_byte_t
lh_ui_canvas_clip_coverage(const lh_ui_canvas_clip_t *self, lh_s32_t x, lh_s32_t y)
{
    lh_byte_t kept = 255U;
    lh_u32_t i;

    for (i = 0U; i < lh_ui_canvas_clip_get_round_count(self) && kept != 0U; ++i)
    {
        kept = lh_ui_radius_scale(kept, lh_ui_canvas_clip_round_coverage(self->rounds + i, x, y));
    }
    return kept;
}

lh_void
lh_ui_canvas_clip_split_row(const lh_ui_canvas_clip_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_s32_t *mid0,
                            lh_s32_t *mid1)
{
    lh_s32_t lo = x0;
    lh_s32_t hi = x1;
    lh_u32_t i;

    for (i = 0U; i < lh_ui_canvas_clip_get_round_count(self); ++i)
    {
        lh_ui_canvas_clip_round_narrow_row(self->rounds + i, y, lh_addr_of(lo), lh_addr_of(hi));
    }
    *mid0 = lh_math_clamp(lo, x0, x1);
    *mid1 = lh_math_clamp(hi, *mid0, x1);
}
