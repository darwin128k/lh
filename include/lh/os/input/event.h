/**
 * @file event.h
 * @brief One input event: ::lh_os_input_event_t.
 *
 * The one thing every input source produces and every consumer reads: a
 * window backend (::lh_os_window_set_on_input), the web host, a
 * microcontroller driver, ::lh_os_input_queue_t, ::lh_os_input_recognizer_t
 * and ::lh_ui_view_input. Plain data — no pointers, no heap — so it can be
 * copied into a queue from an interrupt. Read it only through the accessors.
 *
 * Needs no ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_INPUT_EVENT_H
#define LH_OS_INPUT_EVENT_H

#include <lh/os/input/axis.h>
#include <lh/os/input/button.h>
#include <lh/os/input/coord.h>
#include <lh/os/input/device/kind.h>
#include <lh/os/input/event/fields.h>
#include <lh/os/input/event/ptr.h>
#include <lh/os/input/flags.h>
#include <lh/os/input/kind.h>
#include <lh/os/input/mods.h>
#include <lh/os/key.h>
#include <lh/timer/tick.h>

/**
 * @struct lh_os_input_event
 * @brief One input event, raw or synthesized.
 */
struct lh_os_input_event
{
    /** Members: ::lh_os_input_event_fields. */
    lh_os_input_event_fields(lh_os_input_kind_t, lh_os_input_device_kind_t, lh_os_input_button_t,
                             lh_os_input_axis_t, lh_os_key_t, lh_os_input_coord_t,
                             lh_os_input_mods_t, lh_tick_t, lh_os_input_flags_t);
};
/** @copybrief lh_os_input_event */
typedef struct lh_os_input_event lh_os_input_event_t;

/* -- Building -------------------------------------------------------------- */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Clear @p self to an event of @p kind from a @p device_kind at @p time.
 *
 * Every other field is zero: device and pointer `0`, no button, no mods, no
 * flags. The other `init_*` functions start from this.
 */
lh_void
lh_os_input_event_init(lh_os_input_event_t *self, lh_os_input_kind_t kind,
                       lh_os_input_device_kind_t device_kind, lh_tick_t time);

/**
 * @brief A pointer event: @p kind at client (@p x, @p y), Q24.8.
 *
 * @p button is the button that changed (::lh_os_input_button_none for a
 * move). Pointer `0` is also marked ::LH_OS_INPUT_FLAG_PRIMARY.
 */
lh_void
lh_os_input_event_init_pointer(lh_os_input_event_t *self, lh_os_input_kind_t kind,
                               lh_os_input_device_kind_t device_kind, lh_u8_t pointer,
                               lh_os_input_button_t button, lh_os_input_coord_t x,
                               lh_os_input_coord_t y, lh_tick_t time);

/**
 * @brief A mouse wheel at (@p x, @p y): @p dx, @p dy in 1/120 notch
 *        (::LH_OS_INPUT_WHEEL_NOTCH), positive is left / up.
 */
lh_void
lh_os_input_event_init_wheel(lh_os_input_event_t *self, lh_os_input_coord_t x,
                             lh_os_input_coord_t y, lh_s32_t dx, lh_s32_t dy, lh_tick_t time);

/**
 * @brief @p key going down (@p down) or up; @p repeat marks a held key's
 *        repetition. @p scancode is the native code, `0` when unknown.
 */
lh_void
lh_os_input_event_init_key(lh_os_input_event_t *self, lh_os_key_t key, lh_bool_t down,
                           lh_bool_t repeat, lh_u32_t scancode, lh_tick_t time);

/**
 * @brief One typed character, Unicode code point @p code.
 */
lh_void
lh_os_input_event_init_text(lh_os_input_event_t *self, lh_u32_t code, lh_tick_t time);

/**
 * @brief Controller @p device's @p button going down (@p down) or up.
 */
lh_void
lh_os_input_event_init_controller_button(lh_os_input_event_t *self, lh_u8_t device,
                                         lh_os_input_button_t button, lh_bool_t down,
                                         lh_tick_t time);

/**
 * @brief Controller @p device's @p axis now at @p value.
 */
lh_void
lh_os_input_event_init_controller_axis(lh_os_input_event_t *self, lh_u8_t device,
                                       lh_os_input_axis_t axis, lh_s16_t value, lh_tick_t time);

/** @brief Set the modifiers held during @p self. */
lh_void
lh_os_input_event_set_mods(lh_os_input_event_t *self, lh_os_input_mods_t mods);

/** @brief Set the mask of buttons held after @p self. */
lh_void
lh_os_input_event_set_buttons(lh_os_input_event_t *self, lh_u32_t buttons);

/** @brief Replace the `LH_OS_INPUT_FLAG_*` set of @p self. */
lh_void
lh_os_input_event_set_flags(lh_os_input_event_t *self, lh_os_input_flags_t flags);

/** @brief Set the id of the device @p self came from. */
lh_void
lh_os_input_event_set_device(lh_os_input_event_t *self, lh_u8_t device);

/** @brief Set the click number of @p self. */
lh_void
lh_os_input_event_set_count(lh_os_input_event_t *self, lh_u8_t count);

/** @brief Set the wheel or drag distance of @p self. */
lh_void
lh_os_input_event_set_delta(lh_os_input_event_t *self, lh_s32_t dx, lh_s32_t dy);

/* -- Reading --------------------------------------------------------------- */

/** @brief What happened. */
lh_os_input_kind_t
lh_os_input_event_get_kind(const lh_os_input_event_ptr self);

/** @brief The sort of device. */
lh_os_input_device_kind_t
lh_os_input_event_get_device_kind(const lh_os_input_event_ptr self);

/** @brief The device id. */
lh_u8_t
lh_os_input_event_get_device(const lh_os_input_event_ptr self);

/** @brief The contact id (`0` for the mouse). */
lh_u8_t
lh_os_input_event_get_pointer(const lh_os_input_event_ptr self);

/** @brief The button that changed. */
lh_os_input_button_t
lh_os_input_event_get_button(const lh_os_input_event_ptr self);

/** @brief Every button held after the event (::lh_os_input_button_to_mask). */
lh_u32_t
lh_os_input_event_get_buttons(const lh_os_input_event_ptr self);

/** @brief Client x, Q24.8. */
lh_os_input_coord_t
lh_os_input_event_get_x(const lh_os_input_event_ptr self);

/** @brief Client y, Q24.8. */
lh_os_input_coord_t
lh_os_input_event_get_y(const lh_os_input_event_ptr self);

/** @brief Wheel (1/120 notch) or drag (Q24.8) distance across. */
lh_s32_t
lh_os_input_event_get_dx(const lh_os_input_event_ptr self);

/** @brief Wheel (1/120 notch) or drag (Q24.8) distance down the other axis. */
lh_s32_t
lh_os_input_event_get_dy(const lh_os_input_event_ptr self);

/** @brief The controller axis. */
lh_os_input_axis_t
lh_os_input_event_get_axis(const lh_os_input_event_ptr self);

/** @brief The controller axis position. */
lh_s16_t
lh_os_input_event_get_value(const lh_os_input_event_ptr self);

/** @brief The portable key. */
lh_os_key_t
lh_os_input_event_get_key(const lh_os_input_event_ptr self);

/** @brief The native key code, `0` when unknown. */
lh_u32_t
lh_os_input_event_get_scancode(const lh_os_input_event_ptr self);

/** @brief The typed code point of a text event. */
lh_u32_t
lh_os_input_event_get_code(const lh_os_input_event_ptr self);

/** @brief The modifiers held. */
lh_os_input_mods_t
lh_os_input_event_get_mods(const lh_os_input_event_ptr self);

/** @brief When it happened, ms. */
lh_tick_t
lh_os_input_event_get_time(const lh_os_input_event_ptr self);

/** @brief The click number of a click. */
lh_u8_t
lh_os_input_event_get_count(const lh_os_input_event_ptr self);

/** @brief The `LH_OS_INPUT_FLAG_*` set. */
lh_os_input_flags_t
lh_os_input_event_get_flags(const lh_os_input_event_ptr self);

/** @brief True when every modifier of @p mods is held. */
lh_bool_t
lh_os_input_event_has_mods(const lh_os_input_event_ptr self, lh_os_input_mods_t mods);

/** @brief True when @p flag (one `LH_OS_INPUT_FLAG_*`) is set. */
lh_bool_t
lh_os_input_event_has_flag(const lh_os_input_event_ptr self, lh_os_input_flags_t flag);

/**
 * @brief True for an event at a position: the pointer kinds, the wheel and
 *        the pointer gestures (click to drag end).
 */
lh_bool_t
lh_os_input_event_is_pointer(const lh_os_input_event_ptr self);

/** @brief True for key down, key up and text. */
lh_bool_t
lh_os_input_event_is_keyboard(const lh_os_input_event_ptr self);

/** @brief True for a controller button or axis. */
lh_bool_t
lh_os_input_event_is_controller(const lh_os_input_event_ptr self);

/** @brief True for a held key's repetition. */
lh_bool_t
lh_os_input_event_is_repeat(const lh_os_input_event_ptr self);

/** @brief True when the recognizer made it. */
lh_bool_t
lh_os_input_event_is_synthesized(const lh_os_input_event_ptr self);

/** @brief True for the main contact. */
lh_bool_t
lh_os_input_event_is_primary(const lh_os_input_event_ptr self);

/** @brief True when a gesture was taken away rather than finished. */
lh_bool_t
lh_os_input_event_is_cancelled(const lh_os_input_event_ptr self);

/** @brief True when @p self is @p key going down (a repetition included). */
lh_bool_t
lh_os_input_event_is_key_down(const lh_os_input_event_ptr self, lh_os_key_t key);

/**
 * @brief Store what happened (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param kind The new value.
 */
lh_void
lh_os_input_event_set_kind(lh_os_input_event_t *self, lh_os_input_kind_t kind);

/**
 * @brief Store the kind of device the event came from (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param device_kind The new value.
 */
lh_void
lh_os_input_event_set_device_kind(lh_os_input_event_t *self, lh_os_input_device_kind_t device_kind);

/**
 * @brief Store the pointer id of the event (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param pointer The new value.
 */
lh_void
lh_os_input_event_set_pointer(lh_os_input_event_t *self, lh_u8_t pointer);

/**
 * @brief Store the button that changed (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param button The new value.
 */
lh_void
lh_os_input_event_set_button(lh_os_input_event_t *self, lh_os_input_button_t button);

/**
 * @brief Store the horizontal position, in window pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param x The new value.
 */
lh_void
lh_os_input_event_set_x(lh_os_input_event_t *self, lh_os_input_coord_t x);

/**
 * @brief Store the vertical position, in window pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param y The new value.
 */
lh_void
lh_os_input_event_set_y(lh_os_input_event_t *self, lh_os_input_coord_t y);

/**
 * @brief Store the horizontal wheel or move delta (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param dx The new value.
 */
lh_void
lh_os_input_event_set_dx(lh_os_input_event_t *self, lh_s32_t dx);

/**
 * @brief Store the vertical wheel or move delta (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param dy The new value.
 */
lh_void
lh_os_input_event_set_dy(lh_os_input_event_t *self, lh_s32_t dy);

/**
 * @brief Store the controller axis that moved (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param axis The new value.
 */
lh_void
lh_os_input_event_set_axis(lh_os_input_event_t *self, lh_os_input_axis_t axis);

/**
 * @brief Store the new value of the controller axis (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_os_input_event_set_value(lh_os_input_event_t *self, lh_s16_t value);

/**
 * @brief Store the key that changed (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param key The new value.
 */
lh_void
lh_os_input_event_set_key(lh_os_input_event_t *self, lh_os_key_t key);

/**
 * @brief Store the hardware scancode of the key (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param scancode The new value.
 */
lh_void
lh_os_input_event_set_scancode(lh_os_input_event_t *self, lh_u32_t scancode);

/**
 * @brief Store the Unicode code point of a text event (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param code The new value.
 */
lh_void
lh_os_input_event_set_code(lh_os_input_event_t *self, lh_u32_t code);

/**
 * @brief Store when the event happened, in ticks (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param time The new value.
 */
lh_void
lh_os_input_event_set_time(lh_os_input_event_t *self, lh_tick_t time);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_INPUT_EVENT_H */
