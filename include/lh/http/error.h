/**
 * @file error.h
 * @brief Syntax and budget diagnostics for HTTP message heads.
 *
 * One category per failed head. The byte offset of the offending input comes
 * from ::lh_http_parser_get_error_offset.
 */

#ifndef LH_HTTP_ERROR_H
#define LH_HTTP_ERROR_H

#include <lh/numeric/fixed/types.h>

/**
 * @brief Category of one failed HTTP head parse.
 */
typedef lh_u8_t lh_http_error_t;

/**
 * @def lh_http_error_none
 * @brief No error; also used for an incomplete prefix.
 */
#define lh_http_error_none 0U

/**
 * @def lh_http_error_argument
 * @brief Null output, invalid input/storage, or zero budget.
 */
#define lh_http_error_argument 1U

/**
 * @def lh_http_error_line_ending
 * @brief Bare LF or CR not followed by LF.
 */
#define lh_http_error_line_ending 2U

/**
 * @def lh_http_error_start_line
 * @brief Invalid request/status line, version or status code.
 */
#define lh_http_error_start_line 3U

/**
 * @def lh_http_error_header
 * @brief Invalid field name/value or obsolete folding.
 */
#define lh_http_error_header 4U

/**
 * @def lh_http_error_head_size
 * @brief Head budget exhausted before the final CRLF.
 */
#define lh_http_error_head_size 5U

/**
 * @def lh_http_error_line_size
 * @brief Physical-line text exceeds its budget.
 */
#define lh_http_error_line_size 6U

/**
 * @def lh_http_error_header_capacity
 * @brief More fields than the caller's storage can hold.
 */
#define lh_http_error_header_capacity 7U

#endif /* LH_HTTP_ERROR_H */
