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
 * HTTP/1 minor versions are single digits and are parsed; the application decides
 * which ones to use.
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

/**
 * @struct lh_http_parser
 * @brief Budgets and last-call diagnostics of one parser.
 */
struct lh_http_parser
{
    /** Members: ::lh_http_parser_fields. */
    lh_http_parser_fields(lh_usize_t, lh_http_error_t);
};
/** @copybrief lh_http_parser */
typedef struct lh_http_parser lh_http_parser_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Initialize limits and clear diagnostics.
 *
 * Sets the default budgets: 8192 head bytes including the final CRLF, and 4096
 * bytes per physical line excluding it. No owned resources; no deinit needed.
 *
 * @param self Parser to initialize; null is ignored.
 */
void
lh_http_parser_init(lh_http_parser_t *self);

/**
 * @brief Set the head and line budgets.
 *
 * Both values are checked on the next parse; a zero budget is invalid and makes
 * that parse fail with ::lh_http_error_argument.
 *
 * @param self          Parser to change; null is ignored.
 * @param head_size_max Maximum head bytes, including the final CRLF.
 * @param line_size_max Maximum text bytes of one physical line, excluding CRLF.
 */
void
lh_http_parser_set_limits(lh_http_parser_t *self, lh_usize_t head_size_max,
                          lh_usize_t line_size_max);

/**
 * @brief Bytes through the final CRLF of the last complete parse.
 *
 * @param self Parser to read from.
 * @return Consumed byte count, or zero when no head has been parsed.
 */
lh_usize_t
lh_http_parser_get_consumed(const lh_http_parser_t *self);

/**
 * @brief Category of the last parse.
 *
 * @param self Parser to read from.
 * @return ::lh_http_error_none when the last parse did not fail, the category
 *         of the failure otherwise, or ::lh_http_error_argument for a null
 *         parser.
 */
lh_http_error_t
lh_http_parser_get_error(const lh_http_parser_t *self);

/**
 * @brief Byte offset of the offending input in the last failed parse.
 *
 * @param self Parser to read from.
 * @return The offset into the input, or zero when no error is present.
 */
lh_usize_t
lh_http_parser_get_error_offset(const lh_http_parser_t *self);

/**
 * @brief Parse a request head from the beginning of @p input.
 *
 * The whole head is parsed again on every call, so an incomplete input can
 * become valid after more bytes are appended. The parsed views point into
 * @p input: keep that buffer unchanged while they are in use, and inspect the
 * outputs only on ::lh_http_parse_complete.
 *
 * A null parser or output, an input view with a size but no text, header
 * capacity without storage, or a zero budget is an error
 * (::lh_http_error_argument).
 *
 * @param self            Parser to use.
 * @param input           Bytes to parse; an empty view is incomplete.
 * @param request         Receives the parsed request line.
 * @param headers         Caller-owned storage for the parsed fields.
 * @param header_capacity Number of entries @p headers can hold; zero with a
 *                        null @p headers.
 * @return ::lh_http_parse_complete, ::lh_http_parse_incomplete or
 *         ::lh_http_parse_error.
 */
lh_http_parse_status_t
lh_http_parser_request(lh_http_parser_t *self, lh_str_view_t input, lh_http_request_t *request,
                       lh_http_header_t *headers, lh_usize_t header_capacity);

/**
 * @brief Parse a response head from the beginning of @p input.
 *
 * The whole head is parsed again on every call, so an incomplete input can
 * become valid after more bytes are appended. The parsed views point into
 * @p input: keep that buffer unchanged while they are in use, and inspect the
 * outputs only on ::lh_http_parse_complete.
 *
 * The status code must be 100..599; the reason phrase may be empty. A null
 * parser or output, an input view with a size but no text, header capacity
 * without storage, or a zero budget is an error (:lh_http_error_argument).
 *
 * @param self            Parser to use.
 * @param input           Bytes to parse; an empty view is incomplete.
 * @param response        Receives the parsed status line.
 * @param headers         Caller-owned storage for the parsed fields.
 * @param header_capacity Number of entries @p headers can hold; zero with a
 *                        null @p headers.
 * @return ::lh_http_parse_complete, ::lh_http_parse_incomplete or
 *         ::lh_http_parse_error.
 */
lh_http_parse_status_t
lh_http_parser_response(lh_http_parser_t *self, lh_str_view_t input, lh_http_response_t *response,
                        lh_http_header_t *headers, lh_usize_t header_capacity);

LH_COMPILER_EXTERN_C_END
#endif /* LH_HTTP_PARSER_H */
