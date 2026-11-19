/**
 * @file focus.cpp
 * @brief Tests for `lh/gx/focus.h`, the `key` slot of the class and the key
 *        events.
 *
 * A screen with a field — a probe that takes the pointer and the keys and
 * writes down what it heard — holding a caption that takes nothing, and a
 * button-like probe that takes the pointer only. A press on the field or its
 * caption focuses the field; a press on the button or on nothing takes the
 * focus away; the keys go to the field while it has the focus and nowhere
 * after.
 */

#include <gtest/gtest.h>

#include <lh/gx/class.h>
#include <lh/gx/class/initializer.h>
#include <lh/gx/focus.h>

#include <vector>

namespace
{

/* The content of a probe: every event it heard, through either slot. */
struct Probe
{
    std::vector<lh_gx_event_kind_t> heard;
    lh_gx_key_t last_key = lh_gx_key_none;
    lh_u32_t last_code = 0U;
};

lh_void
probe_take(lh_gx_entity_t *entity, const lh_gx_event_t *event)
{
    auto *probe = static_cast<Probe *>(lh_gx_entity_get_content(entity));

    probe->heard.push_back(lh_gx_event_get_kind(event));
    probe->last_key = lh_gx_event_get_key(event);
    probe->last_code = lh_gx_event_get_code(event);
}

const lh_gx_class_t field_class =
    lh_gx_class_initializer(lh_null, lh_null, lh_null, probe_take, probe_take, lh_null, lh_null);
const lh_gx_class_t button_class =
    lh_gx_class_initializer(lh_null, lh_null, lh_null, probe_take, lh_null, lh_null, lh_null);

struct Scene
{
    Probe field_probe;
    Probe button_probe;
    lh_gx_entity_t screen;
    lh_gx_entity_t field;
    lh_gx_entity_t caption;
    lh_gx_entity_t button;
    lh_gx_focus_t focus;

    Scene()
    {
        lh_gx_entity_init(&screen, nullptr, nullptr, 0, 0, 40, 20);
        lh_gx_entity_init(&field, &field_class, nullptr, 0, 0, 20, 10);
        lh_gx_entity_init(&caption, nullptr, nullptr, 2, 2, 10, 5);
        lh_gx_entity_init(&button, &button_class, nullptr, 20, 0, 20, 10);
        lh_gx_entity_set_content(&field, &field_probe);
        lh_gx_entity_set_content(&button, &button_probe);
        lh_gx_entity_attach(&screen, &field);
        lh_gx_entity_attach(&field, &caption);
        lh_gx_entity_attach(&screen, &button);
        lh_gx_focus_init(&focus);
    }
};

TEST(gx_event, a_key_and_a_text_carry_their_key_and_code)
{
    lh_gx_event_t event;

    lh_gx_event_init_key(&event, lh_gx_key_backspace);
    EXPECT_EQ(lh_gx_event_get_kind(&event), lh_gx_event_kind_key);
    EXPECT_EQ(lh_gx_event_get_key(&event), lh_gx_key_backspace);
    EXPECT_EQ(lh_gx_event_get_code(&event), 0U);
    EXPECT_FALSE(lh_gx_event_is_pointer(&event));
    lh_gx_event_init_text(&event, 0x42FU);
    EXPECT_EQ(lh_gx_event_get_kind(&event), lh_gx_event_kind_text);
    EXPECT_EQ(lh_gx_event_get_code(&event), 0x42FU);
    EXPECT_EQ(lh_gx_event_get_key(&event), lh_gx_key_none);
    lh_gx_point_t at;
    lh_gx_point_init(&at, 3, 4);
    lh_gx_event_init(&event, lh_gx_event_kind_cancel, at);
    EXPECT_TRUE(lh_gx_event_is_pointer(&event));
    lh_gx_event_init_kind(&event, lh_gx_event_kind_blur);
    EXPECT_FALSE(lh_gx_event_is_pointer(&event));
}

TEST(gx_focus, only_an_entity_whose_class_has_key_takes_keys)
{
    Scene scene;

    EXPECT_TRUE(lh_gx_entity_can_take_keys(&scene.field));
    EXPECT_FALSE(lh_gx_entity_can_take_keys(&scene.button));
    EXPECT_FALSE(lh_gx_entity_can_take_keys(&scene.caption)) << "no class at all";
    EXPECT_EQ(lh_gx_focus_find(&scene.caption), &scene.field) << "its owner takes keys";
    EXPECT_TRUE(lh_gx_focus_find(&scene.button) == nullptr);
    EXPECT_TRUE(lh_gx_focus_find(nullptr) == nullptr);
}

TEST(gx_focus, a_press_on_the_field_or_its_caption_focuses_the_field)
{
    Scene scene;

    lh_gx_focus_press(&scene.focus, &scene.caption);
    EXPECT_EQ(lh_gx_focus_get_focused(&scene.focus), &scene.field);
    ASSERT_EQ(scene.field_probe.heard.size(), 1U);
    EXPECT_EQ(scene.field_probe.heard[0], lh_gx_event_kind_focus);
    lh_gx_focus_press(&scene.focus, &scene.field);
    EXPECT_EQ(scene.field_probe.heard.size(), 1U) << "it has the focus already: nothing is told";
}

TEST(gx_focus, a_press_elsewhere_or_on_nothing_takes_the_focus_away)
{
    Scene scene;

    lh_gx_focus_press(&scene.focus, &scene.field);
    lh_gx_focus_press(&scene.focus, &scene.button);
    EXPECT_TRUE(lh_gx_focus_get_focused(&scene.focus) == nullptr);
    EXPECT_EQ(scene.field_probe.heard.back(), lh_gx_event_kind_blur);
    EXPECT_TRUE(scene.button_probe.heard.empty()) << "the button takes no keys: no focus event";

    lh_gx_focus_press(&scene.focus, &scene.field);
    lh_gx_focus_press(&scene.focus, nullptr);
    EXPECT_TRUE(lh_gx_focus_get_focused(&scene.focus) == nullptr);
    EXPECT_EQ(scene.field_probe.heard.size(), 4U) << "focus, blur, focus, blur";
}

TEST(gx_focus, the_keys_go_to_the_entity_with_the_focus_and_nowhere_without_one)
{
    Scene scene;
    lh_gx_event_t event;

    lh_gx_event_init_text(&event, 0x61U);
    EXPECT_FALSE(lh_gx_focus_take(&scene.focus, &event));
    lh_gx_focus_move(&scene.focus, &scene.field);
    EXPECT_TRUE(lh_gx_focus_take(&scene.focus, &event));
    EXPECT_EQ(scene.field_probe.last_code, 0x61U);
    lh_gx_event_init_key(&event, lh_gx_key_left);
    EXPECT_TRUE(lh_gx_focus_take(&scene.focus, &event));
    EXPECT_EQ(scene.field_probe.last_key, lh_gx_key_left);
    EXPECT_EQ(scene.field_probe.heard,
              (std::vector<lh_gx_event_kind_t>{lh_gx_event_kind_focus, lh_gx_event_kind_text,
                                               lh_gx_event_kind_key}));
}

} // namespace
