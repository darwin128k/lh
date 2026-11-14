#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/point/initializer.h>
#include <lh/gl/rect.h>
#include <lh/gl/rect/initializer.h>
#include <lh/gl/size/initializer.h>
#include <lh/test/out.h>

namespace
{

lh_gl_point_t
point_at(lh_math_scalar_t x, lh_math_scalar_t y)
{
    lh_gl_point_t point;

    lh_gl_point_init(lh_addr_of(point), x, y);
    return point;
}

TEST(gl_rect, init_fills_origin_then_size)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), 1, 2, 3, 4);
    EXPECT_EQ(lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(rect))), 1);
    EXPECT_EQ(lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(rect))), 2);
    EXPECT_EQ(lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(rect))), 3);
    EXPECT_EQ(lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(rect))), 4);
}

TEST(gl_rect, init_origin_size_matches_init)
{
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 5, 3);
    lh_gl_rect_t from_parts;

    lh_gl_rect_init_origin_size(lh_addr_of(from_parts), point_at(10, 20), size);
    lh_gl_rect_t from_scalars;

    lh_gl_rect_init(lh_addr_of(from_scalars), 10, 20, 5, 3);
    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(from_parts), lh_addr_of(from_scalars)));
}

TEST(gl_rect, setters_replace_one_part_each)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), 1, 2, 3, 4);
    lh_gl_rect_set_origin(lh_addr_of(rect), point_at(7, 8));
    EXPECT_EQ(lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(rect))), 7);
    EXPECT_EQ(lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(rect))), 3);
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 9, 6);
    lh_gl_rect_set_size(lh_addr_of(rect), size);
    EXPECT_EQ(lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(rect))), 7);
    EXPECT_EQ(lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(rect))), 9);
    EXPECT_EQ(lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(rect))), 6);
}

TEST(gl_rect, mutable_accessors_write_through)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init_empty(lh_addr_of(rect));
    lh_gl_point_set_x(lh_gl_rect_get_origin_mut(lh_addr_of(rect)), 4);
    lh_gl_size_set_height(lh_gl_rect_get_size_mut(lh_addr_of(rect)), 6);
    EXPECT_EQ(lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(rect))), 4);
    EXPECT_EQ(lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(rect))), 6);
}

TEST(gl_rect, init_empty_is_empty)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), 1, 2, 3, 4);
    lh_gl_rect_init_empty(lh_addr_of(rect));
    EXPECT_TRUE(lh_gl_rect_is_empty(lh_addr_of(rect)));
    EXPECT_EQ(lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(rect))), 0);
    EXPECT_EQ(lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(rect))), 0);
}

TEST(gl_rect, get_end_and_from_extent)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), 10, 20, 5, 3);
    const lh_gl_point_t end = lh_test::out(lh_gl_rect_get_end, lh_addr_of(rect));
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(end)), 15);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(end)), 23);

    const lh_gl_point_t min = point_at(10, 20);
    const lh_gl_rect_t from_extent =
        lh_test::out(lh_gl_rect_from_extent, lh_addr_of(min), lh_addr_of(end));
    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(from_extent), lh_addr_of(rect)));

    const lh_gl_point_t origin = point_at(0, 0);
    const lh_gl_rect_t inverted =
        lh_test::out(lh_gl_rect_from_extent, lh_addr_of(end), lh_addr_of(origin));
    EXPECT_TRUE(lh_gl_rect_is_empty(lh_addr_of(inverted)));
}

TEST(gl_rect, from_min_max_is_win32_style)
{
    const lh_gl_rect_t rect = lh_test::out(lh_gl_rect_from_min_max, 1, 2, 6, 5);
    lh_gl_rect_t expected;

    lh_gl_rect_init(lh_addr_of(expected), 1, 2, 5, 3);
    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(rect), lh_addr_of(expected)));
}

TEST(gl_rect, corner_helpers_are_component_wise)
{
    lh_gl_rect_t a;

    lh_gl_rect_init(lh_addr_of(a), 0, 4, 10, 10);
    lh_gl_rect_t b;

    lh_gl_rect_init(lh_addr_of(b), 5, 1, 2, 2);
    const lh_gl_point_t origin_min =
        lh_test::out(lh_gl_rect_get_origin_min, lh_addr_of(a), lh_addr_of(b));
    const lh_gl_point_t origin_max =
        lh_test::out(lh_gl_rect_get_origin_max, lh_addr_of(a), lh_addr_of(b));
    const lh_gl_point_t end_min =
        lh_test::out(lh_gl_rect_get_end_min, lh_addr_of(a), lh_addr_of(b));
    const lh_gl_point_t end_max =
        lh_test::out(lh_gl_rect_get_end_max, lh_addr_of(a), lh_addr_of(b));
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(origin_min)), 0);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(origin_min)), 1);
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(origin_max)), 5);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(origin_max)), 4);
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(end_min)), 7);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(end_min)), 3);
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(end_max)), 10);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(end_max)), 14);
}

TEST(gl_rect, contains_point_is_half_open)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), 10, 20, 5, 3);
    EXPECT_TRUE(lh_gl_rect_contains_point(lh_addr_of(rect), point_at(10, 20)));
    EXPECT_TRUE(lh_gl_rect_contains_point(lh_addr_of(rect), point_at(14, 22)));
    EXPECT_FALSE(lh_gl_rect_contains_point(lh_addr_of(rect), point_at(15, 20)));
    EXPECT_FALSE(lh_gl_rect_contains_point(lh_addr_of(rect), point_at(10, 23)));
    EXPECT_FALSE(lh_gl_rect_contains_point(lh_addr_of(rect), point_at(9, 20)));
    lh_gl_rect_t empty;

    lh_gl_rect_init_empty(lh_addr_of(empty));
    EXPECT_FALSE(lh_gl_rect_contains_point(lh_addr_of(empty), point_at(0, 0)));
}

TEST(gl_rect, intersection_and_union)
{
    lh_gl_rect_t a;

    lh_gl_rect_init(lh_addr_of(a), 0, 0, 10, 10);
    lh_gl_rect_t b;

    lh_gl_rect_init(lh_addr_of(b), 5, 5, 10, 10);
    const lh_gl_rect_t overlap =
        lh_test::out(lh_gl_rect_intersection, lh_addr_of(a), lh_addr_of(b));
    lh_gl_rect_t expected_overlap;

    lh_gl_rect_init(lh_addr_of(expected_overlap), 5, 5, 5, 5);
    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(overlap), lh_addr_of(expected_overlap)));
    EXPECT_TRUE(lh_gl_rect_intersects(lh_addr_of(a), lh_addr_of(b)));

    const lh_gl_rect_t joined = lh_test::out(lh_gl_rect_union, lh_addr_of(a), lh_addr_of(b));
    lh_gl_rect_t expected_joined;

    lh_gl_rect_init(lh_addr_of(expected_joined), 0, 0, 15, 15);
    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(joined), lh_addr_of(expected_joined)));

    lh_gl_rect_t far_away;

    lh_gl_rect_init(lh_addr_of(far_away), 100, 100, 1, 1);
    EXPECT_FALSE(lh_gl_rect_intersects(lh_addr_of(a), lh_addr_of(far_away)));
    const lh_gl_rect_t none =
        lh_test::out(lh_gl_rect_intersection, lh_addr_of(a), lh_addr_of(far_away));
    EXPECT_TRUE(lh_gl_rect_is_empty(lh_addr_of(none)));
}

TEST(gl_rect, union_with_an_empty_side_copies_the_other)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), 3, 4, 5, 6);
    lh_gl_rect_t empty;

    lh_gl_rect_init_empty(lh_addr_of(empty));
    const lh_gl_rect_t left = lh_test::out(lh_gl_rect_union, lh_addr_of(empty), lh_addr_of(rect));
    const lh_gl_rect_t right = lh_test::out(lh_gl_rect_union, lh_addr_of(rect), lh_addr_of(empty));
    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(left), lh_addr_of(rect)));
    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(right), lh_addr_of(rect)));
}

TEST(gl_rect, offset_and_inset)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), 0, 0, 10, 8);
    const lh_gl_rect_t moved = lh_test::out(lh_gl_rect_offset, lh_addr_of(rect), 3, -2);
    EXPECT_EQ(lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(moved))), 3);
    EXPECT_EQ(lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(moved))), -2);
    EXPECT_EQ(lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(moved))), 10);

    const lh_gl_rect_t inner = lh_test::out(lh_gl_rect_inset, lh_addr_of(rect), 1, 2);
    lh_gl_rect_t expected;

    lh_gl_rect_init(lh_addr_of(expected), 1, 2, 8, 4);
    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(inner), lh_addr_of(expected)));
}

TEST(gl_rect, compare_suite_origin_then_size)
{
    lh_gl_rect_t a;

    lh_gl_rect_init(lh_addr_of(a), 0, 0, 10, 10);
    lh_gl_rect_t b;

    lh_gl_rect_init(lh_addr_of(b), 0, 0, 10, 11);
    lh_gl_rect_t c;

    lh_gl_rect_init(lh_addr_of(c), 1, 0, 1, 1);
    EXPECT_EQ(lh_gl_rect_eq(lh_addr_of(a), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_gl_rect_eq(lh_addr_of(a), lh_addr_of(b)), lh_bool_false);
    EXPECT_EQ(lh_gl_rect_is_less(lh_addr_of(a), lh_addr_of(b)), lh_bool_true);
    EXPECT_EQ(lh_gl_rect_is_less(lh_addr_of(a), lh_addr_of(c)), lh_bool_true);
    EXPECT_EQ(lh_gl_rect_is_less(lh_addr_of(c), lh_addr_of(a)), lh_bool_false);
    EXPECT_EQ(lh_gl_rect_is_greater(lh_addr_of(c), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_gl_rect_is_at_least(lh_addr_of(b), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_gl_rect_is_at_least(lh_addr_of(a), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_gl_rect_is_at_least(lh_addr_of(a), lh_addr_of(b)), lh_bool_false);
}

TEST(gl_rect_initializer, fills_origin_then_size)
{
    const lh_gl_rect_t rect =
        lh_gl_rect_initializer(lh_gl_point_initializer(10, 20), lh_gl_size_initializer(640, 480));

    EXPECT_EQ(lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(rect))), 10);
    EXPECT_EQ(lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(rect))), 20);
    EXPECT_EQ(lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(rect))), 640);
    EXPECT_EQ(lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(rect))), 480);
}

} // namespace