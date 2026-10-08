#include <gtest/gtest.h>

#include <lh/ui/rect.h>
#include <lh/ui/shadow.h>
#include <lh/util/addr.h>

namespace
{

/* A shadow of @p shadow over the 40 x 40 box at (20, 20), and the shadow's
   strength at one point of it. */
lh_byte_t
alpha_at(const lh_ui_shadow_t *shadow, lh_ui_scalar_t x, lh_ui_scalar_t y)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(20), lh_ui_scalar(20), lh_ui_scalar(40), lh_ui_scalar(40));
    return lh_ui_shadow_alpha_at(shadow, x, y, lh_addr_of(rect), lh_ui_scalar(0));
}

TEST(ui_shadow, a_fresh_one_paints_nothing)
{
    lh_ui_shadow_t shadow;
    lh_ui_rect_t rect;

    lh_ui_shadow_init(lh_addr_of(shadow));
    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(20), lh_ui_scalar(20), lh_ui_scalar(40), lh_ui_scalar(40));
    EXPECT_EQ(lh_ui_shadow_get_color(lh_addr_of(shadow)).a, 0);
    EXPECT_EQ(lh_ui_shadow_get_spread(lh_addr_of(shadow)), 0);
    EXPECT_EQ(lh_ui_shadow_get_outset(lh_addr_of(shadow), lh_addr_of(rect)), 0);
    EXPECT_EQ(alpha_at(lh_addr_of(shadow), lh_ui_scalar(18), lh_ui_scalar(40)), 0);
    EXPECT_TRUE(lh_ui_shadow_is_empty(lh_addr_of(shadow)));
}

/* The question a caller draws every frame asks first. It has to be the same
   answer ::lh_ui_canvas_shadow gives — which is why a style with no shadow costs
   one call and not a rectangle of measurements. */
TEST(ui_shadow, empty_means_no_peak_or_no_fade)
{
    lh_ui_shadow_t shadow;
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(20), lh_ui_scalar(20), lh_ui_scalar(40), lh_ui_scalar(40));
    lh_ui_shadow_init(lh_addr_of(shadow));

    lh_ui_shadow_set_color(lh_addr_of(shadow), lh_ui_color_t{0, 0, 0, 200});
    EXPECT_TRUE(lh_ui_shadow_is_empty(lh_addr_of(shadow))) << "a colour with no fade paints nothing";
    lh_ui_shadow_set_spread(lh_addr_of(shadow), lh_ui_scalar(8));
    EXPECT_FALSE(lh_ui_shadow_is_empty(lh_addr_of(shadow)));

    lh_ui_shadow_set_color(lh_addr_of(shadow), lh_ui_color_t{0, 0, 0, 0});
    EXPECT_TRUE(lh_ui_shadow_is_empty(lh_addr_of(shadow))) << "a fade with no peak paints nothing";
}

TEST(ui_shadow, nothing_is_painted_inside_the_box)
{
    lh_ui_shadow_t shadow;

    lh_ui_shadow_init(lh_addr_of(shadow));
    lh_ui_shadow_set_color(lh_addr_of(shadow), lh_ui_color_t{0, 0, 0, 200});
    lh_ui_shadow_set_spread(lh_addr_of(shadow), lh_ui_scalar(8));

    /* The fill owns these pixels, so a shadow under it would only ever darken
       the thing it is supposed to be standing behind. */
    EXPECT_EQ(alpha_at(lh_addr_of(shadow), lh_ui_scalar(20), lh_ui_scalar(20)), 0);
    EXPECT_EQ(alpha_at(lh_addr_of(shadow), lh_ui_scalar(59), lh_ui_scalar(59)), 0);
    EXPECT_EQ(alpha_at(lh_addr_of(shadow), lh_ui_scalar(40), lh_ui_scalar(40)), 0);

    /* And the column and the row just past the box are NOT the fill's: a square
       box is half-open, so they carry the shadow. Treating them as inside is what
       leaves a bright line between a box and the shadow under it. */
    EXPECT_GT(alpha_at(lh_addr_of(shadow), lh_ui_scalar(60), lh_ui_scalar(40)), 0);
    EXPECT_GT(alpha_at(lh_addr_of(shadow), lh_ui_scalar(40), lh_ui_scalar(60)), 0);
}

TEST(ui_shadow, it_fades_outwards_and_stops_one_spread_past_the_edge)
{
    lh_ui_shadow_t shadow;

    lh_ui_shadow_init(lh_addr_of(shadow));
    lh_ui_shadow_set_color(lh_addr_of(shadow), lh_ui_color_t{0, 0, 0, 200});
    lh_ui_shadow_set_spread(lh_addr_of(shadow), lh_ui_scalar(8));

    /* Just outside the edge is the strongest, and each pixel further is weaker. */
    EXPECT_GT(alpha_at(lh_addr_of(shadow), lh_ui_scalar(19), lh_ui_scalar(40)), 0);
    EXPECT_GE(alpha_at(lh_addr_of(shadow), lh_ui_scalar(19), lh_ui_scalar(40)),
              alpha_at(lh_addr_of(shadow), lh_ui_scalar(17), lh_ui_scalar(40)));
    EXPECT_GE(alpha_at(lh_addr_of(shadow), lh_ui_scalar(17), lh_ui_scalar(40)),
              alpha_at(lh_addr_of(shadow), lh_ui_scalar(15), lh_ui_scalar(40)));
    /* One spread past the edge is where it ends, and the pixel inside that still
       carries something. */
    EXPECT_GT(alpha_at(lh_addr_of(shadow), lh_ui_scalar(13), lh_ui_scalar(40)), 0);
    EXPECT_EQ(alpha_at(lh_addr_of(shadow), lh_ui_scalar(12), lh_ui_scalar(40)), 0);
}

TEST(ui_shadow, the_colour_alpha_is_the_peak)
{
    lh_ui_shadow_t shadow;

    lh_ui_shadow_init(lh_addr_of(shadow));
    lh_ui_shadow_set_color(lh_addr_of(shadow), lh_ui_color_t{0, 0, 0, 128});
    lh_ui_shadow_set_spread(lh_addr_of(shadow), lh_ui_scalar(8));

    /* Half the alpha in the colour, half the shadow everywhere. */
    EXPECT_LE(alpha_at(lh_addr_of(shadow), lh_ui_scalar(19), lh_ui_scalar(40)), 128);
}

TEST(ui_shadow, an_offset_moves_the_whole_fade)
{
    lh_ui_shadow_t shadow;
    lh_ui_rect_t rect;
    lh_s32_t untouched_side;

    lh_ui_shadow_init(lh_addr_of(shadow));
    lh_ui_shadow_set_color(lh_addr_of(shadow), lh_ui_color_t{0, 0, 0, 255});
    lh_ui_shadow_set_spread(lh_addr_of(shadow), lh_ui_scalar(6));
    lh_ui_shadow_set_offset(lh_addr_of(shadow), lh_ui_scalar(5), lh_ui_scalar(0));

    EXPECT_EQ(lh_ui_shadow_get_offset_x(lh_addr_of(shadow)), 5);
    EXPECT_EQ(lh_ui_shadow_get_offset_y(lh_addr_of(shadow)), 0);

    /* Light from the left: the shadow lies to the right of the box, so the left
       side is already past the fade while the right side is still under it. */
    untouched_side = alpha_at(lh_addr_of(shadow), lh_ui_scalar(14), lh_ui_scalar(40));
    EXPECT_EQ(untouched_side, 0);
    EXPECT_GT(alpha_at(lh_addr_of(shadow), lh_ui_scalar(64), lh_ui_scalar(40)), 0);

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(20), lh_ui_scalar(20), lh_ui_scalar(40), lh_ui_scalar(40));
    EXPECT_EQ(lh_ui_shadow_get_outset(lh_addr_of(shadow), lh_addr_of(rect)), 11);
}

TEST(ui_shadow, the_outset_leaves_room_for_the_fade_and_the_shift)
{
    lh_ui_shadow_t shadow;
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(20), lh_ui_scalar(20), lh_ui_scalar(40), lh_ui_scalar(40));
    lh_ui_shadow_init(lh_addr_of(shadow));
    lh_ui_shadow_set_color(lh_addr_of(shadow), lh_ui_color_t{0, 0, 0, 255});
    lh_ui_shadow_set_spread(lh_addr_of(shadow), lh_ui_scalar(8));
    EXPECT_EQ(lh_ui_shadow_get_outset(lh_addr_of(shadow), lh_addr_of(rect)), 8);

    lh_ui_shadow_set_offset(lh_addr_of(shadow), lh_ui_scalar(-4), lh_ui_scalar(3));
    /* The bigger of the two shifts, not their sum: the shadow moves along one
       axis, and the spread rides along with it. */
    EXPECT_EQ(lh_ui_shadow_get_outset(lh_addr_of(shadow), lh_addr_of(rect)), 12);
}

TEST(ui_shadow, the_distance_is_negative_inside_and_zero_on_the_edge)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(10), lh_ui_scalar(10), lh_ui_scalar(30), lh_ui_scalar(30));
    EXPECT_LT(lh_ui_shadow_distance(lh_ui_scalar(25), lh_ui_scalar(25), lh_addr_of(rect), lh_ui_scalar(0)), 0);
    EXPECT_GT(lh_ui_shadow_distance(lh_ui_scalar(5), lh_ui_scalar(25), lh_addr_of(rect), lh_ui_scalar(0)), 0);
    EXPECT_EQ(lh_ui_shadow_distance(lh_ui_scalar(10), lh_ui_scalar(25), lh_addr_of(rect), lh_ui_scalar(0)), 0);
}

TEST(ui_shadow, the_falloff_runs_from_the_peak_to_nothing)
{
    /* The fade is `spread` wide on each side of the edge and the peak sits at the
       far end of that, so a shadow a spread inside the box is as strong as the
       colour allows and one a spread outside is gone. */
    EXPECT_EQ(lh_ui_shadow_falloff(-8, lh_ui_scalar(8), 200), 200);
    EXPECT_GT(lh_ui_shadow_falloff(-8, lh_ui_scalar(8), 200), lh_ui_shadow_falloff(0, lh_ui_scalar(8), 200));
    EXPECT_EQ(lh_ui_shadow_falloff(8, lh_ui_scalar(8), 200), 0);
    EXPECT_GT(lh_ui_shadow_falloff(0, lh_ui_scalar(8), 200), 0);
    EXPECT_LT(lh_ui_shadow_falloff(0, lh_ui_scalar(8), 200), 200);
    EXPECT_EQ(lh_ui_shadow_falloff(0, lh_ui_scalar(0), 200), 0);
    EXPECT_EQ(lh_ui_shadow_falloff(-4, lh_ui_scalar(8), 0), 0);
}

} // namespace