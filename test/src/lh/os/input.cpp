#include <gtest/gtest.h>

#include <lh/os/input.h>
#include <lh/os/system/input.h>
#include <lh/core/input/wheel.h>

namespace
{

/* The slot is the platform kernel's answer, whatever the system is set to on the
   machine that runs the test, and the context is not read. */
TEST(os_input, the_wheel_lines_are_the_kernels)
{
    int unused = 0;

    EXPECT_EQ(lh_os_input_get_wheel_lines(nullptr), lh_os_system_input_get_wheel_lines());
    EXPECT_EQ(lh_os_input_get_wheel_lines(&unused), lh_os_system_input_get_wheel_lines());
}

/* It fits the contract: a pointer of the contract's type takes it. */
TEST(os_input, the_wheel_lines_fill_the_contract)
{
    lh_input_wheel_lines_fn *slot = lh_os_input_get_wheel_lines;

    EXPECT_EQ(slot(nullptr), lh_os_input_get_wheel_lines(nullptr));
}

TEST(os_input, the_wheel_lines_make_a_step_in_range)
{
    const lh_s32_t step = lh_input_wheel_step_of(lh_os_input_get_wheel_lines(nullptr));

    EXPECT_GE(step, 0);
    EXPECT_LE(step, lh_input_wheel_step_of(LH_INPUT_WHEEL_LINES_MAX));
}

TEST(os_input, the_wheel_lines_are_asked_again_and_answer_the_same)
{
    EXPECT_EQ(lh_os_input_get_wheel_lines(nullptr), lh_os_input_get_wheel_lines(nullptr));
}

} // namespace
