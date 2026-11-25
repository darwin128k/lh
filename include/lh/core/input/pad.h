/**
 * @file pad.h
 * @brief One polled stick or pad: ::lh_input_pad_t.
 *
 * A driver reads axes and buttons and reports the new values. An axis event
 * goes out only when the value changed. A button event goes out only when
 * that button changed. The range is the one on ::lh_input_axis_t: a stick
 * is -32768..32767, a trigger is 0..32767. A dead zone stays in the driver,
 * before the report.
 *
 * Needs no ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_INPUT_PAD_H
#define LH_INPUT_PAD_H

#include <lh/core/timer/tick.h>
#include <lh/core/input/button.h>
#include <lh/core/input/pad/fields.h>
#include <lh/core/input/pad/ptr.h>
#include <lh/core/input/sink/fn.h>

/**
 * @struct lh_input_pad
 * @brief The last axis values and buttons of one controller.
 */
struct lh_input_pad
{
    /** Members: ::lh_input_pad_fields. */
    lh_input_pad_fields(lh_s16_t);
};
/** @copybrief lh_input_pad */
typedef struct lh_input_pad lh_input_pad_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The device id written on events. */
lh_u8_t
lh_input_pad_get_device(const lh_input_pad_t *self);

/** @brief Store the device id. */
lh_void
lh_input_pad_set_device(lh_input_pad_t *self, lh_u8_t device);

/** @brief Buttons held after the last report. */
lh_u32_t
lh_input_pad_get_buttons(const lh_input_pad_t *self);

/** @brief Store the button mask. */
lh_void
lh_input_pad_set_buttons(lh_input_pad_t *self, lh_u32_t buttons);

/**
 * @brief The last value of @p axis.
 *
 * @p axis is not checked. It must be below ::LH_INPUT_AXIS_COUNT.
 */
lh_s16_t
lh_input_pad_get_value(const lh_input_pad_t *self, lh_input_axis_t axis);

/**
 * @brief Store the last value of @p axis.
 *
 * @p axis is not checked. It must be below ::LH_INPUT_AXIS_COUNT.
 */
lh_void
lh_input_pad_set_value(lh_input_pad_t *self, lh_input_axis_t axis, lh_s16_t value);

/** @brief No buttons, every axis at 0. @p device is written on each event. */
lh_void
lh_input_pad_init(lh_input_pad_t *self, lh_u8_t device);

/**
 * @brief Hand an axis event to @p sink when @p value differs from the last one.
 *
 * ::lh_input_axis_none and an axis at or past ::LH_INPUT_AXIS_COUNT do
 * nothing. A null @p sink still stores @p value.
 */
lh_void
lh_input_pad_report_axis(lh_input_pad_t *self, lh_input_axis_t axis, lh_s16_t value, lh_tick_t time,
                         lh_input_sink_fn *sink, lh_self_ptr sink_self);

/**
 * @brief Hand a button event to @p sink when @p down differs from the last one.
 *
 * ::lh_input_button_none does nothing. A null @p sink still stores the bit.
 */
lh_void
lh_input_pad_report_button(lh_input_pad_t *self, lh_input_button_t button, lh_bool_t down,
                           lh_tick_t time, lh_input_sink_fn *sink, lh_self_ptr sink_self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_INPUT_PAD_H */
