/**
 * @file fields.h
 * @brief Fields of the incremental chunked transfer decoder.
 *
 * The scratch line is caller-owned. Counters bound decoded content and the
 * total metadata consumed, while remaining and phase track partial chunks.
 */

#ifndef LH_HTTP_CHUNKED_DECODER_FIELDS_H
#define LH_HTTP_CHUNKED_DECODER_FIELDS_H

/** @brief Data needed to resume decoding when input or output runs out. */
#define lh_http_chunked_decoder_fields(ptr_type, size_type, phase_type)                            \
    ptr_type line;                                                                                 \
    size_type line_capacity;                                                                       \
    size_type line_size;                                                                           \
    size_type remaining;                                                                           \
    size_type decoded;                                                                             \
    size_type decoded_limit;                                                                       \
    size_type metadata;                                                                            \
    size_type metadata_limit;                                                                      \
    phase_type phase

#endif /* LH_HTTP_CHUNKED_DECODER_FIELDS_H */
