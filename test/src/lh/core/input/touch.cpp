#include <gtest/gtest.h>

#include <lh/core/input/event.h>
#include <lh/core/input/touch.h>

namespace
{

struct bag
{
    lh_input_event_t events[8];
    int count;
};

lh_void
take(lh_self_ptr self, const lh_input_event_ptr event)
{
    bag *got = static_cast<bag *>(self);

    if (got->count >= 8)
    {
        return;
    }
    got->events[got->count] = *event;
    got->count = got->count + 1;
}

lh_input_sample_t
finger(lh_input_contact_id_t contact, int x, int y)
{
    lh_input_sample_t sample;

    lh_input_sample_init(lh_addr_of(sample), contact, lh_input_coord_from_int(x),
                         lh_input_coord_from_int(y));
    return sample;
}

} // namespace

TEST(input_touch, a_frame_emits_down_move_and_up_per_finger)
{
    bag got = {};
    lh_input_touch_t touch;
    lh_input_sample_t frame[2];

    lh_input_touch_init(lh_addr_of(touch), lh_input_device_kind_touch, 3U);
    frame[0] = finger(40U, 10, 20);
    lh_input_touch_report(lh_addr_of(touch), frame, 1U, 5U, take, lh_addr_of(got));
    ASSERT_EQ(got.count, 1);
    EXPECT_EQ(lh_input_event_get_kind(lh_addr_of(got.events[0])), lh_input_kind_pointer_down);
    EXPECT_EQ(lh_input_event_get_pointer(lh_addr_of(got.events[0])), 1U);
    EXPECT_EQ(lh_input_event_get_device(lh_addr_of(got.events[0])), 3U);
    EXPECT_EQ(lh_input_event_get_x(lh_addr_of(got.events[0])), lh_input_coord_from_int(10));
    got.count = 0;
    lh_input_touch_report(lh_addr_of(touch), frame, 1U, 6U, take, lh_addr_of(got));
    EXPECT_EQ(got.count, 0);
    frame[0] = finger(40U, 11, 20);
    frame[1] = finger(41U, 1, 2);
    lh_input_touch_report(lh_addr_of(touch), frame, 2U, 7U, take, lh_addr_of(got));
    ASSERT_EQ(got.count, 2);
    EXPECT_EQ(lh_input_event_get_kind(lh_addr_of(got.events[0])), lh_input_kind_pointer_move);
    EXPECT_EQ(lh_input_event_get_pointer(lh_addr_of(got.events[0])), 1U);
    EXPECT_EQ(lh_input_event_get_kind(lh_addr_of(got.events[1])), lh_input_kind_pointer_down);
    EXPECT_EQ(lh_input_event_get_pointer(lh_addr_of(got.events[1])), 2U);
    got.count = 0;
    frame[0] = finger(41U, 1, 2);
    lh_input_touch_report(lh_addr_of(touch), frame, 1U, 8U, take, lh_addr_of(got));
    ASSERT_EQ(got.count, 1);
    EXPECT_EQ(lh_input_event_get_kind(lh_addr_of(got.events[0])), lh_input_kind_pointer_up);
    EXPECT_EQ(lh_input_event_get_pointer(lh_addr_of(got.events[0])), 1U);
    EXPECT_EQ(lh_input_touch_get_count(lh_addr_of(touch)), 1U);
}

TEST(input_touch, a_fifth_finger_waits_for_a_free_slot)
{
    bag got = {};
    lh_input_touch_t touch;
    lh_input_sample_t frame[5];

    lh_input_touch_init(lh_addr_of(touch), lh_input_device_kind_touch, 0U);
    for (int i = 0; i < 5; ++i)
    {
        frame[i] = finger(lh_cast_static(lh_input_contact_id_t, 10 + i), i, 0);
    }
    lh_input_touch_report(lh_addr_of(touch), frame, 5U, 1U, take, lh_addr_of(got));
    EXPECT_EQ(got.count, 4);
    EXPECT_EQ(lh_input_touch_get_count(lh_addr_of(touch)), 4U);
    got.count = 0;
    lh_input_touch_report(lh_addr_of(touch), lh_null, 0U, 2U, take, lh_addr_of(got));
    EXPECT_EQ(got.count, 4);
    EXPECT_EQ(lh_input_event_get_kind(lh_addr_of(got.events[0])), lh_input_kind_pointer_up);
    EXPECT_EQ(lh_input_touch_get_count(lh_addr_of(touch)), 0U);
}
