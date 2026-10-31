/**
 * @file stats.c
 * @brief Implementation of `lh/ui/frame/stats.h`.
 */
#include <lh/assert.h>

#include <lh/ui/frame/stats.h>
#include <lh/util/return.h>

lh_void
lh_ui_frame_stats_init(lh_ui_frame_stats_t *self)
{
    lh_ui_frame_stats_set_frames(self, 0U);
    lh_ui_frame_stats_set_unknown(self, 0U);
    lh_ui_frame_stats_set_wasteful(self, 0U);
    lh_ui_frame_stats_set_drawn_px(self, 0U);
    lh_ui_frame_stats_set_asked_px(self, 0U);
    lh_ui_frame_stats_set_rects(self, 0U);
    lh_ui_frame_stats_set_us(self, 0U);
    lh_ui_frame_stats_set_worst_us(self, 0U);
    lh_ui_frame_stats_set_on_frame_fn(self, lh_null);
    lh_ui_frame_stats_set_on_frame_context(self, lh_null);
}

lh_void
lh_ui_frame_stats_set_on_frame(lh_ui_frame_stats_t *self, lh_ui_frame_stats_on_frame_cb cb,
                               lh_ptr context)
{
    lh_ui_frame_stats_set_on_frame_fn(self, cb);
    lh_ui_frame_stats_set_on_frame_context(self, context);
}

lh_u64_t
lh_ui_frame_area(const lh_ui_rect_t *rect)
{

    lh_assert_runtime_ref(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size(rect);
    lh_s64_t w = lh_cast_static(lh_s64_t, lh_ui_size_get_width(size));
    lh_s64_t h = lh_cast_static(lh_s64_t, lh_ui_size_get_height(size));
    lh_return_if(w <= 0 || h <= 0, 0U);
    return lh_cast_static(lh_u64_t, w) * lh_cast_static(lh_u64_t, h);
}

lh_void
lh_ui_frame_stats_record(lh_ui_frame_stats_t *self, const lh_ui_frame_t *frame)
{
    lh_assert_runtime_ref(frame);
    lh_ui_frame_stats_set_frames(self, lh_ui_frame_stats_get_frames(self) + 1);
    lh_ui_frame_stats_set_us(self, lh_ui_frame_stats_get_us(self) + (lh_ui_frame_get_us(frame)));
    if (lh_ui_frame_get_us(frame) > lh_ui_frame_stats_get_worst_us(self))
    {
        lh_ui_frame_stats_set_worst_us(self, lh_ui_frame_get_us(frame));
    }
    if (!lh_ui_frame_is_asked_known(frame))
    {
        /* Not in the pixel totals: a frame whose request is unknown would count its
           drawing against nothing and make every ratio worse by an amount nobody
           measured. */
        lh_ui_frame_stats_set_unknown(self, lh_ui_frame_stats_get_unknown(self) + 1);
    }
    else
    {
        const lh_u64_t drawn = lh_ui_frame_area(lh_ui_frame_get_drawn(frame));

        lh_ui_frame_stats_set_drawn_px(self, lh_ui_frame_stats_get_drawn_px(self) + (drawn));
        lh_ui_frame_stats_set_asked_px(self, lh_ui_frame_stats_get_asked_px(self) +
                                                 (lh_ui_frame_get_asked_px(frame)));
        lh_ui_frame_stats_set_rects(self, lh_ui_frame_stats_get_rects(self) +
                                              (lh_ui_frame_get_rects(frame)));
        if (drawn > 0U && drawn >= lh_ui_frame_get_asked_px(frame) * 2U)
        {
            lh_ui_frame_stats_set_wasteful(self, lh_ui_frame_stats_get_wasteful(self) + 1);
        }
    }
    if (lh_null_ne(lh_ui_frame_stats_get_on_frame_fn(self)))
    {
        lh_ui_frame_stats_get_on_frame_fn(self)(frame,
                                                lh_ui_frame_stats_get_on_frame_context(self));
    }
}

lh_u32_t
lh_ui_frame_stats_get_frames(const lh_ui_frame_stats_t *self)
{
    lh_assert_runtime_ref(self);
    return self->frames;
}

lh_u32_t
lh_ui_frame_stats_get_unknown(const lh_ui_frame_stats_t *self)
{
    lh_assert_runtime_ref(self);
    return self->unknown;
}

lh_u32_t
lh_ui_frame_stats_get_wasteful(const lh_ui_frame_stats_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wasteful;
}

lh_u64_t
lh_ui_frame_stats_get_drawn_px(const lh_ui_frame_stats_t *self)
{
    lh_assert_runtime_ref(self);
    return self->drawn_px;
}

lh_u64_t
lh_ui_frame_stats_get_asked_px(const lh_ui_frame_stats_t *self)
{
    lh_assert_runtime_ref(self);
    return self->asked_px;
}

lh_u64_t
lh_ui_frame_stats_get_rects(const lh_ui_frame_stats_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rects;
}

lh_u64_t
lh_ui_frame_stats_get_us(const lh_ui_frame_stats_t *self)
{
    lh_assert_runtime_ref(self);
    return self->us;
}

lh_u64_t
lh_ui_frame_stats_get_worst_us(const lh_ui_frame_stats_t *self)
{
    lh_assert_runtime_ref(self);
    return self->worst_us;
}

lh_void
lh_ui_frame_stats_set_frames(lh_ui_frame_stats_t *self, lh_u32_t frames)
{
    lh_assert_runtime_ref(self);
    self->frames = frames;
}

lh_void
lh_ui_frame_stats_set_unknown(lh_ui_frame_stats_t *self, lh_u32_t unknown)
{
    lh_assert_runtime_ref(self);
    self->unknown = unknown;
}

lh_void
lh_ui_frame_stats_set_wasteful(lh_ui_frame_stats_t *self, lh_u32_t wasteful)
{
    lh_assert_runtime_ref(self);
    self->wasteful = wasteful;
}

lh_void
lh_ui_frame_stats_set_drawn_px(lh_ui_frame_stats_t *self, lh_u64_t drawn_px)
{
    lh_assert_runtime_ref(self);
    self->drawn_px = drawn_px;
}

lh_void
lh_ui_frame_stats_set_asked_px(lh_ui_frame_stats_t *self, lh_u64_t asked_px)
{
    lh_assert_runtime_ref(self);
    self->asked_px = asked_px;
}

lh_void
lh_ui_frame_stats_set_rects(lh_ui_frame_stats_t *self, lh_u64_t rects)
{
    lh_assert_runtime_ref(self);
    self->rects = rects;
}

lh_void
lh_ui_frame_stats_set_us(lh_ui_frame_stats_t *self, lh_u64_t us)
{
    lh_assert_runtime_ref(self);
    self->us = us;
}

lh_void
lh_ui_frame_stats_set_worst_us(lh_ui_frame_stats_t *self, lh_u64_t worst_us)
{
    lh_assert_runtime_ref(self);
    self->worst_us = worst_us;
}

lh_ui_frame_stats_on_frame_cb
lh_ui_frame_stats_get_on_frame_fn(const lh_ui_frame_stats_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_frame;
}

lh_void
lh_ui_frame_stats_set_on_frame_fn(lh_ui_frame_stats_t *self, lh_ui_frame_stats_on_frame_cb on_frame)
{
    lh_assert_runtime_ref(self);
    self->on_frame = on_frame;
}

lh_ptr
lh_ui_frame_stats_get_on_frame_context(const lh_ui_frame_stats_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_frame_context;
}

lh_void
lh_ui_frame_stats_set_on_frame_context(lh_ui_frame_stats_t *self, lh_ptr on_frame_context)
{
    lh_assert_runtime_ref(self);
    self->on_frame_context = on_frame_context;
}
