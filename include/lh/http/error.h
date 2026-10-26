/** @file error.h @brief Syntax/budget diagnostics for HTTP message heads. */
#ifndef LH_HTTP_ERROR_H
#define LH_HTTP_ERROR_H

#include <lh/numeric/fixed/types.h>

typedef lh_u8_t lh_http_error_t;
/** No error; also used for an incomplete prefix. */
#define lh_http_error_none 0U
/** Null output, invalid input/storage, or zero budget. */
#define lh_http_error_argument 1U
/** Bare LF or CR not followed by LF. */
#define lh_http_error_line_ending 2U
/** Invalid request/status line, version or status code. */
#define lh_http_error_start_line 3U
/** Invalid field name/value or obsolete folding. */
#define lh_http_error_header 4U
/** Head budget exhausted before the final CRLF. */
#define lh_http_error_head_size 5U
/** Physical-line text exceeds its budget. */
#define lh_http_error_line_size 6U
/** More fields than the caller's storage can hold. */
#define lh_http_error_header_capacity 7U

#endif
