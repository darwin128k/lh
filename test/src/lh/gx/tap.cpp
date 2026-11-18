/**
 * @file tap.cpp
 * @brief Tests for `lh/gx/tap.h`.
 *
 * An entity at (10,10) 20 by 10 on a screen, and the events its `input` slot
 * would get: a press begins the gesture, a release on the entity is a tap and
 * one beside it is not, a cancel ends it with no tap, and an end with no press
 * in progress — or a move — changes nothing.
 */

#include <gtest/gtest.h>

#include <lh/gx/tap.h>

namespace
{

/* A screen and the entity on it. */
struct Scene
{
    lh_gx_entity_t screen;
    lh_gx_entity_t card;
    lh_gx_tap_t tap;

    Scene()
    {
        lh_gx_entity_init(&screen, nullptr, nullptr, 0, 0, 40, 30);
        lh_gx_entity_init(&card, nullptr, nullptr, 10, 10, 20, 10);
        lh_gx_entity_attach(&screen, &card);
        lh_gx_tap_init(&tap);
    }

    lh_gx_tap_step_t
    take(lh_gx_event_kind_t kind, lh_gx_scalar_t x, lh_gx_scalar_t y)
    {
        lh_gx_point_t at;

        lh_gx_point_init(&at, x, y);
        lh_gx_event_t event;

        lh_gx_event_init(&event, kind, at);
        return lh_gx_tap_take(&tap, &card, &event);
    }
};

TEST(gx_tap, a_press_and_a_release_on_the_entity_is_a_tap)
{
    Scene scene;

    EXPECT_FALSE(lh_gx_tap_is_pressed(&scene.tap));
    EXPECT_EQ(scene.take(lh_gx_event_kind_press, 12, 12), lh_gx_tap_step_down);
    EXPECT_TRUE(lh_gx_tap_is_pressed(&scene.tap));
    EXPECT_EQ(scene.take(lh_gx_event_kind_move, 0, 0), lh_gx_tap_step_none);
    EXPECT_TRUE(lh_gx_tap_is_pressed(&scene.tap)) << "a move does not end it";
    EXPECT_EQ(scene.take(lh_gx_event_kind_release, 29, 19), lh_gx_tap_step_tap);
    EXPECT_FALSE(lh_gx_tap_is_pressed(&scene.tap));
}

TEST(gx_tap, a_release_beside_the_entity_or_a_cancel_is_no_tap)
{
    Scene scene;

    scene.take(lh_gx_event_kind_press, 12, 12);
    EXPECT_EQ(scene.take(lh_gx_event_kind_release, 30, 12), lh_gx_tap_step_up)
        << "one past the right edge";
    scene.take(lh_gx_event_kind_press, 12, 12);
    EXPECT_EQ(scene.take(lh_gx_event_kind_cancel, 0, 0), lh_gx_tap_step_up);
    EXPECT_FALSE(lh_gx_tap_is_pressed(&scene.tap));
}

TEST(gx_tap, an_end_with_no_press_in_progress_changes_nothing)
{
    Scene scene;

    EXPECT_EQ(scene.take(lh_gx_event_kind_release, 12, 12), lh_gx_tap_step_none);
    EXPECT_EQ(scene.take(lh_gx_event_kind_cancel, 0, 0), lh_gx_tap_step_none);
    EXPECT_EQ(lh_gx_tap_end(&scene.tap, lh_bool_true), lh_gx_tap_step_none);
}

} // namespace
