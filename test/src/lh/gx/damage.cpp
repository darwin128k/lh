/**
 * @file damage.cpp
 * @brief Tests for `lh/gx/damage.h`.
 *
 * The damage is the part of the screen that changed. What matters here is the
 * one rule the whole thing rests on: a merge may never cover a pixel that did
 * not change. So two rects that meet edge to edge become one, and two that
 * overlap in an L stay two — the hull of an L is bigger than the L, and the
 * bigger part would be repainted for nothing.
 */

#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gx/damage.h>
#include <lh/gx/rect.h>
#include <lh/gx/scalar.h>
#include <lh/test/out.h>

namespace
{

/** A rect of the gx plane, by its corners. */
lh_gx_rect_t
rect_at(lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t width, lh_gx_scalar_t height)
{
    lh_gx_rect_t rect;

    lh_gx_rect_init(lh_addr_of(rect), x, y, width, height);
    return rect;
}

TEST(gx_damage, a_new_damage_is_empty)
{
    lh_gx_damage_t damage;

    lh_gx_damage_init(lh_addr_of(damage));

    EXPECT_EQ(lh_gx_damage_is_empty(lh_addr_of(damage)), lh_bool_true);
    EXPECT_EQ(lh_gx_damage_get_count(lh_addr_of(damage)), 0u);
}

TEST(gx_damage, a_rect_that_changes_is_one_rect)
{
    lh_gx_damage_t damage;
    const lh_gx_rect_t rect = rect_at(10, 20, 30, 40);

    lh_gx_damage_init(lh_addr_of(damage));
    lh_gx_damage_add(lh_addr_of(damage), lh_addr_of(rect));

    EXPECT_EQ(lh_gx_damage_get_count(lh_addr_of(damage)), 1u);
    EXPECT_EQ(lh_gx_rect_eq(lh_gx_damage_get_rect(lh_addr_of(damage), 0u), lh_addr_of(rect)),
              lh_bool_true);
}

TEST(gx_damage, an_empty_rect_changes_nothing)
{
    lh_gx_damage_t damage;
    const lh_gx_rect_t rect = rect_at(0, 0, 0, 0);

    lh_gx_damage_init(lh_addr_of(damage));
    lh_gx_damage_add(lh_addr_of(damage), lh_addr_of(rect));

    EXPECT_EQ(lh_gx_damage_get_count(lh_addr_of(damage)), 0u);
}

TEST(gx_damage, rects_that_meet_edge_to_edge_are_one)
{
    lh_gx_damage_t damage;
    const lh_gx_rect_t top = rect_at(0, 0, 10, 5);
    const lh_gx_rect_t bottom = rect_at(0, 5, 10, 5);
    const lh_gx_rect_t whole = rect_at(0, 0, 10, 10);

    lh_gx_damage_init(lh_addr_of(damage));
    lh_gx_damage_add(lh_addr_of(damage), lh_addr_of(top));
    lh_gx_damage_add(lh_addr_of(damage), lh_addr_of(bottom));

    EXPECT_EQ(lh_gx_damage_get_count(lh_addr_of(damage)), 1u);
    EXPECT_EQ(lh_gx_rect_eq(lh_gx_damage_get_rect(lh_addr_of(damage), 0u), lh_addr_of(whole)),
              lh_bool_true);
}

TEST(gx_damage, rects_that_overlap_in_an_l_stay_two)
{
    lh_gx_damage_t damage;
    const lh_gx_rect_t left = rect_at(0, 0, 4, 10);
    const lh_gx_rect_t right = rect_at(2, 2, 8, 4);

    lh_gx_damage_init(lh_addr_of(damage));
    lh_gx_damage_add(lh_addr_of(damage), lh_addr_of(left));
    lh_gx_damage_add(lh_addr_of(damage), lh_addr_of(right));

    EXPECT_EQ(lh_gx_damage_get_count(lh_addr_of(damage)), 2u);
}

TEST(gx_damage, rects_apart_stay_two)
{
    lh_gx_damage_t damage;
    const lh_gx_rect_t first = rect_at(0, 0, 10, 10);
    const lh_gx_rect_t second = rect_at(100, 100, 10, 10);

    lh_gx_damage_init(lh_addr_of(damage));
    lh_gx_damage_add(lh_addr_of(damage), lh_addr_of(first));
    lh_gx_damage_add(lh_addr_of(damage), lh_addr_of(second));

    EXPECT_EQ(lh_gx_damage_get_count(lh_addr_of(damage)), 2u);
}

TEST(gx_damage, a_merge_never_covers_a_pixel_that_did_not_change)
{
    const lh_gx_rect_t left = rect_at(0, 0, 4, 10);
    const lh_gx_rect_t right = rect_at(2, 2, 8, 4);

    EXPECT_EQ(lh_gx_damage_joins(lh_addr_of(left), lh_addr_of(right)), lh_bool_false);
    EXPECT_GT(lh_gx_damage_waste(lh_addr_of(left), lh_addr_of(right)), lh_gx_scalar(0));
}

TEST(gx_damage, rects_that_touch_waste_nothing)
{
    const lh_gx_rect_t top = rect_at(0, 0, 10, 5);
    const lh_gx_rect_t bottom = rect_at(0, 5, 10, 5);

    EXPECT_EQ(lh_gx_damage_joins(lh_addr_of(top), lh_addr_of(bottom)), lh_bool_true);
    EXPECT_EQ(lh_gx_damage_waste(lh_addr_of(top), lh_addr_of(bottom)), lh_gx_scalar(0));
}

TEST(gx_damage, a_full_list_never_drops_a_rect)
{
    lh_gx_damage_t damage;

    lh_gx_damage_init(lh_addr_of(damage));
    for (lh_u32_t i = 0u; i < (lh_u32_t)LH_GX_DAMAGE_MAX; ++i)
    {
        const lh_gx_rect_t rect = rect_at((lh_gx_scalar_t)(i * 20), 0, 10, 10);

        lh_gx_damage_add(lh_addr_of(damage), lh_addr_of(rect));
    }
    EXPECT_EQ(lh_gx_damage_get_count(lh_addr_of(damage)), (lh_u32_t)LH_GX_DAMAGE_MAX);

    const lh_gx_rect_t late = rect_at(1000, 1000, 10, 10);

    lh_gx_damage_add(lh_addr_of(damage), lh_addr_of(late));
    EXPECT_EQ(lh_gx_damage_get_count(lh_addr_of(damage)), (lh_u32_t)LH_GX_DAMAGE_MAX);
    EXPECT_GT(lh_gx_damage_area(lh_addr_of(damage)), lh_gx_scalar(0));
}

TEST(gx_damage, the_area_is_the_pixels_held)
{
    lh_gx_damage_t damage;
    const lh_gx_rect_t rect = rect_at(0, 0, 10, 20);

    lh_gx_damage_init(lh_addr_of(damage));
    lh_gx_damage_add(lh_addr_of(damage), lh_addr_of(rect));

    EXPECT_EQ(lh_gx_damage_rect_area(lh_addr_of(rect)), lh_gx_scalar(200));
    EXPECT_EQ(lh_gx_damage_area(lh_addr_of(damage)), lh_gx_scalar(200));
}

TEST(gx_damage, a_cleared_damage_is_empty)
{
    lh_gx_damage_t damage;
    const lh_gx_rect_t rect = rect_at(0, 0, 10, 10);

    lh_gx_damage_init(lh_addr_of(damage));
    lh_gx_damage_add(lh_addr_of(damage), lh_addr_of(rect));
    lh_gx_damage_clear(lh_addr_of(damage));

    EXPECT_EQ(lh_gx_damage_is_empty(lh_addr_of(damage)), lh_bool_true);
}

} // namespace
