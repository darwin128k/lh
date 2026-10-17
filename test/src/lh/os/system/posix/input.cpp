#include <gtest/gtest.h>

#include <lh/os/input/event.h>

/* Backend-private (src/lh/os/system/posix/input.h), so declared here: the test
   includes only the public tree. */
extern "C" lh_os_input_button_t
lh_os_system_posix_input_button_of(lh_u32_t xbutton);
extern "C" lh_bool_t
lh_os_system_posix_input_wheel_of(lh_u32_t xbutton, lh_s32_t *dx, lh_s32_t *dy);
extern "C" lh_os_input_mods_t
lh_os_system_posix_input_mods_of(lh_u32_t state);
extern "C" lh_u32_t
lh_os_system_posix_input_buttons_of(lh_u32_t state);

namespace
{

TEST(os_system_posix_input, x_numbers_its_buttons_left_middle_right)
{
    EXPECT_EQ(lh_os_system_posix_input_button_of(1U), lh_os_input_button_primary);
    EXPECT_EQ(lh_os_system_posix_input_button_of(2U), lh_os_input_button_middle)
        << "2 is the middle button, not the right one";
    EXPECT_EQ(lh_os_system_posix_input_button_of(3U), lh_os_input_button_secondary);
    EXPECT_EQ(lh_os_system_posix_input_button_of(8U), lh_os_input_button_back);
    EXPECT_EQ(lh_os_system_posix_input_button_of(9U), lh_os_input_button_forward);
    EXPECT_EQ(lh_os_system_posix_input_button_of(4U), lh_os_input_button_none) << "the wheel";
}

TEST(os_system_posix_input, a_wheel_button_is_one_notch_positive_up_and_left)
{
    lh_s32_t dx = 7;
    lh_s32_t dy = 7;

    EXPECT_TRUE(lh_os_system_posix_input_wheel_of(4U, &dx, &dy));
    EXPECT_EQ(dx, 0);
    EXPECT_EQ(dy, LH_OS_INPUT_WHEEL_NOTCH);
    EXPECT_TRUE(lh_os_system_posix_input_wheel_of(5U, &dx, &dy));
    EXPECT_EQ(dy, -LH_OS_INPUT_WHEEL_NOTCH);
    EXPECT_TRUE(lh_os_system_posix_input_wheel_of(6U, &dx, &dy));
    EXPECT_EQ(dx, LH_OS_INPUT_WHEEL_NOTCH);
    EXPECT_EQ(dy, 0);
    EXPECT_TRUE(lh_os_system_posix_input_wheel_of(7U, &dx, &dy));
    EXPECT_EQ(dx, -LH_OS_INPUT_WHEEL_NOTCH);
    EXPECT_FALSE(lh_os_system_posix_input_wheel_of(1U, &dx, &dy));
    EXPECT_EQ(dx, 0);
    EXPECT_EQ(dy, 0);
}

TEST(os_system_posix_input, the_state_mask_reads_as_modifiers)
{
    EXPECT_EQ(lh_os_system_posix_input_mods_of(0U), LH_OS_INPUT_MODS_NONE);
    EXPECT_EQ(lh_os_system_posix_input_mods_of(1U << 0), LH_OS_INPUT_MODS_SHIFT);
    EXPECT_EQ(lh_os_system_posix_input_mods_of(1U << 2), LH_OS_INPUT_MODS_CONTROL);
    EXPECT_EQ(lh_os_system_posix_input_mods_of(1U << 3), LH_OS_INPUT_MODS_ALT) << "Mod1";
    EXPECT_EQ(lh_os_system_posix_input_mods_of(1U << 6), LH_OS_INPUT_MODS_META) << "Mod4";
    EXPECT_EQ(lh_os_system_posix_input_mods_of(1U << 1), LH_OS_INPUT_MODS_CAPS_LOCK) << "Lock";
    EXPECT_EQ(lh_os_system_posix_input_mods_of(1U << 4), LH_OS_INPUT_MODS_NUM_LOCK) << "Mod2";
}

TEST(os_system_posix_input, the_state_mask_reads_as_buttons_held)
{
    EXPECT_EQ(lh_os_system_posix_input_buttons_of(0U), 0U);
    EXPECT_EQ(lh_os_system_posix_input_buttons_of(1U << 8),
              lh_os_input_button_to_mask(lh_os_input_button_primary));
    EXPECT_EQ(lh_os_system_posix_input_buttons_of(1U << 9),
              lh_os_input_button_to_mask(lh_os_input_button_middle));
    EXPECT_EQ(lh_os_system_posix_input_buttons_of(1U << 10),
              lh_os_input_button_to_mask(lh_os_input_button_secondary));
    EXPECT_EQ(lh_os_system_posix_input_buttons_of(1U << 0), 0U) << "Shift is no button";
}

} // namespace
