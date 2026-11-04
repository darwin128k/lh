/**
 * @file fields.h
 * @brief Storage fields for a DEFLATE block's token buffer.
 *
 * A token is a literal byte or a length/distance pair. The buffer keeps them
 * in two parallel arrays: a distance of zero marks a literal whose byte is the
 * value, any other distance marks a match whose length minus three is the
 * value. Both fit eight and sixteen bits, so a token takes three bytes.
 */

#ifndef LH_COMPRESS_DEFLATE_TOKENS_FIELDS_H
#define LH_COMPRESS_DEFLATE_TOKENS_FIELDS_H

/** @brief Expand a token buffer's count, value array and distance array. */
#define lh_compress_deflate_tokens_fields(size_type, value_type, distance_type, limit)             \
    size_type count;                                                                               \
    value_type values[limit];                                                                      \
    distance_type distances[limit]

#endif /* LH_COMPRESS_DEFLATE_TOKENS_FIELDS_H */
