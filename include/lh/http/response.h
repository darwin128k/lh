/**
 * @file response.h
 * @brief HTTP/1 status-line views and parsed header count.
 */

#ifndef LH_HTTP_RESPONSE_H
#define LH_HTTP_RESPONSE_H

#include <lh/str/view.h>
#include <lh/http/response/fields.h>

/**
 * @struct lh_http_response
 * @brief Parsed status line: reason, status code, version and field count.
 *
 * The views point into the caller's input buffer and expire with it.
 */
struct lh_http_response
{
    /** Members: ::lh_http_response_fields. */
    lh_http_response_fields(lh_str_view_t, lh_u16_t, lh_u8_t, lh_usize_t);
};
/** @copybrief lh_http_response */
typedef struct lh_http_response lh_http_response_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The reason phrase; may be empty.
 * The view points into the parsed input and lives as long as that buffer.
 */
const lh_str_view_t *
lh_http_response_get_reason(const lh_http_response_t *self);

/**
 * @brief Store the reason phrase; may be empty.
 *
 * Level-1 setter: no validation; the parser fills it.
 */
lh_void
lh_http_response_set_reason(lh_http_response_t *self, lh_str_view_t reason);

/** @brief The three-digit status code, 100..599. */
lh_u16_t
lh_http_response_get_status(const lh_http_response_t *self);

/**
 * @brief Store the three-digit status code, 100..599.
 *
 * Level-1 setter: no validation; the parser fills it.
 */
lh_void
lh_http_response_set_status(lh_http_response_t *self, lh_u16_t status);

/** @brief The minor digit of `HTTP/1.x` (0..9). */
lh_u8_t
lh_http_response_get_version_minor(const lh_http_response_t *self);

/**
 * @brief Store the minor digit of `HTTP/1.x` (0..9).
 *
 * Level-1 setter: no validation; the parser fills it.
 */
lh_void
lh_http_response_set_version_minor(lh_http_response_t *self, lh_u8_t version_minor);

/** @brief Number of header fields the parse stored in the caller's array. */
lh_usize_t
lh_http_response_get_header_count(const lh_http_response_t *self);

/**
 * @brief Store number of header fields the parse stored in the caller's array.
 *
 * Level-1 setter: no validation; the parser fills it.
 */
lh_void
lh_http_response_set_header_count(lh_http_response_t *self, lh_usize_t header_count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_RESPONSE_H */
