#include <gtest/gtest.h>

#include <lh/math.h>
#include <lh/math/box.h>

namespace
{

const lh_float_t k_eps = 1e-5f;

lh_float_t
at(lh_float_t x, lh_float_t y, lh_float_t side, lh_float_t radius = 0.0f)
{
    return lh_math_box_distance(lh_math_vec2_make(x, y), lh_math_vec2_make(side, side), radius);
}

TEST(Box, the_centre_is_half_the_side_inside)
{
    EXPECT_NEAR(at(2.0f, 2.0f, 4.0f), -2.0f, k_eps);
    EXPECT_NEAR(at(2.0f, 2.0f, 4.0f, 1.0f), -2.0f, k_eps);
}

TEST(Box, the_nearest_edge_is_what_a_point_inside_measures)
{
    // Nearer the top edge than the left one, so the top one is the answer.
    EXPECT_NEAR(at(2.0f, 0.5f, 4.0f), -0.5f, k_eps);
    EXPECT_NEAR(at(0.5f, 2.0f, 4.0f), -0.5f, k_eps);
}

TEST(Box, a_point_outside_measures_to_the_rim)
{
    EXPECT_NEAR(at(-1.0f, 2.0f, 4.0f), 1.0f, k_eps);
    EXPECT_NEAR(at(2.0f, 6.0f, 4.0f), 2.0f, k_eps);
}

TEST(Box, a_corner_measures_diagonally_out)
{
    // sqrt(1 + 1) from the corner: both axes are outside, so both count.
    EXPECT_NEAR(at(-1.0f, -1.0f, 4.0f), 1.41421356f, k_eps);
}

TEST(Box, the_sign_changes_on_the_rim)
{
    // The rim is where the answer is zero, on the edge and on the straight part
    // of it, which is what the coverage ramp is measured from.
    EXPECT_NEAR(at(0.0f, 2.0f, 4.0f), 0.0f, k_eps);
    EXPECT_NEAR(at(4.0f, 2.0f, 4.0f), 0.0f, k_eps);
    EXPECT_NEAR(at(2.0f, 0.0f, 4.0f), 0.0f, k_eps);
}

TEST(Box, a_radius_cuts_the_corner_away)
{
    // Just outside a square corner, sqrt(1 + 1) away. Rounding the corner off
    // takes the shape away from that point, so the same point is further out:
    // the corner arc is one radius in from the edges it joins, and there is
    // nothing left at the corner itself.
    EXPECT_NEAR(at(-1.0f, -1.0f, 10.0f), 1.41421356f, k_eps);
    EXPECT_NEAR(at(-1.0f, -1.0f, 10.0f, 3.0f), 5.65685425f - 3.0f, k_eps);
    // The edges are untouched by it: a point on one is still on the rim.
    EXPECT_NEAR(at(0.0f, 5.0f, 10.0f, 3.0f), 0.0f, k_eps);
    EXPECT_NEAR(at(0.0f, 0.0f, 10.0f, 3.0f), 4.24264069f - 3.0f, k_eps);
}

TEST(Box, a_radius_beyond_the_box_is_a_disc)
{
    // Larger than half the side, so the shape is a circle of that half: every
    // direction out of the middle measures the same.
    const lh_float_t big = 100.0f;
    EXPECT_NEAR(at(5.0f, 5.0f, 10.0f, big), -5.0f, k_eps);
    EXPECT_NEAR(at(5.0f + 3.0f, 5.0f, 10.0f, big), -2.0f, k_eps);
    EXPECT_NEAR(at(0.0f, 5.0f, 10.0f, big), 0.0f, k_eps);
    EXPECT_NEAR(at(-1.0f, 5.0f, 10.0f, big), 1.0f, k_eps);
    EXPECT_NEAR(at(-3.0f, 5.0f, 10.0f, big), 3.0f, k_eps);
}

TEST(Box, a_box_with_no_size_is_a_point)
{
    // Nothing to be inside of, so the answer is never negative and is the
    // distance to the origin itself.
    EXPECT_NEAR(at(0.0f, 0.0f, 0.0f), 0.0f, k_eps);
    EXPECT_NEAR(at(3.0f, 4.0f, 0.0f), 5.0f, k_eps);
    EXPECT_GE(at(0.5f, 0.5f, 0.0f), 0.0f);
}

TEST(Box, a_negative_size_is_no_size)
{
    // Read the way lh_math_rect_is_empty reads a side: there is no interior, so
    // the answer is the distance to the origin and never goes below it.
    EXPECT_NEAR(at(3.0f, 4.0f, -4.0f), 5.0f, k_eps);
    EXPECT_NEAR(at(0.0f, 0.0f, -4.0f), 0.0f, k_eps);
    EXPECT_GE(at(0.5f, 0.5f, -4.0f), 0.0f);
}

} // namespace
