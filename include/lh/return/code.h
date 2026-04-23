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

#include <lh/numeric/limits.h>

#ifndef LH_RETURN_CODE_T_SIZE
/**
 * @def LH_RETURN_CODE_T_SIZE
 * @brief Size of ::lh_return_code_t in bytes.
 *
 * Expands to ::LH_SINT_T_SIZE.
 */
#    define LH_RETURN_CODE_T_SIZE LH_SINT_T_SIZE
#endif // LH_RETURN_CODE_T_SIZE

#ifndef LH_RETURN_CODE_T_MIN
/**
 * @def LH_RETURN_CODE_T_MIN
 * @brief Minimum value of ::lh_return_code_t.
 *
 * Expands to ::LH_SINT_T_MIN.
 */
#    define LH_RETURN_CODE_T_MIN LH_SINT_T_MIN
#endif // LH_RETURN_CODE_T_MIN

#ifndef LH_RETURN_CODE_T_MAX
/**
 * @def LH_RETURN_CODE_T_MAX
 * @brief Maximum value of ::lh_return_code_t.
 *
 * Expands to ::LH_SINT_T_MAX.
 */
#    define LH_RETURN_CODE_T_MAX LH_SINT_T_MAX
#endif // LH_RETURN_CODE_T_MAX

/**
 * @typedef lh_return_code_t
 * @brief Signed integer type for function return statuses.
 *
 * Prefer this name in public API signatures
 * so return conventions stay explicit and consistent.
 */
typedef lh_sint_t lh_return_code_t;

#endif // LH_RETURN_CODE_H
