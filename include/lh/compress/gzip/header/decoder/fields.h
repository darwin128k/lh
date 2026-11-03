/**
 * @file fields.h
 * @brief Member fields of an incremental gzip header decoder.
 *
 * The decoder owns its scalar state and borrows each input fragment only while
 * it is fed. The byte count is limited independently of payload size.
 */

#ifndef LH_COMPRESS_GZIP_HEADER_DECODER_FIELDS_H
#define LH_COMPRESS_GZIP_HEADER_DECODER_FIELDS_H

/** @brief Expand storage for incremental header validation. */
#define lh_compress_gzip_header_decoder_fields(size_type, crc_type, word_type, byte_type,          \
                                               phase_type)                                         \
    size_type size;                                                                                \
    size_type limit;                                                                               \
    size_type remaining;                                                                           \
    crc_type crc;                                                                                  \
    word_type value;                                                                               \
    byte_type flags;                                                                               \
    byte_type fixed;                                                                               \
    phase_type phase

#endif /* LH_COMPRESS_GZIP_HEADER_DECODER_FIELDS_H */
