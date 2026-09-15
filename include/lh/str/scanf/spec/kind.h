/**
 * @file kind.h
 * @brief One token produced by ::lh_str_ptr_scanf_next from a format mask.
 */

#ifndef LH_STR_SCANF_SPEC_KIND_H
#define LH_STR_SCANF_SPEC_KIND_H

#include <lh/byte.h>

/**
 * @def lh_str_scanf_spec_kind_invalid
 * @brief Malformed mask (`%` with no conversion, unknown conversion, overflowed
 *        width/precision). Callers treat this as failure.
 */
#define lh_str_scanf_spec_kind_invalid 0

/**
 * @def lh_str_scanf_spec_kind_literal
 * @brief A run of ordinary text (no `%`). `literal` / `literal_size` hold it.
 */
#define lh_str_scanf_spec_kind_literal 1

/**
 * @def lh_str_scanf_spec_kind_percent
 * @brief `%%` — a literal `%`.
 */
#define lh_str_scanf_spec_kind_percent 2

/**
 * @def lh_str_scanf_spec_kind_char
 * @brief `%c`.
 */
#define lh_str_scanf_spec_kind_char 3

/**
 * @def lh_str_scanf_spec_kind_str
 * @brief `%s`.
 */
#define lh_str_scanf_spec_kind_str 4

/**
 * @def lh_str_scanf_spec_kind_uint
 * @brief `%u`.
 */
#define lh_str_scanf_spec_kind_uint 5

/**
 * @def lh_str_scanf_spec_kind_sint
 * @brief `%d` or `%i`.
 */
#define lh_str_scanf_spec_kind_sint 6

/**
 * @def lh_str_scanf_spec_kind_hex
 * @brief `%x` or `%X` (`uppercase` distinguishes them).
 */
#define lh_str_scanf_spec_kind_hex 7

/**
 * @typedef lh_str_scanf_spec_kind_t
 * @brief Format-mask token kind; one of the `lh_str_scanf_spec_kind_*` constants.
 */
typedef lh_byte_t lh_str_scanf_spec_kind_t;

#endif /* LH_STR_SCANF_SPEC_KIND_H */
