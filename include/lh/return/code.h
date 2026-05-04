/**
 * @file lh/return/code.h
 * @brief Function return status type.
 *
 * Defines `lh_return_code_t` as the conventional type for values returned
 * by library functions to signal success, failure, or other outcomes.
 * It is a typedef of ::lh_sint_t and exposes corresponding size/range macros:
 * ::LH_RETURN_CODE_T_SIZE, ::LH_RETURN_CODE_T_MIN, ::LH_RETURN_CODE_T_MAX.
 */

#ifndef LH_RETURN_CODE_H
#define LH_RETURN_CODE_H

#include <lh/numeric/types.h>

/**
 * @typedef lh_return_code_t
 * @brief Signed integer type for function return statuses.
 *
 * Prefer this name in public API signatures
 * so return conventions stay explicit and consistent.
 */
typedef lh_int_t lh_return_code_t;

#endif // LH_RETURN_CODE_H
