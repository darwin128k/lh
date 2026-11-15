/**
 * @file recognizer.h
 * @brief Gestures from raw input: ::lh_os_input_recognizer_t.
 *
 * Between the source of events (a window, the web host, a queue) and their
 * consumer (::lh_ui_view_input, the app). Every event fed in is passed on
 * unchanged — except a vertical wheel with Shift held, which is turned sideways
 * when the config says so — and the gestures it completes follow it:
 *
 * | Fed in                                   | Passed on after it                        |
 * |------------------------------------------|-------------------------------------------|
 * | down, up within the slop                 | ::lh_os_input_kind_click (`count` 1, 2 …) |
 * | the second click of a quick, near pair   | also ::lh_os_input_kind_double_click      |
 * | down held still for `long_press_ms`      | ::lh_os_input_kind_long_press, no click   |
 * | move past the slop while down            | ::lh_os_input_kind_drag_begin             |
 * | every move after that                    | ::lh_os_input_kind_drag (`dx`, `dy`)      |
 * | up, or pointer cancel, while dragging    | ::lh_os_input_kind_drag_end               |
 * | that up, when the drag travelled         | also ::lh_os_input_kind_swipe (`dx`, `dy` |
 * | `swipe_px`                               | from where it went down)                  |
 * | arrow key, or a d-pad button down        | ::lh_os_input_kind_navigate (`dx`, `dy`   |
 * |                                          | are -1, 0 or 1)                           |
 * | Enter, Space, or the south button down   | ::lh_os_input_kind_activate               |
 * | focus out with pointers down             | a synthesized pointer cancel for each     |
 *
 * A gesture follows one button per pointer: the first one down. Pointer ids
 * at or above ::LH_OS_INPUT_POINTER_MAX get no gestures. Time is the events'
 * `time` and the `now` given to ::lh_os_input_recognizer_update — the same
 * ::lh_tick_t clock — so the recognizer is deterministic and needs no OS:
 * it runs the same on a desktop, in the web host and on a microcontroller.
 *
 * Example usage:
 * @code{.c}
 * static lh_os_input_recognizer_t recognizer;
 *
 * static lh_void
 * on_input(lh_self_ptr self, const lh_os_input_event_ptr event)
 * {
 *     lh_os_input_recognizer_feed(&recognizer, event, on_gesture, self);
 * }
 *
 * // and on a timer:
 * lh_os_input_recognizer_update(&recognizer, lh_os_tick_ms(), on_gesture, context);
 * @endcode
 */

#ifndef LH_OS_INPUT_RECOGNIZER_H
#define LH_OS_INPUT_RECOGNIZER_H

#include <lh/os/input/recognizer/config.h>
#include <lh/os/input/recognizer/fields.h>
#include <lh/os/input/recognizer/pointer.h>
#include <lh/os/input/recognizer/ptr.h>
#include <lh/os/input/sink/cb.h>

/**
 * @struct lh_os_input_recognizer
 * @brief The thresholds and one gesture per pointer.
 */
struct lh_os_input_recognizer
{
    /** Members: ::lh_os_input_recognizer_fields. */
    lh_os_input_recognizer_fields(lh_os_input_recognizer_config_t,
                                  lh_os_input_recognizer_pointer_t);
};
/** @copybrief lh_os_input_recognizer */
typedef struct lh_os_input_recognizer lh_os_input_recognizer_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @p event to @p sink with @p flags added. */
lh_void
lh_os_input_recognizer_send(lh_os_input_event_t *event, lh_os_input_flags_t flags,
                            lh_os_input_sink_cb sink, lh_ptr context);

/**
 * @brief Make @p self a recognizer with nothing held, using a copy of
 *        @p config (::lh_null for the defaults).
 */
lh_void
lh_os_input_recognizer_init(lh_os_input_recognizer_t *self,
                            const lh_os_input_recognizer_config_ptr config);

/** A pointer down: the first button down on an idle pointer starts a press. */
lh_void
lh_os_input_recognizer_on_down(lh_os_input_recognizer_pointer_ptr state,
                               const lh_os_input_event_ptr event);

/** A pointer move: past the slop a press becomes a drag; a drag reports how far it went. */
lh_void
lh_os_input_recognizer_on_move(lh_os_input_recognizer_t *self,
                               lh_os_input_recognizer_pointer_ptr state,
                               const lh_os_input_event_ptr event, lh_os_input_sink_cb sink,
                               lh_ptr context);

/** An up of the button that started the press: a click from a press, a drag end from a drag,
   nothing after a long press. */
lh_void
lh_os_input_recognizer_on_up(lh_os_input_recognizer_t *self,
                             lh_os_input_recognizer_pointer_ptr state,
                             const lh_os_input_event_ptr event, lh_os_input_sink_cb sink,
                             lh_ptr context);

/** What a pointer event does to the gesture of its pointer. */
lh_void
lh_os_input_recognizer_on_pointer(lh_os_input_recognizer_t *self, const lh_os_input_event_ptr event,
                                  lh_os_input_sink_cb sink, lh_ptr context);

/** A vertical wheel with Shift held, turned sideways when the config says so: true when it
   was, and went to @p sink. */
lh_bool_t
lh_os_input_recognizer_send_shifted_wheel(lh_os_input_recognizer_t *self,
                                          const lh_os_input_event_ptr event,
                                          lh_os_input_sink_cb sink, lh_ptr context);

/**
 * @brief Hand @p event to @p sink, then the gestures it completes.
 *
 * Timeouts due by the event's time (a long press) go out first, as
 * ::lh_os_input_recognizer_update would have sent them. A synthesized event is
 * passed on and not looked at again. @p sink gets @p context as its `self`,
 * here and in ::lh_os_input_recognizer_update and ::lh_os_input_recognizer_reset.
 */
/**
 * @brief Replace a controller axis with the dead-zone value, or swallow it when
 *        that value did not change.
 *
 * @return True when @p event was an axis and the dead zone is on. The raw event
 *         must not be handed on as well.
 */
lh_bool_t
lh_os_input_recognizer_take_axis(lh_os_input_recognizer_t *self, const lh_os_input_event_ptr event,
                                 lh_os_input_sink_cb sink, lh_ptr context);

/** @brief A navigate or activate step copied from @p event. */
lh_void
lh_os_input_recognizer_send_step(const lh_os_input_event_ptr event, lh_os_input_kind_t kind,
                                 lh_s32_t dx, lh_s32_t dy, lh_os_input_sink_cb sink, lh_ptr context);

/** @brief Arrow keys, Enter, Space and the d-pad and south button become steps. */
lh_void
lh_os_input_recognizer_on_command(lh_os_input_recognizer_t *self, const lh_os_input_event_ptr event,
                                  lh_os_input_sink_cb sink, lh_ptr context);

/** @brief Repeat the held controller buttons that are due by @p now. */
lh_void
lh_os_input_recognizer_repeat_pad(lh_os_input_recognizer_t *self, lh_tick_t now,
                                  lh_os_input_sink_cb sink, lh_ptr context);

lh_void
lh_os_input_recognizer_feed(lh_os_input_recognizer_t *self, const lh_os_input_event_ptr event,
                            lh_os_input_sink_cb sink, lh_ptr context);

/**
 * @brief Send what is due by @p now with no new event: long presses.
 *
 * Call it from a timer; ::lh_os_input_recognizer_get_next_deadline says when.
 */
lh_void
lh_os_input_recognizer_update(lh_os_input_recognizer_t *self, lh_tick_t now,
                              lh_os_input_sink_cb sink, lh_ptr context);

/** Pointer @p pointer of @p state let go of without finishing its gesture at @p time: a drag
   ends cancelled, and the series of clicks is over. */
lh_void
lh_os_input_recognizer_abandon(lh_os_input_recognizer_pointer_ptr state, lh_u8_t pointer,
                               lh_tick_t time, lh_os_input_sink_cb sink, lh_ptr context);

/** Every pointer still down is taken away at @p time: a synthesized pointer cancel for it,
   then what ::lh_os_input_recognizer_abandon sends. */
lh_void
lh_os_input_recognizer_cancel_all(lh_os_input_recognizer_t *self, lh_tick_t time,
                                  lh_os_input_sink_cb sink, lh_ptr context);

/**
 * @brief Cancel every gesture at @p now: a synthesized pointer cancel for each
 *        pointer down, a cancelled drag end for each drag, and no click series
 *        left to continue.
 */
lh_void
lh_os_input_recognizer_reset(lh_os_input_recognizer_t *self, lh_tick_t now,
                             lh_os_input_sink_cb sink, lh_ptr context);

/** When the press of @p state turns into a long press, into @p out; false when it never will
   (not pending, or long presses are off). */
lh_bool_t
lh_os_input_recognizer_get_long_press_due(const lh_os_input_recognizer_ptr self,
                                          const lh_os_input_recognizer_pointer_ptr state,
                                          lh_tick_t *out);

/**
 * @brief When ::lh_os_input_recognizer_update next has something to send,
 *        into @p out.
 *
 * @return False, @p out untouched, when nothing waits on time.
 */
lh_bool_t
lh_os_input_recognizer_get_next_deadline(const lh_os_input_recognizer_ptr self, lh_tick_t *out);

/** The state of @p pointer, or ::lh_null for an id with no gestures. */
lh_os_input_recognizer_pointer_ptr
lh_os_input_recognizer_find(lh_os_input_recognizer_t *self, lh_u8_t pointer);

/** The read-only state of @p pointer, or ::lh_null for an id with no gestures. */
const lh_os_input_recognizer_pointer_ptr
lh_os_input_recognizer_find_const(const lh_os_input_recognizer_ptr self, lh_u8_t pointer);

/**
 * @brief Where @p pointer is in its gesture;
 *        ::lh_os_input_recognizer_phase_idle for an id with no gestures.
 */
lh_os_input_recognizer_phase_t
lh_os_input_recognizer_get_phase(const lh_os_input_recognizer_ptr self, lh_u8_t pointer);

/** @brief True while @p pointer drags. */
lh_bool_t
lh_os_input_recognizer_is_dragging(const lh_os_input_recognizer_ptr self, lh_u8_t pointer);

/** @brief The thresholds @p self works with. */
const lh_os_input_recognizer_config_ptr
lh_os_input_recognizer_get_config(const lh_os_input_recognizer_ptr self);

/**
 * @brief The thresholds @p self works with, to change in place (level 1).
 *
 * @param self The recognizer; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_input_recognizer_config_ptr
lh_os_input_recognizer_get_config_mut(lh_os_input_recognizer_t *self);

/**
 * @brief Store @p config as the thresholds of @p self (level 1).
 *
 * A level-1 setter: the thresholds are copied as given and are not validated.
 *
 * @param self   The recognizer; checked by ::lh_assert_runtime_ref.
 * @param config The thresholds; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_os_input_recognizer_set_config(lh_os_input_recognizer_t *self,
                                  const lh_os_input_recognizer_config_ptr config);

/**
 * @brief The state of pointer @p pointer (level 1).
 *
 * The fast form: @p pointer is not checked and must be below ::LH_OS_INPUT_POINTER_MAX.
 * ::lh_os_input_recognizer_find_const checks it.
 *
 * @param self    The recognizer; checked by ::lh_assert_runtime_ref.
 * @param pointer Pointer id.
 */
const lh_os_input_recognizer_pointer_ptr
lh_os_input_recognizer_get_pointer(const lh_os_input_recognizer_ptr self, lh_u8_t pointer);

/**
 * @brief The state of pointer @p pointer, to change in place (level 1).
 *
 * The fast form: @p pointer is not checked and must be below ::LH_OS_INPUT_POINTER_MAX.
 * ::lh_os_input_recognizer_find checks it.
 *
 * @param self    The recognizer; checked by ::lh_assert_runtime_ref.
 * @param pointer Pointer id.
 */
lh_os_input_recognizer_pointer_ptr
lh_os_input_recognizer_get_pointer_mut(lh_os_input_recognizer_t *self, lh_u8_t pointer);

/** @brief The controller whose buttons this recognizer repeats. */
lh_u8_t
lh_os_input_recognizer_get_pad_device(const lh_os_input_recognizer_t *self);

/** @brief Store the controller whose buttons this recognizer repeats. */
lh_void
lh_os_input_recognizer_set_pad_device(lh_os_input_recognizer_t *self, lh_u8_t pad_device);

/** @brief Buttons held on that controller. */
lh_u32_t
lh_os_input_recognizer_get_pad_buttons(const lh_os_input_recognizer_t *self);

/** @brief Store the buttons held on that controller. */
lh_void
lh_os_input_recognizer_set_pad_buttons(lh_os_input_recognizer_t *self, lh_u32_t pad_buttons);

/** @brief When the next button repeat is due. */
lh_tick_t
lh_os_input_recognizer_get_pad_repeat_at(const lh_os_input_recognizer_t *self);

/** @brief Store when the next button repeat is due. */
lh_void
lh_os_input_recognizer_set_pad_repeat_at(lh_os_input_recognizer_t *self, lh_tick_t pad_repeat_at);

/** @brief True when a held button is waiting to repeat. */
lh_bool_t
lh_os_input_recognizer_get_pad_armed(const lh_os_input_recognizer_t *self);

/** @brief Store whether a held button is waiting to repeat. */
lh_void
lh_os_input_recognizer_set_pad_armed(lh_os_input_recognizer_t *self, lh_bool_t pad_armed);

/** @brief The controller whose axes this recognizer is filtering. */
lh_u8_t
lh_os_input_recognizer_get_axis_device(const lh_os_input_recognizer_t *self);

/** @brief Store the controller whose axes this recognizer is filtering. */
lh_void
lh_os_input_recognizer_set_axis_device(lh_os_input_recognizer_t *self, lh_u8_t axis_device);

/**
 * @brief True when @p axis is outside the dead zone.
 *
 * @p axis is not checked. It must be below ::LH_OS_INPUT_AXIS_COUNT.
 */
lh_bool_t
lh_os_input_recognizer_get_axis_hot(const lh_os_input_recognizer_t *self, lh_os_input_axis_t axis);

/** @brief Store whether @p axis is outside the dead zone. @p axis is not checked. */
lh_void
lh_os_input_recognizer_set_axis_hot(lh_os_input_recognizer_t *self, lh_os_input_axis_t axis,
                                    lh_bool_t hot);

/** @brief The last value handed on for @p axis. @p axis is not checked. */
lh_s16_t
lh_os_input_recognizer_get_axis_out(const lh_os_input_recognizer_t *self, lh_os_input_axis_t axis);

/** @brief Store the last value handed on for @p axis. @p axis is not checked. */
lh_void
lh_os_input_recognizer_set_axis_out(lh_os_input_recognizer_t *self, lh_os_input_axis_t axis,
                                    lh_s16_t value);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_INPUT_RECOGNIZER_H */
