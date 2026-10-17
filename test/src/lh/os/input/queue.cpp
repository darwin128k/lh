/**
 * @file queue.cpp
 * @brief ::lh_os_input_queue_t: order, wrap of the indices, overflow, folded moves, drain.
 */

#include <gtest/gtest.h>

#include <lh/os/input/queue.h>
#include <lh/os/input/queue/initializer.h>

#include <vector>

namespace
{

lh_os_input_event_t
move_to(int x, lh_tick_t time)
{
    lh_os_input_event_t event;

    lh_os_input_event_init_pointer(&event, lh_os_input_kind_pointer_move,
                                   lh_os_input_device_kind_mouse, 0U, lh_os_input_button_none,
                                   lh_os_input_coord_from_int(x), 0, time);
    return event;
}

lh_os_input_event_t
key(lh_tick_t time)
{
    lh_os_input_event_t event;

    lh_os_input_event_init_key(&event, lh_os_key_tab, lh_bool_true, lh_bool_false, 0U, time);
    return event;
}

lh_void
collect(const lh_os_input_event_t *event, lh_ptr context)
{
    static_cast<std::vector<lh_tick_t> *>(context)->push_back(lh_os_input_event_get_time(event));
}

TEST(os_input_queue, empty_initializer_matches_init)
{
    lh_os_input_event_t storage[4];
    lh_os_input_queue_t declared = lh_os_input_queue_empty_initializer(storage, 4U);
    lh_os_input_queue_t initialized;

    lh_os_input_queue_init(&initialized, storage, 4U);
    EXPECT_EQ(lh_os_input_queue_get_capacity(&declared), 4U);
    EXPECT_EQ(lh_os_input_queue_get_count(&declared), lh_os_input_queue_get_count(&initialized));
    EXPECT_TRUE(lh_os_input_queue_is_empty(&declared));
    EXPECT_EQ(lh_os_input_queue_get_dropped(&declared), 0U);
}

TEST(os_input_queue, pops_in_push_order_across_many_wraps)
{
    lh_os_input_event_t storage[3];
    lh_os_input_queue_t queue;
    lh_os_input_event_t event;
    lh_tick_t next_in = 0U;
    lh_tick_t next_out = 0U;

    lh_os_input_queue_init(&queue, storage, 3U);
    EXPECT_FALSE(lh_os_input_queue_pop(&queue, &event));
    for (int round = 0; round < 50; ++round)
    {
        for (int i = 0; i < 2; ++i)
        {
            event = key(next_in++);
            ASSERT_TRUE(lh_os_input_queue_push(&queue, &event));
        }
        EXPECT_EQ(lh_os_input_queue_get_count(&queue), 2U);
        for (int i = 0; i < 2; ++i)
        {
            ASSERT_TRUE(lh_os_input_queue_pop(&queue, &event));
            EXPECT_EQ(lh_os_input_event_get_time(&event), next_out++);
        }
        EXPECT_TRUE(lh_os_input_queue_is_empty(&queue));
    }
}

TEST(os_input_queue, full_ring_uses_every_slot_then_drops_and_counts)
{
    lh_os_input_event_t storage[2];
    lh_os_input_queue_t queue;
    lh_os_input_event_t event;

    lh_os_input_queue_init(&queue, storage, 2U);
    event = key(1U);
    EXPECT_TRUE(lh_os_input_queue_push(&queue, &event));
    event = key(2U);
    EXPECT_TRUE(lh_os_input_queue_push(&queue, &event));
    EXPECT_EQ(lh_os_input_queue_get_count(&queue), 2U);
    event = key(3U);
    EXPECT_FALSE(lh_os_input_queue_push(&queue, &event));
    EXPECT_EQ(lh_os_input_queue_get_dropped(&queue), 1U);
    ASSERT_TRUE(lh_os_input_queue_pop(&queue, &event));
    EXPECT_EQ(lh_os_input_event_get_time(&event), 1U);
    ASSERT_TRUE(lh_os_input_queue_pop(&queue, &event));
    EXPECT_EQ(lh_os_input_event_get_time(&event), 2U);
}

TEST(os_input_queue, a_move_folds_into_the_last_move_when_full)
{
    lh_os_input_event_t storage[2];
    lh_os_input_queue_t queue;
    lh_os_input_event_t event;

    lh_os_input_queue_init(&queue, storage, 2U);
    event = key(1U);
    ASSERT_TRUE(lh_os_input_queue_push(&queue, &event));
    event = move_to(10, 2U);
    ASSERT_TRUE(lh_os_input_queue_push(&queue, &event));
    event = move_to(20, 3U);
    EXPECT_TRUE(lh_os_input_queue_push(&queue, &event));
    event = move_to(30, 4U);
    EXPECT_TRUE(lh_os_input_queue_push(&queue, &event));
    EXPECT_EQ(lh_os_input_queue_get_dropped(&queue), 0U);
    EXPECT_EQ(lh_os_input_queue_get_count(&queue), 2U);
    ASSERT_TRUE(lh_os_input_queue_pop(&queue, &event));
    EXPECT_EQ(lh_os_input_event_get_kind(&event), lh_os_input_kind_key_down);
    ASSERT_TRUE(lh_os_input_queue_pop(&queue, &event));
    EXPECT_EQ(lh_os_input_coord_to_int(lh_os_input_event_get_x(&event)), 30);
    EXPECT_EQ(lh_os_input_event_get_time(&event), 4U);
}

TEST(os_input_queue, a_move_of_another_pointer_does_not_fold)
{
    lh_os_input_event_t storage[2];
    lh_os_input_queue_t queue;
    lh_os_input_event_t event;

    lh_os_input_queue_init(&queue, storage, 2U);
    event = move_to(1, 1U);
    ASSERT_TRUE(lh_os_input_queue_push(&queue, &event));
    event = move_to(2, 2U);
    ASSERT_TRUE(lh_os_input_queue_push(&queue, &event));
    lh_os_input_event_init_pointer(&event, lh_os_input_kind_pointer_move,
                                   lh_os_input_device_kind_touch, 1U, lh_os_input_button_none, 0, 0,
                                   3U);
    EXPECT_FALSE(lh_os_input_queue_push(&queue, &event));
    EXPECT_EQ(lh_os_input_queue_get_dropped(&queue), 1U);
}

TEST(os_input_queue, drain_hands_everything_on_in_order)
{
    lh_os_input_event_t storage[4];
    lh_os_input_queue_t queue;
    lh_os_input_event_t event;
    std::vector<lh_tick_t> seen;

    lh_os_input_queue_init(&queue, storage, 4U);
    for (lh_tick_t t = 10U; t < 13U; ++t)
    {
        event = key(t);
        ASSERT_TRUE(lh_os_input_queue_push(&queue, &event));
    }
    EXPECT_EQ(lh_os_input_queue_drain(&queue, collect, &seen), 3U);
    EXPECT_EQ(seen, (std::vector<lh_tick_t>{10U, 11U, 12U}));
    EXPECT_TRUE(lh_os_input_queue_is_empty(&queue));
    EXPECT_EQ(lh_os_input_queue_drain(&queue, collect, &seen), 0U);
}

} // namespace
