/**
 * @file error.h
 * @brief Why a JSON text could not be read (::lh_json_error_t).
 */

#ifndef LH_JSON_ERROR_H
#define LH_JSON_ERROR_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/str/ptr.h>

/**
 * @typedef lh_json_error_t
 * @brief Syntax error found by ::lh_json_reader_read. Alias for ::lh_u8_t;
 *        values are the `lh_json_error_*` constants below.
 */
typedef lh_u8_t lh_json_error_t;

/**
 * @def lh_json_error_none
 * @brief No error.
 */
#define lh_json_error_none 0U

/**
 * @def lh_json_error_unexpected_end
 * @brief The text ends inside a value, or holds no value at all.
 */
#define lh_json_error_unexpected_end 1U

/**
 * @def lh_json_error_unexpected_char
 * @brief A character no value can start with, or a missing `,`, `:`, `]`,
 *        `}`, or a key that is not a string.
 */
#define lh_json_error_unexpected_char 2U

/**
 * @def lh_json_error_bad_literal
 * @brief A word that is not `null`, `true` or `false`.
 */
#define lh_json_error_bad_literal 3U

/**
 * @def lh_json_error_bad_number
 * @brief A number that breaks the grammar: `01`, `1.`, `.5`, `1e`, `-`.
 */
#define lh_json_error_bad_number 4U

/**
 * @def lh_json_error_bad_string
 * @brief A control character (below U+0020) written raw inside a string.
 */
#define lh_json_error_bad_string 5U

/**
 * @def lh_json_error_bad_escape
 * @brief A backslash followed by something other than `"\/bfnrtu`.
 */
#define lh_json_error_bad_escape 6U

/**
 * @def lh_json_error_bad_unicode
 * @brief A `\u` escape without four hex digits, a lone surrogate, or
 *        `\u0000` (strings here end at a NUL).
 */
#define lh_json_error_bad_unicode 7U

/**
 * @def lh_json_error_too_deep
 * @brief Arrays and objects nested deeper than ::LH_JSON_READER_DEPTH_MAX.
 */
#define lh_json_error_too_deep 8U

/**
 * @def lh_json_error_trailing
 * @brief Something other than whitespace after the value.
 */
#define lh_json_error_trailing 9U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief A short English description of @p error, for a log line.
 *
 * @return A NUL-terminated static string; `"unknown error"` for a value
 *         that is not one of the constants above.
 */
const lh_str_ptr
lh_json_error_get_message(lh_json_error_t error);

LH_COMPILER_EXTERN_C_END

#endif /* LH_JSON_ERROR_H */
