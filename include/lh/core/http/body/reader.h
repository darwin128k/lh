/**
 * @file reader.h
 * @brief Read a framed HTTP request body incrementally from an LH reader.
 *
 * The adapter handles either a known Content-Length or a caller-initialized
 * chunked decoder. Its source, wire scratch and optional decoder are borrowed;
 * no allocation occurs. The wire scratch can start with bytes already read
 * after the request head. Reads yield decoded content, preserve pending/error
 * from the source and retain extra bytes after the body for the caller.
 * A later malformed chunk or premature end is an error: a consumer that has
 * already acted on earlier content is responsible for rolling it back.
 */

#ifndef LH_HTTP_BODY_READER_H
#define LH_HTTP_BODY_READER_H

#include <lh/core/http/body/reader/fields.h>
#include <lh/core/http/chunked/decoder.h>
#include <lh/core/io/reader.h>

/** @brief State for a borrowed source and its HTTP message framing. */
struct lh_http_body_reader
{
    /** Members: ::lh_http_body_reader_fields. */
    lh_http_body_reader_fields(lh_io_reader_t *, lh_http_chunked_decoder_t *, lh_str_ptr,
                               lh_usize_t, lh_bool_t);
};
/** @copybrief lh_http_body_reader */
typedef struct lh_http_body_reader lh_http_body_reader_t;

LH_COMPILER_EXTERN_C_BEGIN

/** Return the stored source; @p self must be valid. */
lh_io_reader_t *
lh_http_body_reader_get_source(const lh_http_body_reader_t *self);

/** Store source without validating it; @p self must be valid. */
lh_void
lh_http_body_reader_set_source(lh_http_body_reader_t *self, lh_io_reader_t *value);

/** Return the stored decoder; @p self must be valid. */
lh_http_chunked_decoder_t *
lh_http_body_reader_get_decoder(const lh_http_body_reader_t *self);

/** Store decoder without validating it; @p self must be valid. */
lh_void
lh_http_body_reader_set_decoder(lh_http_body_reader_t *self, lh_http_chunked_decoder_t *value);

/** Return the stored wire; @p self must be valid. */
lh_str_ptr
lh_http_body_reader_get_wire(const lh_http_body_reader_t *self);

/** Store wire without validating it; @p self must be valid. */
lh_void
lh_http_body_reader_set_wire(lh_http_body_reader_t *self, lh_str_ptr value);

/** Return the stored wire_capacity; @p self must be valid. */
lh_usize_t
lh_http_body_reader_get_wire_capacity(const lh_http_body_reader_t *self);

/** Store wire_capacity without validating it; @p self must be valid. */
lh_void
lh_http_body_reader_set_wire_capacity(lh_http_body_reader_t *self, lh_usize_t value);

/** Return the stored wire_size; @p self must be valid. */
lh_usize_t
lh_http_body_reader_get_wire_size(const lh_http_body_reader_t *self);

/** Store wire_size without validating it; @p self must be valid. */
lh_void
lh_http_body_reader_set_wire_size(lh_http_body_reader_t *self, lh_usize_t value);

/** Return the stored wire_pos; @p self must be valid. */
lh_usize_t
lh_http_body_reader_get_wire_pos(const lh_http_body_reader_t *self);

/** Store wire_pos without validating it; @p self must be valid. */
lh_void
lh_http_body_reader_set_wire_pos(lh_http_body_reader_t *self, lh_usize_t value);

/** Return the stored remaining; @p self must be valid. */
lh_usize_t
lh_http_body_reader_get_remaining(const lh_http_body_reader_t *self);

/** Store remaining without validating it; @p self must be valid. */
lh_void
lh_http_body_reader_set_remaining(lh_http_body_reader_t *self, lh_usize_t value);

/** Return the stored failed; @p self must be valid. */
lh_bool_t
lh_http_body_reader_get_failed(const lh_http_body_reader_t *self);

/** Store failed without validating it; @p self must be valid. */
lh_void
lh_http_body_reader_set_failed(lh_http_body_reader_t *self, lh_bool_t value);

/**
 * @brief Initialize with a Content-Length and any already-read body bytes.
 *
 * The source and scratch are borrowed and must outlive this reader. Bytes
 * already in @p wire begin at offset zero and may include the next message.
 * @param self Reader to initialize.
 * @param source Caller-owned LH reader supplying subsequent wire bytes.
 * @param wire Caller-owned writable wire scratch, at least @p capacity bytes.
 * @param capacity Scratch capacity; must be nonzero.
 * @param available Bytes already present in @p wire, at most capacity.
 * @param length Content-Length in bytes.
 * @pre All pointers and the initialized source are valid; available <= capacity.
 */
lh_void
lh_http_body_reader_init_length(lh_http_body_reader_t *self, lh_io_reader_t *source,
                                lh_str_ptr wire, lh_usize_t capacity, lh_usize_t available,
                                lh_usize_t length);

/**
 * @brief Initialize for chunked input using an initialized incremental decoder.
 *
 * The source, wire scratch and decoder are borrowed for this reader's lifetime.
 * The decoder's line scratch and limits are supplied to its initializer.
 * @param self Reader to initialize.
 * @param source Caller-owned LH reader supplying subsequent wire bytes.
 * @param wire Caller-owned wire scratch containing @p available initial bytes.
 * @param capacity Scratch capacity; must be nonzero.
 * @param available Initial bytes in scratch, at most capacity.
 * @param decoder Caller-owned, freshly initialized chunked decoder.
 * @pre All pointers are valid and available <= capacity.
 */
lh_void
lh_http_body_reader_init_chunked(lh_http_body_reader_t *self, lh_io_reader_t *source,
                                 lh_str_ptr wire, lh_usize_t capacity, lh_usize_t available,
                                 lh_http_chunked_decoder_t *decoder);

/**
 * @brief Refill empty wire scratch from the borrowed source.
 * @param self Initialized reader whose previous scratch is consumed.
 * @return Positive byte count, 0 for source end, ::LH_IO_PENDING or
 *         ::LH_IO_ERROR for failure. No input is owned or allocated.
 * @pre Wire position equals wire size; scratch capacity is nonzero.
 */
lh_ssize_t
lh_http_body_reader_refill(lh_http_body_reader_t *self);

/**
 * @brief Read decoded body bytes; suitable as an ::lh_io_reader_t callback.
 *
 * Returns 0 only once the framed body is complete (or @p size is zero).
 * Incomplete wire input followed by source EOF is ::LH_IO_ERROR; malformed
 * chunks and exhausted limits are sticky errors. If an error follows decoded
 * bytes in the same call, those bytes are returned and the next call reports
 * the error. Pending source reads return ::LH_IO_PENDING. Input, output and
 * scratch must not overlap. The source and all buffers remain caller-owned.
 * @param self Initialized body reader passed as callback context.
 * @param out Caller-owned writable output bytes.
 * @param size Output capacity; zero is allowed.
 * @return Bytes read, 0 at body end, ::LH_IO_PENDING, or ::LH_IO_ERROR.
 * @pre @p self is valid; nonzero @p size has that many writable output bytes.
 */
lh_ssize_t
lh_http_body_reader_read(lh_self_ptr self, lh_ptr out, lh_usize_t size);

/**
 * @brief Construct an LH reader whose callback reads this framed body.
 * @param self Initialized body reader, borrowed for the result's lifetime.
 * @param out Caller-owned reader to initialize; no storage is transferred.
 */
lh_void
lh_http_body_reader_get_reader(lh_http_body_reader_t *self, lh_io_reader_t *out);

/**
 * @brief View wire bytes already read past the completed body.
 * @param self Initialized reader; inspect only after body end.
 * @param out Caller-owned view borrowing the reader's wire scratch.
 * @pre @p self and @p out are valid; the body is complete.
 */
lh_void
lh_http_body_reader_get_unread(const lh_http_body_reader_t *self, lh_str_view_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_BODY_READER_H */
