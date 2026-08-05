/**
 * @file char.h
 * @brief Portable and self-documenting character type definitions.
 *
 * Provides typedefs for plain and explicitly signed character types,
 * using the `lh_` prefix for consistent naming.
 */

#ifndef LH_CHAR_H
#define LH_CHAR_H

/**
 * @typedef lh_char_t
 * @brief Plain character type (`char`) with implementation-defined signedness.
 *
 * Use when semantic compatibility with C string literals (`const char *`) or
 * libc text APIs is required.
 * Signedness is target/compiler-dependent.
 *
 * Alias for: `char`
 */
typedef char lh_char_t;

/**
 * @typedef lh_uchar_t
 * @brief 8-bit unsigned integer (exactly 1 byte).
 *
 * Always unsigned, range: 0 to 255.
 * Suitable for raw bytes, binary data, and unsigned 8-bit arithmetic.
 *
 * Alias for: `unsigned char`
 */
typedef unsigned char lh_uchar_t;

/**
 * @typedef lh_schar_t
 * @brief 8-bit signed integer (exactly 1 byte).
 *
 * Always signed, range: -128 to +127.
 * Use when explicit signed 8-bit behavior is required.
 *
 * Alias for: `signed char`
 */
typedef signed char lh_schar_t;

#endif /* LH_CHAR_H */
