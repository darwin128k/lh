/**
 * @file pointer.h
 * @brief One pointer's gesture inside the recognizer:
 *        ::lh_os_input_recognizer_pointer_t.
 *
 * Part of ::lh_os_input_recognizer_t's storage. The recognizer drives it
 * through the functions here; what a caller usually asks is on the
 * recognizer (::lh_os_input_recognizer_get_phase).
 */

#ifndef LH_OS_INPUT_RECOGNIZER_POINTER_H
#define LH_OS_INPUT_RECOGNIZER_POINTER_H

#include <lh/os/input/event.h>
#include <lh/os/input/recognizer/phase.h>
#include <lh/os/input/recognizer/pointer/fields.h>
#include <lh/os/input/recognizer/pointer/ptr.h>

/**
 * @struct lh_os_input_recognizer_pointer
 * @brief The press, the drag and the last click of one pointer.
 */
struct lh_os_input_recognizer_pointer
{
    /** Members: ::lh_os_input_recognizer_pointer_fields. */
    lh_os_input_recognizer_pointer_fields(lh_os_input_recognizer_phase_t, lh_os_input_device_kind_t,
                                          lh_os_input_button_t, lh_os_input_mods_t,
                                          lh_os_input_coord_t, lh_tick_t);
};
/** @copybrief lh_os_input_recognizer_pointer */
typedef struct lh_os_input_recognizer_pointer lh_os_input_recognizer_pointer_t;

LH_COMPILER_EXTERN_C_BEGIN

/** True when (@p x, @p y) is more than @p slop pixels from (@p from_x, @p from_y) on either
   axis. */
lh_bool_t
lh_os_input_recognizer_is_beyond(lh_os_input_coord_t from_x, lh_os_input_coord_t from_y,
                                 lh_os_input_coord_t x, lh_os_input_coord_t y, lh_u16_t slop);

/** Nothing held, no click series to continue. */
lh_void
lh_os_input_recognizer_pointer_clear(lh_os_input_recognizer_pointer_ptr self);

/** Where the gesture of @p self is. */
lh_os_input_recognizer_phase_t
lh_os_input_recognizer_pointer_get_phase(const lh_os_input_recognizer_pointer_ptr self);

/** Move the gesture of @p self to @p phase. */
lh_void
lh_os_input_recognizer_pointer_set_phase(lh_os_input_recognizer_pointer_ptr self,
                                         lh_os_input_recognizer_phase_t phase);

/** The button that started the press. */
lh_os_input_button_t
lh_os_input_recognizer_pointer_get_button(const lh_os_input_recognizer_pointer_ptr self);

/** When the press went down. */
lh_tick_t
lh_os_input_recognizer_pointer_get_down_time(const lh_os_input_recognizer_pointer_ptr self);

/** No click series to continue: a drag, a long press or a cancel came in between. */
lh_void
lh_os_input_recognizer_pointer_forget_click(lh_os_input_recognizer_pointer_ptr self);

/** Start a press from @p event (a pointer down). */
lh_void
lh_os_input_recognizer_pointer_begin(lh_os_input_recognizer_pointer_ptr self,
                                     const lh_os_input_event_ptr event);

/** True when @p event has moved past @p slop pixels from where the press went down. */
lh_bool_t
lh_os_input_recognizer_pointer_is_past_slop(const lh_os_input_recognizer_pointer_ptr self,
                                            const lh_os_input_event_ptr event, lh_u16_t slop);

/** The distance from the last drag position to @p event, into @p dx and @p dy, and @p event
   becomes the last drag position. */
lh_void
lh_os_input_recognizer_pointer_advance(lh_os_input_recognizer_pointer_ptr self,
                                       const lh_os_input_event_ptr event, lh_s32_t *dx,
                                       lh_s32_t *dy);

/** The click number the up @p event makes: one more than the last click when it was of the
   same button, released no more than @p window_ms before this press went down and within
   @p slop pixels; `1` otherwise. */
lh_u8_t
lh_os_input_recognizer_pointer_count_click(const lh_os_input_recognizer_pointer_ptr self,
                                           const lh_os_input_event_ptr event, lh_u16_t window_ms,
                                           lh_u16_t slop);

/** Remember the click @p event ended as number @p count, for the next one to continue. */
lh_void
lh_os_input_recognizer_pointer_remember_click(lh_os_input_recognizer_pointer_ptr self,
                                              const lh_os_input_event_ptr event, lh_u8_t count);

/** A gesture event of @p kind for pointer @p pointer at (@p x, @p y) and @p time, from the
   press @p self holds, into @p out. */
lh_void
lh_os_input_recognizer_pointer_make(const lh_os_input_recognizer_pointer_ptr self, lh_u8_t pointer,
                                    lh_os_input_kind_t kind, lh_os_input_coord_t x,
                                    lh_os_input_coord_t y, lh_tick_t time,
                                    lh_os_input_event_t *out);

/** The gesture event of @p kind that @p event completes, into @p out: @p event's position,
   time, buttons and mods, @p self's device and button. */
lh_void
lh_os_input_recognizer_pointer_make_from(const lh_os_input_recognizer_pointer_ptr self,
                                         const lh_os_input_event_ptr event, lh_os_input_kind_t kind,
                                         lh_os_input_event_t *out);

/** The gesture event of @p kind at @p time where the press @p self holds last was, into
   @p out: no event of the device's own to take it from (a timeout, a cancel). */
lh_void
lh_os_input_recognizer_pointer_make_at_last(const lh_os_input_recognizer_pointer_ptr self,
                                            lh_u8_t pointer, lh_os_input_kind_t kind,
                                            lh_tick_t time, lh_os_input_event_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_INPUT_RECOGNIZER_POINTER_H */
