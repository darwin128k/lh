/**
 * @file code.h
 * @brief Error code value type.
 *
 * Defines `lh_error_code_t` for numeric error identifiers
 * (e.g. errno-style or library-specific codes).
 *
 * It is a typedef of ::lh_return_code_t and thus shares
 * the same representation as general function return statuses;
 * range and size aliases are provided as:
 *
 * ::LH_ERROR_CODE_T_SIZE, ::LH_ERROR_CODE_T_MIN, ::LH_ERROR_CODE_T_MAX.
 *
 * the separate name documents intent where a value specifically
 * denotes an error kind, not an arbitrary return result.
 */

#ifndef LH_ERROR_CODE_H
#define LH_ERROR_CODE_H

#include <lh/return/code.h>
#include <lh/error/code/limits.h>

/**
 * @def lh_error_code_ok
 * @brief Reserved code meaning the operation succeeded or no error occurred.
 *
 * This is the conventional sentinel @c 0.
 * Library APIs that return ::lh_error_code_t
 * (or ::lh_return_code_t used as an error channel)
 * should use this value for the non-error path unless
 * a richer success encoding is explicitly documented.
 *
 * Callers can test with an exact comparison (e.g. @c result == lh_error_code_ok).
 */
#define lh_error_code_ok LH_ERROR_CODE_T_MIN

/**
 * @typedef lh_error_code_t
 * @brief Type for discrete error code values.
 *
 * Use when the value names a specific error condition;
 * use ::lh_return_code_t when describing any function exit status.
 */
typedef lh_return_code_t lh_error_code_t;

#endif // LH_ERROR_CODE_H
