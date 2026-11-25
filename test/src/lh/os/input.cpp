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

/* The three settings the system has are the kernels'; everything else of the config is
   the caller's and stays. */
TEST(os_input, the_recognizer_config_takes_the_systems_pointer_settings)
{
    lh_input_recognizer_config_t config;

    lh_input_recognizer_config_init_default(&config);
    lh_input_recognizer_config_set_slop(&config, 0xFFFFU);
    lh_input_recognizer_config_set_double_click_ms(&config, 0xFFFFU);
    lh_input_recognizer_config_set_double_click_slop(&config, 0xFFFFU);
    lh_os_input_read_recognizer_config(&config);
    EXPECT_EQ(lh_input_recognizer_config_get_slop(&config), lh_os_system_input_get_drag_slop());
    EXPECT_EQ(lh_input_recognizer_config_get_double_click_slop(&config),
              lh_os_system_input_get_double_click_slop());
    EXPECT_EQ(lh_input_recognizer_config_get_double_click_ms(&config),
              lh_os_system_input_get_double_click_ms());
    EXPECT_EQ(lh_input_recognizer_config_get_long_press_ms(&config), 500U) << "not the system's";
    EXPECT_EQ(lh_input_recognizer_config_get_swipe_px(&config), 48U);
}

/* Whatever the machine is set to, a drag needs some distance and a double click some
   time: the kernels never hand the recognizer a zero for "the call failed". */
TEST(os_input, the_systems_pointer_settings_are_never_zero)
{
    EXPECT_GT(lh_os_system_input_get_drag_slop(), 0U);
    EXPECT_GT(lh_os_system_input_get_double_click_slop(), 0U);
    EXPECT_GT(lh_os_system_input_get_double_click_ms(), 0U);
}

} // namespace
