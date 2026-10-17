#include <gtest/gtest.h>

#include <lh/os/input/wheel.h>

/* Backend-private (src/lh/os/system/unix/input.h), so declared here: the test includes
   only the public tree. */
extern "C" lh_u32_t
lh_os_system_unix_input_get_wheel_lines(void);

namespace
{

/* The driver's increment is not read (that needs libinput or evdev), so a notch is the
   default lines - and the test says so, so a kernel that starts reading the driver has
   to change it. */
TEST(os_system_unix_input, the_driver_is_not_read_so_a_notch_is_the_default_lines)
{
    EXPECT_EQ(lh_os_system_unix_input_get_wheel_lines(),
              static_cast<lh_u32_t>(LH_OS_INPUT_WHEEL_LINES_DEFAULT));
}

} // namespace
