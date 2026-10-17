/**
 * @file coord.h
 * @brief A pointer coordinate: ::lh_os_input_coord_t, client pixels in Q24.8.
 */

#ifndef LH_OS_INPUT_COORD_H
#define LH_OS_INPUT_COORD_H

#include <lh/numeric/fixed/types.h>
#include <lh/util/type.h>

/**
 * @typedef lh_os_input_coord_t
 * @brief Client-area pixels as signed Q24.8: 256 is one pixel.
 *
 * Sub-pixel positions (touch, pen, hi-res mice) without a floating-point
 * unit. Backends that only know whole pixels use
 * ::lh_os_input_coord_from_int.
 */
typedef lh_s32_t lh_os_input_coord_t;

/** @brief One pixel in ::lh_os_input_coord_t units. */
#define LH_OS_INPUT_COORD_ONE 256

/**
 * @def lh_os_input_coord_from_int(px)
 * @brief Whole pixels @p px as an ::lh_os_input_coord_t.
 *
 * Example usage:
 * @code{.c}
 * lh_os_input_coord_from_int(3);   // 768
 * lh_os_input_coord_from_int(-1);  // -256
 * @endcode
 */
#define lh_os_input_coord_from_int(px)                                                             \
    lh_type_cast(lh_os_input_coord_t, (px) * LH_OS_INPUT_COORD_ONE)

/**
 * @def lh_os_input_coord_to_int(coord)
 * @brief @p coord in whole pixels, the fraction cut towards zero.
 *
 * Example usage:
 * @code{.c}
 * lh_os_input_coord_to_int(768);   // 3
 * lh_os_input_coord_to_int(-384);  // -1
 * @endcode
 */
#define lh_os_input_coord_to_int(coord) lh_type_cast(int, (coord) / LH_OS_INPUT_COORD_ONE)

/** @brief One wheel notch in the `dx` / `dy` of a wheel event (as Win32 `WHEEL_DELTA`). */
#define LH_OS_INPUT_WHEEL_NOTCH 120

#endif /* LH_OS_INPUT_COORD_H */
