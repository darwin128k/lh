/**
 * @file keys.h
 * @brief One polled keyboard: ::lh_os_input_keys_t.
 *
 * A driver reads which portable keys are down and reports each change. A key
 * event goes out only when that key changed. ::lh_os_key_other is not a key
 * here: a character the driver knows is a text event it builds itself.
 * Repeating a held key is the same path, with the repeat flag set on the
 * event. This report does not repeat.
 *
 * Shift, Control and Alt in the held mask are written onto the event as
 * modifiers. Needs no ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_INPUT_KEYS_H
#define LH_OS_INPUT_KEYS_H

#include <lh/core/timer/tick.h>
#include <lh/os/input/keys/fields.h>
#include <lh/os/input/keys/ptr.h>
#include <lh/os/input/sink/fn.h>
#include <lh/os/key.h>

/**
 * @struct lh_os_input_keys
 * @brief The keys held after the last report.
 */
struct lh_os_input_keys
{
    /** Members: ::lh_os_input_keys_fields. */
    lh_os_input_keys_fields(lh_u32_t);
};
/** @copybrief lh_os_input_keys */
typedef struct lh_os_input_keys lh_os_input_keys_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The device id written on events. */
lh_u8_t
lh_os_input_keys_get_device(const lh_os_input_keys_t *self);

/** @brief Store the device id. */
lh_void
lh_os_input_keys_set_device(lh_os_input_keys_t *self, lh_u8_t device);

/** @brief The mask of keys held. Bit 0 stays clear. */
lh_u32_t
lh_os_input_keys_get_down(const lh_os_input_keys_t *self);

/** @brief Store the mask of keys held. */
lh_void
lh_os_input_keys_set_down(lh_os_input_keys_t *self, lh_u32_t down);

/** @brief No key held. @p device is written on each event. */
lh_void
lh_os_input_keys_init(lh_os_input_keys_t *self, lh_u8_t device);

/**
 * @brief True when @p key is held.
 *
 * ::lh_os_key_other and a key at or past ::LH_OS_KEY_COUNT are false.
 */
lh_bool_t
lh_os_input_keys_is_down(const lh_os_input_keys_t *self, lh_os_key_t key);

/**
 * @brief Hand a key event to @p sink when @p down differs from the last one.
 *
 * ::lh_os_key_other and a key at or past ::LH_OS_KEY_COUNT do nothing. A null
 * @p sink still stores the bit. @p scancode is copied onto the event; `0` is
 * unknown. The event is not a repeat.
 */
lh_void
lh_os_input_keys_report(lh_os_input_keys_t *self, lh_os_key_t key, lh_bool_t down,
                        lh_u32_t scancode, lh_tick_t time, lh_os_input_sink_fn *sink,
                        lh_self_ptr sink_self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_INPUT_KEYS_H */
