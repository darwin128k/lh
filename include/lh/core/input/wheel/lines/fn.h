/**
 * @file fn.h
 * @brief Function type ::lh_input_wheel_lines_fn: how many lines one wheel notch
 *        scrolls, as whoever is asked is set up now.
 *
 * The contract between a consumer of wheel notches and the place their size is
 * kept. A consumer (::lh_gl_view_t, through ::lh_gl_view_install_wheel_lines)
 * holds one pointer of this type and the context beside it, and calls it once a
 * wheel event; it does not know who answers. The operating system answers with
 * ::lh_os_input_get_wheel_lines (Windows asks its Mouse settings, Linux and macOS
 * answer the default), a board with a setting of its own, a test with what it
 * wants to see. A consumer without one uses its own default.
 *
 * Lines, not content units: the conversion is the core's,
 * ::lh_input_wheel_step_of, the same for every answer.
 *
 * Not a pointer type by itself: the holder stores `lh_input_wheel_lines_fn *`.
 */

#ifndef LH_INPUT_WHEEL_LINES_FN_H
#define LH_INPUT_WHEEL_LINES_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_input_wheel_lines_fn
 * @brief Function type `lh_u32_t(lh_ptr context)`: the lines one notch
 *        (::LH_INPUT_WHEEL_NOTCH) scrolls now.
 *
 * Asked on every wheel event, never cached by the caller, so a change of the
 * setting applies from the next notch.
 *
 * @param context The context installed beside the pointer, as it was given.
 * @return Lines a notch: ::LH_INPUT_WHEEL_LINES_DEFAULT (3) at the usual
 *         setting, ::LH_INPUT_WHEEL_LINES_PAGE for "one screen at a time", `0`
 *         when wheel scrolling is turned off. A value above
 *         ::LH_INPUT_WHEEL_LINES_MAX is read as the maximum by
 *         ::lh_input_wheel_step_of.
 */
typedef lh_u32_t(lh_input_wheel_lines_fn)(lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_INPUT_WHEEL_LINES_FN_H */
