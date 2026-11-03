/**
 * @file fields.h
 * @brief Storage fields for the streaming DEFLATE reader.
 *
 * The reader borrows its source, input scratch and history window. Huffman
 * tables are owned by the reader itself and remain valid across read calls.
 */

#ifndef LH_COMPRESS_DEFLATE_READER_FIELDS_H
#define LH_COMPRESS_DEFLATE_READER_FIELDS_H

/** @brief Expand a reader's resumable state and canonical tables. */
#define lh_compress_deflate_reader_fields(source_type, ptr_type, size_type, bool_type, phase_type, \
                                          word_type)                                               \
    source_type source;                                                                            \
    ptr_type input;                                                                                \
    size_type input_capacity;                                                                      \
    size_type input_size;                                                                          \
    size_type bit_position;                                                                        \
    ptr_type history;                                                                              \
    size_type history_position;                                                                    \
    size_type history_size;                                                                        \
    size_type decoded;                                                                             \
    size_type limit;                                                                               \
    size_type remaining;                                                                           \
    size_type distance;                                                                            \
    bool_type final;                                                                               \
    phase_type phase;                                                                              \
    word_type lit_counts[16];                                                                      \
    word_type lit_symbols[288];                                                                    \
    word_type dist_counts[16];                                                                     \
    word_type dist_symbols[32]

#endif /* LH_COMPRESS_DEFLATE_READER_FIELDS_H */
