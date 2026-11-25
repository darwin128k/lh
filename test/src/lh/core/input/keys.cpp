#include <gtest/gtest.h>

#include <lh/core/input/event.h>
#include <lh/core/input/keys.h>

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

TEST(input_keys, a_key_emits_only_when_it_changes_and_shift_is_a_modifier)
{
    bag got = {};
    lh_input_keys_t keys;

    lh_input_keys_init(lh_addr_of(keys), 4U);
    lh_input_keys_report(lh_addr_of(keys), lh_key_other, lh_bool_true, 1U, 1U, take,
                         lh_addr_of(got));
    EXPECT_EQ(got.count, 0);
    lh_input_keys_report(lh_addr_of(keys), lh_key_enter, lh_bool_false, 28U, 2U, take,
                         lh_addr_of(got));
    EXPECT_EQ(got.count, 0);
    lh_input_keys_report(lh_addr_of(keys), lh_key_enter, lh_bool_true, 28U, 3U, take,
                         lh_addr_of(got));
    ASSERT_EQ(got.count, 1);
    EXPECT_EQ(lh_input_event_get_kind(lh_addr_of(got.events[0])), lh_input_kind_key_down);
    EXPECT_EQ(lh_input_event_get_device(lh_addr_of(got.events[0])), 4U);
    EXPECT_EQ(lh_input_event_get_key(lh_addr_of(got.events[0])), lh_key_enter);
    EXPECT_EQ(lh_input_event_get_scancode(lh_addr_of(got.events[0])), 28U);
    EXPECT_EQ(lh_input_event_get_mods(lh_addr_of(got.events[0])), LH_INPUT_MODS_NONE);
    EXPECT_EQ(lh_input_event_is_repeat(lh_addr_of(got.events[0])), lh_bool_false);
    got.count = 0;
    lh_input_keys_report(lh_addr_of(keys), lh_key_enter, lh_bool_true, 28U, 4U, take,
                         lh_addr_of(got));
    EXPECT_EQ(got.count, 0);
    lh_input_keys_report(lh_addr_of(keys), lh_key_shift, lh_bool_true, 42U, 5U, take,
                         lh_addr_of(got));
    ASSERT_EQ(got.count, 1);
    EXPECT_EQ(lh_input_event_has_mods(lh_addr_of(got.events[0]), LH_INPUT_MODS_SHIFT),
              lh_bool_true);
    got.count = 0;
    lh_input_keys_report(lh_addr_of(keys), lh_key_enter, lh_bool_false, 28U, 6U, take,
                         lh_addr_of(got));
    ASSERT_EQ(got.count, 1);
    EXPECT_EQ(lh_input_event_get_kind(lh_addr_of(got.events[0])), lh_input_kind_key_up);
    EXPECT_EQ(lh_input_event_has_mods(lh_addr_of(got.events[0]), LH_INPUT_MODS_SHIFT),
              lh_bool_true);
    EXPECT_EQ(lh_input_keys_is_down(lh_addr_of(keys), lh_key_enter), lh_bool_false);
    EXPECT_EQ(lh_input_keys_is_down(lh_addr_of(keys), lh_key_shift), lh_bool_true);
}
