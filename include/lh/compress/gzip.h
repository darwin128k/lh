/**
 * @file gzip.h
 * @brief Decode complete gzip members into a caller-owned buffer.
 *
 * This bounded, allocation-free decoder composes the LH DEFLATE and CRC-32
 * bricks. It accepts concatenated gzip members and validates headers, checksums
 * and member sizes before reporting success. Input and output must not overlap.
 */

#ifndef LH_COMPRESS_GZIP_H
#define LH_COMPRESS_GZIP_H

#include <lh/compress/deflate.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Validate one gzip member header, including optional header checksum.
 * @param input Borrowed member bytes starting at the header.
 * @param input_size Number of readable bytes.
 * @param consumed Receives header size only on success.
 * @return True for a complete valid header; false for invalid or truncated
 * input. Output remains unchanged on failure; no data is retained.
 * @pre Input and consumed are valid and input holds input_size readable bytes.
 */
lh_bool_t
lh_compress_gzip_try_header(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *consumed);

/** @brief Checked form of ::lh_compress_gzip_try_header. */
lh_void
lh_compress_gzip_header(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *consumed);

/**
 * @brief Validate the CRC-32 and size in a gzip member trailer.
 * @param trailer Borrowed trailer bytes.
 * @param trailer_size Readable trailer byte count.
 * @param decoded Borrowed uncompressed member bytes.
 * @param decoded_size Number of uncompressed bytes.
 * @return True if the trailer is complete and matches; false otherwise.
 * @pre Both buffers are valid for their stated sizes. No data is retained.
 */
lh_bool_t
lh_compress_gzip_try_trailer(const lh_byte_t *trailer, lh_usize_t trailer_size,
                             const lh_byte_t *decoded, lh_usize_t decoded_size);

/** @brief Checked form of ::lh_compress_gzip_try_trailer. */
lh_void
lh_compress_gzip_trailer(const lh_byte_t *trailer, lh_usize_t trailer_size,
                         const lh_byte_t *decoded, lh_usize_t decoded_size);

/**
 * @brief Decode and validate all members of a gzip stream.
 * @param input Borrowed readable input bytes.
 * @param input_size Input length, which must contain at least one member.
 * @param output Borrowed writable output buffer.
 * @param capacity Output capacity in bytes.
 * @param written Receives total decoded bytes only on success.
 * @return True for a valid complete gzip stream; false for invalid, incomplete,
 * unsupported, or oversized input. Output bytes may be modified on failure.
 * @pre Input, output and written are valid, buffers hold their sizes and do not overlap.
 */
lh_bool_t
lh_compress_gzip_try_decode(const lh_byte_t *input, lh_usize_t input_size, lh_byte_t *output,
                            lh_usize_t capacity, lh_usize_t *written);

/** Checked form of ::lh_compress_gzip_try_decode; fails on invalid input. */
lh_void
lh_compress_gzip_decode(const lh_byte_t *input, lh_usize_t input_size, lh_byte_t *output,
                        lh_usize_t capacity, lh_usize_t *written);

LH_COMPILER_EXTERN_C_END

#endif /* LH_COMPRESS_GZIP_H */
