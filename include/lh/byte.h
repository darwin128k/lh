/**
 * @file byte.h
 * @brief Portable and self-documenting byte type definition.
 *
 * Provides typedef for the 8-bit unsigned integer type
 * with explicit signedness, using the `lh_` prefix for consistent naming.
 */

#ifndef LH_BYTE_H
#define LH_BYTE_H

#include <lh/numeric/fixed.h>

/**
 * @typedef lh_byte_t
 * @brief 8-bit unsigned integer (exactly 1 byte).
 *
 * Always unsigned, range: 0 to 255.
 * Suitable for raw bytes, binary data, and unsigned 8-bit arithmetic.
 *
 * Alias for: `unsigned char`
 */
typedef lh_u8_t lh_byte_t;

#endif // LH_BYTE_H