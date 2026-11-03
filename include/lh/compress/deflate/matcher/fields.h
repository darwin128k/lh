/**
 * @file fields.h
 * @brief Storage fields for the DEFLATE hash-chain match finder.
 *
 * The matcher owns two position tables: the most recent position for each
 * three-byte hash, and the previous position with the same hash for every
 * slot of the 32 KiB window. Positions are stored plus one, so zero means
 * "none". The input itself is borrowed per call and never stored.
 */

#ifndef LH_COMPRESS_DEFLATE_MATCHER_FIELDS_H
#define LH_COMPRESS_DEFLATE_MATCHER_FIELDS_H

/** @brief Expand a matcher's chain limit and position tables. */
#define lh_compress_deflate_matcher_fields(size_type, word_type, hash_size, window_size)           \
    size_type chain_limit;                                                                         \
    word_type head[hash_size];                                                                     \
    word_type prev[window_size]

#endif /* LH_COMPRESS_DEFLATE_MATCHER_FIELDS_H */
