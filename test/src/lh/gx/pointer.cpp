/**
 * @file pointer.cpp
 * @brief Tests for `lh/gx/event.h`, `lh/gx/hit.h`, `lh/gx/pointer.h` and the
 *        `hit` and `input` slots of the class.
 *
 * The test's own class is a probe: it takes input and writes down what it
 * heard. Around it: the front-most entity that takes input is found, a text on
 * top is passed over, a hidden one and one outside its owner are not hit, a
 * class's `hit` narrows its rectangle; a gesture goes to the entity it started
 * on, wherever it ends.
 */

#include <gtest/gtest.h>

#include <lh/gx/class.h>
#include <lh/gx/class/initializer.h>
#include <lh/gx/hit.h>
#include <lh/gx/pointer.h>

#include <vector>

namespace
{

/* The content of a probe: every event it heard. */
struct Probe
{
    std::vector<lh_gx_event_kind_t> heard;
    lh_gx_scalar_t last_x = -1;
};

lh_void
probe_input(lh_gx_entity_t *entity, const lh_gx_event_t *event)
{
    auto *probe = static_cast<Probe *>(lh_gx_entity_get_content(entity));

    probe->heard.push_back(lh_gx_event_get_kind(event));
    probe->last_x = lh_gx_point_get_x(lh_gx_event_get_at(event));
}

/* Hit only in the left half of its rectangle. */
lh_bool_t
left_half_hit(const lh_gx_entity_t *, const lh_gx_rect_t *place, const lh_gx_point_t *at)
{
    const lh_gx_scalar_t left = lh_gx_point_get_x(lh_gx_rect_get_origin(place));

    return lh_cast_static(lh_bool_t,
                          lh_gx_point_get_x(at) <
                              left + lh_gx_size_get_width(lh_gx_rect_get_size(place)) / 2);
}

const lh_gx_class_t probe_class =
    lh_gx_class_initializer(lh_null, lh_null, lh_null, probe_input, lh_null);
const lh_gx_class_t half_class =
    lh_gx_class_initializer(lh_null, lh_null, left_half_hit, probe_input, lh_null);
const lh_gx_class_t layer_class =
    lh_gx_class_initializer(lh_null, lh_null, lh_null, lh_null, lh_null);

lh_gx_point_t
point(lh_gx_scalar_t x, lh_gx_scalar_t y)
{
    lh_gx_point_t at;

    lh_gx_point_init(&at, x, y);
    return at;
}

/* A 20 by 10 screen: card `back` at (0,0) 10 by 10, card `front` over its right
   half at (5,0), and a plain layer `cover` with no input over `front`. */
struct Scene
{
    Probe back_probe;
    Probe front_probe;
    lh_gx_entity_t screen;
    lh_gx_entity_t back;
    lh_gx_entity_t front;
    lh_gx_entity_t cover;

    Scene()
    {
        lh_gx_entity_init(&screen, nullptr, nullptr, 0, 0, 20, 10);
        lh_gx_entity_init(&back, &probe_class, nullptr, 0, 0, 10, 10);
        lh_gx_entity_init(&front, &probe_class, nullptr, 5, 0, 10, 10);
        lh_gx_entity_init(&cover, &layer_class, nullptr, 5, 0, 10, 10);
        lh_gx_entity_set_content(&back, &back_probe);
        lh_gx_entity_set_content(&front, &front_probe);
        lh_gx_entity_attach(&screen, &back);
        lh_gx_entity_attach(&screen, &front);
        lh_gx_entity_attach(&screen, &cover);
    }
};

TEST(gx_event, an_event_is_a_kind_and_a_point)
{
    lh_gx_event_t event;

    lh_gx_event_init(&event, lh_gx_event_kind_release, point(3, 4));
    EXPECT_EQ(lh_gx_event_get_kind(&event), lh_gx_event_kind_release);
    EXPECT_EQ(lh_gx_point_get_y(lh_gx_event_get_at(&event)), 4);
}

TEST(gx_hit, the_front_most_entity_that_takes_input_is_found)
{
    Scene scene;
    const lh_gx_point_t left = point(2, 2);
    const lh_gx_point_t middle = point(7, 2);
    const lh_gx_point_t right = point(17, 2);

    EXPECT_EQ(lh_gx_hit_find(&scene.screen, &left), &scene.back);
    EXPECT_EQ(lh_gx_hit_find(&scene.screen, &middle), &scene.front)
        << "front is drawn after back; the layer over it takes no input";
    EXPECT_TRUE(lh_gx_hit_find(&scene.screen, &right) == nullptr) << "nothing there";
    EXPECT_FALSE(lh_gx_entity_can_take_input(&scene.cover));
    EXPECT_FALSE(lh_gx_entity_can_take_input(&scene.screen)) << "no class at all";
}

TEST(gx_hit, a_hidden_entity_is_not_hit)
{
    Scene scene;
    const lh_gx_point_t middle = point(7, 2);

    lh_gx_entity_set_visible(&scene.front, lh_bool_false);
    EXPECT_EQ(lh_gx_hit_find(&scene.screen, &middle), &scene.back);
}

TEST(gx_hit, a_child_is_not_hit_outside_its_owner)
{
    Scene scene;
    Probe probe;
    lh_gx_entity_t child;
    const lh_gx_point_t outside = point(12, 2);
    const lh_gx_point_t inside = point(8, 2);

    /* At (8,0) inside back, 6 wide: it sticks out of back, where it is cut. */
    lh_gx_entity_init(&child, &probe_class, nullptr, 8, 0, 6, 10);
    lh_gx_entity_set_content(&child, &probe);
    lh_gx_entity_detach(&scene.front);
    lh_gx_entity_attach(&scene.back, &child);
    EXPECT_TRUE(lh_gx_hit_find(&scene.screen, &outside) == nullptr);
    EXPECT_EQ(lh_gx_hit_find(&scene.screen, &inside), &child);
}

TEST(gx_hit, the_class_says_where_in_its_rectangle_it_is)
{
    Scene scene;
    const lh_gx_point_t left_of_front = point(7, 2);
    const lh_gx_point_t right_of_front = point(12, 2);

    lh_gx_entity_set_class(&scene.front, &half_class);
    EXPECT_EQ(lh_gx_hit_find(&scene.screen, &left_of_front), &scene.front);
    EXPECT_TRUE(lh_gx_hit_find(&scene.screen, &right_of_front) == nullptr)
        << "the right half of front is not front";
}

TEST(gx_pointer, a_gesture_goes_to_the_entity_it_started_on)
{
    Scene scene;
    lh_gx_pointer_t pointer;
    const lh_gx_point_t on_front = point(7, 2);
    const lh_gx_point_t on_back = point(2, 2);

    lh_gx_pointer_init(&pointer);
    lh_gx_pointer_press(&pointer, &scene.screen, &on_front);
    EXPECT_EQ(lh_gx_pointer_get_captured(&pointer), &scene.front);
    lh_gx_pointer_move(&pointer, &on_back);
    lh_gx_pointer_release(&pointer, &on_back);
    EXPECT_TRUE(lh_gx_pointer_get_captured(&pointer) == nullptr);
    ASSERT_EQ(scene.front_probe.heard.size(), 3U);
    EXPECT_EQ(scene.front_probe.heard[0], lh_gx_event_kind_press);
    EXPECT_EQ(scene.front_probe.heard[1], lh_gx_event_kind_move);
    EXPECT_EQ(scene.front_probe.heard[2], lh_gx_event_kind_release);
    EXPECT_EQ(scene.front_probe.last_x, 2) << "released over back, still heard by front";
    EXPECT_TRUE(scene.back_probe.heard.empty());
}

TEST(gx_pointer, a_press_on_nothing_captures_nothing)
{
    Scene scene;
    lh_gx_pointer_t pointer;
    const lh_gx_point_t nowhere = point(17, 2);

    lh_gx_pointer_init(&pointer);
    lh_gx_pointer_press(&pointer, &scene.screen, &nowhere);
    lh_gx_pointer_release(&pointer, &nowhere);
    EXPECT_TRUE(lh_gx_pointer_get_captured(&pointer) == nullptr);
    EXPECT_TRUE(scene.front_probe.heard.empty());
}

TEST(gx_pointer, a_new_press_cancels_the_gesture_in_progress)
{
    Scene scene;
    lh_gx_pointer_t pointer;
    const lh_gx_point_t on_front = point(7, 2);
    const lh_gx_point_t on_back = point(2, 2);

    lh_gx_pointer_init(&pointer);
    lh_gx_pointer_press(&pointer, &scene.screen, &on_front);
    lh_gx_pointer_press(&pointer, &scene.screen, &on_back);
    ASSERT_EQ(scene.front_probe.heard.size(), 2U);
    EXPECT_EQ(scene.front_probe.heard[1], lh_gx_event_kind_cancel);
    EXPECT_EQ(lh_gx_pointer_get_captured(&pointer), &scene.back);
    lh_gx_pointer_cancel(&pointer);
    EXPECT_EQ(scene.back_probe.heard.back(), lh_gx_event_kind_cancel);
    EXPECT_TRUE(lh_gx_pointer_get_captured(&pointer) == nullptr);
}

} // namespace
