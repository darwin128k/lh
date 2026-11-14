#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/point.h>
#include <lh/gl/point/initializer.h>
#include <lh/test/out.h>

namespace
{

TEST(gl_point, init_fills_x_then_y)
{
    lh_gl_point_t point;

    lh_gl_point_init(lh_addr_of(point), 7, -3);
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(point)), 7);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(point)), -3);
}

TEST(gl_point, init_empty_is_the_origin)
{
    lh_gl_point_t point;

    lh_gl_point_init(lh_addr_of(point), 5, 5);
    lh_gl_point_init_empty(lh_addr_of(point));
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(point)), 0);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(point)), 0);
}

TEST(gl_point, setters_change_one_coordinate_each)
{
    lh_gl_point_t point;

    lh_gl_point_init(lh_addr_of(point), 1, 2);
    lh_gl_point_set_x(lh_addr_of(point), 10);
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(point)), 10);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(point)), 2);
    lh_gl_point_set_y(lh_addr_of(point), 20);
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(point)), 10);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(point)), 20);
}

TEST(gl_point, eq_needs_both_coordinates)
{
    lh_gl_point_t a;

    lh_gl_point_init(lh_addr_of(a), 1, 8);
    lh_gl_point_t same;

    lh_gl_point_init(lh_addr_of(same), 1, 8);
    lh_gl_point_t other_x;

    lh_gl_point_init(lh_addr_of(other_x), 2, 8);
    lh_gl_point_t other_y;

    lh_gl_point_init(lh_addr_of(other_y), 1, 9);
    EXPECT_EQ(lh_gl_point_eq(lh_addr_of(a), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_gl_point_eq(lh_addr_of(a), lh_addr_of(same)), lh_bool_true);
    EXPECT_EQ(lh_gl_point_eq(lh_addr_of(a), lh_addr_of(other_x)), lh_bool_false);
    EXPECT_EQ(lh_gl_point_eq(lh_addr_of(a), lh_addr_of(other_y)), lh_bool_false);
}

TEST(gl_point, compare_suite_is_lexicographic)
{
    lh_gl_point_t a;

    lh_gl_point_init(lh_addr_of(a), 1, 8);
    lh_gl_point_t b;

    lh_gl_point_init(lh_addr_of(b), 1, 9);
    lh_gl_point_t c;

    lh_gl_point_init(lh_addr_of(c), 2, 0);
    EXPECT_EQ(lh_gl_point_is_less(lh_addr_of(a), lh_addr_of(b)), lh_bool_true);
    EXPECT_EQ(lh_gl_point_is_less(lh_addr_of(a), lh_addr_of(c)), lh_bool_true);
    EXPECT_EQ(lh_gl_point_is_less(lh_addr_of(c), lh_addr_of(a)), lh_bool_false);
    EXPECT_EQ(lh_gl_point_is_less(lh_addr_of(a), lh_addr_of(a)), lh_bool_false);
    EXPECT_EQ(lh_gl_point_is_greater(lh_addr_of(c), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_gl_point_is_greater(lh_addr_of(a), lh_addr_of(c)), lh_bool_false);
    EXPECT_EQ(lh_gl_point_is_greater(lh_addr_of(a), lh_addr_of(a)), lh_bool_false);
    EXPECT_EQ(lh_gl_point_is_at_least(lh_addr_of(b), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_gl_point_is_at_least(lh_addr_of(a), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_gl_point_is_at_least(lh_addr_of(a), lh_addr_of(b)), lh_bool_false);
}

TEST(gl_point, offset_translates_both_coordinates)
{
    lh_gl_point_t point;

    lh_gl_point_init(lh_addr_of(point), 10, 20);
    const lh_gl_point_t moved = lh_test::out(lh_gl_point_offset, lh_addr_of(point), 3, -5);
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(moved)), 13);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(moved)), 15);
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(point)), 10);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(point)), 20);
}

TEST(gl_point, offset_may_write_into_its_input)
{
    lh_gl_point_t point;

    lh_gl_point_init(lh_addr_of(point), 1, 2);
    lh_gl_point_offset(lh_addr_of(point), 4, 5, lh_addr_of(point));
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(point)), 5);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(point)), 7);
}

TEST(gl_point, min_max_are_component_wise)
{
    lh_gl_point_t a;

    lh_gl_point_init(lh_addr_of(a), 1, 8);
    lh_gl_point_t b;

    lh_gl_point_init(lh_addr_of(b), 4, 2);
    const lh_gl_point_t lo = lh_test::out(lh_gl_point_min, lh_addr_of(a), lh_addr_of(b));
    const lh_gl_point_t hi = lh_test::out(lh_gl_point_max, lh_addr_of(a), lh_addr_of(b));
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(lo)), 1);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(lo)), 2);
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(hi)), 4);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(hi)), 8);
}

TEST(gl_point, min_max_may_write_into_an_input)
{
    lh_gl_point_t a;

    lh_gl_point_init(lh_addr_of(a), 1, 8);
    lh_gl_point_t b;

    lh_gl_point_init(lh_addr_of(b), 4, 2);
    lh_gl_point_min(lh_addr_of(a), lh_addr_of(b), lh_addr_of(a));
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(a)), 1);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(a)), 2);
    lh_gl_point_max(lh_addr_of(a), lh_addr_of(b), lh_addr_of(b));
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(b)), 4);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(b)), 2);
}

TEST(gl_point_initializer, fills_x_then_y)
{
    const lh_gl_point_t value = lh_gl_point_initializer(3, 4);

    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(value)), 3);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(value)), 4);
}

} // namespace