/**
 * @file framing.h
 * @brief Determine the length of a supported HTTP request body.
 *
 * The helpers inspect caller-owned parsed fields. They support one decimal
 * Content-Length or no body, and deliberately reject Transfer-Encoding and
 * repeated Content-Length fields. They neither own input nor consume bytes.
 */

#ifndef LH_HTTP_FRAMING_H
#define LH_HTTP_FRAMING_H

#include <lh/http/header.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Determine a request body length from its parsed fields.
 *
 * Field names are compared without ASCII case. A missing Content-Length means
 * zero. Transfer-Encoding, repeated Content-Length, non-decimal values and
 * values outside ::lh_usize_t are rejected. This deliberately rejects even
 * repeated identical lengths and comma-separated lists.
 *
 * @param headers Caller-owned parsed fields; null is allowed when count is 0.
 * @param count Number of fields.
 * @param out Receives the body length on success; unchanged on failure.
 * @return ::lh_bool_true when the framing is supported, ::lh_bool_false for
 *         invalid, ambiguous, unsupported or unrepresentable framing.
 * @pre @p out is valid and a nonzero count has that many valid fields.
 */
lh_bool_t
lh_http_framing_try_get_content_length(const lh_http_header_t *headers, lh_usize_t count,
                                       lh_usize_t *out);

/**
 * @brief Determine a request body length, checking that framing is supported.
 *
 * Delegates to ::lh_http_framing_try_get_content_length. Inputs remain owned
 * by the caller and are neither copied nor modified.
 *
 * @param headers Caller-owned parsed fields, or null when count is zero.
 * @param count Number of fields.
 * @return The body length in bytes; zero when Content-Length is absent.
 * @pre A nonzero count has that many valid fields.
 * @fails ::lh_runtime_error_code_invalid_range Framing is invalid or unsupported.
 */
lh_usize_t
lh_http_framing_get_content_length(const lh_http_header_t *headers, lh_usize_t count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_FRAMING_H */
