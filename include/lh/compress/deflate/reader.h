/**
 * @file reader.h
 * @brief Incremental, allocation-free reader for one raw DEFLATE stream.
 *
 * The reader borrows its source, scratch buffer and 32 KiB history window.
 * Its state and tables survive arbitrary source and output boundaries. It reads
 * no whole bytes past the DEFLATE stream, so the source can supply a gzip
 * trailer afterwards. A decoded-byte limit bounds expansion. Fast setters do
 * not validate state; callers must preserve the invariants established by init
 * and the phase operations. Failure is sticky;
 * if a later failure follows returned bytes, the next read reports an error.
 */

#ifndef LH_COMPRESS_DEFLATE_READER_H
#define LH_COMPRESS_DEFLATE_READER_H

#include <lh/compress/deflate.h>
#include <lh/compress/deflate/reader/fields.h>
#include <lh/compress/deflate/reader/phase.h>
#include <lh/io/reader.h>

/** Bytes needed by the DEFLATE sliding history window. */
#define LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE LH_COMPRESS_DEFLATE_WINDOW_SIZE
/**
 * Scratch size for a dynamic Huffman header and a partial leading byte.
 * It exceeds the maximum header made from 318 code lengths of at most 15
 * bits each plus the code-length table and fixed fields.
 */
#define LH_COMPRESS_DEFLATE_READER_INPUT_SIZE 1024U

/** @brief Persistent, borrowed-buffer DEFLATE reader state. */
struct lh_compress_deflate_reader
{
    /** Members: ::lh_compress_deflate_reader_fields. */
    lh_compress_deflate_reader_fields(lh_io_reader_t *, lh_byte_t *, lh_usize_t, lh_bool_t,
                                      lh_compress_deflate_reader_phase_t, lh_u16_t);
};
/** @copybrief lh_compress_deflate_reader */
typedef struct lh_compress_deflate_reader lh_compress_deflate_reader_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Return the stored source; self must be valid. */
lh_io_reader_t *
lh_compress_deflate_reader_get_source(const lh_compress_deflate_reader_t *self);

/** @brief Store source without validation; self must be valid. */
lh_void
lh_compress_deflate_reader_set_source(lh_compress_deflate_reader_t *self, lh_io_reader_t *value);

/** @brief Return the stored input; self must be valid. */
lh_byte_t *
lh_compress_deflate_reader_get_input(const lh_compress_deflate_reader_t *self);

/** @brief Store input without validation; self must be valid. */
lh_void
lh_compress_deflate_reader_set_input(lh_compress_deflate_reader_t *self, lh_byte_t *value);

/** @brief Return the stored input_capacity; self must be valid. */
lh_usize_t
lh_compress_deflate_reader_get_input_capacity(const lh_compress_deflate_reader_t *self);

/** @brief Store input_capacity without validation; self must be valid. */
lh_void
lh_compress_deflate_reader_set_input_capacity(lh_compress_deflate_reader_t *self, lh_usize_t value);

/** @brief Return the stored input_size; self must be valid. */
lh_usize_t
lh_compress_deflate_reader_get_input_size(const lh_compress_deflate_reader_t *self);

/** @brief Store input_size without validation; self must be valid. */
lh_void
lh_compress_deflate_reader_set_input_size(lh_compress_deflate_reader_t *self, lh_usize_t value);

/** @brief Return the stored bit_position; self must be valid. */
lh_usize_t
lh_compress_deflate_reader_get_bit_position(const lh_compress_deflate_reader_t *self);

/** @brief Store bit_position without validation; self must be valid. */
lh_void
lh_compress_deflate_reader_set_bit_position(lh_compress_deflate_reader_t *self, lh_usize_t value);

/** @brief Return the stored history; self must be valid. */
lh_byte_t *
lh_compress_deflate_reader_get_history(const lh_compress_deflate_reader_t *self);

/** @brief Store history without validation; self must be valid. */
lh_void
lh_compress_deflate_reader_set_history(lh_compress_deflate_reader_t *self, lh_byte_t *value);

/** @brief Return the stored history_position; self must be valid. */
lh_usize_t
lh_compress_deflate_reader_get_history_position(const lh_compress_deflate_reader_t *self);

/** @brief Store history_position without validation; self must be valid. */
lh_void
lh_compress_deflate_reader_set_history_position(lh_compress_deflate_reader_t *self,
                                                lh_usize_t value);

/** @brief Return the stored history_size; self must be valid. */
lh_usize_t
lh_compress_deflate_reader_get_history_size(const lh_compress_deflate_reader_t *self);

/** @brief Store history_size without validation; self must be valid. */
lh_void
lh_compress_deflate_reader_set_history_size(lh_compress_deflate_reader_t *self, lh_usize_t value);

/** @brief Return the stored decoded; self must be valid. */
lh_usize_t
lh_compress_deflate_reader_get_decoded(const lh_compress_deflate_reader_t *self);

/** @brief Store decoded without validation; self must be valid. */
lh_void
lh_compress_deflate_reader_set_decoded(lh_compress_deflate_reader_t *self, lh_usize_t value);

/** @brief Return the stored limit; self must be valid. */
lh_usize_t
lh_compress_deflate_reader_get_limit(const lh_compress_deflate_reader_t *self);

/** @brief Store limit without validation; self must be valid. */
lh_void
lh_compress_deflate_reader_set_limit(lh_compress_deflate_reader_t *self, lh_usize_t value);

/** @brief Return the stored remaining; self must be valid. */
lh_usize_t
lh_compress_deflate_reader_get_remaining(const lh_compress_deflate_reader_t *self);

/** @brief Store remaining without validation; self must be valid. */
lh_void
lh_compress_deflate_reader_set_remaining(lh_compress_deflate_reader_t *self, lh_usize_t value);

/** @brief Return the stored distance; self must be valid. */
lh_usize_t
lh_compress_deflate_reader_get_distance(const lh_compress_deflate_reader_t *self);

/** @brief Store distance without validation; self must be valid. */
lh_void
lh_compress_deflate_reader_set_distance(lh_compress_deflate_reader_t *self, lh_usize_t value);

/** @brief Return the stored final; self must be valid. */
lh_bool_t
lh_compress_deflate_reader_get_final(const lh_compress_deflate_reader_t *self);

/** @brief Store final without validation; self must be valid. */
lh_void
lh_compress_deflate_reader_set_final(lh_compress_deflate_reader_t *self, lh_bool_t value);

/** @brief Return the stored phase; self must be valid. */
lh_compress_deflate_reader_phase_t
lh_compress_deflate_reader_get_phase(const lh_compress_deflate_reader_t *self);

/** @brief Store phase without validation; self must be valid. */
lh_void
lh_compress_deflate_reader_set_phase(lh_compress_deflate_reader_t *self,
                                     lh_compress_deflate_reader_phase_t value);

/** @brief Borrow the reader's writable lit_counts table; self must be valid. */
lh_u16_t *
lh_compress_deflate_reader_get_lit_counts(lh_compress_deflate_reader_t *self);

/** @brief Borrow the reader's writable lit_symbols table; self must be valid. */
lh_u16_t *
lh_compress_deflate_reader_get_lit_symbols(lh_compress_deflate_reader_t *self);

/** @brief Borrow the reader's writable dist_counts table; self must be valid. */
lh_u16_t *
lh_compress_deflate_reader_get_dist_counts(lh_compress_deflate_reader_t *self);

/** @brief Borrow the reader's writable dist_symbols table; self must be valid. */
lh_u16_t *
lh_compress_deflate_reader_get_dist_symbols(lh_compress_deflate_reader_t *self);

/**
 * @brief Initialize a raw DEFLATE reader over a borrowed LH source.
 * @param self Reader state; remains caller-owned.
 * @param source Initialized source, borrowed for this reader's lifetime.
 * @param input Writable scratch; borrowed, at least input_capacity bytes.
 * @param input_capacity Scratch size, at least LH_COMPRESS_DEFLATE_READER_INPUT_SIZE;
 *                       bytes beyond that minimum are not used.
 * @param history Writable 32768-byte history, borrowed for the lifetime.
 * @param limit Maximum number of decoded bytes.
 * @pre All pointers are valid; scratch and history do not overlap.
 */
lh_void
lh_compress_deflate_reader_init(lh_compress_deflate_reader_t *self, lh_io_reader_t *source,
                                lh_byte_t *input, lh_usize_t input_capacity, lh_byte_t *history,
                                lh_usize_t limit);

/**
 * @brief Discard fully consumed scratch bytes while retaining partial bits.
 * @param self Valid initialized reader.
 */
lh_void
lh_compress_deflate_reader_compact(lh_compress_deflate_reader_t *self);

/**
 * @brief Append one source byte to scratch after compacting consumed bytes.
 * @param self Valid initialized reader.
 * @return One for a byte, LH_IO_PENDING if unavailable, or LH_IO_ERROR on
 *         truncation, source error or scratch exhaustion.
 */
lh_ssize_t
lh_compress_deflate_reader_try_refill(lh_compress_deflate_reader_t *self);

/**
 * @brief Emit and remember one decoded byte.
 * @param self Initialized reader with room under its decoded limit.
 * @param output Caller-owned output byte.
 * @param value Byte to emit.
 * @pre Self and output are valid and the decoded limit is not exhausted.
 */
lh_void
lh_compress_deflate_reader_emit(lh_compress_deflate_reader_t *self, lh_byte_t *output,
                                lh_byte_t value);

/**
 * @brief Count unconsumed bits currently in input scratch.
 * @param self Initialized reader with a valid bit position.
 * @return Number of buffered bits; input storage remains borrowed.
 */
lh_usize_t
lh_compress_deflate_reader_get_available_bits(const lh_compress_deflate_reader_t *self);

/**
 * @brief Execute one block-header transition without reading the source.
 * @param self Reader in the block-header phase.
 * @return Zero for progress, LH_IO_PENDING if scratch needs input, or
 *         LH_IO_ERROR for an invalid header.
 * @pre Self is initialized and is in the documented phase.
 */
lh_ssize_t
lh_compress_deflate_reader_try_step_block(lh_compress_deflate_reader_t *self);

/**
 * @brief Execute one stored-block header transition from buffered bits.
 * @param self Reader in the stored-header phase.
 * @return Zero for progress, LH_IO_PENDING for input, or LH_IO_ERROR for an
 *         invalid stored length.
 * @pre Self is initialized and is in the documented phase.
 */
lh_ssize_t
lh_compress_deflate_reader_try_step_stored_header(lh_compress_deflate_reader_t *self);

/**
 * @brief Process a stored-block byte or advance to the next block.
 * @param self Reader in the stored phase.
 * @param output Writable byte, modified only when one is emitted.
 * @return One for a byte, zero for a phase transition, LH_IO_PENDING for
 *         input, or LH_IO_ERROR when the decoded limit is exceeded.
 * @pre Self and output are valid and self is in the stored phase.
 */
lh_ssize_t
lh_compress_deflate_reader_try_step_stored(lh_compress_deflate_reader_t *self, lh_byte_t *output);

/**
 * @brief Attempt to construct a dynamic block's Huffman tables from scratch.
 * @param self Reader in the dynamic-table phase.
 * @return Zero for progress or LH_IO_PENDING when more input is required;
 *         malformed tables eventually fail when input or scratch is exhausted.
 * @pre Self is initialized and is in the documented phase.
 */
lh_ssize_t
lh_compress_deflate_reader_step_dynamic(lh_compress_deflate_reader_t *self);

/**
 * @brief Process a literal, end marker, or complete match description.
 * @param self Reader in the Huffman-code phase.
 * @param output Writable byte, modified only for a literal.
 * @return One for a literal, zero for a state transition, LH_IO_PENDING for
 *         input, or LH_IO_ERROR for an invalid code or limit violation.
 * @pre Self and output are valid and self is in the code phase.
 */
lh_ssize_t
lh_compress_deflate_reader_try_step_codes(lh_compress_deflate_reader_t *self, lh_byte_t *output);

/**
 * @brief Emit one byte of a pending back-reference or finish the match.
 * @param self Reader in the match phase.
 * @param output Writable byte, modified only if a byte is emitted.
 * @return One for a byte or zero for the transition back to codes.
 * @pre Self and output are valid; the phase is match and its distance and
 *      remaining length describe a previously validated match within the limit.
 */
lh_ssize_t
lh_compress_deflate_reader_step_match(lh_compress_deflate_reader_t *self, lh_byte_t *output);

/**
 * @brief Dispatch one decoding transition without reading the source.
 * @param self Initialized reader.
 * @param output Writable byte used only when a byte is emitted.
 * @return One for a byte, zero for progress or completion, LH_IO_PENDING when
 *         scratch needs input, or LH_IO_ERROR for invalid state or input.
 * @pre Self and output are valid.
 */
lh_ssize_t
lh_compress_deflate_reader_try_step(lh_compress_deflate_reader_t *self, lh_byte_t *output);

/**
 * @brief Read decoded bytes from one raw DEFLATE stream.
 * @param context Initialized reader, borrowed by this call.
 * @param output Writable destination.
 * @param size Destination capacity; zero returns zero.
 * @return Positive decoded count, zero at stream end, LH_IO_PENDING, or
 *         LH_IO_ERROR for invalid/truncated input, source failure or limit.
 * @pre Context is valid; nonzero size has a valid writable output buffer.
 */
lh_ssize_t
lh_compress_deflate_reader_read(lh_self_ptr context, lh_ptr output, lh_usize_t size);

/**
 * @brief Construct a borrowed LH reader dispatching to the DEFLATE reader.
 * @param self Initialized DEFLATE reader, valid for the result's lifetime.
 * @param output Caller-owned reader to initialize.
 */
lh_void
lh_compress_deflate_reader_get_reader(lh_compress_deflate_reader_t *self, lh_io_reader_t *output);

/**
 * @brief Checked form of ::lh_compress_deflate_reader_try_refill.
 * A pending result is returned unchanged; an I/O error fails the runtime check.
 * @copydetails lh_compress_deflate_reader_try_refill
 * @fails ::lh_runtime_error_code_invalid_range The operation reports an error.
 */
lh_ssize_t
lh_compress_deflate_reader_refill(lh_compress_deflate_reader_t *self);

/**
 * @brief Checked form of ::lh_compress_deflate_reader_try_step_block.
 * A pending result is returned unchanged; an I/O error fails the runtime check.
 * @copydetails lh_compress_deflate_reader_try_step_block
 * @fails ::lh_runtime_error_code_invalid_range The operation reports an error.
 */
lh_ssize_t
lh_compress_deflate_reader_step_block(lh_compress_deflate_reader_t *self);

/**
 * @brief Checked form of ::lh_compress_deflate_reader_try_step_stored_header.
 * A pending result is returned unchanged; an I/O error fails the runtime check.
 * @copydetails lh_compress_deflate_reader_try_step_stored_header
 * @fails ::lh_runtime_error_code_invalid_range The operation reports an error.
 */
lh_ssize_t
lh_compress_deflate_reader_step_stored_header(lh_compress_deflate_reader_t *self);

/**
 * @brief Checked form of ::lh_compress_deflate_reader_try_step_stored.
 * A pending result is returned unchanged; an I/O error fails the runtime check.
 * @copydetails lh_compress_deflate_reader_try_step_stored
 * @fails ::lh_runtime_error_code_invalid_range The operation reports an error.
 */
lh_ssize_t
lh_compress_deflate_reader_step_stored(lh_compress_deflate_reader_t *self, lh_byte_t *output);

/**
 * @brief Checked form of ::lh_compress_deflate_reader_try_step_codes.
 * A pending result is returned unchanged; an I/O error fails the runtime check.
 * @copydetails lh_compress_deflate_reader_try_step_codes
 * @fails ::lh_runtime_error_code_invalid_range The operation reports an error.
 */
lh_ssize_t
lh_compress_deflate_reader_step_codes(lh_compress_deflate_reader_t *self, lh_byte_t *output);

/**
 * @brief Checked form of ::lh_compress_deflate_reader_try_step.
 * A pending result is returned unchanged; an I/O error fails the runtime check.
 * @copydetails lh_compress_deflate_reader_try_step
 * @fails ::lh_runtime_error_code_invalid_range The operation reports an error.
 */
lh_ssize_t
lh_compress_deflate_reader_step(lh_compress_deflate_reader_t *self, lh_byte_t *output);

LH_COMPILER_EXTERN_C_END

#endif /* LH_COMPRESS_DEFLATE_READER_H */
