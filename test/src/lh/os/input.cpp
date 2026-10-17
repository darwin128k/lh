#include <gtest/gtest.h>

#include <lh/compiler/os.h>
#include <lh/os/input.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS && LH_LIBRARY_OPTION_OS_WINDOW
/* Backend-private (src/lh/os/system/win/input.h), so declared here: the test includes
   only the public tree. */
extern "C" lh_u32_t
lh_os_system_win_input_get_wheel_lines(void);
#endif

namespace
{

/* The step is the platform kernel's lines through the one conversion, whatever the
   system is set to on the machine that runs the test. */
TEST(os_input, the_wheel_step_is_the_kernels_lines_as_content_units)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS && LH_LIBRARY_OPTION_OS_WINDOW
    EXPECT_EQ(lh_os_input_get_wheel_step(),
              lh_os_input_wheel_step_of(lh_os_system_win_input_get_wheel_lines()));
#else
    EXPECT_EQ(lh_os_input_get_wheel_step(), LH_OS_INPUT_WHEEL_STEP_DEFAULT)
        << "no system setting is read here: the default notch";
#endif
}

TEST(os_input, the_wheel_step_stays_in_range)
{
    const lh_s32_t step = lh_os_input_get_wheel_step();

    EXPECT_GE(step, 0);
    EXPECT_LE(step, lh_os_input_wheel_step_of(LH_OS_INPUT_WHEEL_LINES_MAX));
}

TEST(os_input, the_wheel_step_is_asked_again_and_answers_the_same)
{
    EXPECT_EQ(lh_os_input_get_wheel_step(), lh_os_input_get_wheel_step());
}

} // namespace
