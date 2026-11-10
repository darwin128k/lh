/**
 * @file length.h
 * @brief ::lh_str_scanf_spec_length_t — the length modifier of a conversion.
 *
 * Which C type the argument of `%d`, `%u` or `%x` has: `hh` a `char`, `h` a
 * `short`, none an `int`, `l` a `long`, `ll` and `j` a `long long`, `z` and `t` a
 * size. The formatter reads the argument as that type.
 */

#ifndef LH_STR_SCANF_SPEC_LENGTH_H
#define LH_STR_SCANF_SPEC_LENGTH_H

#include <lh/core/byte.h>

/** @brief No modifier: an `int`. */
#define lh_str_scanf_spec_length_none 0

/** @brief `hh`: a `char`, passed as an `int`. */
#define lh_str_scanf_spec_length_char 1

/** @brief `h`: a `short`, passed as an `int`. */
#define lh_str_scanf_spec_length_short 2

/** @brief `l`: a `long`. */
#define lh_str_scanf_spec_length_long 3

/** @brief `ll` or `j`: a `long long`. */
#define lh_str_scanf_spec_length_long_long 4

/** @brief `z` or `t`: a size (::lh_usize_t / ::lh_ssize_t). */
#define lh_str_scanf_spec_length_size 5

/**
 * @typedef lh_str_scanf_spec_length_t
 * @brief One of the `lh_str_scanf_spec_length_*` constants.
 */
typedef lh_byte_t lh_str_scanf_spec_length_t;

#endif /* LH_STR_SCANF_SPEC_LENGTH_H */
