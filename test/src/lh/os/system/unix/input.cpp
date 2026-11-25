#include <gtest/gtest.h>

#include <lh/os/system/input.h>
#include <lh/core/input/recognizer/config/defaults.h>
#include <lh/core/input/wheel.h>

namespace
{

/* The driver's increment is not read (that needs libinput or evdev), so a notch is the
   default lines - and the test says so, so a kernel that starts reading the driver has
   to change it. */
TEST(os_system_unix_input, the_driver_is_not_read_so_a_notch_is_the_default_lines)
{
    EXPECT_EQ(lh_os_system_input_get_wheel_lines(),
              static_cast<lh_u32_t>(LH_INPUT_WHEEL_LINES_DEFAULT));
}

/* X keeps no drag threshold and no double click (each toolkit has its own), so the
   kernel answers lh's defaults. */
TEST(os_system_unix_input, x_keeps_no_pointer_settings_so_they_are_the_defaults)
{
    EXPECT_EQ(lh_os_system_input_get_drag_slop(),
              static_cast<lh_u32_t>(LH_INPUT_RECOGNIZER_SLOP_DEFAULT));
    EXPECT_EQ(lh_os_system_input_get_double_click_slop(),
              static_cast<lh_u32_t>(LH_INPUT_RECOGNIZER_DOUBLE_CLICK_SLOP_DEFAULT));
    EXPECT_EQ(lh_os_system_input_get_double_click_ms(),
              static_cast<lh_u32_t>(LH_INPUT_RECOGNIZER_DOUBLE_CLICK_MS_DEFAULT));
}

} // namespace
