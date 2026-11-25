/**
 * @file touch.h
 * @brief One polled touch source: ::lh_input_touch_t.
 *
 * A driver reads its chip and fills ::lh_input_sample_t. This compares that
 * list with the previous one and hands pointer events to a sink: down for a
 * new contact, move when its position changed, up when the list dropped it.
 * Several contacts are several pointer ids, from `1` up. The mouse stays `0`.
 * A contact past ::LH_INPUT_TOUCH_MAX is skipped until a finger goes up.
 *
 * The driver keeps the chip. This keeps the contact table and the last frame.
 * Needs no ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_INPUT_TOUCH_H
#define LH_INPUT_TOUCH_H

#include <lh/core/timer/tick.h>
#include <lh/core/input/button.h>
#include <lh/core/input/contacts.h>
#include <lh/core/input/kind.h>
#include <lh/core/input/sample.h>
#include <lh/core/input/sink/fn.h>
#include <lh/core/input/touch/fields.h>
#include <lh/core/input/touch/ptr.h>

/**
 * @struct lh_input_touch
 * @brief The last frame of one touch or pen driver.
 */
struct lh_input_touch
{
    /** Members: ::lh_input_touch_fields. */
    lh_input_touch_fields(lh_input_contacts_t, lh_input_sample_t, lh_input_device_kind_t);
};
/** @copybrief lh_input_touch */
typedef struct lh_input_touch lh_input_touch_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The contact table. */
const lh_input_contacts_t *
lh_input_touch_get_contacts(const lh_input_touch_t *self);

/** @brief The contact table, mutable. */
lh_input_contacts_t *
lh_input_touch_get_contacts_mut(lh_input_touch_t *self);

/** @brief How many contacts of the last frame received a pointer id. */
lh_u8_t
lh_input_touch_get_count(const lh_input_touch_t *self);

/** @brief Store how many contacts the last frame kept. */
lh_void
lh_input_touch_set_count(lh_input_touch_t *self, lh_u8_t count);

/** @brief The device id written on events. */
lh_u8_t
lh_input_touch_get_device(const lh_input_touch_t *self);

/** @brief Store the device id. */
lh_void
lh_input_touch_set_device(lh_input_touch_t *self, lh_u8_t device);

/** @brief Touch or pen. */
lh_input_device_kind_t
lh_input_touch_get_device_kind(const lh_input_touch_t *self);

/** @brief Store touch or pen. */
lh_void
lh_input_touch_set_device_kind(lh_input_touch_t *self, lh_input_device_kind_t device_kind);

/**
 * @brief Sample @p index of the last frame.
 *
 * @p index is not checked. It must be below ::lh_input_touch_get_count.
 */
const lh_input_sample_t *
lh_input_touch_get_sample(const lh_input_touch_t *self, lh_u8_t index);

/**
 * @brief Sample @p index, mutable.
 *
 * @p index is not checked. It must be below ::LH_INPUT_TOUCH_MAX.
 */
lh_input_sample_t *
lh_input_touch_get_sample_mut(lh_input_touch_t *self, lh_u8_t index);

/**
 * @brief No fingers down. @p device_kind is touch or pen. @p device is the id
 *        written on each event.
 */
lh_void
lh_input_touch_init(lh_input_touch_t *self, lh_input_device_kind_t device_kind, lh_u8_t device);

/**
 * @brief One pointer event from this source: @p kind at (@p x, @p y).
 *
 * Writes the device id and @p buttons. A null @p sink delivers nothing.
 */
lh_void
lh_input_touch_emit(lh_input_touch_t *self, lh_input_kind_t kind, lh_u8_t pointer,
                    lh_input_button_t button, lh_u32_t buttons, lh_input_coord_t x,
                    lh_input_coord_t y, lh_tick_t time, lh_input_sink_fn *sink,
                    lh_self_ptr sink_self);

/**
 * @brief Diff @p samples against the last frame and hand the events to @p sink.
 *
 * @p count contacts, or none when @p samples is null. A contact already down
 *         emits a move only when x or y changed. A null @p sink updates the
 *         frame and delivers nothing. @p samples lives only for the call.
 */
lh_void
lh_input_touch_report(lh_input_touch_t *self, const lh_input_sample_t *samples, lh_u8_t count,
                      lh_tick_t time, lh_input_sink_fn *sink, lh_self_ptr sink_self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_INPUT_TOUCH_H */
