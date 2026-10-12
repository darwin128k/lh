/**
 * @file stats.c
 * @brief Implementation of `lh/ui/frame/stats.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/ui/frame/stats.h>
#include <lh/ui/point.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_void
lh_ui_frame_stats_init(lh_ui_frame_stats_t *self)
{
    lh_assert_runtime_ref(self);
    self->frames = 0U;
    self->unknown = 0U;
    self->wasteful = 0U;
    self->drawn_px = 0U;
    self->asked_px = 0U;
    self->rects = 0U;
    self->us = 0U;
    self->worst_us = 0U;
    self->on_frame = lh_null;
    self->on_frame_context = lh_null;
}

lh_void
lh_ui_frame_stats_set_on_frame(lh_ui_frame_stats_t *self, lh_ui_frame_stats_on_frame_cb cb, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_frame = cb;
    self->on_frame_context = context;
}

lh_u64_t
lh_ui_frame_area(const lh_ui_rect_t *rect)
{
    const lh_ui_size_t *size;
    lh_s64_t w;
    lh_s64_t h;

    lh_assert_runtime_ref(rect);
    size = lh_ui_rect_get_size_as_const(rect);
    w = lh_cast_static(lh_s64_t, lh_ui_size_get_width(size));
    h = lh_cast_static(lh_s64_t, lh_ui_size_get_height(size));
    lh_return_if(w <= 0 || h <= 0, 0U);
    return lh_cast_static(lh_u64_t, w) * lh_cast_static(lh_u64_t, h);
}

lh_void
lh_ui_frame_stats_record(lh_ui_frame_stats_t *self, const lh_ui_frame_t *frame)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(frame);
    ++self->frames;
    self->us += frame->us;
    if (frame->us > self->worst_us)
    {
        self->worst_us = frame->us;
    }
    if (!frame->asked_known)
    {
        /* Not in the pixel totals: a frame whose request is unknown would count its
           drawing against nothing and make every ratio worse by an amount nobody
           measured. */
        ++self->unknown;
    }
    else
    {
        const lh_u64_t drawn = lh_ui_frame_area(lh_addr_of(frame->drawn));

        self->drawn_px += drawn;
        self->asked_px += frame->asked_px;
        self->rects += frame->rects;
        if (drawn > 0U && drawn >= frame->asked_px * 2U)
        {
            ++self->wasteful;
        }
    }
    if (lh_null_ne(self->on_frame))
    {
        self->on_frame(frame, self->on_frame_context);
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
