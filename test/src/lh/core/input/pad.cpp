#include <gtest/gtest.h>

#include <lh/core/input/event.h>
#include <lh/core/input/pad.h>

namespace
{

struct bag
{
    lh_input_event_t events[4];
    int count;
};

lh_void
take(lh_self_ptr self, const lh_input_event_ptr event)
{
    bag *got = static_cast<bag *>(self);

    if (got->count >= 4)
    {
        return;
    }
    got->events[got->count] = *event;
    got->count = got->count + 1;
}

} // namespace

TEST(input_pad, an_axis_and_a_button_emit_only_when_they_change)
{
    bag got = {};
    lh_input_pad_t pad;

    lh_input_pad_init(lh_addr_of(pad), 2U);
    lh_input_pad_report_axis(lh_addr_of(pad), lh_input_axis_left_x, 0, 1U, take, lh_addr_of(got));
    EXPECT_EQ(got.count, 0);
    lh_input_pad_report_axis(lh_addr_of(pad), lh_input_axis_left_x, 100, 2U, take, lh_addr_of(got));
    ASSERT_EQ(got.count, 1);
    EXPECT_EQ(lh_input_event_get_kind(lh_addr_of(got.events[0])), lh_input_kind_controller_axis);
    EXPECT_EQ(lh_input_event_get_device(lh_addr_of(got.events[0])), 2U);
    EXPECT_EQ(lh_input_event_get_value(lh_addr_of(got.events[0])), 100);
    got.count = 0;
    lh_input_pad_report_axis(lh_addr_of(pad), lh_input_axis_left_x, 100, 3U, take, lh_addr_of(got));
    EXPECT_EQ(got.count, 0);
    lh_input_pad_report_button(lh_addr_of(pad), lh_input_button_south, lh_bool_true, 4U, take,
                               lh_addr_of(got));
    ASSERT_EQ(got.count, 1);
    EXPECT_EQ(lh_input_event_get_kind(lh_addr_of(got.events[0])),
              lh_input_kind_controller_button_down);
    EXPECT_EQ(lh_input_event_get_buttons(lh_addr_of(got.events[0])),
              lh_input_button_to_mask(lh_input_button_south));
    got.count = 0;
    lh_input_pad_report_button(lh_addr_of(pad), lh_input_button_south, lh_bool_true, 5U, take,
                               lh_addr_of(got));
    EXPECT_EQ(got.count, 0);
    lh_input_pad_report_button(lh_addr_of(pad), lh_input_button_south, lh_bool_false, 6U, take,
                               lh_addr_of(got));
    ASSERT_EQ(got.count, 1);
    EXPECT_EQ(lh_input_event_get_kind(lh_addr_of(got.events[0])),
              lh_input_kind_controller_button_up);
    EXPECT_EQ(lh_input_event_get_buttons(lh_addr_of(got.events[0])), 0U);
}
