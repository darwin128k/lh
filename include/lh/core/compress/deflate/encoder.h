/**
 * @file encoder.h
 * @brief Caller-owned scratch state of the DEFLATE stream encoder.
 *
 * The encoder state bundles the two pieces ::lh_compress_deflate_try_encode
 * works with: a ::lh_compress_deflate_matcher_t that finds back-references
 * across the whole input and a ::lh_compress_deflate_tokens_t that holds the
 * block being priced and written. Bundling them gives the caller one object
 * to own; each piece stays reachable through its getter and remains a brick
 * of its own.
 *
 * The state holds no input and no output and survives between inputs: one
 * ::lh_compress_deflate_encoder_init sets the chain limit, and every encode
 * call resets what it uses. At about 300 KiB it is too large for a small
 * stack; keep it static, long-lived or allocated. One state serves one encode
 * call at a time.
 */

#ifndef LH_COMPRESS_DEFLATE_ENCODER_H
#define LH_COMPRESS_DEFLATE_ENCODER_H

#include <lh/core/compress/deflate/encoder/fields.h>
#include <lh/core/compress/deflate/matcher.h>
#include <lh/core/compress/deflate/tokens.h>

/** @brief Matcher and token buffer of one DEFLATE encoder. */
struct lh_compress_deflate_encoder
{
    /** Members: ::lh_compress_deflate_encoder_fields. */
    lh_compress_deflate_encoder_fields(lh_compress_deflate_matcher_t, lh_compress_deflate_tokens_t);
};
/** @copybrief lh_compress_deflate_encoder */
typedef struct lh_compress_deflate_encoder lh_compress_deflate_encoder_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Borrow the read-only matcher; self must be valid. */
const lh_compress_deflate_matcher_t *
lh_compress_deflate_encoder_get_matcher(const lh_compress_deflate_encoder_t *self);

/** @brief Borrow the writable matcher; self must be valid. */
lh_compress_deflate_matcher_t *
lh_compress_deflate_encoder_get_matcher_mut(lh_compress_deflate_encoder_t *self);

/** @brief Borrow the read-only token buffer; self must be valid. */
const lh_compress_deflate_tokens_t *
lh_compress_deflate_encoder_get_tokens(const lh_compress_deflate_encoder_t *self);

/** @brief Borrow the writable token buffer; self must be valid. */
lh_compress_deflate_tokens_t *
lh_compress_deflate_encoder_get_tokens_mut(lh_compress_deflate_encoder_t *self);

/**
 * @brief Prepare the encoder state with a chain limit.
 *
 * Initializes the matcher with ::lh_compress_deflate_matcher_init and empties
 * the token buffer.
 * @param self State; remains caller-owned.
 * @param chain_limit Most chain entries one search examines; zero disables
 *                    matching, and ::LH_COMPRESS_DEFLATE_MATCHER_CHAIN_LIMIT is
 *                    a balanced default.
 * @pre self is valid.
 */
lh_void
lh_compress_deflate_encoder_init(lh_compress_deflate_encoder_t *self, lh_usize_t chain_limit);

LH_COMPILER_EXTERN_C_END

#endif /* LH_COMPRESS_DEFLATE_ENCODER_H */
