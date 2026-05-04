#ifndef LH_ERROR_CODE_LIMITS_H
#define LH_ERROR_CODE_LIMITS_H

#include <lh/return/code/limits.h>

#ifndef LH_ERROR_CODE_T_SIZE
/**
 * @def LH_ERROR_CODE_T_SIZE
 * @brief Size of ::lh_error_code_t in bytes.
 *
 * Expands to ::LH_RETURN_CODE_T_SIZE.
 */
#    define LH_ERROR_CODE_T_SIZE LH_RETURN_CODE_T_SIZE
#endif // LH_ERROR_CODE_T_SIZE

#ifndef LH_ERROR_CODE_T_MIN
/**
 * @def LH_ERROR_CODE_T_MIN
 * @brief Minimum value of ::lh_error_code_t.
 *
 * Expands to ::LH_RETURN_CODE_T_MIN.
 */
#    define LH_ERROR_CODE_T_MIN LH_RETURN_CODE_T_MIN
#endif // LH_ERROR_CODE_T_MIN

#ifndef LH_ERROR_CODE_T_MAX
/**
 * @def LH_ERROR_CODE_T_MAX
 * @brief Maximum value of ::lh_error_code_t.
 *
 * Expands to ::LH_RETURN_CODE_T_MAX.
 */
#    define LH_ERROR_CODE_T_MAX LH_RETURN_CODE_T_MAX
#endif // LH_ERROR_CODE_T_MAX

#endif // LH_ERROR_CODE_LIMITS_H