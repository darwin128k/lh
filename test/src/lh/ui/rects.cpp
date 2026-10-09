/* A region that is one rect is a lie whenever two cuts on a frame stand apart:
   `lh_ui_rect_union` answers with their hull, and the hull covers the space
   between them. That is what a backend presenting its drawn region used to put
   on the screen. These pin the shape that does not. */

#include <gtest/gtest.h>
#include <lh/bool.h>
#include <lh/null.h>

#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/rects.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>

namespace
{

TEST(ui_rects, a_fresh_list_holds_nothing)
{
    lh_ui_rects_t rects;

    lh_ui_rects_init(lh_addr_of(rects));
    EXPECT_TRUE(lh_ui_rects_is_empty(lh_addr_of(rects)));
    EXPECT_EQ(lh_ui_rects_get_count(lh_addr_of(rects)), 0u);
    EXPECT_EQ(lh_ui_rects_get_as_const(lh_addr_of(rects), 0), lh_null);
}

TEST(ui_rects, rects_apart_are_kept_apart)
{
    lh_ui_rects_t rects;
    lh_ui_rect_t left;
    lh_ui_rect_t right;

    lh_ui_rect_init(lh_addr_of(left), 0, 0, 8, 8);
    lh_ui_rect_init(lh_addr_of(right), 24, 0, 8, 8);
    lh_ui_rects_init(lh_addr_of(rects));

    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(left));
    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(right));

    EXPECT_EQ(lh_ui_rects_get_count(lh_addr_of(rects)), 2u);
    EXPECT_EQ(lh_ui_rects_get_as_const(lh_addr_of(rects), 0), &rects.rects[0]);
    EXPECT_EQ(lh_ui_rects_get_as_const(lh_addr_of(rects), 1), &rects.rects[1]);
    EXPECT_EQ(lh_ui_rects_get_as_const(lh_addr_of(rects), 2), lh_null);
}

TEST(ui_rects, an_overlapping_rect_becomes_one)
{
    lh_ui_rects_t rects;
    lh_ui_rect_t wide;
    lh_ui_rect_t part;

    lh_ui_rect_init(lh_addr_of(wide), 0, 0, 20, 10);
    lh_ui_rect_init(lh_addr_of(part), 5, 5, 20, 10);
    lh_ui_rects_init(lh_addr_of(rects));

    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(wide));
    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(part));

    EXPECT_EQ(lh_ui_rects_get_count(lh_addr_of(rects)), 1u);
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(rects.rects[0]))), 25);
    EXPECT_EQ(lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(rects.rects[0]))), 15);
}

/* Sharing an edge is sharing nothing: the two rects already cover every pixel
   between them, so merging them costs no pixel and halves the present calls. */
TEST(ui_rects, rects_that_share_an_edge_become_one)
{
    lh_ui_rects_t rects;
    lh_ui_rect_t left;
    lh_ui_rect_t right;

    lh_ui_rect_init(lh_addr_of(left), 0, 0, 10, 10);
    lh_ui_rect_init(lh_addr_of(right), 10, 0, 10, 10);
    lh_ui_rects_init(lh_addr_of(rects));

    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(left));
    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(right));

    EXPECT_EQ(lh_ui_rects_get_count(lh_addr_of(rects)), 1u);
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(rects.rects[0]))), 20);
    EXPECT_TRUE(lh_ui_rects_touch(lh_addr_of(left), lh_addr_of(right)));
}

/* The closure: a rect that lands between two others has to pull both in, and
   one pass over the pairs is not enough to find out. */
TEST(ui_rects, a_rect_touching_two_others_pulls_both_in)
{
    lh_ui_rects_t rects;
    lh_ui_rect_t left;
    lh_ui_rect_t right;
    lh_ui_rect_t middle;

    lh_ui_rect_init(lh_addr_of(left), 0, 0, 10, 10);
    lh_ui_rect_init(lh_addr_of(right), 30, 0, 10, 10);
    lh_ui_rect_init(lh_addr_of(middle), 9, 0, 22, 10);
    lh_ui_rects_init(lh_addr_of(rects));

    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(left));
    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(right));
    EXPECT_EQ(lh_ui_rects_get_count(lh_addr_of(rects)), 2u);
    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(middle));

    EXPECT_EQ(lh_ui_rects_get_count(lh_addr_of(rects)), 1u);
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(rects.rects[0]))), 40);
}

TEST(ui_rects, an_empty_rect_adds_nothing)
{
    lh_ui_rects_t rects;
    lh_ui_rect_t empty;
    lh_ui_rect_t whole;

    lh_ui_rect_init_empty(lh_addr_of(empty));
    lh_ui_rect_init(lh_addr_of(whole), 0, 0, 4, 4);
    lh_ui_rects_init(lh_addr_of(rects));

    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(empty));
    EXPECT_TRUE(lh_ui_rects_is_empty(lh_addr_of(rects)));
    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(whole));
    EXPECT_EQ(lh_ui_rects_get_count(lh_addr_of(rects)), 1u);
}

TEST(ui_rects, a_hand_filled_list_is_not_a_region_until_it_is_closed)
{
    lh_ui_rects_t rects;

    /* Filled by hand through the struct, in an order no single pass over the
       pairs gets right: the rect that closes the gap is the last one. */
    lh_ui_rect_init(lh_addr_of(rects.rects[0]), 0, 0, 10, 10);
    lh_ui_rect_init(lh_addr_of(rects.rects[1]), 40, 0, 10, 10);
    lh_ui_rect_init(lh_addr_of(rects.rects[2]), 10, 0, 10, 10);
    rects.count = 3u;

    EXPECT_EQ(lh_ui_rects_join_once(lh_addr_of(rects)), lh_bool_true);
    EXPECT_EQ(lh_ui_rects_get_count(lh_addr_of(rects)), 2u);
    /* 0..19 and 40..49, twenty columns apart: closed means it stops there, and
       says so by finding nothing left to join. */
    EXPECT_EQ(lh_ui_rects_join_once(lh_addr_of(rects)), lh_bool_false);
    lh_ui_rects_close(lh_addr_of(rects));
    EXPECT_EQ(lh_ui_rects_get_count(lh_addr_of(rects)), 2u);

    lh_ui_rect_init(lh_addr_of(rects.rects[1]), 20, 0, 10, 10);
    rects.count = 2u;
    lh_ui_rects_close(lh_addr_of(rects));
    EXPECT_EQ(lh_ui_rects_get_count(lh_addr_of(rects)), 1u);
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(rects.rects[0]))), 30);
}

/* Past the table the answer stops being the region exactly, and the only thing
   that must not happen is losing a rect: whatever was drawn is still covered
   by something that gets presented. */
TEST(ui_rects, a_full_list_keeps_every_rect_it_was_given)
{
    lh_ui_rects_t rects;
    lh_ui_rect_t rect;
    int i;

    lh_ui_rects_init(lh_addr_of(rects));
    for (i = 0; i < (int)LH_UI_RECTS_MAX + 6; ++i)
    {
        lh_ui_rect_init(lh_addr_of(rect), i * 20, 0, 10, 10);
        lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(rect));
    }

    EXPECT_EQ(lh_ui_rects_get_count(lh_addr_of(rects)), (lh_u32_t)LH_UI_RECTS_MAX);
    for (i = 0; i < (int)LH_UI_RECTS_MAX + 6; ++i)
    {
        lh_u32_t j;
        lh_bool_t covered = lh_bool_false;
        lh_ui_rect_init(lh_addr_of(rect), i * 20, 0, 10, 10);
        for (j = 0u; j < lh_ui_rects_get_count(lh_addr_of(rects)); ++j)
        {
            covered = covered || lh_ui_rect_intersects(lh_addr_of(rect),
                                                        lh_ui_rects_get_as_const(lh_addr_of(rects), j))
                          ? lh_bool_true
                          : lh_bool_false;
        }
        EXPECT_TRUE(covered);
    }
}

TEST(ui_rects, the_pair_that_wastes_least_is_the_one_that_merges)
{
    lh_ui_rects_t rects;
    lh_ui_rect_t rect;
    lh_u32_t i;
    int x;

    lh_ui_rects_init(lh_addr_of(rects));
    for (i = 0u; i < (lh_u32_t)LH_UI_RECTS_MAX; ++i)
    {
        lh_ui_rect_init(lh_addr_of(rect), (int)i * 100, 0, 10, 10);
        lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(rect));
    }
    /* One pixel from the first rect, a long way from every other. */
    x = 10;
    lh_ui_rect_init(lh_addr_of(rect), x, 0, 4, 10);
    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(rect));

    EXPECT_EQ(lh_ui_rects_get_count(lh_addr_of(rects)), (lh_u32_t)LH_UI_RECTS_MAX);
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(rects.rects[0]))), 14);
}

TEST(ui_rects, waste_is_the_hull_that_nobody_asked_for)
{
    lh_ui_rect_t left;
    lh_ui_rect_t right;

    lh_ui_rect_init(lh_addr_of(left), 0, 0, 10, 10);
    lh_ui_rect_init(lh_addr_of(right), 30, 0, 10, 10);

    /* hull 40x10 = 400, 100 + 100 of it already covered */
    EXPECT_EQ(lh_ui_rects_waste(lh_addr_of(left), lh_addr_of(right)), 200);
    EXPECT_EQ(lh_ui_rects_area(lh_addr_of(left)), 100);
    EXPECT_FALSE(lh_ui_rects_touch(lh_addr_of(left), lh_addr_of(right)));
}

TEST(ui_rects, drop_closes_the_gap_it_leaves)
{
    lh_ui_rects_t rects;
    lh_ui_rect_t a;
    lh_ui_rect_t b;
    lh_ui_rect_t c;

    lh_ui_rect_init(lh_addr_of(a), 0, 0, 10, 10);
    lh_ui_rect_init(lh_addr_of(b), 40, 0, 10, 10);
    lh_ui_rect_init(lh_addr_of(c), 80, 0, 10, 10);
    lh_ui_rects_init(lh_addr_of(rects));
    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(a));
    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(b));
    lh_ui_rects_add(lh_addr_of(rects), lh_addr_of(c));

    lh_ui_rects_drop(lh_addr_of(rects), 0u);
    EXPECT_EQ(lh_ui_rects_get_count(lh_addr_of(rects)), 2u);
    EXPECT_EQ(lh_ui_point_get_x(lh_ui_rect_get_origin_as_const(lh_addr_of(rects.rects[0]))), lh_ui_scalar(40));
    EXPECT_EQ(lh_ui_point_get_x(lh_ui_rect_get_origin_as_const(lh_addr_of(rects.rects[1]))), lh_ui_scalar(80));
    lh_ui_rects_drop(lh_addr_of(rects), 5u);
    EXPECT_EQ(lh_ui_rects_get_count(lh_addr_of(rects)), 2u);
}

} // namespace
