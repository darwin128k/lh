#include <gtest/gtest.h>

#include <lh/core/null.h>
#include <lh/core/size.h>
#include <lh/core/util/addr.h>
#include <lh/core/util/ptr.h>
#include <lh/gl/canvas.h>

namespace
{

lh_gl_rect_t got_rect;
lh_gl_point_t got_center;
lh_gl_scalar_t got_radius;
const lh_gl_color_t *got_color;
const lh_gl_pen_t *got_pen;
lh_usize_t got_count;

lh_void
record_fill_rect(lh_ptr target, const lh_gl_rect_t *clip, const lh_gl_rect_t *rect,
                 const lh_gl_color_t *color)
{
    (void)target;
    (void)clip;
    got_rect = lh_ptr_deref(rect);
    got_color = color;
    got_count += 1;
}

lh_void
record_stroke_rect(lh_ptr target, const lh_gl_rect_t *clip, const lh_gl_rect_t *rect,
                   const lh_gl_pen_t *pen)
{
    (void)target;
    (void)clip;
    got_rect = lh_ptr_deref(rect);
    got_pen = pen;
    got_count += 1;
}

lh_void
record_fill_circle(lh_ptr target, const lh_gl_rect_t *clip, lh_gl_point_t center,
                   lh_gl_scalar_t radius, const lh_gl_color_t *color)
{
    (void)target;
    (void)clip;
    got_center = center;
    got_radius = radius;
    got_color = color;
    got_count += 1;
}

lh_void
record_round(lh_ptr target, const lh_gl_rect_t *clip, const lh_gl_rect_t *rect,
             lh_gl_scalar_t radius, const lh_gl_color_t *color)
{
    (void)target;
    (void)clip;
    got_rect = lh_ptr_deref(rect);
    got_radius = radius;
    got_color = color;
    got_count += 1;
}

lh_gl_canvas_t
canvas_at(lh_gl_scalar_t x, lh_gl_scalar_t y, const lh_gl_canvas_backend_t *backend)
{
    lh_gl_point_t origin;

    lh_gl_point_init(lh_addr_of(origin), x, y);
    lh_gl_rect_t clip;

    lh_gl_rect_init(lh_addr_of(clip), 0, 0, 100, 100);
    lh_gl_canvas_t canvas;

    lh_gl_canvas_init(lh_addr_of(canvas), origin, clip);
    lh_gl_canvas_set_backend(lh_addr_of(canvas), backend);
    return canvas;
}

} // namespace

TEST(gl_canvas, stores_and_replaces_drawing_state)
{
    lh_gl_point_t origin;
    lh_gl_point_init(lh_addr_of(origin), 3, 4);
    lh_gl_rect_t clip;
    lh_gl_rect_init(lh_addr_of(clip), 1, 2, 20, 30);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas), origin, clip);
    EXPECT_TRUE(lh_gl_point_eq(lh_gl_canvas_get_origin(lh_addr_of(canvas)), lh_addr_of(origin)));
    EXPECT_TRUE(lh_gl_rect_eq(lh_gl_canvas_get_clip(lh_addr_of(canvas)), lh_addr_of(clip)));
    lh_gl_point_init(lh_addr_of(origin), -1, 9);
    lh_gl_rect_init(lh_addr_of(clip), 8, 7, 6, 5);
    lh_gl_canvas_set_origin(lh_addr_of(canvas), origin);
    lh_gl_canvas_set_clip(lh_addr_of(canvas), clip);
    EXPECT_TRUE(lh_gl_point_eq(lh_gl_canvas_get_origin(lh_addr_of(canvas)), lh_addr_of(origin)));
    EXPECT_TRUE(lh_gl_rect_eq(lh_gl_canvas_get_clip(lh_addr_of(canvas)), lh_addr_of(clip)));
    EXPECT_EQ(lh_gl_canvas_get_backend(lh_addr_of(canvas)), nullptr);
}

TEST(gl_canvas, primitives_move_by_origin_and_skip_a_miss)
{
    const lh_gl_canvas_backend_t backend = lh_gl_canvas_backend_initializer(
        record_fill_rect, record_stroke_rect, record_fill_circle, lh_null, record_round, lh_null);
    lh_gl_canvas_t canvas = canvas_at(10, 20, lh_addr_of(backend));
    lh_gl_color_t color;

    lh_gl_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_gl_rect_t local;

    lh_gl_rect_init(lh_addr_of(local), 1, 2, 3, 4);
    got_count = 0;
    lh_gl_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(local), lh_addr_of(color));
    ASSERT_EQ(got_count, 1U);
    lh_gl_rect_t expected;

    lh_gl_rect_init(lh_addr_of(expected), 11, 22, 3, 4);
    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(got_rect), lh_addr_of(expected)));
    EXPECT_EQ(got_color, lh_addr_of(color));
    lh_gl_rect_init(lh_addr_of(local), 200, 200, 3, 4);
    lh_gl_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(local), lh_addr_of(color));
    EXPECT_EQ(got_count, 1U);
    lh_gl_pen_t pen;

    lh_gl_pen_init(lh_addr_of(pen), color, 2);
    lh_gl_rect_init(lh_addr_of(local), 90, 0, 4, 4);
    lh_gl_canvas_stroke_rect(lh_addr_of(canvas), lh_addr_of(local), lh_addr_of(pen));
    ASSERT_EQ(got_count, 2U);
    lh_gl_rect_init(lh_addr_of(expected), 100, 20, 4, 4);
    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(got_rect), lh_addr_of(expected)));
    EXPECT_EQ(got_pen, lh_addr_of(pen));
    lh_gl_point_t center;

    lh_gl_point_init(lh_addr_of(center), 5, 6);
    lh_gl_canvas_fill_circle(lh_addr_of(canvas), center, 4, lh_addr_of(color));
    ASSERT_EQ(got_count, 3U);
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(got_center)), 15);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(got_center)), 26);
    EXPECT_EQ(got_radius, 4);
    lh_gl_rect_init(lh_addr_of(local), 0, 0, 10, 8);
    lh_gl_canvas_fill_round_rect(lh_addr_of(canvas), lh_addr_of(local), 9, lh_addr_of(color));
    ASSERT_EQ(got_count, 4U);
    EXPECT_EQ(got_radius, 4);
    lh_gl_canvas_fill_circle(lh_addr_of(canvas), center, 0, lh_addr_of(color));
    EXPECT_EQ(got_count, 4U);
}
