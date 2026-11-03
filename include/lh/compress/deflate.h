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
 * stored-block writers each append to caller-owned output at a bit position.
 * The stream encoder works block by block: the hash-chain matcher
 * (`lh/compress/deflate/matcher.h`) collects a block's literals and matches
 * into a token buffer (`lh/compress/deflate/tokens.h`), the symbols are
 * counted, each block type is priced in bits from the counts, and only the
 * cheapest is written. Nothing is allocated; the encoder state
 * (`lh/compress/deflate/encoder.h`) and all buffers belong to the caller.
 */

#ifndef LH_COMPRESS_DEFLATE_H
#define LH_COMPRESS_DEFLATE_H

#include <lh/compress/deflate/encoder.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/ptr.h>

/** Code length of every distance symbol in a fixed-Huffman block. */
#define LH_COMPRESS_DEFLATE_FIXED_DISTANCE_BITS 5U

/** Literal/length codes of the fixed code, including the unused 286 and 287. */
#define LH_COMPRESS_DEFLATE_FIXED_LITERAL_CODES 288U

/** Distance codes of the fixed code, including the unused 30 and 31. */
#define LH_COMPRESS_DEFLATE_FIXED_DISTANCE_CODES 32U

/** Literal/length symbols a block may use: 256 literals, end of block, 29 lengths. */
#define LH_COMPRESS_DEFLATE_LITERAL_SYMBOLS 286U

/** Distance symbols a block may use. */
#define LH_COMPRESS_DEFLATE_DISTANCE_SYMBOLS 30U

/** Literal/length symbol that ends a Huffman-coded block. */
#define LH_COMPRESS_DEFLATE_END_OF_BLOCK 256U

/** First length symbol; the length index is the symbol minus this. */
#define LH_COMPRESS_DEFLATE_FIRST_LENGTH_SYMBOL 257U

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
 * @brief Fill the code lengths of the fixed Huffman code, RFC 1951, 3.2.6.
 *
 * Writes ::lh_compress_deflate_get_fixed_length for every literal/length code
 * and ::LH_COMPRESS_DEFLATE_FIXED_DISTANCE_BITS for every distance code. The
 * decoder builds its fixed tables and the encoder its fixed codes from these.
 * @param lit_lengths Writable array of ::LH_COMPRESS_DEFLATE_FIXED_LITERAL_CODES
 *                    entries.
 * @param dist_lengths Writable array of
 *                     ::LH_COMPRESS_DEFLATE_FIXED_DISTANCE_CODES entries.
 * @pre Both arrays are valid for their sizes; not checked.
 */
lh_void
lh_compress_deflate_fill_fixed_lengths(lh_byte_t *lit_lengths, lh_byte_t *dist_lengths);

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
 * @brief Number of blocks of at most @p block_size bytes that cover @p size.
 *
 * Every started block counts, and an empty input still takes one block, as a
 * DEFLATE stream always has at least one.
 * @param size Input size in bytes.
 * @param block_size Most bytes one block covers; not zero.
 * @return From 1 upwards.
 * @pre @p block_size is not zero; not checked.
 */
lh_usize_t
lh_compress_deflate_get_block_count(lh_usize_t size, lh_usize_t block_size);

/**
 * @brief Input size plus one stored-block overhead for every block covering it.
 *
 * Adds ::LH_COMPRESS_DEFLATE_STORED_OVERHEAD for each of
 * ::lh_compress_deflate_get_block_count blocks. A stream whose every block is
 * at most a stored block of its bytes, and whose blocks each span at least
 * @p block_size bytes but the last, never exceeds it.
 * @param size Input size in bytes.
 * @param block_size Fewest bytes a block other than the last spans; not zero.
 * @param bound Receives the size in bytes on success.
 * @return True on success; false if the size overflows ::lh_usize_t, with
 *         @p bound unchanged.
 * @pre @p bound is valid and @p block_size is not zero.
 */
lh_bool_t
lh_compress_deflate_try_blocks_bound(lh_usize_t size, lh_usize_t block_size, lh_usize_t *bound);

/** Checked form of ::lh_compress_deflate_try_blocks_bound; aborts on overflow. */
lh_void
lh_compress_deflate_blocks_bound(lh_usize_t size, lh_usize_t block_size, lh_usize_t *bound);

/**
 * @brief Exact size of ::lh_compress_deflate_try_encode_stored output.
 *
 * Every started ::LH_COMPRESS_DEFLATE_STORED_MAX bytes, and an empty input,
 * cost one block of ::LH_COMPRESS_DEFLATE_STORED_OVERHEAD bytes on top of the
 * payload.
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
 * @brief Count how often each symbol occurs in a block's tokens.
 *
 * A literal counts its byte; a match counts its length symbol (257 plus
 * ::lh_compress_deflate_get_length_index) and its distance code
 * (::lh_compress_deflate_get_distance_code). The end-of-block symbol counts
 * once, as every Huffman-coded block ends with it.
 * @param tokens Borrowed block tokens.
 * @param lit_freq Writable array of ::LH_COMPRESS_DEFLATE_LITERAL_SYMBOLS counts;
 *                 fully overwritten.
 * @param dist_freq Writable array of ::LH_COMPRESS_DEFLATE_DISTANCE_SYMBOLS
 *                  counts; fully overwritten.
 * @pre All pointers are valid; not checked.
 */
lh_void
lh_compress_deflate_count_symbols(const lh_compress_deflate_tokens_t *tokens, lh_usize_t *lit_freq,
                                  lh_usize_t *dist_freq);

/**
 * @brief Bits a Huffman-coded block body takes under given code lengths.
 *
 * Every counted symbol costs its code length; length symbols and distance
 * codes add their extra bits. The block header and any code description are
 * not included. A counted symbol of length zero adds nothing, so the result is
 * only meaningful for codes that cover every counted symbol.
 * @param lit_lengths Literal/length code lengths, at least
 *                    ::LH_COMPRESS_DEFLATE_LITERAL_SYMBOLS entries.
 * @param dist_lengths Distance code lengths, at least
 *                     ::LH_COMPRESS_DEFLATE_DISTANCE_SYMBOLS entries.
 * @param lit_freq Counts from ::lh_compress_deflate_count_symbols.
 * @param dist_freq Counts from ::lh_compress_deflate_count_symbols.
 * @return The body size in bits.
 * @pre All pointers are valid and the total fits ::lh_usize_t; not checked.
 *      One token buffer's counts always fit.
 */
lh_usize_t
lh_compress_deflate_get_codes_cost(const lh_byte_t *lit_lengths, const lh_byte_t *dist_lengths,
                                   const lh_usize_t *lit_freq, const lh_usize_t *dist_freq);

/**
 * @brief Bits ::lh_compress_deflate_try_put_stored adds at a bit position.
 *
 * The three-bit header, the padding to the next byte after it, LEN, NLEN and
 * the payload.
 * @param size Payload size, at most ::LH_COMPRESS_DEFLATE_STORED_MAX.
 * @param position Bit position the block starts at.
 * @return The block size in bits.
 * @pre size is within the limit; not checked.
 */
lh_usize_t
lh_compress_deflate_get_stored_cost(lh_usize_t size, lh_usize_t position);

/**
 * @brief Write a block body from its tokens, ending with the end-of-block symbol.
 *
 * Literals go through ::lh_compress_deflate_try_put_symbol, matches through
 * ::lh_compress_deflate_try_put_match. The block header is the caller's.
 * @param tokens Borrowed block tokens.
 * @param lit_codes Literal/length codes of at least
 *                  ::LH_COMPRESS_DEFLATE_LITERAL_SYMBOLS entries.
 * @param lit_lengths Their code lengths.
 * @param dist_codes Distance codes of at least
 *                   ::LH_COMPRESS_DEFLATE_DISTANCE_SYMBOLS entries.
 * @param dist_lengths Their code lengths.
 * @param output Borrowed writable output, valid for @p capacity bytes.
 * @param capacity Output size in bytes.
 * @param position In/out bit position, just after the block header.
 * @return True on success; false if a symbol has no code or the output is too
 *         small. The position is unchanged on failure; output bytes after it
 *         may have been modified.
 * @pre Pointers are valid and the tables have the stated sizes.
 */
lh_bool_t
lh_compress_deflate_try_put_tokens(const lh_compress_deflate_tokens_t *tokens,
                                   const lh_u16_t *lit_codes, const lh_byte_t *lit_lengths,
                                   const lh_u16_t *dist_codes, const lh_byte_t *dist_lengths,
                                   lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position);

/** Checked form of ::lh_compress_deflate_try_put_tokens; aborts on failure. */
lh_void
lh_compress_deflate_put_tokens(const lh_compress_deflate_tokens_t *tokens,
                               const lh_u16_t *lit_codes, const lh_byte_t *lit_lengths,
                               const lh_u16_t *dist_codes, const lh_byte_t *dist_lengths,
                               lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position);

/**
 * @brief Collect the tokens of the block that starts at @p begin.
 *
 * Empties @p tokens, then takes the longest match at each position
 * (::lh_compress_deflate_matcher_try_find) or a literal, inserting every
 * covered position into the matcher, until the buffer is full or the block
 * spans ::LH_COMPRESS_DEFLATE_STORED_MAX bytes. Matches never cross that
 * block limit, so the block fits one stored block; they may reach back into
 * earlier blocks. The matcher must hold exactly the positions before
 * @p begin, as after ::lh_compress_deflate_matcher_reset and earlier calls.
 * @param matcher Caller-owned matcher; its chain limit applies.
 * @param tokens Caller-owned buffer; receives the block's tokens.
 * @param input Borrowed input; may be null when @p input_size is zero.
 * @param input_size Input size, at most ::LH_COMPRESS_DEFLATE_MATCHER_INPUT_LIMIT.
 * @param begin First position of the block, at most @p input_size.
 * @param end Receives the position after the block on success; it equals
 *            @p begin only when @p begin is @p input_size.
 * @return True on success; false for an oversized input or a @p begin past the
 *         end, with nothing changed.
 * @pre Pointers are valid and the input holds @p input_size bytes.
 */
lh_bool_t
lh_compress_deflate_try_collect(lh_compress_deflate_matcher_t *matcher,
                                lh_compress_deflate_tokens_t *tokens, const lh_byte_t *input,
                                lh_usize_t input_size, lh_usize_t begin, lh_usize_t *end);

/** Checked form of ::lh_compress_deflate_try_collect; aborts on failure. */
lh_void
lh_compress_deflate_collect(lh_compress_deflate_matcher_t *matcher,
                            lh_compress_deflate_tokens_t *tokens, const lh_byte_t *input,
                            lh_usize_t input_size, lh_usize_t begin, lh_usize_t *end);

/**
 * @brief Write one block as the cheaper of fixed Huffman and stored.
 *
 * Counts the symbols with ::lh_compress_deflate_count_symbols and prices the
 * fixed block (three header bits plus ::lh_compress_deflate_get_codes_cost
 * under the fixed code) against ::lh_compress_deflate_get_stored_cost at the
 * current position. A stored block is written only when strictly cheaper, so
 * the block never takes more bits than a stored block of the same bytes.
 * @param tokens Borrowed tokens of the block.
 * @param input Borrowed bytes the tokens encode; may be null when
 *              @p input_size is zero.
 * @param input_size Their count, at most ::LH_COMPRESS_DEFLATE_STORED_MAX.
 * @param final Whether this block ends the stream.
 * @param output Borrowed writable output, valid for @p capacity bytes.
 * @param capacity Output size in bytes.
 * @param position In/out bit position of the block start.
 * @return True on success; false for an oversized block or insufficient
 *         output. The position is unchanged on failure; output bytes after it
 *         may have been modified.
 * @pre Pointers are valid and the tokens encode exactly the given bytes.
 */
lh_bool_t
lh_compress_deflate_try_put_block(const lh_compress_deflate_tokens_t *tokens,
                                  const lh_byte_t *input, lh_usize_t input_size, lh_bool_t final,
                                  lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position);

/** Checked form of ::lh_compress_deflate_try_put_block; aborts on failure. */
lh_void
lh_compress_deflate_put_block(const lh_compress_deflate_tokens_t *tokens, const lh_byte_t *input,
                              lh_usize_t input_size, lh_bool_t final, lh_byte_t *output,
                              lh_usize_t capacity, lh_usize_t *position);

/**
 * @brief Largest stream ::lh_compress_deflate_try_encode writes for a size.
 *
 * Every block is written at most as large as a stored block of its bytes,
 * and every block but the last spans at least
 * ::LH_COMPRESS_DEFLATE_TOKENS_LIMIT bytes. So the stream takes at most the
 * input size plus ::LH_COMPRESS_DEFLATE_STORED_OVERHEAD bytes for every
 * started ::LH_COMPRESS_DEFLATE_TOKENS_LIMIT bytes, and for an empty input.
 * @param size Input size in bytes.
 * @param bound Receives the size in bytes on success.
 * @return True on success; false if the size overflows ::lh_usize_t, with
 *         @p bound unchanged.
 * @pre @p bound is valid.
 */
lh_bool_t
lh_compress_deflate_try_bound(lh_usize_t size, lh_usize_t *bound);

/** Checked form of ::lh_compress_deflate_try_bound; aborts on overflow. */
lh_void
lh_compress_deflate_bound(lh_usize_t size, lh_usize_t *bound);

/**
 * @brief Encode an input as a raw DEFLATE stream, block by block.
 *
 * Resets the encoder's matcher, then repeats ::lh_compress_deflate_try_collect
 * and ::lh_compress_deflate_try_put_block until the input is consumed, marking
 * the last block final, and pads the stream to a whole byte. Each block is
 * matched once and written once. An empty input becomes one empty fixed
 * block.
 * @param encoder Caller-owned state from ::lh_compress_deflate_encoder_init,
 *                used as scratch; its chain limit applies.
 * @param input Borrowed input; may be null when @p input_size is zero.
 * @param input_size Input size, at most ::LH_COMPRESS_DEFLATE_MATCHER_INPUT_LIMIT.
 * @param output Borrowed writable output, valid for @p capacity bytes.
 * @param capacity Output size in bytes; ::lh_compress_deflate_try_bound always
 *                 suffices.
 * @param written Receives the stream size in bytes on success.
 * @return True on success; false for an oversized input or insufficient
 *         output, with @p written unchanged. Output bytes and the encoder
 *         state may have been modified on failure.
 * @pre Pointers are valid, buffers hold their stated sizes and do not overlap.
 */
lh_bool_t
lh_compress_deflate_try_encode(lh_compress_deflate_encoder_t *encoder, const lh_byte_t *input,
                               lh_usize_t input_size, lh_byte_t *output, lh_usize_t capacity,
                               lh_usize_t *written);

/** Checked form of ::lh_compress_deflate_try_encode; aborts on failure. */
lh_void
lh_compress_deflate_encode(lh_compress_deflate_encoder_t *encoder, const lh_byte_t *input,
                           lh_usize_t input_size, lh_byte_t *output, lh_usize_t capacity,
                           lh_usize_t *written);

LH_COMPILER_EXTERN_C_END

#endif /* LH_COMPRESS_DEFLATE_H */
