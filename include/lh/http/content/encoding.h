/**
 * @file encoding.h
 * @brief Validate HTTP request content coding supported by the web host.
 *
 * The helpers inspect caller-owned fields and do not decode or own data. A
 * missing coding or identity needs no transform; gzip requests decompression.
 * Repeated fields, lists and other codings are rejected.
 */

#ifndef LH_HTTP_CONTENT_ENCODING_H
#define LH_HTTP_CONTENT_ENCODING_H

#include <lh/http/header.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Determine whether parsed content coding is gzip.
 * @param headers Borrowed parsed fields; null is allowed for zero count.
 * @param count Number of parsed fields.
 * @param out Receives true for gzip, false for absent or identity coding.
 * @return True for a supported coding; false for repeated, unknown, or list
 *         coding. Output is unchanged on failure; no data is retained.
 * @pre Out is valid; nonzero count has that many valid parsed fields.
 */
lh_bool_t
lh_http_content_encoding_try_is_gzip(const lh_http_header_t *headers, lh_usize_t count,
                                     lh_bool_t *out);

/** @brief Checked form of ::lh_http_content_encoding_try_is_gzip. */
lh_bool_t
lh_http_content_encoding_is_gzip(const lh_http_header_t *headers, lh_usize_t count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_CONTENT_ENCODING_H */
