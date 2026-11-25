/**
 * @file controller.cpp
 * @brief ::lh_controller_t on synthetic events: feed with gestures on and off, a
 *        disabled controller, a synthesized click reaching the handler, the context as
 *        `self`, update and cancel with and without their handlers, the initializers and
 *        the sink adapter.
 */

#include <gtest/gtest.h>

#include <lh/core/controller.h>
#include <lh/core/controller/initializer.h>

#include <vector>

namespace
{

/* What reached the handlers, in order. */
struct handler_log
{
    std::vector<lh_input_event_t> events;
    std::vector<lh_tick_t> updates;
    int cancels = 0;
    lh_bool_t answer = lh_bool_true;

    std::vector<lh_input_kind_t>
    kinds() const
    {
        std::vector<lh_input_kind_t> out;

        for (const lh_input_event_t &event : events)
        {
            out.push_back(lh_input_event_get_kind(&event));
        }
        return out;
    }
};

lh_bool_t
record(lh_self_ptr self, const lh_input_event_t *event)
{
    handler_log *log = static_cast<handler_log *>(self);

    log->events.push_back(*event);
    return log->answer;
}

lh_void
record_update(lh_self_ptr self, lh_tick_t now)
{
    static_cast<handler_log *>(self)->updates.push_back(now);
}

lh_void
record_cancel(lh_self_ptr self)
{
    ++static_cast<handler_log *>(self)->cancels;
}

lh_input_event_t
pointer_event(lh_input_kind_t kind, int x, int y, lh_tick_t time,
              lh_input_button_t button = lh_input_button_primary)
{
    lh_input_event_t event{};

    lh_input_event_init_pointer(&event, kind, lh_input_device_kind_mouse, 0U, button,
                                lh_input_coord_from_int(x), lh_input_coord_from_int(y), time);
    return event;
}

struct fixture
{
    handler_log log;
    lh_controller_t controller;

    fixture()
    {
        lh_controller_init(&controller, record, &log, nullptr);
    }

    lh_bool_t
    feed(const lh_input_event_t &event)
    {
        return lh_controller_feed(&controller, &event);
    }

    lh_bool_t
    down(lh_tick_t time)
    {
        return feed(pointer_event(lh_input_kind_pointer_down, 10, 10, time));
    }

    lh_bool_t
    up(lh_tick_t time)
    {
        return feed(pointer_event(lh_input_kind_pointer_up, 10, 10, time));
    }
};

const std::vector<lh_input_kind_t> raw_click = {lh_input_kind_pointer_down,
                                                lh_input_kind_pointer_up};

} // namespace

TEST(controller, init_is_enabled_with_gestures_and_no_optional_handlers)
{
    fixture f;

    EXPECT_EQ(lh_controller_get_flags(&f.controller),
              lh_controller_flags_enabled | lh_controller_flags_gestures);
    EXPECT_TRUE(lh_controller_is_enabled(&f.controller));
    EXPECT_TRUE(lh_controller_has_flag(&f.controller, lh_controller_flags_gestures));
    EXPECT_EQ(lh_controller_get_handle_cb(&f.controller), &record);
    EXPECT_EQ(lh_controller_get_update_cb(&f.controller), nullptr);
    EXPECT_EQ(lh_controller_get_cancel_cb(&f.controller), nullptr);
    EXPECT_EQ(lh_controller_get_context(&f.controller), &f.log);
    EXPECT_EQ(lh_input_recognizer_get_phase(lh_controller_get_recognizer(&f.controller), 0U),
              lh_input_recognizer_phase_idle);
}

TEST(controller, feed_with_gestures_sends_the_raw_events_then_a_synthesized_click)
{
    fixture f;

    EXPECT_TRUE(f.down(0U));
    EXPECT_TRUE(f.up(10U));
    ASSERT_EQ(f.log.kinds(),
              (std::vector<lh_input_kind_t>{lh_input_kind_pointer_down, lh_input_kind_pointer_up,
                                            lh_input_kind_click}));
    EXPECT_FALSE(lh_input_event_is_synthesized(&f.log.events[0]));
    EXPECT_FALSE(lh_input_event_is_synthesized(&f.log.events[1]));
    EXPECT_TRUE(lh_input_event_is_synthesized(&f.log.events[2]));
    EXPECT_EQ(lh_input_event_get_count(&f.log.events[2]), 1U);
}

TEST(controller, feed_with_gestures_makes_a_double_click_of_a_quick_pair)
{
    fixture f;

    f.down(0U);
    f.up(10U);
    f.down(100U);
    f.up(110U);
    EXPECT_EQ(f.log.kinds().back(), lh_input_kind_double_click);
}

TEST(controller, feed_without_gestures_sends_the_raw_events_only)
{
    fixture f;

    lh_controller_set_flags(&f.controller, lh_controller_flags_enabled);
    EXPECT_TRUE(f.down(0U));
    EXPECT_EQ(lh_input_recognizer_get_phase(lh_controller_get_recognizer(&f.controller), 0U),
              lh_input_recognizer_phase_idle)
        << "the recognizer was fed";
    EXPECT_TRUE(f.up(10U));
    EXPECT_EQ(f.log.kinds(), raw_click);
}

TEST(controller, feed_when_disabled_reaches_nothing)
{
    fixture f;

    lh_controller_set_enabled(&f.controller, lh_bool_false);
    EXPECT_FALSE(lh_controller_is_enabled(&f.controller));
    EXPECT_TRUE(lh_controller_has_flag(&f.controller, lh_controller_flags_gestures))
        << "disabling cleared another flag";
    EXPECT_FALSE(f.down(0U));
    EXPECT_FALSE(f.up(10U));
    EXPECT_TRUE(f.log.events.empty());
    EXPECT_EQ(lh_input_recognizer_get_phase(lh_controller_get_recognizer(&f.controller), 0U),
              lh_input_recognizer_phase_idle);

    lh_controller_set_enabled(&f.controller, lh_bool_true);
    EXPECT_TRUE(f.down(20U));
    EXPECT_EQ(f.log.events.size(), 1U);
}

/* feed already leaves a disabled recognizer alone. update used to run it anyway and
   drop the long press in handle, so the phase became holding and the click was gone. */
TEST(controller, update_when_disabled_leaves_the_press_pending)
{
    fixture f;

    EXPECT_TRUE(f.down(0U));
    lh_controller_set_enabled(&f.controller, lh_bool_false);
    lh_controller_set_update_cb(&f.controller, record_update);
    EXPECT_FALSE(lh_controller_update(&f.controller, 600U));
    EXPECT_EQ(f.log.kinds(), (std::vector<lh_input_kind_t>{lh_input_kind_pointer_down}));
    EXPECT_EQ(f.log.updates, (std::vector<lh_tick_t>{600U}));
    EXPECT_EQ(lh_input_recognizer_get_phase(lh_controller_get_recognizer(&f.controller), 0U),
              lh_input_recognizer_phase_pending);

    lh_controller_set_enabled(&f.controller, lh_bool_true);
    EXPECT_TRUE(lh_controller_update(&f.controller, 600U));
    EXPECT_EQ(f.log.kinds().back(), lh_input_kind_long_press);
    EXPECT_EQ(lh_input_recognizer_get_phase(lh_controller_get_recognizer(&f.controller), 0U),
              lh_input_recognizer_phase_holding);
}

TEST(controller, feed_answers_false_when_the_handler_acts_on_nothing)
{
    fixture f;

    f.log.answer = lh_bool_false;
    EXPECT_FALSE(f.down(0U));
    EXPECT_FALSE(f.up(10U));
    EXPECT_EQ(f.log.events.size(), 3U) << "declined events still reach the handler";
}

TEST(controller, the_context_arrives_as_self)
{
    fixture f;
    handler_log other;

    lh_controller_set_context(&f.controller, &other);
    EXPECT_EQ(lh_controller_get_context(&f.controller), &other);
    f.down(0U);
    EXPECT_TRUE(f.log.events.empty());
    EXPECT_EQ(other.events.size(), 1U);
}

TEST(controller, has_flag_needs_every_bit)
{
    fixture f;

    lh_controller_set_flags(&f.controller, lh_controller_flags_gestures);
    EXPECT_FALSE(lh_controller_has_flag(&f.controller, lh_controller_flags_enabled |
                                                           lh_controller_flags_gestures));
    EXPECT_TRUE(lh_controller_has_flag(&f.controller, lh_controller_flags_none));
    EXPECT_FALSE(lh_controller_is_enabled(&f.controller));
}

TEST(controller, handle_passes_the_recognizer_by)
{
    const lh_input_event_t down = pointer_event(lh_input_kind_pointer_down, 10, 10, 0U);

    fixture f;
    EXPECT_TRUE(lh_controller_handle_event_is_handled(&f.controller, &down));
    EXPECT_EQ(f.log.events.size(), 1U);
    EXPECT_EQ(lh_input_recognizer_get_phase(lh_controller_get_recognizer(&f.controller), 0U),
              lh_input_recognizer_phase_idle);

    lh_controller_set_enabled(&f.controller, lh_bool_false);
    EXPECT_FALSE(lh_controller_handle_event_is_handled(&f.controller, &down));
    EXPECT_EQ(f.log.events.size(), 1U);
}

TEST(controller, handle_without_a_handler_drops_the_event)
{
    const lh_input_event_t down = pointer_event(lh_input_kind_pointer_down, 10, 10, 0U);

    fixture f;
    lh_controller_set_handle_cb(&f.controller, nullptr);
    EXPECT_FALSE(lh_controller_handle_event_is_handled(&f.controller, &down));
    EXPECT_FALSE(lh_controller_feed(&f.controller, &down));
    EXPECT_TRUE(f.log.events.empty());
}

TEST(controller, update_with_no_update_cb_sends_a_long_press)
{
    fixture f;

    f.down(0U);
    EXPECT_FALSE(lh_controller_update(&f.controller, 100U));
    EXPECT_TRUE(lh_controller_update(&f.controller, 600U));
    ASSERT_EQ(f.log.events.size(), 2U);
    EXPECT_EQ(lh_input_event_get_kind(&f.log.events[1]), lh_input_kind_long_press);
    EXPECT_TRUE(lh_input_event_is_synthesized(&f.log.events[1]));
}

TEST(controller, update_calls_update_cb_after_the_recognizer)
{
    fixture f;

    lh_controller_set_update_cb(&f.controller, record_update);
    EXPECT_EQ(lh_controller_get_update_cb(&f.controller), &record_update);
    f.down(0U);
    lh_controller_update(&f.controller, 600U);
    EXPECT_EQ(f.log.updates, (std::vector<lh_tick_t>{600U}));
    EXPECT_EQ(f.log.kinds().back(), lh_input_kind_long_press);
}

TEST(controller, update_without_gestures_leaves_the_recognizer_alone)
{
    fixture f;

    lh_controller_set_update_cb(&f.controller, record_update);
    lh_controller_set_flags(&f.controller, lh_controller_flags_enabled);
    EXPECT_FALSE(lh_controller_update(&f.controller, 42U));
    EXPECT_EQ(f.log.updates, (std::vector<lh_tick_t>{42U}));
    EXPECT_TRUE(f.log.events.empty());
}

TEST(controller, cancel_with_no_cancel_cb_forgets_the_press)
{
    fixture f;

    f.down(0U);
    lh_controller_cancel(&f.controller);
    EXPECT_EQ(lh_input_recognizer_get_phase(lh_controller_get_recognizer(&f.controller), 0U),
              lh_input_recognizer_phase_idle);
    lh_controller_update(&f.controller, 600U);
    f.up(700U);
    EXPECT_EQ(f.log.kinds(), raw_click) << "no long press, no click after a cancel";
}

TEST(controller, cancel_keeps_the_thresholds)
{
    lh_input_recognizer_config_t config = lh_input_recognizer_config_default_initializer();

    config.long_press_ms = 1000U;
    lh_controller_t controller;
    handler_log log;
    lh_controller_init(&controller, record, &log, &config);
    lh_controller_cancel(&controller);
    EXPECT_EQ(lh_input_recognizer_config_get_long_press_ms(
                  lh_input_recognizer_get_config(lh_controller_get_recognizer(&controller))),
              1000U);
}

TEST(controller, cancel_calls_cancel_cb)
{
    fixture f;

    lh_controller_set_cancel_cb(&f.controller, record_cancel);
    EXPECT_EQ(lh_controller_get_cancel_cb(&f.controller), &record_cancel);
    lh_controller_cancel(&f.controller);
    EXPECT_EQ(f.log.cancels, 1);
    EXPECT_TRUE(f.log.events.empty()) << "the cancel sent events";
}

TEST(controller, default_initializer_is_what_init_leaves)
{
    handler_log log;
    lh_controller_t controller = lh_controller_default_initializer(record, &log);
    const lh_input_event_t down = pointer_event(lh_input_kind_pointer_down, 10, 10, 0U);
    const lh_input_event_t up = pointer_event(lh_input_kind_pointer_up, 10, 10, 10U);

    EXPECT_EQ(lh_controller_get_flags(&controller),
              lh_controller_flags_enabled | lh_controller_flags_gestures);
    EXPECT_EQ(lh_controller_get_update_cb(&controller), nullptr);
    EXPECT_EQ(lh_controller_get_cancel_cb(&controller), nullptr);
    EXPECT_EQ(lh_controller_get_context(&controller), &log);
    lh_controller_feed(&controller, &down);
    lh_controller_feed(&controller, &up);
    EXPECT_EQ(log.kinds().back(), lh_input_kind_click);
}

TEST(controller, initializer_takes_every_field_in_order)
{
    handler_log log;
    lh_controller_t controller = lh_controller_initializer(
        record, record_update, record_cancel, &log,
        lh_input_recognizer_initializer(lh_input_recognizer_config_default_initializer(),
                                        {lh_input_recognizer_pointer_idle_initializer()}),
        lh_controller_flags_enabled);

    EXPECT_EQ(lh_controller_get_handle_cb(&controller), &record);
    EXPECT_EQ(lh_controller_get_update_cb(&controller), &record_update);
    EXPECT_EQ(lh_controller_get_cancel_cb(&controller), &record_cancel);
    EXPECT_EQ(lh_controller_get_flags(&controller), lh_controller_flags_enabled);
    lh_controller_update(&controller, 5U);
    lh_controller_cancel(&controller);
    EXPECT_EQ(log.updates, (std::vector<lh_tick_t>{5U}));
    EXPECT_EQ(log.cancels, 1);
}

TEST(controller, empty_initializer_drops_every_event)
{
    lh_controller_t controller = lh_controller_empty_initializer();
    const lh_input_event_t down = pointer_event(lh_input_kind_pointer_down, 10, 10, 0U);

    EXPECT_EQ(lh_controller_get_flags(&controller), lh_controller_flags_none);
    EXPECT_EQ(lh_controller_get_handle_cb(&controller), nullptr);
    EXPECT_EQ(lh_controller_get_context(&controller), nullptr);
    EXPECT_FALSE(lh_controller_feed(&controller, &down));
    EXPECT_FALSE(lh_controller_update(&controller, 600U));
    lh_controller_cancel(&controller);
}

TEST(controller, sink_feeds_the_controller_it_is_given)
{
    const lh_input_sink_cb sink = lh_controller_sink;
    const lh_input_event_t down = pointer_event(lh_input_kind_pointer_down, 10, 10, 0U);
    const lh_input_event_t up = pointer_event(lh_input_kind_pointer_up, 10, 10, 10U);

    fixture f;
    sink(&f.controller, &down);
    sink(&f.controller, &up);
    EXPECT_EQ(f.log.kinds(),
              (std::vector<lh_input_kind_t>{lh_input_kind_pointer_down, lh_input_kind_pointer_up,
                                            lh_input_kind_click}));
}
