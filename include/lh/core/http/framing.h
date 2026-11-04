/**
 * @file framing.h
 * @brief Determine the length of a supported HTTP request body.
 *
 * The helpers inspect caller-owned parsed fields. They identify a single
 * chunked transfer coding or determine one decimal Content-Length, rejecting
 * ambiguous framing. They neither own input nor consume bytes.
 */

#ifndef LH_HTTP_FRAMING_H
#define LH_HTTP_FRAMING_H

#include <lh/core/http/header.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Identify supported request transfer coding chains.
 *
 * Accepts no Transfer-Encoding, one chunked coding, or gzip followed by
 * chunked. Repeated fields, empty list members, other chains, and any
 * Transfer-Encoding combined with Content-Length are rejected. Tokens are
 * compared case-insensitively and surrounding whitespace is ignored.
 * @param headers Borrowed parsed fields; null is allowed for zero count.
 * @param count Number of fields.
 * @param chunked Receives whether chunk framing is present.
 * @param gzip Receives whether gzip precedes chunked.
 * @return True for a supported chain; false otherwise. Outputs remain
 *         unchanged on failure; no data is retained.
 * @pre Outputs are valid and distinct; nonzero count has valid parsed fields.
 */
lh_bool_t
lh_http_framing_try_get_transfer(const lh_http_header_t *headers, lh_usize_t count,
                                 lh_bool_t *chunked, lh_bool_t *gzip);

/** @brief Checked form of ::lh_http_framing_try_get_transfer. */
lh_void
lh_http_framing_get_transfer(const lh_http_header_t *headers, lh_usize_t count, lh_bool_t *chunked,
                             lh_bool_t *gzip);

/**
 * @brief Determine whether the request uses the supported chunked coding.
 *
 * A single `chunked` coding or `gzip, chunked` without Content-Length is
 * supported. Repeated fields, other chains, and Transfer-Encoding combined
 * with Content-Length are rejected. If there is no
 * Transfer-Encoding, the caller must validate Content-Length separately.
 *
 * @param headers Caller-owned fields; null is allowed when @p count is zero.
 * @param count Number of fields.
 * @param out Receives true for chunked or false for no Transfer-Encoding;
 *            unchanged if validation fails.
 * @return True when the transfer coding is supported, false otherwise.
 * @pre @p out is valid; nonzero @p count has that many valid fields.
 */
lh_bool_t
lh_http_framing_try_is_chunked(const lh_http_header_t *headers, lh_usize_t count, lh_bool_t *out);

/**
 * @brief Check the transfer coding and report whether it is chunked.
 *
 * Delegates to ::lh_http_framing_try_is_chunked; field storage remains owned
 * by the caller. No Transfer-Encoding returns false.
 * @param headers Caller-owned fields, or null when @p count is zero.
 * @param count Number of fields.
 * @return True when a supported chain ends in chunked, false if absent.
 * @pre A nonzero count has that many valid fields.
 * @fails ::lh_runtime_error_code_invalid_range The coding is invalid or unsupported.
 */
lh_bool_t
lh_http_framing_is_chunked(const lh_http_header_t *headers, lh_usize_t count);

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
