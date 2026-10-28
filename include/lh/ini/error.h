/**
 * @file error.h
 * @brief Why an INI text could not be read (::lh_ini_error_t).
 */

#ifndef LH_INI_ERROR_H
#define LH_INI_ERROR_H

#include <lh/compiler/extern/c.h>
#include <lh/str/ptr.h>

/**
 * @typedef lh_ini_error_t
 * @brief Syntax error found by ::lh_ini_parser_next. Alias for ::lh_u8_t;
 *        values are the `lh_ini_error_*` constants below.
 */
typedef lh_u8_t lh_ini_error_t;

/**
 * @def lh_ini_error_none
 * @brief No error.
 */
#define lh_ini_error_none 0U

/**
 * @def lh_ini_error_bad_section
 * @brief A line starts with `[` but is not `[name]`: no closing `]`, text
 *        after it, or a bracket inside the name.
 */
#define lh_ini_error_bad_section 1U

/**
 * @def lh_ini_error_empty_section
 * @brief `[]`: a section header without a name.
 */
#define lh_ini_error_empty_section 2U

/**
 * @def lh_ini_error_missing_equals
 * @brief A line that is neither a header, a comment, nor `key = value`.
 */
#define lh_ini_error_missing_equals 3U

/**
 * @def lh_ini_error_empty_key
 * @brief `= value`: a pair without a key.
 */
#define lh_ini_error_empty_key 4U

/**
 * @def lh_ini_error_no_section
 * @brief A pair before the first section header.
 */
#define lh_ini_error_no_section 5U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief A short English description of @p error, for a log line.
 *
 * @return A NUL-terminated static string; `"unknown error"` for a value
 *         that is not one of the constants above.
 */
const lh_str_ptr
lh_ini_error_get_message(lh_ini_error_t error);

LH_COMPILER_EXTERN_C_END

#endif /* LH_INI_ERROR_H */
