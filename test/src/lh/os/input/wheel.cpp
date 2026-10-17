#include <gtest/gtest.h>

#include <lh/os/input/wheel.h>

namespace
{

/* The default is what a view scrolled before it asked the system, so it has to stay 40,
   and the default three lines have to make exactly that - not 39 or 42 from a line
   height rounded to a whole unit. */
TEST(os_input_wheel, the_default_lines_are_the_default_step)
{
    EXPECT_EQ(LH_OS_INPUT_WHEEL_STEP_DEFAULT, 40);
    EXPECT_EQ(LH_OS_INPUT_WHEEL_LINES_DEFAULT, 3);
    EXPECT_EQ(lh_os_input_wheel_step_of(LH_OS_INPUT_WHEEL_LINES_DEFAULT),
              LH_OS_INPUT_WHEEL_STEP_DEFAULT);
}

TEST(os_input_wheel, a_line_is_forty_thirds_cut_towards_zero)
{
    EXPECT_EQ(lh_os_input_wheel_step_of(1U), 13);
    EXPECT_EQ(lh_os_input_wheel_step_of(2U), 26);
    EXPECT_EQ(lh_os_input_wheel_step_of(5U), 66);
    EXPECT_EQ(lh_os_input_wheel_step_of(6U), 80) << "twice the default lines, twice the step";
    EXPECT_EQ(lh_os_input_wheel_step_of(LH_OS_INPUT_WHEEL_LINES_PAGE), 266);
}

TEST(os_input_wheel, no_lines_scroll_nothing)
{
    EXPECT_EQ(lh_os_input_wheel_step_of(0U), 0) << "the system is set not to scroll";
}

TEST(os_input_wheel, more_lines_than_the_most_count_as_the_most)
{
    const lh_s32_t most = lh_os_input_wheel_step_of(LH_OS_INPUT_WHEEL_LINES_MAX);

    EXPECT_EQ(most, 1333);
    EXPECT_EQ(lh_os_input_wheel_step_of(LH_OS_INPUT_WHEEL_LINES_MAX + 1U), most);
    EXPECT_EQ(lh_os_input_wheel_step_of(0xFFFFFFFFU), most)
        << "a raw WHEEL_PAGESCROLL must not overflow the step";
}

} // namespace
