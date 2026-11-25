/**
 * @file event.cpp
 * @brief ::lh_input_event_t: each way of building one, and every accessor.
 */

#include <gtest/gtest.h>

#include <lh/core/input/event.h>
#include <lh/core/input/event/initializer.h>

namespace
{

TEST(input_event, initializer_follows_the_field_order)
{
    const lh_input_event_t event = lh_input_event_initializer(
        lh_input_kind_pointer_down, lh_input_device_kind_touch, 3U, 2U, lh_input_button_primary,
        0x2U, lh_input_coord_from_int(10), lh_input_coord_from_int(20), 5, -6, lh_input_axis_left_x,
        7, lh_key_tab, 8U, 9U, LH_INPUT_MODS_SHIFT, 1000U, 1U, LH_INPUT_FLAG_PRIMARY);

    EXPECT_EQ(lh_input_event_get_kind(&event), lh_input_kind_pointer_down);
    EXPECT_EQ(lh_input_event_get_device_kind(&event), lh_input_device_kind_touch);
    EXPECT_EQ(lh_input_event_get_device(&event), 3U);
    EXPECT_EQ(lh_input_event_get_pointer(&event), 2U);
    EXPECT_EQ(lh_input_event_get_button(&event), lh_input_button_primary);
    EXPECT_EQ(lh_input_event_get_buttons(&event), 0x2U);
    EXPECT_EQ(lh_input_event_get_x(&event), 2560);
    EXPECT_EQ(lh_input_event_get_y(&event), 5120);
    EXPECT_EQ(lh_input_event_get_dx(&event), 5);
    EXPECT_EQ(lh_input_event_get_dy(&event), -6);
    EXPECT_EQ(lh_input_event_get_axis(&event), lh_input_axis_left_x);
    EXPECT_EQ(lh_input_event_get_value(&event), 7);
    EXPECT_EQ(lh_input_event_get_key(&event), lh_key_tab);
    EXPECT_EQ(lh_input_event_get_scancode(&event), 8U);
    EXPECT_EQ(lh_input_event_get_code(&event), 9U);
    EXPECT_EQ(lh_input_event_get_mods(&event), LH_INPUT_MODS_SHIFT);
    EXPECT_EQ(lh_input_event_get_time(&event), 1000U);
    EXPECT_EQ(lh_input_event_get_count(&event), 1U);
    EXPECT_EQ(lh_input_event_get_flags(&event), LH_INPUT_FLAG_PRIMARY);
}

TEST(input_event, empty_initializer_matches_init)
{
    const lh_input_event_t empty = lh_input_event_empty_initializer();
    lh_input_event_t event;

    lh_input_event_init(&event, lh_input_kind_none, lh_input_device_kind_none, 0U);
    EXPECT_EQ(lh_input_event_get_kind(&empty), lh_input_event_get_kind(&event));
    EXPECT_EQ(lh_input_event_get_device_kind(&empty), lh_input_event_get_device_kind(&event));
    EXPECT_EQ(lh_input_event_get_button(&empty), lh_input_event_get_button(&event));
    EXPECT_EQ(lh_input_event_get_buttons(&empty), lh_input_event_get_buttons(&event));
    EXPECT_EQ(lh_input_event_get_x(&empty), lh_input_event_get_x(&event));
    EXPECT_EQ(lh_input_event_get_axis(&empty), lh_input_event_get_axis(&event));
    EXPECT_EQ(lh_input_event_get_key(&empty), lh_input_event_get_key(&event));
    EXPECT_EQ(lh_input_event_get_mods(&empty), lh_input_event_get_mods(&event));
    EXPECT_EQ(lh_input_event_get_time(&empty), lh_input_event_get_time(&event));
    EXPECT_EQ(lh_input_event_get_count(&empty), lh_input_event_get_count(&event));
    EXPECT_EQ(lh_input_event_get_flags(&empty), lh_input_event_get_flags(&event));
}

TEST(input_event, pointer_zero_is_primary_and_others_are_not)
{
    lh_input_event_t event;

    lh_input_event_init_pointer(&event, lh_input_kind_pointer_up, lh_input_device_kind_mouse, 0U,
                                lh_input_button_secondary, lh_input_coord_from_int(-1), 384, 42U);
    EXPECT_TRUE(lh_input_event_is_pointer(&event));
    EXPECT_TRUE(lh_input_event_is_primary(&event));
    EXPECT_FALSE(lh_input_event_is_keyboard(&event));
    EXPECT_EQ(lh_input_coord_to_int(lh_input_event_get_x(&event)), -1);
    EXPECT_EQ(lh_input_coord_to_int(lh_input_event_get_y(&event)), 1);
    EXPECT_EQ(lh_input_event_get_button(&event), lh_input_button_secondary);
    EXPECT_EQ(lh_input_event_get_time(&event), 42U);

    lh_input_event_init_pointer(&event, lh_input_kind_pointer_down, lh_input_device_kind_touch, 1U,
                                lh_input_button_primary, 0, 0, 0U);
    EXPECT_FALSE(lh_input_event_is_primary(&event));
}

TEST(input_event, wheel_is_a_mouse_pointer_event_in_notch_units)
{
    lh_input_event_t event;

    lh_input_event_init_wheel(&event, 0, 0, 0, -LH_INPUT_WHEEL_NOTCH, 7U);
    EXPECT_EQ(lh_input_event_get_kind(&event), lh_input_kind_wheel);
    EXPECT_EQ(lh_input_event_get_device_kind(&event), lh_input_device_kind_mouse);
    EXPECT_EQ(lh_input_event_get_dy(&event), -120);
    EXPECT_TRUE(lh_input_event_is_pointer(&event));
}

TEST(input_event, key_carries_direction_repeat_and_scancode)
{
    lh_input_event_t event;

    lh_input_event_init_key(&event, lh_key_enter, lh_bool_true, lh_bool_true, 0x0DU, 5U);
    EXPECT_EQ(lh_input_event_get_kind(&event), lh_input_kind_key_down);
    EXPECT_EQ(lh_input_event_get_device_kind(&event), lh_input_device_kind_keyboard);
    EXPECT_TRUE(lh_input_event_is_repeat(&event));
    EXPECT_TRUE(lh_input_event_is_keyboard(&event));
    EXPECT_TRUE(lh_input_event_is_key_down(&event, lh_key_enter));
    EXPECT_FALSE(lh_input_event_is_key_down(&event, lh_key_tab));
    EXPECT_EQ(lh_input_event_get_scancode(&event), 0x0DU);

    lh_input_event_init_key(&event, lh_key_enter, lh_bool_false, lh_bool_false, 0U, 6U);
    EXPECT_EQ(lh_input_event_get_kind(&event), lh_input_kind_key_up);
    EXPECT_FALSE(lh_input_event_is_repeat(&event));
    EXPECT_FALSE(lh_input_event_is_key_down(&event, lh_key_enter));
}

TEST(input_event, text_carries_the_code_point)
{
    lh_input_event_t event;

    lh_input_event_init_text(&event, 0x1F600U, 1U);
    EXPECT_EQ(lh_input_event_get_kind(&event), lh_input_kind_text);
    EXPECT_EQ(lh_input_event_get_code(&event), 0x1F600U);
    EXPECT_TRUE(lh_input_event_is_keyboard(&event));
}

TEST(input_event, controller_button_and_axis)
{
    lh_input_event_t event;

    lh_input_event_init_controller_button(&event, 2U, lh_input_button_south, lh_bool_true, 3U);
    EXPECT_EQ(lh_input_event_get_kind(&event), lh_input_kind_controller_button_down);
    EXPECT_EQ(lh_input_event_get_device(&event), 2U);
    EXPECT_EQ(lh_input_event_get_button(&event), lh_input_button_south);
    EXPECT_TRUE(lh_input_event_is_controller(&event));
    EXPECT_FALSE(lh_input_event_is_pointer(&event));

    lh_input_event_init_controller_button(&event, 2U, lh_input_button_south, lh_bool_false, 4U);
    EXPECT_EQ(lh_input_event_get_kind(&event), lh_input_kind_controller_button_up);

    lh_input_event_init_controller_axis(&event, 1U, lh_input_axis_trigger_left, 32767, 5U);
    EXPECT_EQ(lh_input_event_get_kind(&event), lh_input_kind_controller_axis);
    EXPECT_EQ(lh_input_event_get_axis(&event), lh_input_axis_trigger_left);
    EXPECT_EQ(lh_input_event_get_value(&event), 32767);
    EXPECT_TRUE(lh_input_event_is_controller(&event));
}

TEST(input_event, setters_and_flag_queries)
{
    lh_input_event_t event;

    lh_input_event_init(&event, lh_input_kind_click, lh_input_device_kind_mouse, 9U);
    lh_input_event_set_mods(&event, LH_INPUT_MODS_SHIFT | LH_INPUT_MODS_CONTROL);
    lh_input_event_set_buttons(&event, lh_input_button_to_mask(lh_input_button_middle));
    lh_input_event_set_device(&event, 4U);
    lh_input_event_set_count(&event, 2U);
    lh_input_event_set_delta(&event, 3, -4);
    lh_input_event_set_flags(&event, LH_INPUT_FLAG_SYNTHESIZED | LH_INPUT_FLAG_CANCELLED);

    EXPECT_TRUE(lh_input_event_has_mods(&event, LH_INPUT_MODS_SHIFT));
    EXPECT_TRUE(lh_input_event_has_mods(&event, LH_INPUT_MODS_SHIFT | LH_INPUT_MODS_CONTROL));
    EXPECT_FALSE(lh_input_event_has_mods(&event, LH_INPUT_MODS_ALT));
    EXPECT_EQ(lh_input_event_get_buttons(&event), 0x8U);
    EXPECT_EQ(lh_input_event_get_device(&event), 4U);
    EXPECT_EQ(lh_input_event_get_count(&event), 2U);
    EXPECT_EQ(lh_input_event_get_dx(&event), 3);
    EXPECT_EQ(lh_input_event_get_dy(&event), -4);
    EXPECT_TRUE(lh_input_event_has_flag(&event, LH_INPUT_FLAG_SYNTHESIZED));
    EXPECT_FALSE(lh_input_event_has_flag(&event, LH_INPUT_FLAG_REPEAT));
    EXPECT_TRUE(lh_input_event_is_synthesized(&event));
    EXPECT_TRUE(lh_input_event_is_cancelled(&event));
    EXPECT_FALSE(lh_input_event_is_repeat(&event));
    EXPECT_FALSE(lh_input_event_is_primary(&event));
    EXPECT_TRUE(lh_input_event_is_pointer(&event));
}

TEST(input_event, focus_and_devices_are_neither_pointer_nor_keyboard)
{
    lh_input_event_t event;

    lh_input_event_init(&event, lh_input_kind_focus_out, lh_input_device_kind_none, 0U);
    EXPECT_FALSE(lh_input_event_is_pointer(&event));
    EXPECT_FALSE(lh_input_event_is_keyboard(&event));
    EXPECT_FALSE(lh_input_event_is_controller(&event));
    lh_input_event_init(&event, lh_input_kind_navigate, lh_input_device_kind_none, 0U);
    EXPECT_FALSE(lh_input_event_is_pointer(&event));
}

} // namespace
