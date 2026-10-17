/**
 * @file mods.h
 * @brief Modifier keys held during an event: ::lh_os_input_mods_t.
 */

#ifndef LH_OS_INPUT_MODS_H
#define LH_OS_INPUT_MODS_H

#include <lh/bit/ops.h>
#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_os_input_mods_t
 * @brief A set of `LH_OS_INPUT_MODS_*` bits, carried by every event.
 */
typedef lh_u16_t lh_os_input_mods_t;

/** @brief No modifier held. */
#define LH_OS_INPUT_MODS_NONE 0U
/** @brief Either Shift key. */
#define LH_OS_INPUT_MODS_SHIFT lh_bit_mask(0)
/** @brief Either Control key. */
#define LH_OS_INPUT_MODS_CONTROL lh_bit_mask(1)
/** @brief Either Alt key. */
#define LH_OS_INPUT_MODS_ALT lh_bit_mask(2)
/** @brief Either Windows / Super / Command key. */
#define LH_OS_INPUT_MODS_META lh_bit_mask(3)
/** @brief Caps Lock is on. */
#define LH_OS_INPUT_MODS_CAPS_LOCK lh_bit_mask(4)
/** @brief Num Lock is on. */
#define LH_OS_INPUT_MODS_NUM_LOCK lh_bit_mask(5)

#endif /* LH_OS_INPUT_MODS_H */
