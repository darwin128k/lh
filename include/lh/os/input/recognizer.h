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

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_INPUT_RECOGNIZER_H */
