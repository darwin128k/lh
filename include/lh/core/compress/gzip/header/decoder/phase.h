/**
 * @file phase.h
 * @brief Phases of incremental gzip member-header parsing.
 *
 * A decoder progresses through the fixed header, present optional fields and
 * header checksum. Done and error are terminal until the decoder is reset.
 */

#ifndef LH_COMPRESS_GZIP_HEADER_DECODER_PHASE_H
#define LH_COMPRESS_GZIP_HEADER_DECODER_PHASE_H

/** @brief Current parsing phase of a gzip member header. */
typedef enum lh_compress_gzip_header_decoder_phase
{
    lh_compress_gzip_header_decoder_phase_fixed,
    lh_compress_gzip_header_decoder_phase_extra_low,
    lh_compress_gzip_header_decoder_phase_extra_high,
    lh_compress_gzip_header_decoder_phase_extra,
    lh_compress_gzip_header_decoder_phase_name,
    lh_compress_gzip_header_decoder_phase_comment,
    lh_compress_gzip_header_decoder_phase_crc_low,
    lh_compress_gzip_header_decoder_phase_crc_high,
    lh_compress_gzip_header_decoder_phase_done,
    lh_compress_gzip_header_decoder_phase_error
} lh_compress_gzip_header_decoder_phase_t;

#endif /* LH_COMPRESS_GZIP_HEADER_DECODER_PHASE_H */
