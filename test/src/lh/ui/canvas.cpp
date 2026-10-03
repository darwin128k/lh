#include <gtest/gtest.h>

#include <lh/ui/canvas.h>

#include <vector>

namespace
{

const lh_ui_color_t k_black = lh_ui_color_make(0, 0, 0, 255);
const lh_ui_color_t k_red = lh_ui_color_make(255, 0, 0, 255);

bool
same(lh_ui_color_t a, lh_ui_color_t b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

TEST(ui_canvas, fill_rect_stays_inside_clip_and_image)
{
    std::vector<lh_ui_color_t> pixels(4 * 3, k_black);
    lh_ui_canvas_t canvas;
    lh_ui_canvas_init(&canvas, pixels.data(), 4, 3, 4);

    lh_ui_canvas_set_clip(&canvas, lh_ui_rect_make(1, 0, 10, 10)); // cut to the image
    const lh_ui_rect_t clip = lh_ui_canvas_get_clip(&canvas);
    EXPECT_EQ(clip.origin.x, 1);
    EXPECT_EQ(clip.size.width, 3);
    EXPECT_EQ(clip.size.height, 3);

    lh_ui_canvas_fill_rect(&canvas, lh_ui_rect_make(-5, 1, 100, 1), k_red);
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 0, 1), k_black)); // clipped
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 1, 1), k_red));
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 3, 1), k_red));
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 1, 0), k_black));
}

TEST(ui_canvas, stride_skips_padding)
{
    std::vector<lh_ui_color_t> pixels(5 * 2, k_black); // 3 wide, 2 of padding per row
    lh_ui_canvas_t canvas;
    lh_ui_canvas_init(&canvas, pixels.data(), 3, 2, 5);
    lh_ui_canvas_fill_rect(&canvas, lh_ui_rect_make(0, 1, 3, 1), k_red);
    EXPECT_TRUE(same(pixels[5], k_red));
    EXPECT_TRUE(same(pixels[3], k_black)); // padding of row 0
    EXPECT_TRUE(same(pixels[8], k_black)); // padding of row 1
}

TEST(ui_canvas, alpha_blends_over_what_is_there)
{
    std::vector<lh_ui_color_t> pixels(1, lh_ui_color_make(0, 0, 200, 255));
    lh_ui_canvas_t canvas;
    lh_ui_canvas_init(&canvas, pixels.data(), 1, 1, 1);

    lh_ui_canvas_blend_pixel(&canvas, 0, 0, lh_ui_color_make(255, 0, 0, 128));
    const lh_ui_color_t c = lh_ui_canvas_get_pixel(&canvas, 0, 0);
    EXPECT_NEAR(c.r, 128, 1);
    EXPECT_NEAR(c.b, 100, 1);
    EXPECT_EQ(c.a, 255);

    lh_ui_canvas_blend_pixel(&canvas, 0, 0, lh_ui_color_make(9, 9, 9, 0)); // invisible
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 0, 0), c));
    lh_ui_canvas_blend_pixel(&canvas, 5, 5, k_red); // outside: ignored
}

} // namespace
