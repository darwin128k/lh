#include <gtest/gtest.h>

#include <lh/math/rect.h>
#include <lh/util/addr.h>

namespace
{

TEST(math_rect, contains_is_half_open)
{
    lh_math_rect_t r;

    lh_math_rect_init(lh_addr_of(r), 10, 20, 5, 3);
    EXPECT_TRUE(lh_math_rect_contains_point(&r, ([&]() { lh_math_point_t _v; lh_math_point_init(lh_addr_of(_v), 10, 20); return _v; })()));
    EXPECT_TRUE(lh_math_rect_contains_point(&r, ([&]() { lh_math_point_t _v; lh_math_point_init(lh_addr_of(_v), 14, 22); return _v; })()));
    EXPECT_FALSE(lh_math_rect_contains_point(&r, ([&]() { lh_math_point_t _v; lh_math_point_init(lh_addr_of(_v), 15, 20); return _v; })()));
    EXPECT_FALSE(lh_math_rect_contains_point(&r, ([&]() { lh_math_point_t _v; lh_math_point_init(lh_addr_of(_v), 10, 23); return _v; })()));
}

TEST(math_rect, make_origin_size_matches_make_fields)
{
    lh_math_point_t origin;

    lh_math_point_init(lh_addr_of(origin), 10, 20);
    lh_math_size_t size;

    lh_math_size_init(lh_addr_of(size), 5, 3);
    lh_math_rect_t from_parts;

    lh_math_rect_init_origin_size(lh_addr_of(from_parts), origin, size);
    lh_math_rect_t from_fields;

    lh_math_rect_init(lh_addr_of(from_fields), 10, 20, 5, 3);
    EXPECT_TRUE(lh_math_rect_eq(&from_parts, &from_fields));
}

TEST(math_rect, far_and_from_extent)
{
    lh_math_rect_t r;

    lh_math_rect_init(lh_addr_of(r), 10, 20, 5, 3);
    const lh_math_point_t far = lh_math_rect_far(&r);
    EXPECT_EQ(lh_math_point_get_x(&far), 15);
    EXPECT_EQ(lh_math_point_get_y(&far), 23);

    lh_math_point_t min;


    lh_math_point_init(lh_addr_of(min), 10, 20);
    const lh_math_rect_t from_extent = lh_math_rect_from_extent(&min, &far);
    EXPECT_TRUE(lh_math_rect_eq(&from_extent, &r));

    lh_math_point_t inverted;


    lh_math_point_init(lh_addr_of(inverted), 0, 0);
    const lh_math_rect_t empty = lh_math_rect_from_extent(&far, &inverted);
    EXPECT_TRUE(lh_math_rect_is_empty(&empty));
}

TEST(math_rect, init_origin_size_fills_self)
{
    lh_math_rect_t r;
    lh_math_rect_init_origin_size(&r, ([&]() { lh_math_point_t _v; lh_math_point_init(lh_addr_of(_v), 1, 2); return _v; })(), ([&]() { lh_math_size_t _v; lh_math_size_init(lh_addr_of(_v), 3, 4); return _v; })());
    EXPECT_EQ(lh_math_point_get_x(lh_math_rect_get_origin_as_const(&r)), 1);
    EXPECT_EQ(lh_math_point_get_y(lh_math_rect_get_origin_as_const(&r)), 2);
    EXPECT_EQ(lh_math_size_get_width(lh_math_rect_get_size_as_const(&r)), 3);
    EXPECT_EQ(lh_math_size_get_height(lh_math_rect_get_size_as_const(&r)), 4);
}

TEST(math_rect, intersection_and_union)
{
    lh_math_rect_t a;

    lh_math_rect_init(lh_addr_of(a), 0, 0, 10, 10);
    lh_math_rect_t b;

    lh_math_rect_init(lh_addr_of(b), 5, 5, 10, 10);
    const lh_math_rect_t i = lh_math_rect_intersection(&a, &b);
    lh_math_rect_t expected_i;

    lh_math_rect_init(lh_addr_of(expected_i), 5, 5, 5, 5);
    EXPECT_TRUE(lh_math_rect_eq(&i, &expected_i));
    EXPECT_TRUE(lh_math_rect_intersects(&a, &b));

    const lh_math_rect_t u = lh_math_rect_union(&a, &b);
    lh_math_rect_t expected_u;

    lh_math_rect_init(lh_addr_of(expected_u), 0, 0, 15, 15);
    EXPECT_TRUE(lh_math_rect_eq(&u, &expected_u));

    lh_math_rect_t far;


    lh_math_rect_init(lh_addr_of(far), 100, 100, 1, 1);
    EXPECT_FALSE(lh_math_rect_intersects(&a, &far));
    const lh_math_rect_t none = lh_math_rect_intersection(&a, &far);
    EXPECT_TRUE(lh_math_rect_is_empty(&none));
}

TEST(math_rect, offset_and_inset)
{
    lh_math_rect_t r;

    lh_math_rect_init(lh_addr_of(r), 0, 0, 10, 8);
    const lh_math_rect_t moved = lh_math_rect_offset(&r, 3, -2);
    EXPECT_EQ(lh_math_point_get_x(lh_math_rect_get_origin_as_const(&moved)), 3);
    EXPECT_EQ(lh_math_point_get_y(lh_math_rect_get_origin_as_const(&moved)), -2);
    EXPECT_EQ(lh_math_size_get_width(lh_math_rect_get_size_as_const(&moved)), 10);

    const lh_math_rect_t inner = lh_math_rect_inset(&r, 1, 2);
    lh_math_rect_t expected;

    lh_math_rect_init(lh_addr_of(expected), 1, 2, 8, 4);
    EXPECT_TRUE(lh_math_rect_eq(&inner, &expected));
}

} // namespace
