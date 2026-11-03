/**
 * @file reader.h
 * @brief Bounded incremental gzip decoding over an LH byte reader.
 *
 * Reader, source, subdecoders and caller-provided scratch/history remain
 * independently owned. Headers, CRC, sizes and concatenated members are
 * validated across arbitrary read boundaries. The source must return EOF at the
 * gzip content boundary; an HTTP body-framing reader provides that boundary.
 * The decoded and gzip metadata limits are independent. A later checksum error is sticky and
 * requires the consumer to discard already received but not yet committed bytes.
 */

#ifndef LH_COMPRESS_GZIP_READER_H
#define LH_COMPRESS_GZIP_READER_H

#include <lh/compress/deflate/reader.h>
#include <lh/compress/gzip/header/decoder.h>
#include <lh/compress/gzip/reader/fields.h>
#include <lh/compress/gzip/reader/phase.h>

/** @brief Persistent state for gzip members read from a borrowed source. */
struct lh_compress_gzip_reader
{
    /** Members: ::lh_compress_gzip_reader_fields. */
    lh_compress_gzip_reader_fields(lh_io_reader_t *, lh_compress_gzip_header_decoder_t *,
                                   lh_compress_deflate_reader_t *, lh_byte_t *, lh_usize_t,
                                   lh_u32_t, lh_compress_gzip_reader_phase_t, lh_byte_t);
};
/** @copybrief lh_compress_gzip_reader */
typedef struct lh_compress_gzip_reader lh_compress_gzip_reader_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Return the stored source; self must be valid. */
lh_io_reader_t *
lh_compress_gzip_reader_get_source(const lh_compress_gzip_reader_t *self);

/** @brief Store source without validation; self must be valid. */
lh_void
lh_compress_gzip_reader_set_source(lh_compress_gzip_reader_t *self, lh_io_reader_t *value);

/** @brief Return the stored header; self must be valid. */
lh_compress_gzip_header_decoder_t *
lh_compress_gzip_reader_get_header(const lh_compress_gzip_reader_t *self);

/** @brief Store header without validation; self must be valid. */
lh_void
lh_compress_gzip_reader_set_header(lh_compress_gzip_reader_t *self,
                                   lh_compress_gzip_header_decoder_t *value);

/** @brief Return the stored deflate; self must be valid. */
lh_compress_deflate_reader_t *
lh_compress_gzip_reader_get_deflate(const lh_compress_gzip_reader_t *self);

/** @brief Store deflate without validation; self must be valid. */
lh_void
lh_compress_gzip_reader_set_deflate(lh_compress_gzip_reader_t *self,
                                    lh_compress_deflate_reader_t *value);

/** @brief Return the stored scratch; self must be valid. */
lh_byte_t *
lh_compress_gzip_reader_get_scratch(const lh_compress_gzip_reader_t *self);

/** @brief Store scratch without validation; self must be valid. */
lh_void
lh_compress_gzip_reader_set_scratch(lh_compress_gzip_reader_t *self, lh_byte_t *value);

/** @brief Return the stored history; self must be valid. */
lh_byte_t *
lh_compress_gzip_reader_get_history(const lh_compress_gzip_reader_t *self);

/** @brief Store history without validation; self must be valid. */
lh_void
lh_compress_gzip_reader_set_history(lh_compress_gzip_reader_t *self, lh_byte_t *value);

/** @brief Return the stored limit; self must be valid. */
lh_usize_t
lh_compress_gzip_reader_get_limit(const lh_compress_gzip_reader_t *self);

/** @brief Store limit without validation; self must be valid. */
lh_void
lh_compress_gzip_reader_set_limit(lh_compress_gzip_reader_t *self, lh_usize_t value);

/** @brief Return the stored metadata_limit; self must be valid. */
lh_usize_t
lh_compress_gzip_reader_get_metadata_limit(const lh_compress_gzip_reader_t *self);

/** @brief Store metadata_limit without validation; self must be valid. */
lh_void
lh_compress_gzip_reader_set_metadata_limit(lh_compress_gzip_reader_t *self, lh_usize_t value);

/** @brief Return the stored metadata; self must be valid. */
lh_usize_t
lh_compress_gzip_reader_get_metadata(const lh_compress_gzip_reader_t *self);

/** @brief Store metadata without validation; self must be valid. */
lh_void
lh_compress_gzip_reader_set_metadata(lh_compress_gzip_reader_t *self, lh_usize_t value);

/** @brief Return the stored total; self must be valid. */
lh_usize_t
lh_compress_gzip_reader_get_total(const lh_compress_gzip_reader_t *self);

/** @brief Store total without validation; self must be valid. */
lh_void
lh_compress_gzip_reader_set_total(lh_compress_gzip_reader_t *self, lh_usize_t value);

/** @brief Return the stored member_size; self must be valid. */
lh_usize_t
lh_compress_gzip_reader_get_member_size(const lh_compress_gzip_reader_t *self);

/** @brief Store member_size without validation; self must be valid. */
lh_void
lh_compress_gzip_reader_set_member_size(lh_compress_gzip_reader_t *self, lh_usize_t value);

/** @brief Return the stored crc; self must be valid. */
lh_u32_t
lh_compress_gzip_reader_get_crc(const lh_compress_gzip_reader_t *self);

/** @brief Store crc without validation; self must be valid. */
lh_void
lh_compress_gzip_reader_set_crc(lh_compress_gzip_reader_t *self, lh_u32_t value);

/** @brief Return the stored trailer_size; self must be valid. */
lh_usize_t
lh_compress_gzip_reader_get_trailer_size(const lh_compress_gzip_reader_t *self);

/** @brief Store trailer_size without validation; self must be valid. */
lh_void
lh_compress_gzip_reader_set_trailer_size(lh_compress_gzip_reader_t *self, lh_usize_t value);

/** @brief Return the stored phase; self must be valid. */
lh_compress_gzip_reader_phase_t
lh_compress_gzip_reader_get_phase(const lh_compress_gzip_reader_t *self);

/** @brief Store phase without validation; self must be valid. */
lh_void
lh_compress_gzip_reader_set_phase(lh_compress_gzip_reader_t *self,
                                  lh_compress_gzip_reader_phase_t value);

/** @brief Borrow the writable eight-byte trailer buffer. */
lh_byte_t *
lh_compress_gzip_reader_get_trailer(lh_compress_gzip_reader_t *self);

/**
 * @brief Initialize a gzip reader with caller-owned components.
 * @param self Reader state, owned by caller.
 * @param source Initialized LH input, borrowed throughout decoding.
 * @param header Borrowed header decoder state.
 * @param deflate Borrowed DEFLATE reader state.
 * @param scratch Writable DEFLATE scratch of at least 1024 bytes.
 * @param history Writable DEFLATE history of 32768 bytes.
 * @param limit Maximum total decompressed bytes across all members.
 * @param metadata_limit Maximum total gzip header and trailer bytes.
 * @pre All pointers are valid; scratch and history do not overlap.
 */
lh_void
lh_compress_gzip_reader_init(lh_compress_gzip_reader_t *self, lh_io_reader_t *source,
                             lh_compress_gzip_header_decoder_t *header,
                             lh_compress_deflate_reader_t *deflate, lh_byte_t *scratch,
                             lh_byte_t *history, lh_usize_t limit, lh_usize_t metadata_limit);

/**
 * @brief Feed one already-read header byte and advance member state.
 * @param self Initialized reader in the header phase.
 * @param value Next member header byte.
 * @return True if accepted, false for invalid header or metadata limit.
 * @pre Self is valid, in the header phase, and its header decoder is active.
 */
lh_bool_t
lh_compress_gzip_reader_try_header_byte(lh_compress_gzip_reader_t *self, lh_byte_t value);

/** @brief Checked form of ::lh_compress_gzip_reader_try_header_byte. */
lh_void
lh_compress_gzip_reader_header_byte(lh_compress_gzip_reader_t *self, lh_byte_t value);

/**
 * @brief Advance gzip header or next-member parsing by one source byte.
 * @param self Initialized reader in the header or next-member phase.
 * @return Zero for progress or validated end, LH_IO_PENDING when the source
 *         has no data, or LH_IO_ERROR for a malformed or truncated header.
 * @pre Self is valid and in the documented phase.
 */
lh_ssize_t
lh_compress_gzip_reader_try_step_header(lh_compress_gzip_reader_t *self);

/**
 * @brief Read from a member's DEFLATE stream and accumulate CRC and size.
 * @param self Initialized reader in the DEFLATE phase.
 * @param output Writable destination borrowed for this call.
 * @param size Positive destination capacity.
 * @return Positive decoded count, zero after transition to trailer,
 *         LH_IO_PENDING for source availability, or LH_IO_ERROR on failure.
 * @pre Self and output are valid, size is nonzero and phase is DEFLATE.
 */
lh_ssize_t
lh_compress_gzip_reader_try_step_deflate(lh_compress_gzip_reader_t *self, lh_ptr output,
                                         lh_usize_t size);

/**
 * @brief Read and validate a member's gzip trailer.
 * @param self Initialized reader in the trailer phase.
 * @return Zero for progress, LH_IO_PENDING for source availability, or
 *         LH_IO_ERROR for truncation, metadata limit or integrity failure.
 * @pre Self is valid and is in the trailer phase.
 */
lh_ssize_t
lh_compress_gzip_reader_try_step_trailer(lh_compress_gzip_reader_t *self);

/**
 * @brief Dispatch one gzip reader step and mark errors as sticky.
 * @param self Initialized reader.
 * @param output Writable destination for any decoded bytes.
 * @param size Positive destination capacity.
 * @return Positive decoded count, zero for progress or completion,
 *         LH_IO_PENDING when source input is unavailable, or LH_IO_ERROR.
 * @pre Self and output are valid, and size is nonzero.
 */
lh_ssize_t
lh_compress_gzip_reader_try_step(lh_compress_gzip_reader_t *self, lh_ptr output, lh_usize_t size);

/**
 * @brief Read uncompressed gzip bytes, validating each member before final EOF.
 * @param context Initialized reader, borrowed for the call.
 * @param output Caller-owned writable output.
 * @param size Output capacity; zero returns zero.
 * @return Positive byte count, zero after all members validate, LH_IO_PENDING,
 *         or LH_IO_ERROR for malformed, truncated, oversized or corrupt input.
 *         Errors can follow earlier positive reads and remain sticky.
 * @pre Context is valid; nonzero size has that many writable bytes.
 */
lh_ssize_t
lh_compress_gzip_reader_read(lh_self_ptr context, lh_ptr output, lh_usize_t size);

/**
 * @brief Construct an LH reader borrowing this gzip reader.
 * @param self Initialized gzip reader, valid while output is used.
 * @param output Caller-owned LH reader to initialize.
 */
lh_void
lh_compress_gzip_reader_get_reader(lh_compress_gzip_reader_t *self, lh_io_reader_t *output);

/**
 * @brief Checked form of ::lh_compress_gzip_reader_try_step_header.
 * A pending result is returned unchanged; an I/O error fails the runtime check.
 * @copydetails lh_compress_gzip_reader_try_step_header
 * @fails ::lh_runtime_error_code_invalid_range The operation reports an error.
 */
lh_ssize_t
lh_compress_gzip_reader_step_header(lh_compress_gzip_reader_t *self);

/**
 * @brief Checked form of ::lh_compress_gzip_reader_try_step_deflate.
 * A pending result is returned unchanged; an I/O error fails the runtime check.
 * @copydetails lh_compress_gzip_reader_try_step_deflate
 * @fails ::lh_runtime_error_code_invalid_range The operation reports an error.
 */
lh_ssize_t
lh_compress_gzip_reader_step_deflate(lh_compress_gzip_reader_t *self, lh_ptr output,
                                     lh_usize_t size);

/**
 * @brief Checked form of ::lh_compress_gzip_reader_try_step_trailer.
 * A pending result is returned unchanged; an I/O error fails the runtime check.
 * @copydetails lh_compress_gzip_reader_try_step_trailer
 * @fails ::lh_runtime_error_code_invalid_range The operation reports an error.
 */
lh_ssize_t
lh_compress_gzip_reader_step_trailer(lh_compress_gzip_reader_t *self);

/**
 * @brief Checked form of ::lh_compress_gzip_reader_try_step.
 * A pending result is returned unchanged; an I/O error fails the runtime check.
 * @copydetails lh_compress_gzip_reader_try_step
 * @fails ::lh_runtime_error_code_invalid_range The operation reports an error.
 */
lh_ssize_t
lh_compress_gzip_reader_step(lh_compress_gzip_reader_t *self, lh_ptr output, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_COMPRESS_GZIP_READER_H */
