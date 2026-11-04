/**
 * @file fields.h
 * @brief Fields of the incremental gzip reader.
 *
 * Sources, subdecoders and scratch are borrowed. The current member checksum,
 * trailer and aggregate limits are owned by the reader state.
 */

#ifndef LH_COMPRESS_GZIP_READER_FIELDS_H
#define LH_COMPRESS_GZIP_READER_FIELDS_H

/** @brief Expand persistent state of a streaming gzip reader. */
#define lh_compress_gzip_reader_fields(source_type, header_type, deflate_type, ptr_type,           \
                                       size_type, crc_type, phase_type, byte_type)                 \
    source_type source;                                                                            \
    header_type header;                                                                            \
    deflate_type deflate;                                                                          \
    ptr_type scratch;                                                                              \
    ptr_type history;                                                                              \
    size_type limit;                                                                               \
    size_type metadata_limit;                                                                      \
    size_type metadata;                                                                            \
    size_type total;                                                                               \
    size_type member_size;                                                                         \
    crc_type crc;                                                                                  \
    size_type trailer_size;                                                                        \
    phase_type phase;                                                                              \
    byte_type trailer[LH_COMPRESS_GZIP_TRAILER_SIZE]

#endif /* LH_COMPRESS_GZIP_READER_FIELDS_H */
