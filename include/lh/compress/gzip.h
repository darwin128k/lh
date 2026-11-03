/**
 * @file gzip.h
 * @brief Decode and encode complete gzip members in caller-owned buffers.
 *
 * This bounded, allocation-free codec composes the LH DEFLATE and CRC-32
 * bricks. The decoder accepts concatenated gzip members and validates headers,
 * checksums and member sizes before reporting success. The encoder writes one
 * member: a minimal header, a DEFLATE stream from ::lh_compress_deflate_try_encode
 * and the trailer. Input and output must not overlap.
 */

#ifndef LH_COMPRESS_GZIP_H
#define LH_COMPRESS_GZIP_H

#include <lh/compress/deflate.h>
#include <lh/compress/gzip/header/format.h>

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
 * @brief Compare a gzip trailer with an already computed CRC and byte count.
 *
 * This form lets a streaming decoder validate its accumulated CRC and size
 * without retaining uncompressed output. The count is encoded modulo 2^32.
 * @param trailer Borrowed trailer bytes.
 * @param trailer_size Number of readable trailer bytes.
 * @param crc Finalized CRC-32 of the uncompressed member.
 * @param decoded_size Number of uncompressed member bytes.
 * @return True if a complete trailer matches; false for truncation or mismatch.
 * @pre Trailer is valid for its stated size. No data is retained.
 */
lh_bool_t
lh_compress_gzip_try_trailer_values(const lh_byte_t *trailer, lh_usize_t trailer_size, lh_u32_t crc,
                                    lh_usize_t decoded_size);

/** @brief Checked form of ::lh_compress_gzip_try_trailer_values. */
lh_void
lh_compress_gzip_trailer_values(const lh_byte_t *trailer, lh_usize_t trailer_size, lh_u32_t crc,
                                lh_usize_t decoded_size);

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

/**
 * @brief Append a minimal ten-byte gzip member header.
 *
 * Writes the identification bytes, the DEFLATE method, no flags, a zero
 * modification time, no extra flags and the "unknown" operating system.
 * @param output Borrowed writable output, valid for @p capacity bytes.
 * @param capacity Output size in bytes.
 * @param written In/out count of bytes already in @p output; the header is
 *                appended after them and the count grows by
 *                ::LH_COMPRESS_GZIP_HEADER_SIZE on success.
 * @return True on success; false if the header does not fit, with output and
 *         count unchanged.
 * @pre Output and written are valid.
 */
lh_bool_t
lh_compress_gzip_try_put_header(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written);

/** @brief Checked form of ::lh_compress_gzip_try_put_header. */
lh_void
lh_compress_gzip_put_header(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written);

/**
 * @brief Append a gzip member trailer: the inverse of ::lh_compress_gzip_try_trailer_values.
 * @param crc Finalized CRC-32 of the uncompressed member.
 * @param decoded_size Number of uncompressed bytes; stored modulo 2^32.
 * @param output Borrowed writable output, valid for @p capacity bytes.
 * @param capacity Output size in bytes.
 * @param written In/out count of bytes already in @p output; grows by
 *                ::LH_COMPRESS_GZIP_TRAILER_SIZE on success.
 * @return True on success; false if the trailer does not fit, with output and
 *         count unchanged.
 * @pre Output and written are valid.
 */
lh_bool_t
lh_compress_gzip_try_put_trailer(lh_u32_t crc, lh_usize_t decoded_size, lh_byte_t *output,
                                 lh_usize_t capacity, lh_usize_t *written);

/** @brief Checked form of ::lh_compress_gzip_try_put_trailer. */
lh_void
lh_compress_gzip_put_trailer(lh_u32_t crc, lh_usize_t decoded_size, lh_byte_t *output,
                             lh_usize_t capacity, lh_usize_t *written);

/**
 * @brief Largest member ::lh_compress_gzip_try_encode writes for an input size.
 *
 * The stored DEFLATE size (::lh_compress_deflate_try_stored_bound) plus the
 * header and trailer; an output of this size always suffices.
 * @param size Input size in bytes.
 * @param bound Receives the size in bytes on success.
 * @return True on success; false if the size overflows, with @p bound unchanged.
 * @pre @p bound is valid.
 */
lh_bool_t
lh_compress_gzip_try_bound(lh_usize_t size, lh_usize_t *bound);

/** @brief Checked form of ::lh_compress_gzip_try_bound. */
lh_void
lh_compress_gzip_bound(lh_usize_t size, lh_usize_t *bound);

/**
 * @brief Encode an input as one gzip member.
 * @param matcher Caller-owned DEFLATE matcher, used as scratch.
 * @param input Borrowed input; may be null when @p input_size is zero.
 * @param input_size Input size in bytes.
 * @param output Borrowed writable output, valid for @p capacity bytes.
 * @param capacity Output size; ::lh_compress_gzip_try_bound always suffices.
 * @param written Receives the member size in bytes on success.
 * @return True on success; false for insufficient output, with @p written
 *         unchanged. Output bytes and the matcher may be modified on failure.
 * @pre Pointers are valid, buffers hold their stated sizes and do not overlap.
 */
lh_bool_t
lh_compress_gzip_try_encode(lh_compress_deflate_matcher_t *matcher, const lh_byte_t *input,
                            lh_usize_t input_size, lh_byte_t *output, lh_usize_t capacity,
                            lh_usize_t *written);

/** @brief Checked form of ::lh_compress_gzip_try_encode. */
lh_void
lh_compress_gzip_encode(lh_compress_deflate_matcher_t *matcher, const lh_byte_t *input,
                        lh_usize_t input_size, lh_byte_t *output, lh_usize_t capacity,
                        lh_usize_t *written);

LH_COMPILER_EXTERN_C_END

#endif /* LH_COMPRESS_GZIP_H */
