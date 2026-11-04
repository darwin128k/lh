/**
 * @file tokens.h
 * @brief Fixed-capacity buffer of the literals and matches of one DEFLATE block.
 *
 * The encoder does not write a block while it searches: it first collects the
 * block's tokens here, then counts their symbols, prices every block type from
 * the counts and writes only the cheapest one. Searching once and pricing from
 * counts is what lets a block fall back to stored without having been encoded
 * twice, and it is the input a dynamic-Huffman code is built from.
 *
 * The caller owns the buffer, usually inside a ::lh_compress_deflate_encoder_t.
 * It holds at most ::LH_COMPRESS_DEFLATE_TOKENS_LIMIT tokens of three bytes;
 * ::lh_compress_deflate_tokens_reset empties it for the next block. Tokens are
 * appended in input order and read back by index.
 */

#ifndef LH_COMPRESS_DEFLATE_TOKENS_H
#define LH_COMPRESS_DEFLATE_TOKENS_H

#include <lh/core/compress/deflate/tokens/fields.h>
#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>
#include <lh/core/size.h>

/**
 * Most tokens one block holds. Every token covers at least one input byte, so
 * a block that ends because the buffer is full spans at least this many bytes.
 */
#define LH_COMPRESS_DEFLATE_TOKENS_LIMIT 16384U

/** @brief Literals and matches of one DEFLATE block, in input order. */
struct lh_compress_deflate_tokens
{
    /** Members: ::lh_compress_deflate_tokens_fields. */
    lh_compress_deflate_tokens_fields(lh_usize_t, lh_byte_t, lh_u16_t,
                                      LH_COMPRESS_DEFLATE_TOKENS_LIMIT);
};
/** @copybrief lh_compress_deflate_tokens */
typedef struct lh_compress_deflate_tokens lh_compress_deflate_tokens_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Return the stored token count; self must be valid. */
lh_usize_t
lh_compress_deflate_tokens_get_count(const lh_compress_deflate_tokens_t *self);

/** @brief Store the token count without validation; self must be valid. */
lh_void
lh_compress_deflate_tokens_set_count(lh_compress_deflate_tokens_t *self, lh_usize_t value);

/**
 * @brief Borrow the read-only value array of ::LH_COMPRESS_DEFLATE_TOKENS_LIMIT
 *        entries; self must be valid.
 */
const lh_byte_t *
lh_compress_deflate_tokens_get_values(const lh_compress_deflate_tokens_t *self);

/** @brief Borrow the writable value array; self must be valid. */
lh_byte_t *
lh_compress_deflate_tokens_get_values_mut(lh_compress_deflate_tokens_t *self);

/**
 * @brief Borrow the read-only distance array of ::LH_COMPRESS_DEFLATE_TOKENS_LIMIT
 *        entries; self must be valid.
 */
const lh_u16_t *
lh_compress_deflate_tokens_get_distances(const lh_compress_deflate_tokens_t *self);

/** @brief Borrow the writable distance array; self must be valid. */
lh_u16_t *
lh_compress_deflate_tokens_get_distances_mut(lh_compress_deflate_tokens_t *self);

/**
 * @brief Empty the buffer for a new block; the arrays are not cleared.
 * @param self Buffer; remains caller-owned.
 * @pre self is valid.
 */
lh_void
lh_compress_deflate_tokens_reset(lh_compress_deflate_tokens_t *self);

/**
 * @brief Tell whether the buffer holds ::LH_COMPRESS_DEFLATE_TOKENS_LIMIT tokens.
 * @param self Buffer.
 * @return True if no further token fits; false otherwise.
 * @pre self is valid.
 */
lh_bool_t
lh_compress_deflate_tokens_is_full(const lh_compress_deflate_tokens_t *self);

/**
 * @brief Distance of a token; zero for a literal.
 * @param self Buffer.
 * @param index Token index, below the count.
 * @return From 0 through 32768.
 * @pre self is valid and index is below the count; not checked.
 */
lh_uint_t
lh_compress_deflate_tokens_get_distance(const lh_compress_deflate_tokens_t *self, lh_usize_t index);

/**
 * @brief Byte of a literal token.
 * @param self Buffer.
 * @param index Index of a literal token, below the count.
 * @return The literal byte, from 0 through 255.
 * @pre self is valid and the token at index is a literal; not checked.
 */
lh_uint_t
lh_compress_deflate_tokens_get_literal(const lh_compress_deflate_tokens_t *self, lh_usize_t index);

/**
 * @brief Length of a match token.
 * @param self Buffer.
 * @param index Index of a match token, below the count.
 * @return From 3 through 258.
 * @pre self is valid and the token at index is a match; not checked.
 */
lh_uint_t
lh_compress_deflate_tokens_get_length(const lh_compress_deflate_tokens_t *self, lh_usize_t index);

/**
 * @brief Tell whether a token is a literal.
 * @param self Buffer.
 * @param index Token index, below the count.
 * @return True for a literal, false for a match.
 * @pre self is valid and index is below the count; not checked.
 */
lh_bool_t
lh_compress_deflate_tokens_is_literal(const lh_compress_deflate_tokens_t *self, lh_usize_t index);

/**
 * @brief Append a literal byte.
 * @param self Buffer.
 * @param literal Byte to append.
 * @return True on success; false if the buffer is full, with it unchanged.
 * @pre self is valid.
 */
lh_bool_t
lh_compress_deflate_tokens_try_push_literal(lh_compress_deflate_tokens_t *self, lh_byte_t literal);

/**
 * @brief Append a length/distance pair.
 * @param self Buffer.
 * @param length Match length from 3 through 258.
 * @param distance Match distance from 1 through 32768.
 * @return True on success; false if the buffer is full or the length or the
 *         distance is out of range, with the buffer unchanged.
 * @pre self is valid.
 */
lh_bool_t
lh_compress_deflate_tokens_try_push_match(lh_compress_deflate_tokens_t *self, lh_uint_t length,
                                          lh_uint_t distance);

/**
 * @brief Append a literal, checking ::lh_compress_deflate_tokens_try_push_literal.
 * @copydetails lh_compress_deflate_tokens_try_push_literal
 * @fails ::lh_runtime_error_code_invalid_range The buffer is full.
 */
lh_void
lh_compress_deflate_tokens_push_literal(lh_compress_deflate_tokens_t *self, lh_byte_t literal);

/**
 * @brief Append a match, checking ::lh_compress_deflate_tokens_try_push_match.
 * @copydetails lh_compress_deflate_tokens_try_push_match
 * @fails ::lh_runtime_error_code_invalid_range The buffer is full or the pair
 *        is out of range.
 */
lh_void
lh_compress_deflate_tokens_push_match(lh_compress_deflate_tokens_t *self, lh_uint_t length,
                                      lh_uint_t distance);

LH_COMPILER_EXTERN_C_END

#endif /* LH_COMPRESS_DEFLATE_TOKENS_H */
