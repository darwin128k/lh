#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/canvas.h>

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
}
