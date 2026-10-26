/**
 * @file parser.h
 * @brief Bounded, non-owning HTTP/1 request/response head parser.
 *
 * Parses a complete growing input buffer from its beginning on each call.
 * No OS, allocation, locale, global state, NUL terminator or SIMD is required.
 * Output views refer to the original bytes: retain that buffer unchanged while
 * using them. Only inspect outputs on ::lh_http_parse_complete. Header storage
 * belongs to the caller. Duplicate fields are preserved, not combined.
 *
 * Strict CRLF, token field names, no obsolete folding or whitespace before a
 * colon. OWS surrounding values is removed; value bytes >= 0x80 are preserved.
 * Request methods are arbitrary tokens; targets are nonempty visible ASCII.
 * HTTP/1.<digit> is parsed; the application decides which minor versions to use.
 * This parses the head only. Content-Length/Transfer-Encoding, body framing,
 * Host requirements, methods, URI interpretation and connection policy are the
 * caller's responsibility. Success reports the body/next-message offset.
 *
 * Init defaults: 8192 head bytes (including CRLF), 4096 bytes per physical line
 * (excluding CRLF). Set limits before parsing. Zero limits are invalid.
 * Incomplete input can become valid after appending bytes. An error requires
 * changing input/limits/storage, not blindly retrying the same bytes.
 */
#ifndef LH_HTTP_PARSER_H
#define LH_HTTP_PARSER_H

#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/view.h>
#include <lh/http/header.h>
#include <lh/http/request.h>
#include <lh/http/response.h>
#include <lh/http/error.h>
#include <lh/http/parse/status.h>
#include <lh/http/parser/fields.h>

struct lh_http_parser
{
    lh_http_parser_fields(lh_usize_t, lh_http_error_t);
};
typedef struct lh_http_parser lh_http_parser_t;

LH_COMPILER_EXTERN_C_BEGIN

/** Initialize limits and clear diagnostics. No owned resources; no deinit needed. */
void
lh_http_parser_init(lh_http_parser_t *self);

/** Set nonzero budgets. Values are checked on the next parse. */
void
lh_http_parser_set_limits(lh_http_parser_t *self, lh_usize_t head_size_max,
                          lh_usize_t line_size_max);

/** Bytes through the final CRLF on complete; zero otherwise. */
lh_usize_t
lh_http_parser_get_consumed(const lh_http_parser_t *self);

lh_http_error_t
lh_http_parser_get_error(const lh_http_parser_t *self);

/** Byte offset of the error in input; zero when no error is present. */
lh_usize_t
lh_http_parser_get_error_offset(const lh_http_parser_t *self);

/** Empty {null,0} input is incomplete. Null header storage requires capacity=0.
 * A null parser/output or inconsistent input view returns error.
 */
lh_http_parse_status_t
lh_http_parser_request(lh_http_parser_t *self, lh_str_view_t input, lh_http_request_t *request,
                       lh_http_header_t *headers, lh_usize_t header_capacity);

/** Status code must be 100..599; the reason phrase may be empty. */
lh_http_parse_status_t
lh_http_parser_response(lh_http_parser_t *self, lh_str_view_t input, lh_http_response_t *response,
                        lh_http_header_t *headers, lh_usize_t header_capacity);

LH_COMPILER_EXTERN_C_END
#endif
