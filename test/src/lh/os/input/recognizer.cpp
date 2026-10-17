/**
 * @file recognizer.cpp
 * @brief ::lh_os_input_recognizer_t on synthetic events and hand-set ticks: click, double
 *        click, long press, drag, cancel in every phase, focus out, the wrap of the tick,
 *        two pointers at once, the wheel with Shift.
 */

#include <gtest/gtest.h>

#include <lh/os/input/recognizer.h>
#include <lh/os/input/recognizer/initializer.h>

#include <vector>

namespace
{

/* What reached the sink, in order. */
struct event_log
{
    std::vector<lh_os_input_event_t> events;

    std::vector<lh_os_input_kind_t>
    kinds() const
    {
        std::vector<lh_os_input_kind_t> out;

        for (const lh_os_input_event_t &event : events)
        {
            out.push_back(lh_os_input_event_get_kind(&event));
        }
        return out;
    }

    /* The synthesized events only. */
    std::vector<lh_os_input_kind_t>
    gestures() const
    {
        std::vector<lh_os_input_kind_t> out;

        for (const lh_os_input_event_t &event : events)
        {
            if (lh_os_input_event_is_synthesized(&event))
            {
                out.push_back(lh_os_input_event_get_kind(&event));
            }
        }
        return out;
    }

    const lh_os_input_event_t *
    last(lh_os_input_kind_t kind) const
    {
        for (auto it = events.rbegin(); it != events.rend(); ++it)
        {
            if (lh_os_input_event_get_kind(&*it) == kind)
            {
                return &*it;
            }
        }
        return nullptr;
    }
};

lh_void
record(lh_self_ptr self, const lh_os_input_event_t *event)
{
    static_cast<event_log *>(self)->events.push_back(*event);
}

struct fixture
{
    lh_os_input_recognizer_t recognizer;
    event_log seen;

    fixture()
    {
        lh_os_input_recognizer_init(&recognizer, nullptr);
    }

    explicit fixture(const lh_os_input_recognizer_config_t &config)
    {
        lh_os_input_recognizer_init(&recognizer, &config);
    }

    void
    pointer(lh_os_input_kind_t kind, int x, int y, lh_tick_t time, lh_u8_t id = 0U,
            lh_os_input_button_t button = lh_os_input_button_primary)
    {
        lh_os_input_event_t event;

        lh_os_input_event_init_pointer(
            &event, kind, lh_os_input_device_kind_mouse, id,
            kind == lh_os_input_kind_pointer_move ? lh_os_input_button_none : button,
            lh_os_input_coord_from_int(x), lh_os_input_coord_from_int(y), time);
        lh_os_input_recognizer_feed(&recognizer, &event, record, &seen);
    }

    void
    down(int x, int y, lh_tick_t time, lh_u8_t id = 0U,
         lh_os_input_button_t button = lh_os_input_button_primary)
    {
        pointer(lh_os_input_kind_pointer_down, x, y, time, id, button);
    }

    void
    up(int x, int y, lh_tick_t time, lh_u8_t id = 0U,
       lh_os_input_button_t button = lh_os_input_button_primary)
    {
        pointer(lh_os_input_kind_pointer_up, x, y, time, id, button);
    }

    void
    move(int x, int y, lh_tick_t time, lh_u8_t id = 0U)
    {
        pointer(lh_os_input_kind_pointer_move, x, y, time, id);
    }

    void
    plain(lh_os_input_kind_t kind, lh_tick_t time)
    {
        lh_os_input_event_t event;

        lh_os_input_event_init(&event, kind, lh_os_input_device_kind_none, time);
        lh_os_input_recognizer_feed(&recognizer, &event, record, &seen);
    }

    void
    update(lh_tick_t now)
    {
        lh_os_input_recognizer_update(&recognizer, now, record, &seen);
    }
};

using kinds = std::vector<lh_os_input_kind_t>;

TEST(os_input_recognizer, initializer_gives_the_state_init_gives)
{
    lh_os_input_recognizer_t declared =
        lh_os_input_recognizer_initializer(lh_os_input_recognizer_config_default_initializer(),
                                           {lh_os_input_recognizer_pointer_idle_initializer()});
    lh_os_input_recognizer_t initialized;

    lh_os_input_recognizer_init(&initialized, nullptr);
    EXPECT_EQ(
        lh_os_input_recognizer_config_get_slop(lh_os_input_recognizer_get_config(&declared)),
        lh_os_input_recognizer_config_get_slop(lh_os_input_recognizer_get_config(&initialized)));
    EXPECT_EQ(lh_os_input_recognizer_get_phase(&declared, 0U), lh_os_input_recognizer_phase_idle);
    EXPECT_EQ(lh_os_input_recognizer_get_phase(&initialized, 0U),
              lh_os_input_recognizer_phase_idle);
}

TEST(os_input_recognizer, defaults)
{
    lh_os_input_recognizer_config_t config;

    lh_os_input_recognizer_config_init_default(&config);
    EXPECT_EQ(lh_os_input_recognizer_config_get_slop(&config), 4U);
    EXPECT_EQ(lh_os_input_recognizer_config_get_double_click_ms(&config), 500U);
    EXPECT_EQ(lh_os_input_recognizer_config_get_double_click_slop(&config), 4U);
    EXPECT_EQ(lh_os_input_recognizer_config_get_long_press_ms(&config), 500U);
    EXPECT_EQ(lh_os_input_recognizer_config_get_repeat_delay_ms(&config), 400U);
    EXPECT_EQ(lh_os_input_recognizer_config_get_repeat_rate_ms(&config), 50U);
    EXPECT_EQ(lh_os_input_recognizer_config_get_axis_deadzone(&config), 8000U);
    EXPECT_EQ(lh_os_input_recognizer_config_get_axis_hysteresis(&config), 2000U);
    EXPECT_TRUE(lh_os_input_recognizer_config_is_wheel_shift_horizontal(&config));
}

TEST(os_input_recognizer, down_and_up_within_the_slop_is_a_click_after_the_up)
{
    fixture f;

    f.down(10, 10, 100U);
    f.move(13, 7, 120U);
    EXPECT_EQ(lh_os_input_recognizer_get_phase(&f.recognizer, 0U),
              lh_os_input_recognizer_phase_pending);
    f.up(13, 7, 150U);
    EXPECT_EQ(f.seen.kinds(), (kinds{lh_os_input_kind_pointer_down, lh_os_input_kind_pointer_move,
                                     lh_os_input_kind_pointer_up, lh_os_input_kind_click}));
    const lh_os_input_event_t *click = f.seen.last(lh_os_input_kind_click);
    ASSERT_NE(click, nullptr);
    EXPECT_EQ(lh_os_input_event_get_count(click), 1U);
    EXPECT_EQ(lh_os_input_event_get_time(click), 150U);
    EXPECT_EQ(lh_os_input_coord_to_int(lh_os_input_event_get_x(click)), 13);
    EXPECT_EQ(lh_os_input_event_get_button(click), lh_os_input_button_primary);
    EXPECT_TRUE(lh_os_input_event_is_synthesized(click));
    EXPECT_TRUE(lh_os_input_event_is_primary(click));
    EXPECT_EQ(lh_os_input_recognizer_get_phase(&f.recognizer, 0U),
              lh_os_input_recognizer_phase_idle);
}

TEST(os_input_recognizer, two_quick_near_clicks_are_a_double_click_and_a_third_counts_on)
{
    fixture f;

    f.down(10, 10, 100U);
    f.up(10, 10, 150U);
    f.down(12, 11, 400U);
    f.up(12, 11, 450U);
    f.down(11, 11, 700U);
    f.up(11, 11, 750U);
    EXPECT_EQ(f.seen.gestures(), (kinds{lh_os_input_kind_click, lh_os_input_kind_click,
                                        lh_os_input_kind_double_click, lh_os_input_kind_click}));
    std::vector<int> counts;
    for (const lh_os_input_event_t &event : f.seen.events)
    {
        if (lh_os_input_event_is_synthesized(&event))
        {
            counts.push_back(lh_os_input_event_get_count(&event));
        }
    }
    EXPECT_EQ(counts, (std::vector<int>{1, 2, 2, 3}));
}

TEST(os_input_recognizer, a_slow_or_far_second_click_starts_a_new_series)
{
    fixture f;

    f.down(10, 10, 100U);
    f.up(10, 10, 150U);
    f.down(10, 10, 651U); /* 501 ms after the last click */
    f.up(10, 10, 700U);
    f.down(20, 10, 800U); /* 10 px away */
    f.up(20, 10, 850U);
    EXPECT_EQ(f.seen.gestures(),
              (kinds{lh_os_input_kind_click, lh_os_input_kind_click, lh_os_input_kind_click}));
    EXPECT_EQ(lh_os_input_event_get_count(f.seen.last(lh_os_input_kind_click)), 1U);
}

TEST(os_input_recognizer, another_button_does_not_continue_the_series)
{
    fixture f;

    f.down(10, 10, 100U);
    f.up(10, 10, 150U);
    f.down(10, 10, 200U, 0U, lh_os_input_button_secondary);
    f.up(10, 10, 250U, 0U, lh_os_input_button_secondary);
    EXPECT_EQ(f.seen.gestures(), (kinds{lh_os_input_kind_click, lh_os_input_kind_click}));
    const lh_os_input_event_t *click = f.seen.last(lh_os_input_kind_click);
    EXPECT_EQ(lh_os_input_event_get_count(click), 1U);
    EXPECT_EQ(lh_os_input_event_get_button(click), lh_os_input_button_secondary);
}

TEST(os_input_recognizer, a_second_button_while_one_is_held_is_passed_on_and_ignored)
{
    fixture f;

    f.down(10, 10, 100U);
    f.down(10, 10, 110U, 0U, lh_os_input_button_secondary);
    f.up(10, 10, 120U, 0U, lh_os_input_button_secondary);
    EXPECT_TRUE(f.seen.gestures().empty());
    f.up(10, 10, 130U);
    EXPECT_EQ(f.seen.gestures(), (kinds{lh_os_input_kind_click}));
    EXPECT_EQ(f.seen.events.size(), 5U);
}

TEST(os_input_recognizer, held_still_is_a_long_press_on_update_and_then_no_click)
{
    fixture f;
    lh_tick_t due = 0U;

    f.down(10, 10, 1000U);
    ASSERT_TRUE(lh_os_input_recognizer_get_next_deadline(&f.recognizer, &due));
    EXPECT_EQ(due, 1500U);
    f.update(1499U);
    EXPECT_TRUE(f.seen.gestures().empty());
    f.update(1500U);
    EXPECT_EQ(f.seen.gestures(), (kinds{lh_os_input_kind_long_press}));
    EXPECT_EQ(lh_os_input_recognizer_get_phase(&f.recognizer, 0U),
              lh_os_input_recognizer_phase_holding);
    EXPECT_FALSE(lh_os_input_recognizer_get_next_deadline(&f.recognizer, &due));
    f.update(2000U);
    f.up(10, 10, 2100U);
    EXPECT_EQ(f.seen.gestures(), (kinds{lh_os_input_kind_long_press}));
    const lh_os_input_event_t *long_press = f.seen.last(lh_os_input_kind_long_press);
    EXPECT_EQ(lh_os_input_coord_to_int(lh_os_input_event_get_x(long_press)), 10);
    EXPECT_EQ(lh_os_input_event_get_buttons(long_press),
              lh_os_input_button_to_mask(lh_os_input_button_primary));
}

TEST(os_input_recognizer, a_late_event_sends_the_long_press_it_passed_first)
{
    fixture f;

    f.down(10, 10, 1000U);
    f.up(10, 10, 1600U);
    EXPECT_EQ(f.seen.kinds(), (kinds{lh_os_input_kind_pointer_down, lh_os_input_kind_long_press,
                                     lh_os_input_kind_pointer_up}));
}

TEST(os_input_recognizer, a_long_press_across_the_wrap_of_the_tick)
{
    fixture f;
    lh_tick_t due = 0U;

    f.down(10, 10, 0xFFFFFF00U);
    ASSERT_TRUE(lh_os_input_recognizer_get_next_deadline(&f.recognizer, &due));
    EXPECT_EQ(due, 0xFFFFFF00U + 500U);
    f.update(0xFFFFFFFFU);
    f.update(0x10U);
    EXPECT_TRUE(f.seen.gestures().empty());
    f.update(0xF4U); /* 500 ms after the press */
    EXPECT_EQ(f.seen.gestures(), (kinds{lh_os_input_kind_long_press}));
}

TEST(os_input_recognizer, long_press_off_keeps_a_slow_press_a_click)
{
    lh_os_input_recognizer_config_t config = lh_os_input_recognizer_config_initializer(
        4U, 500U, 4U, 0U, 400U, 50U, 8000U, 2000U, lh_bool_true);
    fixture f(config);
    lh_tick_t due = 0U;

    f.down(10, 10, 0U);
    EXPECT_FALSE(lh_os_input_recognizer_get_next_deadline(&f.recognizer, &due));
    f.update(60000U);
    f.up(10, 10, 60000U);
    EXPECT_EQ(f.seen.gestures(), (kinds{lh_os_input_kind_click}));
}

TEST(os_input_recognizer, moving_past_the_slop_drags_and_reports_each_distance)
{
    fixture f;

    f.down(10, 10, 100U);
    f.move(15, 10, 110U);
    EXPECT_TRUE(lh_os_input_recognizer_is_dragging(&f.recognizer, 0U));
    f.move(15, 10, 115U); /* no distance, no drag event */
    f.move(20, 7, 120U);
    f.up(22, 7, 130U);
    EXPECT_EQ(f.seen.gestures(), (kinds{lh_os_input_kind_drag_begin, lh_os_input_kind_drag,
                                        lh_os_input_kind_drag_end}));
    std::vector<std::pair<int, int>> deltas;
    for (const lh_os_input_event_t &event : f.seen.events)
    {
        if (lh_os_input_event_is_synthesized(&event))
        {
            deltas.emplace_back(lh_os_input_event_get_dx(&event) / LH_OS_INPUT_COORD_ONE,
                                lh_os_input_event_get_dy(&event) / LH_OS_INPUT_COORD_ONE);
        }
    }
    EXPECT_EQ(deltas, (std::vector<std::pair<int, int>>{{5, 0}, {5, -3}, {2, 0}}));
    EXPECT_FALSE(lh_os_input_recognizer_is_dragging(&f.recognizer, 0U));
    EXPECT_FALSE(lh_os_input_event_is_cancelled(f.seen.last(lh_os_input_kind_drag_end)));
}

TEST(os_input_recognizer, a_drag_ends_any_click_series)
{
    fixture f;

    f.down(10, 10, 100U);
    f.up(10, 10, 110U);
    f.down(10, 10, 200U);
    f.move(30, 10, 210U);
    f.up(30, 10, 220U);
    f.down(30, 10, 300U);
    f.up(30, 10, 310U);
    EXPECT_EQ(lh_os_input_event_get_count(f.seen.last(lh_os_input_kind_click)), 1U);
}

TEST(os_input_recognizer, a_long_press_may_still_turn_into_a_drag)
{
    fixture f;

    f.down(10, 10, 0U);
    f.update(500U);
    f.move(30, 10, 600U);
    f.up(30, 10, 700U);
    EXPECT_EQ(f.seen.gestures(), (kinds{lh_os_input_kind_long_press, lh_os_input_kind_drag_begin,
                                        lh_os_input_kind_drag_end}));
}

TEST(os_input_recognizer, cancel_while_pending_is_no_click)
{
    fixture f;

    f.down(10, 10, 0U);
    f.pointer(lh_os_input_kind_pointer_cancel, 10, 10, 10U);
    f.up(10, 10, 20U);
    EXPECT_TRUE(f.seen.gestures().empty());
    EXPECT_EQ(lh_os_input_recognizer_get_phase(&f.recognizer, 0U),
              lh_os_input_recognizer_phase_idle);
}

TEST(os_input_recognizer, cancel_while_holding_is_nothing_more)
{
    fixture f;

    f.down(10, 10, 0U);
    f.update(500U);
    f.pointer(lh_os_input_kind_pointer_cancel, 10, 10, 600U);
    EXPECT_EQ(f.seen.gestures(), (kinds{lh_os_input_kind_long_press}));
    EXPECT_EQ(lh_os_input_recognizer_get_phase(&f.recognizer, 0U),
              lh_os_input_recognizer_phase_idle);
}

TEST(os_input_recognizer, cancel_while_dragging_ends_the_drag_cancelled)
{
    fixture f;

    f.down(10, 10, 0U);
    f.move(30, 10, 10U);
    f.pointer(lh_os_input_kind_pointer_cancel, 30, 10, 20U);
    EXPECT_EQ(f.seen.kinds(), (kinds{lh_os_input_kind_pointer_down, lh_os_input_kind_pointer_move,
                                     lh_os_input_kind_drag_begin, lh_os_input_kind_pointer_cancel,
                                     lh_os_input_kind_drag_end}));
    EXPECT_TRUE(lh_os_input_event_is_cancelled(f.seen.last(lh_os_input_kind_drag_end)));
}

TEST(os_input_recognizer, focus_out_cancels_every_pointer_down)
{
    fixture f;

    f.down(10, 10, 0U, 0U);
    f.down(50, 50, 0U, 1U);
    f.move(80, 50, 5U, 1U);
    f.plain(lh_os_input_kind_focus_out, 10U);
    EXPECT_EQ(f.seen.gestures(),
              (kinds{lh_os_input_kind_drag_begin, lh_os_input_kind_pointer_cancel,
                     lh_os_input_kind_pointer_cancel, lh_os_input_kind_drag_end}));
    EXPECT_TRUE(lh_os_input_event_is_cancelled(f.seen.last(lh_os_input_kind_pointer_cancel)));
    EXPECT_EQ(lh_os_input_event_get_pointer(f.seen.last(lh_os_input_kind_pointer_cancel)), 1U);
    f.up(10, 10, 20U, 0U);
    EXPECT_EQ(f.seen.last(lh_os_input_kind_click), nullptr);
}

TEST(os_input_recognizer, focus_out_with_nothing_down_sends_nothing_more)
{
    fixture f;

    f.plain(lh_os_input_kind_focus_out, 10U);
    EXPECT_EQ(f.seen.kinds(), (kinds{lh_os_input_kind_focus_out}));
}

TEST(os_input_recognizer, reset_cancels_and_forgets_the_series)
{
    fixture f;

    f.down(10, 10, 0U);
    f.up(10, 10, 10U);
    f.down(10, 10, 20U);
    lh_os_input_recognizer_reset(&f.recognizer, 30U, record, &f.seen);
    EXPECT_EQ(f.seen.last(lh_os_input_kind_pointer_cancel) != nullptr, true);
    f.down(10, 10, 40U);
    f.up(10, 10, 50U);
    EXPECT_EQ(lh_os_input_event_get_count(f.seen.last(lh_os_input_kind_click)), 1U);
}

TEST(os_input_recognizer, two_pointers_are_independent)
{
    fixture f;

    f.down(10, 10, 0U, 0U);
    f.down(100, 100, 5U, 1U);
    f.move(130, 100, 10U, 1U);
    f.up(10, 10, 20U, 0U);
    f.up(130, 100, 30U, 1U);
    EXPECT_EQ(f.seen.gestures(), (kinds{lh_os_input_kind_drag_begin, lh_os_input_kind_click,
                                        lh_os_input_kind_drag_end}));
    const lh_os_input_event_t *click = f.seen.last(lh_os_input_kind_click);
    EXPECT_EQ(lh_os_input_event_get_pointer(click), 0U);
    EXPECT_EQ(lh_os_input_event_get_pointer(f.seen.last(lh_os_input_kind_drag_end)), 1U);
    EXPECT_FALSE(lh_os_input_event_is_primary(f.seen.last(lh_os_input_kind_drag_end)));
}

TEST(os_input_recognizer, a_pointer_id_past_the_limit_is_passed_on_without_gestures)
{
    fixture f;
    const lh_u8_t id = LH_OS_INPUT_POINTER_MAX;

    f.down(10, 10, 0U, id);
    f.up(10, 10, 10U, id);
    EXPECT_EQ(f.seen.kinds(), (kinds{lh_os_input_kind_pointer_down, lh_os_input_kind_pointer_up}));
    EXPECT_EQ(lh_os_input_recognizer_get_phase(&f.recognizer, id),
              lh_os_input_recognizer_phase_idle);
}

TEST(os_input_recognizer, a_synthesized_event_fed_back_is_passed_on_and_not_recognized)
{
    fixture f;
    lh_os_input_event_t event;

    lh_os_input_event_init_pointer(&event, lh_os_input_kind_pointer_down,
                                   lh_os_input_device_kind_mouse, 0U, lh_os_input_button_primary, 0,
                                   0, 0U);
    lh_os_input_event_set_flags(&event, LH_OS_INPUT_FLAG_SYNTHESIZED);
    lh_os_input_recognizer_feed(&f.recognizer, &event, record, &f.seen);
    EXPECT_EQ(f.seen.events.size(), 1U);
    EXPECT_EQ(lh_os_input_recognizer_get_phase(&f.recognizer, 0U),
              lh_os_input_recognizer_phase_idle);
}

TEST(os_input_recognizer, shift_turns_a_vertical_wheel_sideways)
{
    fixture f;
    lh_os_input_event_t event;

    lh_os_input_event_init_wheel(&event, 0, 0, 0, LH_OS_INPUT_WHEEL_NOTCH, 0U);
    lh_os_input_event_set_mods(&event, LH_OS_INPUT_MODS_SHIFT);
    lh_os_input_recognizer_feed(&f.recognizer, &event, record, &f.seen);
    lh_os_input_event_set_mods(&event, LH_OS_INPUT_MODS_NONE);
    lh_os_input_recognizer_feed(&f.recognizer, &event, record, &f.seen);
    ASSERT_EQ(f.seen.events.size(), 2U);
    EXPECT_EQ(lh_os_input_event_get_dx(&f.seen.events[0]), LH_OS_INPUT_WHEEL_NOTCH);
    EXPECT_EQ(lh_os_input_event_get_dy(&f.seen.events[0]), 0);
    EXPECT_EQ(lh_os_input_event_get_dx(&f.seen.events[1]), 0);
    EXPECT_EQ(lh_os_input_event_get_dy(&f.seen.events[1]), LH_OS_INPUT_WHEEL_NOTCH);
}

TEST(os_input_recognizer, shift_policy_off_leaves_the_wheel_alone)
{
    lh_os_input_recognizer_config_t config = lh_os_input_recognizer_config_initializer(
        4U, 500U, 4U, 500U, 400U, 50U, 8000U, 2000U, lh_bool_false);
    fixture f(config);
    lh_os_input_event_t event;

    lh_os_input_event_init_wheel(&event, 0, 0, 0, LH_OS_INPUT_WHEEL_NOTCH, 0U);
    lh_os_input_event_set_mods(&event, LH_OS_INPUT_MODS_SHIFT);
    lh_os_input_recognizer_feed(&f.recognizer, &event, record, &f.seen);
    ASSERT_EQ(f.seen.events.size(), 1U);
    EXPECT_EQ(lh_os_input_event_get_dy(&f.seen.events[0]), LH_OS_INPUT_WHEEL_NOTCH);
}

TEST(os_input_recognizer, the_earliest_deadline_of_several_presses)
{
    fixture f;
    lh_tick_t due = 0U;

    f.down(10, 10, 300U, 0U);
    f.down(50, 50, 100U, 1U);
    ASSERT_TRUE(lh_os_input_recognizer_get_next_deadline(&f.recognizer, &due));
    EXPECT_EQ(due, 600U);
}

} // namespace
