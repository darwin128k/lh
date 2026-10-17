#include <gtest/gtest.h>

#include <lh/os/input/event.h>
#include <lh/os/input/wheel.h>
#include <lh/size.h>

/* Backend-private (src/lh/os/system/win/input.h), so declared here: the test
   includes only the public tree. wParam is a UINT_PTR and lParam a LONG_PTR. */
extern "C" lh_os_input_button_t
lh_os_system_win_input_button_of(lh_u32_t msg, lh_usize_t wparam);
extern "C" lh_bool_t
lh_os_system_win_input_is_button_down(lh_u32_t msg);
extern "C" lh_u32_t
lh_os_system_win_input_buttons_of(lh_usize_t wparam);
extern "C" lh_bool_t
lh_os_system_win_input_is_repeat(lh_ssize_t lparam);
extern "C" lh_void
lh_os_system_win_input_wheel_of(lh_u32_t msg, lh_usize_t wparam, lh_s32_t *dx, lh_s32_t *dy);
/* BOOL is an int and UINT an unsigned int. */
extern "C" lh_u32_t
lh_os_system_win_input_wheel_lines_of(int got, unsigned int lines);
extern "C" lh_u32_t
lh_os_system_win_input_get_wheel_lines(void);

namespace
{

TEST(os_system_win_input, each_button_message_names_its_button)
{
    EXPECT_EQ(lh_os_system_win_input_button_of(0x0201U, 0U), lh_os_input_button_primary);
    EXPECT_EQ(lh_os_system_win_input_button_of(0x0205U, 0U), lh_os_input_button_secondary);
    EXPECT_EQ(lh_os_system_win_input_button_of(0x0207U, 0U), lh_os_input_button_middle);
    EXPECT_EQ(lh_os_system_win_input_button_of(0x020BU, 0x00010000U), lh_os_input_button_back)
        << "XBUTTON1 in the high word";
    EXPECT_EQ(lh_os_system_win_input_button_of(0x020CU, 0x00020000U), lh_os_input_button_forward)
        << "XBUTTON2 in the high word";
    EXPECT_EQ(lh_os_system_win_input_button_of(0x0200U, 0U), lh_os_input_button_none)
        << "a move is about no button";
    EXPECT_TRUE(lh_os_system_win_input_is_button_down(0x0204U));
    EXPECT_FALSE(lh_os_system_win_input_is_button_down(0x0202U));
}

TEST(os_system_win_input, the_mk_bits_read_as_buttons_held)
{
    EXPECT_EQ(lh_os_system_win_input_buttons_of(0x0001U),
              lh_os_input_button_to_mask(lh_os_input_button_primary));
    EXPECT_EQ(lh_os_system_win_input_buttons_of(0x0002U | 0x0010U),
              lh_os_input_button_to_mask(lh_os_input_button_secondary) |
                  lh_os_input_button_to_mask(lh_os_input_button_middle));
    EXPECT_EQ(lh_os_system_win_input_buttons_of(0x0020U | 0x0040U),
              lh_os_input_button_to_mask(lh_os_input_button_back) |
                  lh_os_input_button_to_mask(lh_os_input_button_forward));
    EXPECT_EQ(lh_os_system_win_input_buttons_of(0x0004U | 0x0008U), 0U)
        << "MK_SHIFT and MK_CONTROL are no buttons";
}

TEST(os_system_win_input, bit_30_is_a_repeat)
{
    EXPECT_FALSE(lh_os_system_win_input_is_repeat(0x00000001));
    EXPECT_TRUE(lh_os_system_win_input_is_repeat(0x40000001));
}

TEST(os_system_win_input, the_wheels_count_up_and_left_positive)
{
    lh_s32_t dx = 7;
    lh_s32_t dy = 7;

    lh_os_system_win_input_wheel_of(0x020AU, 0x00780000U, &dx, &dy);
    EXPECT_EQ(dx, 0);
    EXPECT_EQ(dy, 120) << "a notch up";
    lh_os_system_win_input_wheel_of(0x020AU, 0xFF880000U, &dx, &dy);
    EXPECT_EQ(dy, -120) << "a notch down";
    lh_os_system_win_input_wheel_of(0x020EU, 0x00780000U, &dx, &dy);
    EXPECT_EQ(dx, -120) << "Windows counts right positive, an event left";
    EXPECT_EQ(dy, 0);
}

TEST(os_system_win_input, a_failed_spi_call_reads_as_the_default_lines)
{
    EXPECT_EQ(lh_os_system_win_input_wheel_lines_of(0, 7U),
              static_cast<lh_u32_t>(LH_OS_INPUT_WHEEL_LINES_DEFAULT))
        << "what the call left behind is not an answer";
    EXPECT_EQ(lh_os_system_win_input_wheel_lines_of(0, 0xFFFFFFFFU),
              static_cast<lh_u32_t>(LH_OS_INPUT_WHEEL_LINES_DEFAULT));
}

TEST(os_system_win_input, page_scroll_reads_as_a_page_of_lines)
{
    EXPECT_EQ(lh_os_system_win_input_wheel_lines_of(1, 0xFFFFFFFFU),
              static_cast<lh_u32_t>(LH_OS_INPUT_WHEEL_LINES_PAGE))
        << "WHEEL_PAGESCROLL is a page a notch, not four billion lines";
}

TEST(os_system_win_input, a_line_count_is_taken_as_it_is)
{
    EXPECT_EQ(lh_os_system_win_input_wheel_lines_of(1, 3U), 3U);
    EXPECT_EQ(lh_os_system_win_input_wheel_lines_of(1, 1U), 1U);
    EXPECT_EQ(lh_os_system_win_input_wheel_lines_of(1, 0U), 0U) << "wheel scrolling turned off";
    EXPECT_EQ(lh_os_system_win_input_wheel_lines_of(1, 250U), 250U)
        << "cut to the most by the step, not here";
}

/* What this machine is set to is its own business; that the answer has been through
   lh_os_system_win_input_wheel_lines_of - no raw WHEEL_PAGESCROLL - is the kernel's. */
TEST(os_system_win_input, the_system_answers_a_count_of_lines)
{
    EXPECT_NE(lh_os_system_win_input_get_wheel_lines(), 0xFFFFFFFFU);
}

} // namespace
