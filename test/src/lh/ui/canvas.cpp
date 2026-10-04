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

    lh_ui_canvas_set_clip(&canvas, lh_math_rect_make(1, 0, 10, 10)); // cut to the image
    const lh_math_rect_t clip = lh_ui_canvas_get_clip(&canvas);
    EXPECT_EQ(clip.origin.x, 1);
    EXPECT_EQ(clip.size.width, 3);
    EXPECT_EQ(clip.size.height, 3);

    lh_ui_canvas_fill_rect(&canvas, lh_math_rect_make(-5, 1, 100, 1), k_red);
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
    lh_ui_canvas_fill_rect(&canvas, lh_math_rect_make(0, 1, 3, 1), k_red);
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

TEST(ui_canvas, disc_coverage_is_255_inside_and_0_outside)
{
    // A disc of radius 3 centred on (4, 4). Pixel centers sit on the
    // half-integers, so (2, 2) is 2.12 away and inside, (0, 0) is 4.95 and out.
    EXPECT_EQ(lh_ui_canvas_disc_coverage(4, 4, 4.0f, 4.0f, 3), 255);
    EXPECT_EQ(lh_ui_canvas_disc_coverage(2, 2, 4.0f, 4.0f, 3), 255);
    EXPECT_EQ(lh_ui_canvas_disc_coverage(0, 0, 4.0f, 4.0f, 3), 0);
    EXPECT_EQ(lh_ui_canvas_disc_coverage(4, 4, 4.0f, 4.0f, 0), 0); // no radius, no disc

    // The rim is a one pixel ramp, not a step: a center 2.55 from a radius of 3
    // is inside by less than half a pixel, so it is nearly but not fully in.
    const lh_byte_t rim = lh_ui_canvas_disc_coverage(1, 4, 4.0f, 4.0f, 3);
    EXPECT_GT(rim, 240);
    EXPECT_LT(rim, 255);
}

TEST(ui_canvas, ring_coverage_keeps_the_darker_of_the_two_rims)
{
    // The center is in neither rim: full on the outside, holed on the inside.
    EXPECT_EQ(lh_ui_canvas_ring_coverage(4, 4, 4.0f, 4.0f, 6, 3), 0);
    // The equator of the outer circle, well outside the inner one: solid.
    EXPECT_EQ(lh_ui_canvas_ring_coverage(7, 4, 4.0f, 4.0f, 6, 3), 255);
    // A ring of no width is not a ring.
    EXPECT_EQ(lh_ui_canvas_ring_coverage(7, 4, 4.0f, 4.0f, 3, 3), 0);
    // An inner of 0 is a full disc again.
    EXPECT_EQ(lh_ui_canvas_ring_coverage(4, 4, 4.0f, 4.0f, 3, 0), 255);
}

TEST(ui_canvas, fill_ring_draws_an_annulus_and_leaves_the_hole_alone)
{
    std::vector<lh_ui_color_t> pixels(21 * 21, k_black);
    lh_ui_canvas_t canvas;
    lh_ui_canvas_init(&canvas, pixels.data(), 21, 21, 21);

    lh_ui_canvas_fill_ring(&canvas, 10.0f, 10.0f, 9, 6, k_red);
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 10, 10), k_black)); // hole
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 10, 2), k_red));   // band
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 0, 10), k_black)); // past outer
}

TEST(ui_canvas, stroke_disc_outlines_without_repainting_the_middle)
{
    std::vector<lh_ui_color_t> pixels(21 * 21, k_black);
    lh_ui_canvas_t canvas;
    lh_ui_canvas_init(&canvas, pixels.data(), 21, 21, 21);

    lh_ui_canvas_stroke_disc(&canvas, 10.0f, 10.0f, 8, 2, k_red);
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 10, 10), k_black)); // untouched middle
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 10, 2), k_red));   // on the radius
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 10, 0), k_black)); // past the width
}

TEST(ui_canvas, a_square_border_is_hollow_in_the_middle_not_merely_dark)
{
    // A 1 wide border on a 4x4 box leaves a 2x2 hole; a 2 wide one closes it.
    // Both are the hole being subtracted, not a second pass of painting.
    EXPECT_EQ(lh_ui_canvas_box_stroke_coverage(2, 0, 0, 0, 4, 4, 0, 1), 255); // top edge
    EXPECT_EQ(lh_ui_canvas_box_stroke_coverage(2, 1, 0, 0, 4, 4, 0, 1), 0);   // the hole
    EXPECT_EQ(lh_ui_canvas_box_stroke_coverage(2, 2, 0, 0, 4, 4, 0, 1), 0);   // the hole
    EXPECT_EQ(lh_ui_canvas_box_stroke_coverage(2, 3, 0, 0, 4, 4, 0, 1), 255); // far edge
    EXPECT_EQ(lh_ui_canvas_box_stroke_coverage(2, 4, 0, 0, 4, 4, 0, 1), 0);   // outside
    EXPECT_EQ(lh_ui_canvas_box_stroke_coverage(2, 1, 0, 0, 4, 4, 0, 2), 255);
    // A width of 0 is no border, whatever the radius.
    EXPECT_EQ(lh_ui_canvas_box_stroke_coverage(2, 0, 0, 0, 4, 4, 2, 0), 0);
    // A border wider than the box is the box, not an error.
    EXPECT_EQ(lh_ui_canvas_box_stroke_coverage(2, 1, 0, 0, 4, 4, 0, 9), 255);
}

TEST(ui_canvas, line_coverage_measures_to_the_segment_not_its_ends)
{
    // A 2 wide run covers the rows it runs through, and nothing a row away.
    EXPECT_EQ(lh_ui_canvas_line_coverage(4, 2, 0, 2, 8, 2, 2), 255);
    EXPECT_EQ(lh_ui_canvas_line_coverage(4, 1, 0, 2, 8, 2, 2), 255);
    EXPECT_EQ(lh_ui_canvas_line_coverage(4, 0, 0, 2, 8, 2, 2), 0);
    // A 1 wide run on an integer y splits evenly over the two rows it straddles.
    EXPECT_EQ(lh_ui_canvas_line_coverage(4, 2, 0, 2, 8, 2, 1), 127);
    EXPECT_EQ(lh_ui_canvas_line_coverage(4, 1, 0, 2, 8, 2, 1), 127);
    // The diagonal keeps the same width, measured across it and not along it.
    EXPECT_EQ(lh_ui_canvas_line_coverage(4, 4, 0, 0, 8, 8, 2), 255);
    EXPECT_EQ(lh_ui_canvas_line_coverage(4, 0, 0, 0, 8, 8, 2), 0);
    // A width of 0 draws nothing.
    EXPECT_EQ(lh_ui_canvas_line_coverage(4, 2, 0, 2, 8, 2, 0), 0);
    // Endpoints in one place are a disc, not a division by zero. Its center
    // is the integer (4, 2) while the pixel center is (4.5, 2.5), so even a
    // 2 wide dot leaves that pixel a quarter short of full cover.
    const lh_byte_t dot = lh_ui_canvas_line_coverage(4, 2, 4, 2, 4, 2, 2);
    EXPECT_GT(dot, 200);
    EXPECT_LT(dot, 255);
    EXPECT_EQ(lh_ui_canvas_line_coverage(4, 2, 4, 2, 4, 2, 4), 255);
}

TEST(ui_canvas, stroke_line_and_stroke_rect_respect_the_clip)
{
    std::vector<lh_ui_color_t> pixels(8 * 8, k_black);
    lh_ui_canvas_t canvas;
    lh_ui_canvas_init(&canvas, pixels.data(), 8, 8, 8);
    lh_ui_canvas_set_clip(&canvas, lh_math_rect_make(2, 2, 3, 3));

    lh_ui_canvas_stroke_line(&canvas, 0, 3, 7, 3, 2, k_red);
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 0, 3), k_black)); // outside the clip
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 3, 3), k_red));   // inside it

    std::vector<lh_ui_color_t> more(8 * 8, k_black);
    lh_ui_canvas_init(&canvas, more.data(), 8, 8, 8);
    lh_ui_canvas_stroke_rect(&canvas, lh_math_rect_make(1, 1, 6, 6), 1, k_red);
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 1, 1), k_red));
    EXPECT_TRUE(same(lh_ui_canvas_get_pixel(&canvas, 4, 4), k_black)); // hollow
}

} // namespace
