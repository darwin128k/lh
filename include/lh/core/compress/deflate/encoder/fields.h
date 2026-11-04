/**
 * @file fields.h
 * @brief Storage fields for the DEFLATE encoder state.
 *
 * The encoder state is the hash-chain matcher, which spans the whole input,
 * and the token buffer, which holds one block at a time. Neither keeps the
 * input or the output: both are borrowed per call.
 */

#ifndef LH_COMPRESS_DEFLATE_ENCODER_FIELDS_H
#define LH_COMPRESS_DEFLATE_ENCODER_FIELDS_H

/** @brief Expand an encoder's matcher and token buffer. */
#define lh_compress_deflate_encoder_fields(matcher_type, tokens_type)                              \
    matcher_type matcher;                                                                          \
    tokens_type tokens

#endif /* LH_COMPRESS_DEFLATE_ENCODER_FIELDS_H */
