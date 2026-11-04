/**
 * @file status.h
 * @brief Result of parsing one growing HTTP input buffer.
 */

#ifndef LH_HTTP_PARSE_STATUS_H
#define LH_HTTP_PARSE_STATUS_H

#include <lh/core/numeric/fixed/types.h>

/**
 * @brief Outcome of a growing HTTP head or chunked body parse.
 */
typedef lh_u8_t lh_http_parse_status_t;

/**
 * @def lh_http_parse_incomplete
 * @brief Append input and retry from its beginning; output is not yet usable.
 */
#define lh_http_parse_incomplete 0U

/**
 * @def lh_http_parse_complete
 * @brief Parsing completed; documented outputs are valid.
 */
#define lh_http_parse_complete 1U

/**
 * @def lh_http_parse_error
 * @brief Input is invalid; documented outputs are not usable.
 */
#define lh_http_parse_error 2U

#endif /* LH_HTTP_PARSE_STATUS_H */
