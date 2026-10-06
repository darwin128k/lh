#include <gtest/gtest.h>
#include <type_traits>

#include <lh/math/frect.h>
#include <lh/math/rect.h>
#include <lh/math/fscalar.h>
#include <lh/util/addr.h>

namespace
{

TEST(math_frect, components_are_the_scalar)
{
    const lh_math_frect_t r = ([&]() { lh_math_frect_t _v; lh_math_frect_init_origin_size(lh_addr_of(_v), ([&]() { lh_math_fpoint_t _v; lh_math_fpoint_init(lh_addr_of(_v), 1, 2); return _v; })(),
                                                            ([&]() { lh_math_fsize_t _v; lh_math_fsize_init(lh_addr_of(_v), 3, 4); return _v; })()); return _v; })();
    using component = decltype(lh_math_fpoint_get_x(lh_math_frect_get_origin_as_const(lh_addr_of(r))));
    EXPECT_TRUE((std::is_same<component, lh_math_fscalar_t>::value));
}

TEST(math_frect, holds_origin_and_size)
{
    lh_math_frect_t r;

    lh_math_frect_init(lh_addr_of(r), 10, 20, 30, 40);
    EXPECT_EQ(lh_math_fpoint_get_x(lh_math_frect_get_origin_as_const(lh_addr_of(r))), 10);
    EXPECT_EQ(lh_math_fpoint_get_y(lh_math_frect_get_origin_as_const(lh_addr_of(r))), 20);
    EXPECT_EQ(lh_math_fsize_get_width(lh_math_frect_get_size_as_const(lh_addr_of(r))), 30);
    EXPECT_EQ(lh_math_fsize_get_height(lh_math_frect_get_size_as_const(lh_addr_of(r))), 40);
    EXPECT_FALSE(lh_math_frect_is_empty(lh_addr_of(r)));
}

TEST(math_frect, zero_size_is_empty)
{
    lh_math_frect_t r;

    lh_math_frect_init_empty(lh_addr_of(r));
    EXPECT_TRUE(lh_math_frect_is_empty(lh_addr_of(r)));
    EXPECT_EQ(lh_math_fsize_get_width(lh_math_frect_get_size_as_const(lh_addr_of(r))), 0);
    EXPECT_EQ(lh_math_fsize_get_height(lh_math_frect_get_size_as_const(lh_addr_of(r))), 0);
}

TEST(math_frect, far_and_from_extent)
{
    lh_math_frect_t r;

    lh_math_frect_init(lh_addr_of(r), 10, 20, 5, 3);
    const lh_math_fpoint_t far = lh_math_frect_far(lh_addr_of(r));
    EXPECT_EQ(lh_math_fpoint_get_x(lh_addr_of(far)), 15);
    EXPECT_EQ(lh_math_fpoint_get_y(lh_addr_of(far)), 23);

    lh_math_fpoint_t min;


    lh_math_fpoint_init(lh_addr_of(min), 10, 20);
    const lh_math_frect_t from_extent = lh_math_frect_from_extent(lh_addr_of(min), lh_addr_of(far));
    EXPECT_TRUE(lh_math_frect_eq(lh_addr_of(from_extent), lh_addr_of(r)));
}

TEST(math_frect, intersection_and_contains)
{
    lh_math_frect_t a;

    lh_math_frect_init(lh_addr_of(a), 0, 0, 10, 10);
    lh_math_frect_t b;

    lh_math_frect_init(lh_addr_of(b), 5, 5, 10, 10);
    const lh_math_frect_t i = lh_math_frect_intersection(lh_addr_of(a), lh_addr_of(b));
    lh_math_frect_t expected;

    lh_math_frect_init(lh_addr_of(expected), 5, 5, 5, 5);
    EXPECT_TRUE(lh_math_frect_eq(lh_addr_of(i), lh_addr_of(expected)));
    EXPECT_TRUE(lh_math_frect_intersects(lh_addr_of(a), lh_addr_of(b)));
    EXPECT_TRUE(lh_math_frect_contains_point(lh_addr_of(a), ([&]() { lh_math_fpoint_t _v; lh_math_fpoint_init(lh_addr_of(_v), 0, 0); return _v; })()));
    EXPECT_FALSE(lh_math_frect_contains_point(lh_addr_of(a), ([&]() { lh_math_fpoint_t _v; lh_math_fpoint_init(lh_addr_of(_v), 10, 0); return _v; })()));
}

TEST(math_frect, negative_extent_is_empty)
{
    lh_math_frect_t r;

    lh_math_frect_init(lh_addr_of(r), 1, 1, -4, 8);
    EXPECT_TRUE(lh_math_frect_is_empty(lh_addr_of(r)));
}

TEST(math_frect, roundtrip_through_rect)
{
    lh_math_rect_t screen;

    lh_math_rect_init(lh_addr_of(screen), -8, 15, 4, 9);
    const lh_math_frect_t r = lh_math_rect_to_frect(screen);
    const lh_math_rect_t back = lh_math_frect_to_rect(r);
    EXPECT_EQ(lh_math_point_get_x(lh_math_rect_get_origin_as_const(lh_addr_of(back))), -8);
    EXPECT_EQ(lh_math_point_get_y(lh_math_rect_get_origin_as_const(lh_addr_of(back))), 15);
    EXPECT_EQ(lh_math_size_get_width(lh_math_rect_get_size_as_const(lh_addr_of(back))), 4);
    EXPECT_EQ(lh_math_size_get_height(lh_math_rect_get_size_as_const(lh_addr_of(back))), 9);
}

TEST(math_frect, to_rect_truncates_toward_zero)
{
    lh_math_frect_t r;

    lh_math_frect_init(lh_addr_of(r), 3.9f, -3.9f, 8.2f, 1.1f);
    const lh_math_rect_t screen = lh_math_frect_to_rect(r);
    EXPECT_EQ(lh_math_point_get_x(lh_math_rect_get_origin_as_const(lh_addr_of(screen))), 3);
    EXPECT_EQ(lh_math_point_get_y(lh_math_rect_get_origin_as_const(lh_addr_of(screen))), -3);
    EXPECT_EQ(lh_math_size_get_width(lh_math_rect_get_size_as_const(lh_addr_of(screen))), 8);
    EXPECT_EQ(lh_math_size_get_height(lh_math_rect_get_size_as_const(lh_addr_of(screen))), 1);
}

} /* namespace */
