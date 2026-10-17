/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_ui_mask_t.
 */

#ifndef LH_UI_MASK_INITIALIZER_H
#define LH_UI_MASK_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_ui_mask_initializer(bits, width, height, row_bytes, bpp)
 * @brief Produces a brace-enclosed initializer for ::lh_ui_mask_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_ui_mask_fields.
 *
 * Nothing is checked; ::lh_ui_mask_init checks @p bpp and @p row_bytes.
 *
 * @param bits      Packed coverage bytes (not owned), or ::lh_null.
 * @param width     Width in pixels.
 * @param height    Height in pixels.
 * @param row_bytes Bytes from one row to the next.
 * @param bpp       Bits per pixel: 1, 2, 4 or 8.
 *
 * Example usage:
 * @code{.c}
 * static const lh_byte_t bits[2] = {0xF0, 0x0F};
 * const lh_ui_mask_t mask = lh_ui_mask_initializer(bits, 8, 2, 1, 1);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ui_mask_t
 * @see lh_ui_mask_init
 */
#define lh_ui_mask_initializer(bits, width, height, row_bytes, bpp)                                \
    lh_initializer(bits, width, height, row_bytes, bpp)

#endif /* LH_UI_MASK_INITIALIZER_H */
