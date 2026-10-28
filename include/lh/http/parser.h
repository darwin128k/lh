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

#include <lh/bool.h>
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
lh_void
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
lh_void
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
 * @brief Record a failure at @p offset and report it.
 *
 * Every error of the parser is raised through this brick, so the diagnostics
 * read by ::lh_http_parser_get_error and ::lh_http_parser_get_error_offset
 * always come from one place.
 *
 * @param self Parser to record in; must not be null.
 * @param error Category of the failure.
 * @param offset Byte offset of the offending input.
 * @return ::lh_http_parse_error.
 */
lh_http_parse_status_t
lh_http_parser_fail(lh_http_parser_t *self, lh_http_error_t error, lh_usize_t offset);

/**
 * @brief Clear the diagnostics of @p self and check the arguments of a parse.
 *
 * @param self Parser that starts a parse; null fails.
 * @param input Bytes to parse.
 * @param output Request or response the parse fills.
 * @param headers Field storage.
 * @param capacity Number of entries @p headers holds.
 * @return ::lh_bool_false (with ::lh_http_error_argument recorded when @p self
 *         is not null) for a null @p output, an input with a size but no text,
 *         a capacity without storage or a zero budget.
 */
lh_bool_t
lh_http_parser_begin(lh_http_parser_t *self, lh_str_view_t input, const lh_void *output,
                     const lh_http_header_t *headers, lh_usize_t capacity);

/**
 * @brief Find the CRLF that ends the physical line starting at @p start.
 *
 * Looks no further than the input or the head budget, whichever is shorter.
 *
 * @param self Parser whose budgets apply.
 * @param input Bytes being parsed.
 * @param start Offset of the first byte of the line.
 * @param end Receives the offset of the line's CR on ::lh_http_parse_complete.
 * @return ::lh_http_parse_complete; ::lh_http_parse_incomplete when the input
 *         ends first; ::lh_http_parse_error for a bare LF or CR
 *         (::lh_http_error_line_ending), a line over its budget
 *         (::lh_http_error_line_size) or a head over its budget
 *         (::lh_http_error_head_size).
 */
lh_http_parse_status_t
lh_http_parser_find_line(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t start,
                         lh_usize_t *end);

/**
 * @brief Parse the request line `method SP target SP HTTP/1.x` in `[0, end)`.
 *
 * @param self Parser to record a failure in.
 * @param input Bytes being parsed; the line starts at offset 0.
 * @param end Offset of the line's CR (::lh_http_parser_find_line).
 * @param request Receives method, target and minor version.
 * @return ::lh_http_parse_complete, or ::lh_http_parse_error with
 *         ::lh_http_error_start_line.
 */
lh_http_parse_status_t
lh_http_parser_parse_request_line(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t end,
                                  lh_http_request_t *request);

/**
 * @brief Parse the status line `HTTP/1.x SP code SP reason` in `[0, end)`.
 *
 * The code must be 100..599; the reason phrase may be empty.
 *
 * @param self Parser to record a failure in.
 * @param input Bytes being parsed; the line starts at offset 0.
 * @param end Offset of the line's CR (::lh_http_parser_find_line).
 * @param response Receives minor version, status code and reason.
 * @return ::lh_http_parse_complete, or ::lh_http_parse_error with
 *         ::lh_http_error_start_line.
 */
lh_http_parse_status_t
lh_http_parser_parse_status_line(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t end,
                                 lh_http_response_t *response);

/**
 * @brief Parse one field line `name: value` in `[begin, end)`.
 *
 * The name is a token; the value loses its surrounding spaces and tabs.
 *
 * @param self Parser to record a failure in.
 * @param input Bytes being parsed.
 * @param begin Offset of the first byte of the line.
 * @param end Offset of the line's CR.
 * @param header Receives the name and value views.
 * @return ::lh_http_parse_complete, or ::lh_http_parse_error with
 *         ::lh_http_error_header.
 */
lh_http_parse_status_t
lh_http_parser_parse_field(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t begin,
                           lh_usize_t end, lh_http_header_t *header);

/**
 * @brief Parse field lines from @p offset up to and including the empty line.
 *
 * Composed of ::lh_http_parser_find_line and ::lh_http_parser_parse_field. On
 * success records the consumed byte count (::lh_http_parser_get_consumed).
 *
 * @param self Parser to use.
 * @param input Bytes being parsed.
 * @param offset Offset of the first field line.
 * @param headers Caller-owned storage for the fields.
 * @param capacity Number of entries @p headers holds.
 * @param count Receives the number of fields on ::lh_http_parse_complete.
 * @return ::lh_http_parse_complete, ::lh_http_parse_incomplete, or
 *         ::lh_http_parse_error (::lh_http_error_header_capacity when the
 *         storage is full, or a failure of the bricks above).
 */
lh_http_parse_status_t
lh_http_parser_parse_fields(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t offset,
                            lh_http_header_t *headers, lh_usize_t capacity, lh_usize_t *count);

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
 * without storage, or a zero budget is an error (::lh_http_error_argument).
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
