/**
 * @file code.h
 * @brief OS-layer error code alias.
 *
 * Same representation as ::lh_error_code_t. Kept as its own type so OS-layer
 * APIs read as OS-layer in their signatures, without implying they share a
 * code space with unrelated modules.
 */

#ifndef LH_OS_ERROR_CODE_H
#define LH_OS_ERROR_CODE_H

#include <lh/error/code.h>

/**
 * @def lh_os_error_code_ok
 * @brief OS alias for the non-error sentinel (::lh_error_code_ok).
 */
#define lh_os_error_code_ok lh_error_code_ok

/**
 * @typedef lh_os_error_code_t
 * @brief Same representation as ::lh_error_code_t for OS-layer error codes.
 */
typedef lh_error_code_t lh_os_error_code_t;

#endif /* LH_OS_ERROR_CODE_H */
