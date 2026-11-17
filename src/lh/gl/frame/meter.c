/**
 * @file meter.c
 * @brief Implementation of `lh/gl/frame/meter.h`.
 */

#include <lh/gl/frame/meter.h>

#include <lh/core/util/return.h>

lh_gl_frame_stats_t *
lh_gl_frame_meter_get_stats(const lh_gl_frame_meter_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stats;
}

lh_void
lh_gl_frame_meter_set_stats(lh_gl_frame_meter_t *self, lh_gl_frame_stats_t *stats)
{
    lh_assert_runtime_ref(self);
    self->stats = stats;
}

lh_os_window_on_paint_cb
lh_gl_frame_meter_get_on_paint_fn(const lh_gl_frame_meter_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_paint;
}

lh_void
lh_gl_frame_meter_set_on_paint_fn(lh_gl_frame_meter_t *self, lh_os_window_on_paint_cb on_paint)
{
    lh_assert_runtime_ref(self);
    self->on_paint = on_paint;
}

lh_ptr
lh_gl_frame_meter_get_on_paint_context(const lh_gl_frame_meter_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_paint_context;
}

lh_void
lh_gl_frame_meter_set_on_paint_context(lh_gl_frame_meter_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_paint_context = context;
}

lh_u64_t
lh_gl_frame_meter_now_us(const lh_os_window_t *window)
{
    const lh_os_window_backend_t *backend = lh_os_window_get_backend(window);

    lh_return_if(lh_null_eq(backend), 0U);
    lh_os_window_backend_now_us_fn *now_us = lh_os_window_backend_get_now_us(backend);

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, now_us)), 0U);
    return now_us(lh_os_window_get_backend_context(window));
}

lh_void
lh_gl_frame_meter_read_request(const lh_os_window_t *window, lh_gl_frame_t *frame)
{
    lh_u64_t asked_px = 0U;
    lh_u32_t rects = 0U;
    lh_bool_t asked_known = lh_bool_false;
    const lh_os_window_backend_t *backend = lh_os_window_get_backend(window);

    if (lh_null_ne(backend))
    {
        lh_os_window_backend_get_paint_region_fn *get_paint_region =
            lh_os_window_backend_get_get_paint_region(backend);

        if (lh_null_ne(lh_ptr_rcast(lh_void, get_paint_region)))
        {
            asked_known = get_paint_region(lh_os_window_get_backend_context(window),
                                           lh_os_window_get_paint_target(window),
                                           lh_addr_of(asked_px), lh_addr_of(rects));
        }
    }
    lh_gl_frame_set_asked_known(frame, asked_known);
    lh_gl_frame_set_asked_px(frame, asked_px);
    lh_gl_frame_set_rects(frame, rects);
}

lh_void
lh_gl_frame_meter_read_drawn(const lh_os_window_t *window, lh_gl_frame_t *frame)
{
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;

    lh_os_window_get_paint_rect(window, lh_addr_of(left), lh_addr_of(top), lh_addr_of(right),
                                lh_addr_of(bottom));
    lh_gl_rect_init(lh_gl_frame_get_drawn_mut(frame), left, top, right - left, bottom - top);
}

lh_void
lh_gl_frame_meter_init(lh_gl_frame_meter_t *self, lh_gl_frame_stats_t *stats,
                       lh_os_window_on_paint_cb on_paint, lh_ptr context)
{
    lh_gl_frame_meter_set_stats(self, stats);
    lh_gl_frame_meter_set_on_paint_fn(self, on_paint);
    lh_gl_frame_meter_set_on_paint_context(self, context);
}

lh_void
lh_gl_frame_meter_on_paint(lh_os_window_t *window, lh_ptr context)
{
    const lh_gl_frame_meter_t *self = lh_ptr_rcast(const lh_gl_frame_meter_t, context);
    lh_gl_frame_t frame;

    /* The request is read before the paint, off the target the paint is about to draw
       on: what the backend collected from every invalidation, against the one rectangle
       it hands the paint. */
    lh_gl_frame_meter_read_drawn(window, lh_addr_of(frame));
    lh_gl_frame_meter_read_request(window, lh_addr_of(frame));
    lh_u64_t started_us = lh_gl_frame_meter_now_us(window);
    lh_os_window_on_paint_cb on_paint = lh_gl_frame_meter_get_on_paint_fn(self);

    if (lh_null_ne(lh_ptr_rcast(lh_void, on_paint)))
    {
        on_paint(window, lh_gl_frame_meter_get_on_paint_context(self));
    }
    lh_gl_frame_set_us(lh_addr_of(frame), lh_gl_frame_meter_now_us(window) - started_us);
    lh_gl_frame_stats_record(lh_gl_frame_meter_get_stats(self), lh_addr_of(frame));
}

lh_void
lh_gl_frame_meter_attach(lh_gl_frame_meter_t *self, lh_os_window_t *window)
{
    lh_os_window_set_on_paint(window, lh_gl_frame_meter_on_paint, self);
}
