/**
 * @file decoder.h
 * @brief Incremental chunked transfer decoder with caller-owned buffers.
 *
 * A decoder resumes across arbitrary input and output boundaries. The caller
 * supplies scratch for one chunk-size or trailer line, an aggregate metadata
 * limit and a decoded-body limit. Neither scratch nor input/output buffers are
 * owned. Decoded bytes can be consumed as soon as they are produced; if a
 * later fragment is malformed the caller must discard or roll back its
 * partial output. Trailers are validated and discarded, not merged into fields.
 */

#ifndef LH_HTTP_CHUNKED_DECODER_H
#define LH_HTTP_CHUNKED_DECODER_H

#include <lh/http/chunked.h>
#include <lh/http/chunked/decoder/fields.h>
#include <lh/http/chunked/decoder/phase.h>

/** @brief State for an incremental chunked transfer. */
struct lh_http_chunked_decoder
{
    /** Members: ::lh_http_chunked_decoder_fields. */
    lh_http_chunked_decoder_fields(lh_str_ptr, lh_usize_t, lh_http_chunked_decoder_phase_t);
};
/** @copybrief lh_http_chunked_decoder */
typedef struct lh_http_chunked_decoder lh_http_chunked_decoder_t;

LH_COMPILER_EXTERN_C_BEGIN

/** Return the stored line; @p self must be valid. */
lh_str_ptr
lh_http_chunked_decoder_get_line(const lh_http_chunked_decoder_t *self);

/** Store line without validating it; @p self must be valid. */
lh_void
lh_http_chunked_decoder_set_line(lh_http_chunked_decoder_t *self, lh_str_ptr value);

/** Return the stored line_capacity; @p self must be valid. */
lh_usize_t
lh_http_chunked_decoder_get_line_capacity(const lh_http_chunked_decoder_t *self);

/** Store line_capacity without validating it; @p self must be valid. */
lh_void
lh_http_chunked_decoder_set_line_capacity(lh_http_chunked_decoder_t *self, lh_usize_t value);

/** Return the stored line_size; @p self must be valid. */
lh_usize_t
lh_http_chunked_decoder_get_line_size(const lh_http_chunked_decoder_t *self);

/** Store line_size without validating it; @p self must be valid. */
lh_void
lh_http_chunked_decoder_set_line_size(lh_http_chunked_decoder_t *self, lh_usize_t value);

/** Return the stored remaining; @p self must be valid. */
lh_usize_t
lh_http_chunked_decoder_get_remaining(const lh_http_chunked_decoder_t *self);

/** Store remaining without validating it; @p self must be valid. */
lh_void
lh_http_chunked_decoder_set_remaining(lh_http_chunked_decoder_t *self, lh_usize_t value);

/** Return the stored decoded; @p self must be valid. */
lh_usize_t
lh_http_chunked_decoder_get_decoded(const lh_http_chunked_decoder_t *self);

/** Store decoded without validating it; @p self must be valid. */
lh_void
lh_http_chunked_decoder_set_decoded(lh_http_chunked_decoder_t *self, lh_usize_t value);

/** Return the stored decoded_limit; @p self must be valid. */
lh_usize_t
lh_http_chunked_decoder_get_decoded_limit(const lh_http_chunked_decoder_t *self);

/** Store decoded_limit without validating it; @p self must be valid. */
lh_void
lh_http_chunked_decoder_set_decoded_limit(lh_http_chunked_decoder_t *self, lh_usize_t value);

/** Return the stored metadata; @p self must be valid. */
lh_usize_t
lh_http_chunked_decoder_get_metadata(const lh_http_chunked_decoder_t *self);

/** Store metadata without validating it; @p self must be valid. */
lh_void
lh_http_chunked_decoder_set_metadata(lh_http_chunked_decoder_t *self, lh_usize_t value);

/** Return the stored metadata_limit; @p self must be valid. */
lh_usize_t
lh_http_chunked_decoder_get_metadata_limit(const lh_http_chunked_decoder_t *self);

/** Store metadata_limit without validating it; @p self must be valid. */
lh_void
lh_http_chunked_decoder_set_metadata_limit(lh_http_chunked_decoder_t *self, lh_usize_t value);

/** Return the stored phase; @p self must be valid. */
lh_http_chunked_decoder_phase_t
lh_http_chunked_decoder_get_phase(const lh_http_chunked_decoder_t *self);

/** Store phase without validating it; @p self must be valid. */
lh_void
lh_http_chunked_decoder_set_phase(lh_http_chunked_decoder_t *self,
                                  lh_http_chunked_decoder_phase_t value);

/**
 * @brief Start decoding with caller-owned line scratch and explicit limits.
 *
 * Metadata counts chunk-size lines, their delimiters, data delimiters and all
 * trailers. Scratch and the decoder remain caller-owned and must outlive feeds.
 * @param self Decoder to initialize.
 * @param line Writable scratch for one line, without CRLF.
 * @param line_capacity Number of scratch bytes; must be nonzero.
 * @param decoded_limit Largest permitted decoded body in bytes.
 * @param metadata_limit Largest permitted aggregate metadata in wire bytes.
 * @pre All pointers are valid and the limits fit ::lh_usize_t.
 */
lh_void
lh_http_chunked_decoder_init(lh_http_chunked_decoder_t *self, lh_str_ptr line,
                             lh_usize_t line_capacity, lh_usize_t decoded_limit,
                             lh_usize_t metadata_limit);

/**
 * @brief Validate a completed scratch line and advance the decoder phase.
 *
 * Uses the shared chunk-size and HTTP field parsers. Only valid in the
 * size-LF and trailer-LF phases, after their LF has been consumed.
 * @param self Initialized decoder; line contents remain caller-owned.
 * @return True on valid line and limits; false for invalid syntax or limits.
 */
lh_bool_t
lh_http_chunked_decoder_try_finish_line(lh_http_chunked_decoder_t *self);

/**
 * @brief Finish a line and check ::lh_http_chunked_decoder_try_finish_line.
 * @param self Initialized caller-owned decoder in a line-LF phase.
 * @fails ::lh_runtime_error_code_invalid_range Invalid line or exceeded limit.
 */
lh_void
lh_http_chunked_decoder_finish_line(lh_http_chunked_decoder_t *self);

/**
 * @brief Consume input and write as many decoded bytes as fit in output.
 *
 * On incomplete, call again with the unconsumed input and fresh output space;
 * it may mean more input or output is needed. Complete stops at the end of
 * trailers and leaves following bytes unconsumed. Error is sticky. Even on an
 * error the byte counts describe progress; already produced data is not rolled
 * back. Input, output and decoder scratch must not overlap each other.
 * @param self Initialized decoder, retaining state across calls.
 * @param input Borrowed input bytes, possibly empty.
 * @param output Caller-owned writable decoded output buffer.
 * @param capacity Bytes available in @p output; zero is allowed.
 * @param consumed Receives the number of input bytes consumed on every call.
 * @param written Receives the number of output bytes written on every call.
 * @return Complete, incomplete, or error for malformed data or a limit.
 * @pre Decoder, view and output counters are valid; a nonzero capacity has
 *      that many writable bytes. Input, output, scratch and counters do not overlap.
 */
lh_http_parse_status_t
lh_http_chunked_decoder_feed(lh_http_chunked_decoder_t *self, lh_str_view_t input,
                             lh_str_ptr output, lh_usize_t capacity, lh_usize_t *consumed,
                             lh_usize_t *written);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_CHUNKED_DECODER_H */
