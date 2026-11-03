/**
 * @file deflate.h
 * @brief Bounded, allocation-free DEFLATE decoding and encoding.
 *
 * The decoder consumes a complete raw DEFLATE stream from borrowed input and
 * writes to caller-owned output. It accepts stored, fixed-Huffman and dynamic-
 * Huffman blocks. Input and output must not overlap. Errors include malformed
 * codes, truncated input, invalid back references and insufficient output.
 *
 * The encoder is built from the same tables. Bit, code, symbol, match and
 * stored-block writers each append to caller-owned output at a bit position;
 * the stream encoders compose them with the hash-chain matcher
 * (`lh/compress/deflate/matcher.h`) into stored or fixed-Huffman streams.
 * Nothing is allocated; the matcher and all buffers belong to the caller.
 */

#ifndef LH_COMPRESS_DEFLATE_H
#define LH_COMPRESS_DEFLATE_H

#include <lh/compress/deflate/matcher.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/ptr.h>

/** Code length of every distance symbol in a fixed-Huffman block. */
#define LH_COMPRESS_DEFLATE_FIXED_DISTANCE_BITS 5U

/** Largest payload of one stored block, in bytes. */
#define LH_COMPRESS_DEFLATE_STORED_MAX 65535U

/** Bytes a stored block adds to its payload: header, padding, LEN and NLEN. */
#define LH_COMPRESS_DEFLATE_STORED_OVERHEAD 5U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Base match length of a length symbol.
 * @param index Length symbol minus 257, from 0 through 28.
 * @return The shortest length the symbol encodes, from 3 through 258.
 * @pre @p index is at most 28; not checked.
 */
lh_uint_t
lh_compress_deflate_get_length_base(lh_uint_t index);

/**
 * @brief Number of extra bits that follow a length symbol.
 * @param index Length symbol minus 257, from 0 through 28.
 * @return From 0 through 5.
 * @pre @p index is at most 28; not checked.
 */
lh_uint_t
lh_compress_deflate_get_length_extra(lh_uint_t index);

/**
 * @brief Base distance of a distance code.
 * @param code Distance code from 0 through 29.
 * @return The shortest distance the code encodes, from 1 through 24577.
 * @pre @p code is at most 29; not checked.
 */
lh_uint_t
lh_compress_deflate_get_distance_base(lh_uint_t code);

/**
 * @brief Number of extra bits that follow a distance code.
 * @param code Distance code from 0 through 29.
 * @return From 0 through 13.
 * @pre @p code is at most 29; not checked.
 */
lh_uint_t
lh_compress_deflate_get_distance_extra(lh_uint_t code);

/**
 * @brief Length index whose range contains a match length.
 * @param length Match length from 3 through 258.
 * @return The largest index whose ::lh_compress_deflate_get_length_base does
 *         not exceed @p length; the length symbol is 257 plus this index.
 * @pre @p length is from 3 through 258; not checked.
 */
lh_uint_t
lh_compress_deflate_get_length_index(lh_uint_t length);

/**
 * @brief Distance code whose range contains a distance.
 * @param distance Distance from 1 through 32768.
 * @return The largest code whose ::lh_compress_deflate_get_distance_base does
 *         not exceed @p distance.
 * @pre @p distance is from 1 through 32768; not checked.
 */
lh_uint_t
lh_compress_deflate_get_distance_code(lh_uint_t distance);

/**
 * @brief Code length of a literal/length symbol in a fixed-Huffman block.
 * @param symbol Literal/length symbol from 0 through 287.
 * @return 8 for 0-143, 9 for 144-255, 7 for 256-279 and 8 for 280-287.
 * @pre @p symbol is at most 287; not checked.
 */
lh_uint_t
lh_compress_deflate_get_fixed_length(lh_uint_t symbol);

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
 * @brief Check whether Huffman length counts form an allowed DEFLATE tree.
 * @param counts Borrowed 16-entry code length counts from table construction.
 * @param allow_single Permit a single one-bit symbol.
 * @param allow_empty Permit a tree with no symbols, used for unused distances.
 * @return True for a complete tree or an explicitly allowed special case;
 *         false for oversubscribed or otherwise incomplete trees.
 * @pre Counts is valid and contains at most 320 total symbols.
 */
lh_bool_t
lh_compress_deflate_is_tree_valid(const lh_u16_t *counts, lh_bool_t allow_single,
                                  lh_bool_t allow_empty);

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
 * @brief Read the byte-aligned LEN and NLEN of a stored DEFLATE block.
 * @param input Borrowed compressed input.
 * @param input_size Number of readable bytes.
 * @param position In/out bit position after the three-bit block header.
 * @param length Receives the stored payload length.
 * @return True on success, false on truncated input, overflow or mismatched
 *         LEN/NLEN. Position and length remain unchanged on failure.
 * @pre Input, position and length are valid; input holds input_size bytes.
 */
lh_bool_t
lh_compress_deflate_try_stored_header(const lh_byte_t *input, lh_usize_t input_size,
                                      lh_usize_t *position, lh_uint_t *length);

/** @brief Checked form of ::lh_compress_deflate_try_stored_header. */
lh_void
lh_compress_deflate_stored_header(const lh_byte_t *input, lh_usize_t input_size,
                                  lh_usize_t *position, lh_uint_t *length);

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
 * @brief Build the fixed DEFLATE literal/length and distance code tables.
 * @param lit_counts Caller-owned 16-entry length-count array.
 * @param lit_symbols Caller-owned 288-entry literal/length symbol array.
 * @param dist_counts Caller-owned 16-entry distance length-count array.
 * @param dist_symbols Caller-owned 32-entry distance symbol array.
 * @return True on success, false if table construction fails. Arrays may be
 *         modified on failure; no memory is retained.
 * @pre Arrays are valid and have the stated capacities.
 */
lh_bool_t
lh_compress_deflate_try_fixed(lh_u16_t *lit_counts, lh_u16_t *lit_symbols, lh_u16_t *dist_counts,
                              lh_u16_t *dist_symbols);

/** @brief Checked form of ::lh_compress_deflate_try_fixed. */
lh_void
lh_compress_deflate_fixed(lh_u16_t *lit_counts, lh_u16_t *lit_symbols, lh_u16_t *dist_counts,
                          lh_u16_t *dist_symbols);

/**
 * @brief Read a dynamic DEFLATE block's Huffman tables after its block header.
 * @param input Borrowed compressed bytes.
 * @param input_size Readable input size in bytes.
 * @param position In/out bit offset after the three-bit block header; unchanged
 *                 on failure.
 * @param lit_counts Caller-owned 16-entry literal/length counts.
 * @param lit_symbols Caller-owned 288-entry literal/length symbol table.
 * @param dist_counts Caller-owned 16-entry distance counts.
 * @param dist_symbols Caller-owned 32-entry distance symbol table.
 * @return True for complete valid tables; false on truncation or invalid code
 *         lengths. Tables may be partially modified on failure.
 * @pre All pointers are valid, with arrays of the stated capacities.
 */
lh_bool_t
lh_compress_deflate_try_dynamic(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *position,
                                lh_u16_t *lit_counts, lh_u16_t *lit_symbols, lh_u16_t *dist_counts,
                                lh_u16_t *dist_symbols);

/** @brief Checked form of ::lh_compress_deflate_try_dynamic. */
lh_void
lh_compress_deflate_dynamic(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *position,
                            lh_u16_t *lit_counts, lh_u16_t *lit_symbols, lh_u16_t *dist_counts,
                            lh_u16_t *dist_symbols);

/**
 * @brief Decode the length and distance associated with a length symbol.
 *
 * Reads the symbol's extra length bits, one distance code and its extra bits.
 * The bit offset and both outputs change only after the whole match is read.
 * The caller retains all input and table storage; no allocation occurs.
 * @param input Borrowed compressed bytes.
 * @param input_size Number of readable input bytes.
 * @param position In/out bit position immediately after the length symbol.
 * @param symbol Decoded length symbol in the range 257 through 285.
 * @param dist_counts Valid 16-entry distance code length counts.
 * @param dist_symbols Valid 32-entry ordered distance symbols.
 * @param length Receives match length on success.
 * @param distance Receives positive match distance on success.
 * @return True on success; false for invalid symbol, distance code, or
 *         truncated input. Position and outputs remain unchanged on failure.
 * @pre All pointers are valid, output pointers are distinct, and arrays have
 *      the stated capacities.
 */
lh_bool_t
lh_compress_deflate_try_match(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *position,
                              lh_uint_t symbol, const lh_u16_t *dist_counts,
                              const lh_u16_t *dist_symbols, lh_uint_t *length, lh_uint_t *distance);

/**
 * @brief Decode a match, checking ::lh_compress_deflate_try_match.
 * @copydetails lh_compress_deflate_try_match
 * @fails ::lh_runtime_error_code_invalid_range Match data is invalid or incomplete.
 */
lh_void
lh_compress_deflate_match(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *position,
                          lh_uint_t symbol, const lh_u16_t *dist_counts,
                          const lh_u16_t *dist_symbols, lh_uint_t *length, lh_uint_t *distance);

/**
 * @brief Expand a DEFLATE back-reference into an existing output buffer.
 *
 * Appends @p length bytes by repeating the @p distance bytes immediately
 * before the current end. This handles overlapping references, including a
 * distance of one. The caller owns the buffer; no allocation occurs.
 * @param output Borrowed writable buffer holding the previously decoded bytes.
 * @param capacity Number of writable bytes in @p output.
 * @param written In/out count of already decoded bytes; changed only on success.
 * @param distance Positive backwards distance, at most the existing count and
 *                 at most 32768 bytes as required by DEFLATE.
 * @param length Number of bytes to append, which may be zero.
 * @return True on success; false if the distance is invalid, written exceeds
 *         capacity, or there is insufficient remaining space. On failure the
 *         output and written count remain unchanged.
 * @pre Output and written are valid and the buffer holds capacity bytes.
 */
lh_bool_t
lh_compress_deflate_try_copy_match(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written,
                                   lh_usize_t distance, lh_usize_t length);

/**
 * @brief Expand a back-reference, checking ::lh_compress_deflate_try_copy_match.
 * @copydetails lh_compress_deflate_try_copy_match
 * @fails ::lh_runtime_error_code_invalid_range The reference or capacity is invalid.
 */
lh_void
lh_compress_deflate_copy_match(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written,
                               lh_usize_t distance, lh_usize_t length);

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

/**
 * @brief Write up to 16 low-bit-first bits into a caller-owned buffer.
 *
 * The inverse of ::lh_compress_deflate_try_bits. Each written bit is set or
 * cleared, so the buffer needs no prior zeroing; bits outside the written
 * range are left as they are.
 * @param output Borrowed writable bytes, valid for @p capacity bytes.
 * @param capacity Output size in bytes.
 * @param position In/out bit position; unchanged on failure.
 * @param count Number of bits, at most 16.
 * @param value Bits to write, low bit first; bits above @p count are ignored.
 * @return True on success; false if the bits do not fit, with the output and
 *         position unchanged.
 * @pre Output and position are valid and count <= 16.
 */
lh_bool_t
lh_compress_deflate_try_put_bits(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position,
                                 lh_uint_t count, lh_uint_t value);

/** Checked form of ::lh_compress_deflate_try_put_bits; aborts on failure. */
lh_void
lh_compress_deflate_put_bits(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position,
                             lh_uint_t count, lh_uint_t value);

/**
 * @brief Number of zero bits that bring a bit position to a byte boundary.
 * @param position Bit position.
 * @return From 0 through 7; zero when @p position is already byte-aligned.
 */
lh_uint_t
lh_compress_deflate_get_pad_bits(lh_usize_t position);

/**
 * @brief Assign canonical Huffman codes to code lengths, ready to write.
 *
 * Builds the length counts with ::lh_compress_deflate_try_build, then gives
 * each symbol the canonical code of RFC 1951, section 3.2.2, mirrored with
 * ::lh_bit_reverse_u32 so ::lh_compress_deflate_try_put_bits writes it
 * high bit first. A symbol of length zero receives code zero.
 * @param lengths Borrowed code lengths, each at most 15.
 * @param size Number of code lengths, at most 288.
 * @param codes Writable array of at least @p size entries.
 * @return False for invalid lengths or an oversubscribed code; true otherwise.
 *         Codes may be partially written on failure.
 * @pre Buffers are valid for their documented sizes; size <= 288.
 */
lh_bool_t
lh_compress_deflate_try_codes(const lh_byte_t *lengths, lh_usize_t size, lh_u16_t *codes);

/** Checked form of ::lh_compress_deflate_try_codes; aborts on failure. */
lh_void
lh_compress_deflate_codes(const lh_byte_t *lengths, lh_usize_t size, lh_u16_t *codes);

/**
 * @brief Write one Huffman-coded symbol.
 * @param output Borrowed writable bytes, valid for @p capacity bytes.
 * @param capacity Output size in bytes.
 * @param position In/out bit position; unchanged on failure.
 * @param codes Codes from ::lh_compress_deflate_try_codes.
 * @param lengths The code lengths the codes were built from.
 * @param symbol Symbol to write; an index into both tables.
 * @return True on success; false if the symbol has no code or does not fit.
 *         Output and position are unchanged on failure.
 * @pre All pointers are valid and @p symbol indexes both tables.
 */
lh_bool_t
lh_compress_deflate_try_put_symbol(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position,
                                   const lh_u16_t *codes, const lh_byte_t *lengths,
                                   lh_uint_t symbol);

/** Checked form of ::lh_compress_deflate_try_put_symbol; aborts on failure. */
lh_void
lh_compress_deflate_put_symbol(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position,
                               const lh_u16_t *codes, const lh_byte_t *lengths, lh_uint_t symbol);

/**
 * @brief Write a length/distance pair: the inverse of ::lh_compress_deflate_try_match.
 *
 * Writes the length symbol, its extra bits, the distance code and its extra
 * bits. The position changes only after the whole pair is written.
 * @param output Borrowed writable bytes, valid for @p capacity bytes.
 * @param capacity Output size in bytes.
 * @param position In/out bit position; unchanged on failure.
 * @param lit_codes Literal/length codes of at least 286 entries.
 * @param lit_lengths Their code lengths.
 * @param dist_codes Distance codes of at least 30 entries.
 * @param dist_lengths Their code lengths.
 * @param length Match length from 3 through 258.
 * @param distance Match distance from 1 through 32768.
 * @return True on success; false for an out-of-range length or distance, a
 *         symbol without a code, or insufficient output. The position is
 *         unchanged on failure; output bytes after it may have been modified.
 * @pre All pointers are valid and the tables have the stated sizes.
 */
lh_bool_t
lh_compress_deflate_try_put_match(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position,
                                  const lh_u16_t *lit_codes, const lh_byte_t *lit_lengths,
                                  const lh_u16_t *dist_codes, const lh_byte_t *dist_lengths,
                                  lh_uint_t length, lh_uint_t distance);

/** Checked form of ::lh_compress_deflate_try_put_match; aborts on failure. */
lh_void
lh_compress_deflate_put_match(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position,
                              const lh_u16_t *lit_codes, const lh_byte_t *lit_lengths,
                              const lh_u16_t *dist_codes, const lh_byte_t *dist_lengths,
                              lh_uint_t length, lh_uint_t distance);

/**
 * @brief Write one stored block: the inverse of ::lh_compress_deflate_try_stored.
 *
 * Writes the three-bit block header, zero padding to the next byte, LEN, NLEN
 * and the payload copied with ::lh_memory_std_copy.
 * @param input Borrowed payload of at most ::LH_COMPRESS_DEFLATE_STORED_MAX bytes.
 * @param input_size Payload size in bytes.
 * @param final Whether this block ends the stream.
 * @param output Borrowed writable bytes, valid for @p capacity bytes.
 * @param capacity Output size in bytes.
 * @param position In/out bit position; on success it is byte-aligned after the
 *                 payload.
 * @return True on success; false for an oversized payload or insufficient
 *         output. The position is unchanged on failure; output bytes after it
 *         may have been modified.
 * @pre All pointers are valid, buffers hold their stated sizes and do not overlap.
 */
lh_bool_t
lh_compress_deflate_try_put_stored(const lh_byte_t *input, lh_usize_t input_size, lh_bool_t final,
                                   lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position);

/** Checked form of ::lh_compress_deflate_try_put_stored; aborts on failure. */
lh_void
lh_compress_deflate_put_stored(const lh_byte_t *input, lh_usize_t input_size, lh_bool_t final,
                               lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position);

/**
 * @brief Exact size of ::lh_compress_deflate_try_encode_stored output.
 *
 * Every started ::LH_COMPRESS_DEFLATE_STORED_MAX bytes, and an empty input,
 * cost one block of ::LH_COMPRESS_DEFLATE_STORED_OVERHEAD bytes on top of the
 * payload. It is also the most ::lh_compress_deflate_try_encode ever writes,
 * so it sizes a caller's output buffer.
 * @param size Input size in bytes.
 * @param bound Receives the size in bytes on success.
 * @return True on success; false if the size overflows ::lh_usize_t, with
 *         @p bound unchanged.
 * @pre @p bound is valid.
 */
lh_bool_t
lh_compress_deflate_try_stored_bound(lh_usize_t size, lh_usize_t *bound);

/** Checked form of ::lh_compress_deflate_try_stored_bound; aborts on overflow. */
lh_void
lh_compress_deflate_stored_bound(lh_usize_t size, lh_usize_t *bound);

/**
 * @brief Encode an input as a raw DEFLATE stream of stored blocks.
 * @param input Borrowed input; may be null when @p input_size is zero.
 * @param input_size Input size in bytes.
 * @param output Borrowed writable output, valid for @p capacity bytes.
 * @param capacity Output size; ::lh_compress_deflate_try_stored_bound is enough.
 * @param written Receives the stream size in bytes on success.
 * @return True on success; false for insufficient output, with @p written
 *         unchanged. Output bytes may have been modified on failure.
 * @pre Pointers are valid, buffers hold their stated sizes and do not overlap.
 */
lh_bool_t
lh_compress_deflate_try_encode_stored(const lh_byte_t *input, lh_usize_t input_size,
                                      lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written);

/** Checked form of ::lh_compress_deflate_try_encode_stored; aborts on failure. */
lh_void
lh_compress_deflate_encode_stored(const lh_byte_t *input, lh_usize_t input_size, lh_byte_t *output,
                                  lh_usize_t capacity, lh_usize_t *written);

/**
 * @brief Write the codes of a fixed-Huffman block body, ending with symbol 256.
 *
 * Resets @p matcher with ::lh_compress_deflate_matcher_reset, keeping its
 * chain limit, then takes the longest match at each position
 * (::lh_compress_deflate_matcher_try_find) or a literal, and writes them with
 * ::lh_compress_deflate_try_put_match and ::lh_compress_deflate_try_put_symbol.
 * The three-bit block header is the caller's, so the body can follow any
 * header the caller writes.
 * @param matcher Caller-owned matcher, used as scratch; its chain limit applies.
 * @param input Borrowed input; may be null when @p input_size is zero.
 * @param input_size Input size, at most ::LH_COMPRESS_DEFLATE_MATCHER_INPUT_LIMIT.
 * @param output Borrowed writable output, valid for @p capacity bytes.
 * @param capacity Output size in bytes.
 * @param position In/out bit position, just after the block header.
 * @return True on success; false for an oversized input or insufficient
 *         output. The position is unchanged on failure; output bytes after it
 *         and the matcher's tables may have been modified.
 * @pre Pointers are valid, buffers hold their stated sizes and do not overlap.
 */
lh_bool_t
lh_compress_deflate_try_put_fixed_codes(lh_compress_deflate_matcher_t *matcher,
                                        const lh_byte_t *input, lh_usize_t input_size,
                                        lh_byte_t *output, lh_usize_t capacity,
                                        lh_usize_t *position);

/** Checked form of ::lh_compress_deflate_try_put_fixed_codes; aborts on failure. */
lh_void
lh_compress_deflate_put_fixed_codes(lh_compress_deflate_matcher_t *matcher, const lh_byte_t *input,
                                    lh_usize_t input_size, lh_byte_t *output, lh_usize_t capacity,
                                    lh_usize_t *position);

/**
 * @brief Encode an input as one fixed-Huffman raw DEFLATE block.
 *
 * Writes the final fixed-block header, the body with
 * ::lh_compress_deflate_try_put_fixed_codes, and zero bits up to the next byte.
 * @param matcher Caller-owned matcher, used as scratch; its chain limit applies.
 * @param input Borrowed input; may be null when @p input_size is zero.
 * @param input_size Input size, at most ::LH_COMPRESS_DEFLATE_MATCHER_INPUT_LIMIT.
 * @param output Borrowed writable output, valid for @p capacity bytes.
 * @param capacity Output size in bytes.
 * @param written Receives the stream size in bytes on success.
 * @return True on success; false for an oversized input or insufficient
 *         output, with @p written unchanged. Output bytes and the matcher's
 *         tables may have been modified on failure.
 * @pre Pointers are valid, buffers hold their stated sizes and do not overlap.
 */
lh_bool_t
lh_compress_deflate_try_encode_fixed(lh_compress_deflate_matcher_t *matcher, const lh_byte_t *input,
                                     lh_usize_t input_size, lh_byte_t *output, lh_usize_t capacity,
                                     lh_usize_t *written);

/** Checked form of ::lh_compress_deflate_try_encode_fixed; aborts on failure. */
lh_void
lh_compress_deflate_encode_fixed(lh_compress_deflate_matcher_t *matcher, const lh_byte_t *input,
                                 lh_usize_t input_size, lh_byte_t *output, lh_usize_t capacity,
                                 lh_usize_t *written);

/**
 * @brief Encode an input as the smaller of a fixed-Huffman or stored stream.
 *
 * Tries ::lh_compress_deflate_try_encode_fixed within one byte less than the
 * stored size; when that does not fit, falls back to
 * ::lh_compress_deflate_try_encode_stored. Incompressible input therefore
 * never grows beyond ::lh_compress_deflate_try_stored_bound.
 * @param matcher Caller-owned matcher, used as scratch; its chain limit applies.
 * @param input Borrowed input; may be null when @p input_size is zero.
 * @param input_size Input size in bytes.
 * @param output Borrowed writable output, valid for @p capacity bytes.
 * @param capacity Output size in bytes; the stored bound always suffices.
 * @param written Receives the stream size in bytes on success.
 * @return True on success; false for insufficient output or an input whose
 *         stored size overflows, with @p written unchanged. Output bytes and
 *         the matcher's tables may have been modified on failure.
 * @pre Pointers are valid, buffers hold their stated sizes and do not overlap.
 */
lh_bool_t
lh_compress_deflate_try_encode(lh_compress_deflate_matcher_t *matcher, const lh_byte_t *input,
                               lh_usize_t input_size, lh_byte_t *output, lh_usize_t capacity,
                               lh_usize_t *written);

/** Checked form of ::lh_compress_deflate_try_encode; aborts on failure. */
lh_void
lh_compress_deflate_encode(lh_compress_deflate_matcher_t *matcher, const lh_byte_t *input,
                           lh_usize_t input_size, lh_byte_t *output, lh_usize_t capacity,
                           lh_usize_t *written);

LH_COMPILER_EXTERN_C_END

#endif /* LH_COMPRESS_DEFLATE_H */
