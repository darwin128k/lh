/**
 * @file drag.cpp
 * @brief Tests for `lh/gx/drag.h`.
 *
 * A press begins a drag where the pointer is, moves follow it, a release ends
 * it wherever it is, a cancel takes it away without a point, and nothing but a
 * press counts while no drag is in progress.
 */

#include <gtest/gtest.h>

#include <lh/gx/drag.h>

namespace
{

lh_gx_drag_step_t
take(lh_gx_drag_t *drag, lh_gx_event_kind_t kind, lh_gx_scalar_t x, lh_gx_scalar_t y)
{
    lh_gx_point_t at;

    lh_gx_point_init(&at, x, y);
    lh_gx_event_t event;

    lh_gx_event_init(&event, kind, at);
    return lh_gx_drag_take(drag, &event);
}

lh_gx_scalar_t
x_of(const lh_gx_drag_t *drag)
{
    return lh_gx_point_get_x(lh_gx_drag_get_at(drag));
}

TEST(gx_drag, a_press_begins_moves_follow_and_a_release_ends_it_anywhere)
{
    lh_gx_drag_t drag;

    lh_gx_drag_init(&drag);
    EXPECT_FALSE(lh_gx_drag_is_active(&drag));
    EXPECT_EQ(take(&drag, lh_gx_event_kind_press, 3, 4), lh_gx_drag_step_begin);
    EXPECT_TRUE(lh_gx_drag_is_active(&drag));
    EXPECT_EQ(x_of(&drag), 3);
    EXPECT_EQ(take(&drag, lh_gx_event_kind_move, 9, 4), lh_gx_drag_step_move);
    EXPECT_EQ(x_of(&drag), 9);
    EXPECT_EQ(take(&drag, lh_gx_event_kind_release, -50, 4), lh_gx_drag_step_end);
    EXPECT_EQ(x_of(&drag), -50) << "where it ends does not matter";
    EXPECT_FALSE(lh_gx_drag_is_active(&drag));
}

TEST(gx_drag, a_cancel_takes_it_away_and_keeps_the_last_point)
{
    lh_gx_drag_t drag;

    lh_gx_drag_init(&drag);
    take(&drag, lh_gx_event_kind_press, 3, 4);
    take(&drag, lh_gx_event_kind_move, 7, 4);
    EXPECT_EQ(take(&drag, lh_gx_event_kind_cancel, 0, 0), lh_gx_drag_step_cancel);
    EXPECT_FALSE(lh_gx_drag_is_active(&drag));
    EXPECT_EQ(x_of(&drag), 7);
}

TEST(gx_drag, only_a_press_counts_with_no_drag_in_progress)
{
    lh_gx_drag_t drag;

    lh_gx_drag_init(&drag);
    EXPECT_EQ(take(&drag, lh_gx_event_kind_move, 5, 5), lh_gx_drag_step_none);
    EXPECT_EQ(take(&drag, lh_gx_event_kind_release, 5, 5), lh_gx_drag_step_none);
    EXPECT_EQ(take(&drag, lh_gx_event_kind_cancel, 5, 5), lh_gx_drag_step_none);
    EXPECT_EQ(x_of(&drag), 0) << "untouched";
}

} // namespace
