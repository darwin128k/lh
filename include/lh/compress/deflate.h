/**
 * @file deflate.h
 * @brief Bounded, allocation-free DEFLATE decoding.
 *
 * The decoder consumes a complete raw DEFLATE stream from borrowed input and
 * writes to caller-owned output. It accepts stored, fixed-Huffman and dynamic-
 * Huffman blocks. Input and output must not overlap. Errors include malformed
 * codes, truncated input, invalid back references and insufficient output.
 */

#ifndef LH_COMPRESS_DEFLATE_H
#define LH_COMPRESS_DEFLATE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Read up to 16 low-bit-first bits from a borrowed input buffer.
 * @param input Borrowed bytes, valid for @p size bytes.
 * @param size Input size in bytes.
 * @param position In/out bit position; unchanged on failure.
 * @param count Number of bits, at most 16.
 * @param value Receives the bits on success, unchanged on failure.
 * @return True on success, false if the input ends before the requested bits.
 * @pre Counters are valid, count <= 16, and the buffer is valid for size.
 */
lh_bool_t
lh_compress_deflate_try_bits(const lh_byte_t *input, lh_usize_t size, lh_usize_t *position,
                             lh_uint_t count, lh_uint_t *value);

/**
 * @brief Build canonical Huffman length counts and ordered symbol numbers.
 * @param lengths Borrowed code lengths, each at most 15.
 * @param size Number of code lengths, at most 288.
 * @param counts Writable array of 16 entries.
 * @param symbols Writable array of at least @p size entries.
 * @return False for invalid lengths or oversubscribed codes; true otherwise.
 * Output arrays may be partially modified on failure.
 * @pre Buffers are valid for their documented sizes; size <= 288.
 */
lh_bool_t
lh_compress_deflate_try_build(const lh_byte_t *lengths, lh_usize_t size, lh_u16_t *counts,
                              lh_u16_t *symbols);

/**
 * @brief Decode one symbol using a canonical Huffman table.
 * @param input Borrowed compressed bytes.
 * @param size Input size in bytes.
 * @param position In/out bit position, unchanged on failure.
 * @param counts Counts produced by ::lh_compress_deflate_try_build.
 * @param symbols Symbol table produced by the same build operation.
 * @param symbol Receives the symbol on success, unchanged on failure.
 * @return True on success; false for truncated input or an invalid code.
 * @pre All pointers and Huffman tables are valid for their documented sizes.
 */
lh_bool_t
lh_compress_deflate_try_symbol(const lh_byte_t *input, lh_usize_t size, lh_usize_t *position,
                               const lh_u16_t *counts, const lh_u16_t *symbols, lh_uint_t *symbol);

/**
 * @brief Decode a single stored block after its three-bit block header.
 *
 * Aligns the input position to the next byte, verifies LEN and NLEN, and
 * appends the bytes to the caller-owned output buffer. Buffers do not overlap.
 * @param input Borrowed DEFLATE input buffer.
 * @param input_size Number of readable input bytes.
 * @param position In/out bit offset immediately after the block header.
 * @param output Borrowed writable output buffer.
 * @param capacity Number of writable output bytes.
 * @param written In/out number of output bytes already present.
 * @return True on success; false on truncation, invalid lengths, arithmetic
 *         overflow, or insufficient output. Both offsets remain unchanged on
 *         failure. No memory is allocated or transferred.
 * @pre All pointers are valid and buffers hold their stated sizes.
 */
lh_bool_t
lh_compress_deflate_try_stored(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *position,
                               lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written);

/**
 * @brief Decode a stored block, checking ::lh_compress_deflate_try_stored.
 * @copydetails lh_compress_deflate_try_stored
 * @fails ::lh_runtime_error_code_invalid_range The stored block is invalid.
 */
lh_void
lh_compress_deflate_stored(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *position,
                           lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written);

/**
 * @brief Decode one raw DEFLATE stream into caller-owned storage.
 * @param input Borrowed input bytes; valid for input_size bytes.
 * @param input_size Input length in bytes.
 * @param output Borrowed writable output; valid for capacity bytes.
 * @param capacity Output capacity in bytes.
 * @param consumed Receives the number of input bytes consumed on success.
 * @param written Receives the number of decoded bytes on success.
 * @return True on a complete valid stream; false on malformed or truncated
 *         data, invalid back-references, or insufficient output space. The
 *         output buffer may have been modified on failure; output counters
 *         remain unchanged. No allocation or ownership transfer occurs.
 * @pre Input, output and counters are valid, buffers hold their stated sizes and do not overlap.
 */
lh_bool_t
lh_compress_deflate_try_decode(const lh_byte_t *input, lh_usize_t input_size, lh_byte_t *output,
                               lh_usize_t capacity, lh_usize_t *consumed, lh_usize_t *written);

/** Checked form of ::lh_compress_deflate_try_decode; fails on invalid input. */
lh_void
lh_compress_deflate_decode(const lh_byte_t *input, lh_usize_t input_size, lh_byte_t *output,
                           lh_usize_t capacity, lh_usize_t *consumed, lh_usize_t *written);

/** Checked form of ::lh_compress_deflate_try_bits; aborts on failure. */
lh_void
lh_compress_deflate_bits(const lh_byte_t *input, lh_usize_t size, lh_usize_t *position,
                         lh_uint_t count, lh_uint_t *value);

/** Checked form of ::lh_compress_deflate_try_build; aborts on failure. */
lh_void
lh_compress_deflate_build(const lh_byte_t *lengths, lh_usize_t size, lh_u16_t *counts,
                          lh_u16_t *symbols);

/** Checked form of ::lh_compress_deflate_try_symbol; aborts on failure. */
lh_void
lh_compress_deflate_symbol(const lh_byte_t *input, lh_usize_t size, lh_usize_t *position,
                           const lh_u16_t *counts, const lh_u16_t *symbols, lh_uint_t *symbol);

LH_COMPILER_EXTERN_C_END

#endif /* LH_COMPRESS_DEFLATE_H */
