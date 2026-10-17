/**
 * @file flags.h
 * @brief Facts about one event: ::lh_os_input_flags_t.
 */

#ifndef LH_OS_INPUT_FLAGS_H
#define LH_OS_INPUT_FLAGS_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_os_input_flags_t
 * @brief A set of `LH_OS_INPUT_FLAG_*` bits.
 */
typedef lh_u8_t lh_os_input_flags_t;

/** @brief No flag. */
#define LH_OS_INPUT_FLAG_NONE 0U
/** @brief A key held down and repeating (::lh_os_input_kind_key_down). */
#define LH_OS_INPUT_FLAG_REPEAT (1U << 0)
/** @brief Made by ::lh_os_input_recognizer_t, not by a device. */
#define LH_OS_INPUT_FLAG_SYNTHESIZED (1U << 1)
/** @brief The main contact: the mouse, the first finger down. */
#define LH_OS_INPUT_FLAG_PRIMARY (1U << 2)
/** @brief A mouse event the system made up from a touch. */
#define LH_OS_INPUT_FLAG_EMULATED (1U << 3)
/** @brief The gesture ended because it was taken away, not finished. */
#define LH_OS_INPUT_FLAG_CANCELLED (1U << 4)

#endif /* LH_OS_INPUT_FLAGS_H */
