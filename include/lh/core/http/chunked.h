/**
 * @file chunked.h
 * @brief Parse and decode HTTP/1 chunked transfer coding in caller storage.
 *
 * The parser reads a growing borrowed buffer without changing it. It validates
 * chunk sizes, extensions, data delimiters and trailer field syntax, and
 * reports the wire length separately from the decoded content length. The
 * decoder compacts complete content in place only after validation succeeds;
 * trailers are validated and discarded, never merged into the header fields.
 * All offsets and lengths are bytes, and all storage remains caller-owned.
 */

#ifndef LH_HTTP_CHUNKED_H
#define LH_HTTP_CHUNKED_H

#include <lh/core/http/parser.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Validate chunk extensions after the hexadecimal size.
 * @param text Borrowed extension text; empty is valid.
 * @return True for valid extension syntax, false for invalid syntax.
 * @pre The input is a valid view.
 */
lh_bool_t
lh_http_chunked_are_extensions_valid(lh_str_view_t text);

/**
 * @brief Parse a chunk-size line, excluding its CRLF.
 * @param line Borrowed line holding hex digits and optional extensions.
 * @param out Receives the size on success and is unchanged on failure.
 * @return True for a valid representable size and extensions, false otherwise.
 * @pre The view and @p out are valid.
 */
lh_bool_t
lh_http_chunked_try_parse_size(lh_str_view_t line, lh_usize_t *out);

/**
 * @brief Parse a chunk-size line and check that it is valid.
 *
 * Delegates to ::lh_http_chunked_try_parse_size. The line stays caller-owned.
 * @param line Valid borrowed view of a size line without CRLF.
 * @return The parsed size in bytes.
 * @fails ::lh_runtime_error_code_invalid_range The line is invalid or too large.
 */
lh_usize_t
lh_http_chunked_parse_size(lh_str_view_t line);

/**
 * @brief Parse one chunk from a growing buffer.
 *
 * The terminating zero chunk ends immediately after its size line; its
 * trailer section is parsed separately. Outputs are changed only on success.
 * @param input Borrowed wire bytes, potentially incomplete.
 * @param offset Offset of the chunk-size line, at most input size.
 * @param data_offset Receives the start of the chunk data.
 * @param data_size Receives its byte length, zero for the last chunk.
 * @param next Receives the offset immediately after the chunk.
 * @return Complete, incomplete, or error for invalid syntax or overflow.
 * @pre The input and all output pointers are valid.
 */
lh_http_parse_status_t
lh_http_chunked_parse_part(lh_str_view_t input, lh_usize_t offset, lh_usize_t *data_offset,
                           lh_usize_t *data_size, lh_usize_t *next);

/**
 * @brief Parse and validate a chunked trailer section without retaining it.
 *
 * Uses the HTTP field parser, discards fields and stops after the empty line.
 * The input is unchanged; @p next is changed only on completion.
 * @param input Borrowed wire bytes, potentially incomplete.
 * @param offset Offset of the first trailer field, at most input size.
 * @param next Receives the offset after the terminating empty line.
 * @return Complete, incomplete, or error for invalid field syntax.
 * @pre The input and @p next are valid.
 */
lh_http_parse_status_t
lh_http_chunked_parse_trailers(lh_str_view_t input, lh_usize_t offset, lh_usize_t *next);

/**
 * @brief Inspect a complete chunked body and its trailer section.
 *
 * Trailer field syntax is checked with the HTTP parser; fields are discarded.
 * Each call scans from the beginning of the input. Bytes after the final empty
 * trailer line are not consumed. Neither input
 * nor outputs are changed on incomplete or invalid input.
 * @param input Borrowed wire bytes, potentially incomplete.
 * @param consumed Receives the number of consumed wire bytes.
 * @param decoded Receives the total number of content bytes.
 * @return Complete, incomplete, or error on invalid syntax or overflow.
 * @pre The input and both output pointers are valid.
 */
lh_http_parse_status_t
lh_http_chunked_scan(lh_str_view_t input, lh_usize_t *consumed, lh_usize_t *decoded);

/**
 * @brief Validate and compact a chunked body at the start of its buffer.
 *
 * Scans before writing, then moves the chunk data in order using LH's memory
 * kernel. Trailers and chunk framing are discarded; bytes beyond @p consumed
 * are not interpreted. On incomplete or invalid input no bytes or outputs
 * are changed. The caller owns the buffer and both results.
 * @param data Mutable wire buffer.
 * @param size Number of bytes present in @p data.
 * @param consumed Receives the number of consumed wire bytes.
 * @param decoded Receives the number of decoded bytes at @p data.
 * @return Complete, incomplete, or error for invalid syntax or overflow.
 * @pre @p data has @p size writable bytes and outputs are valid, non-overlapping
 *      with the buffer.
 */
lh_http_parse_status_t
lh_http_chunked_decode(lh_str_ptr data, lh_usize_t size, lh_usize_t *consumed, lh_usize_t *decoded);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_CHUNKED_H */
