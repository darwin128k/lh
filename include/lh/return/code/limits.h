/**
 * @file limits.h
 * @brief Size and range limits for ::lh_return_code_t.
 *
 * Provides macros that expand to the limits of the underlying
 * integer type used for return codes.
 *
 * @see lh_error_code_t
 * @see lh_error_code_limits.h
 */

#ifndef LH_RUNTIME_CODE_LIMITS_H
#define LH_RUNTIME_CODE_LIMITS_H

#include <lh/numeric/limits.h>

#ifndef LH_RETURN_CODE_T_SIZE
/**
 * @def LH_RETURN_CODE_T_SIZE
 * @brief Size of ::lh_return_code_t in bytes.
 *
 * Expands to ::LH_INT_T_SIZE.
 */
#    define LH_RETURN_CODE_T_SIZE LH_INT_T_SIZE
#endif // LH_RETURN_CODE_T_SIZE

#ifndef LH_RETURN_CODE_T_MIN
/**
 * @def LH_RETURN_CODE_T_MIN
 * @brief Minimum value of ::lh_return_code_t.
 *
 * Expands to ::LH_INT_T_MIN.
 */
#    define LH_RETURN_CODE_T_MIN LH_INT_T_MIN
#endif // LH_RETURN_CODE_T_MIN

#ifndef LH_RETURN_CODE_T_MAX
/**
 * @def LH_RETURN_CODE_T_MAX
 * @brief Maximum value of ::lh_return_code_t.
 *
 * Expands to ::LH_INT_T_MAX.
 */
#    define LH_RETURN_CODE_T_MAX LH_INT_T_MAX
#endif // LH_RETURN_CODE_T_MAX

#endif // LH_RUNTIME_CODE_LIMITS_H