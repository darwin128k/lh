/**
 * @file error.h
 * @brief Why a request target does not name a safe relative path
 *        (::lh_http_target_error_t).
 *
 * Returned by ::lh_http_target_try_decode_path, ::lh_http_target_get_path_error
 * and ::lh_http_target_try_copy_path. ::lh_http_target_error_get_name gives each
 * one a short stable name for a log line or an access record.
 */

#ifndef LH_HTTP_TARGET_ERROR_H
#define LH_HTTP_TARGET_ERROR_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/str/ptr.h>

/**
 * @typedef lh_http_target_error_t
 * @brief Refusal of a request target as a file path. Alias for ::lh_u8_t; values
 *        are the `lh_http_target_error_*` constants below.
 */
typedef lh_u8_t lh_http_target_error_t;

/** @brief No error: the target names a safe relative path. */
#define lh_http_target_error_none 0U

/** @brief The path is not `/` followed by at least one character. */
#define lh_http_target_error_bad_target 1U

/** @brief A `%` not followed by two hexadecimal digits. */
#define lh_http_target_error_bad_encoding 2U

/** @brief `%2F` or `%00`: an encoded separator or NUL. */
#define lh_http_target_error_encoded_separator 3U

/** @brief A decoded byte no published name may hold (::lh_http_is_name_char). */
#define lh_http_target_error_bad_char 4U

/** @brief The decoded path does not fit the caller's buffer. */
#define lh_http_target_error_too_long 5U

/** @brief A `~` followed by a digit: an 8.3 short name such as `NAME~1`. */
#define lh_http_target_error_short_name 6U

/** @brief An empty component: `//`, or a trailing `/`. */
#define lh_http_target_error_empty_component 7U

/** @brief A component that starts with `.`: hidden, `.` or `..`. */
#define lh_http_target_error_hidden_or_dot 8U

/** @brief A component that ends in `.` or a space, which Windows would trim. */
#define lh_http_target_error_trailing_dot_or_space 9U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The short stable name of @p error: `"none"`, `"bad_target"`,
 *        `"bad_encoding"`, `"encoded_separator"`, `"bad_char"`, `"uri_too_long"`,
 *        `"short_name"`, `"empty_component"`, `"hidden_or_dot"`,
 *        `"trailing_dot_or_space"`.
 *
 * @return A NUL-terminated static string; `"unknown"` for a value that is not
 *         one of the constants above.
 */
const lh_str_ptr
lh_http_target_error_get_name(lh_http_target_error_t error);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_TARGET_ERROR_H */
