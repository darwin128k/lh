#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/rect.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{

struct call_log
{
    int begin;
    int end;
    int clear;
    int fill_rect;
};

lh_void
log_begin(lh_ptr context)
{
    ++lh_ptr_rcast(call_log, context)->begin;
}

lh_void
log_end(lh_ptr context)
{
    ++lh_ptr_rcast(call_log, context)->end;
}

lh_void
log_clear(lh_ptr context, const lh_ui_color_t *color)
{
    (void)color;
    ++lh_ptr_rcast(call_log, context)->clear;
}

lh_void
log_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    (void)rect;
    (void)color;
    ++lh_ptr_rcast(call_log, context)->fill_rect;
}

const lh_ui_canvas_backend_t g_log_backend = {log_begin, log_end, log_clear, log_fill_rect};

TEST(ui_canvas, dispatches_every_call_with_the_context)
{
    call_log log{};
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    lh_ui_rect_t rect;

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(g_log_backend), lh_addr_of(log));
    EXPECT_EQ(lh_ui_canvas_get_backend(lh_addr_of(canvas)), lh_addr_of(g_log_backend));
    EXPECT_EQ(lh_ui_canvas_get_context(lh_addr_of(canvas)), lh_addr_of(log));

    lh_ui_canvas_begin(lh_addr_of(canvas));
    lh_ui_canvas_clear(lh_addr_of(canvas), lh_addr_of(color));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(color));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(color));
    lh_ui_canvas_end(lh_addr_of(canvas));

    EXPECT_EQ(log.begin, 1);
    EXPECT_EQ(log.clear, 1);
    EXPECT_EQ(log.fill_rect, 2);
    EXPECT_EQ(log.end, 1);
}

TEST(ui_canvas, null_backend_and_null_slots_are_no_ops)
{
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    lh_ui_rect_t rect;

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);

    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), nullptr);
    lh_ui_canvas_begin(lh_addr_of(canvas));
    lh_ui_canvas_clear(lh_addr_of(canvas), lh_addr_of(color));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(color));
    lh_ui_canvas_end(lh_addr_of(canvas));

    lh_ui_canvas_deinit(lh_addr_of(canvas));
    EXPECT_TRUE(lh_null_eq(lh_ui_canvas_get_backend(lh_addr_of(canvas))));
    lh_ui_canvas_begin(lh_addr_of(canvas));
    lh_ui_canvas_end(lh_addr_of(canvas));
}

} // namespace
