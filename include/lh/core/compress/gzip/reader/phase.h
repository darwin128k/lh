/**
 * @file phase.h
 * @brief Stages of an incremental gzip member reader.
 *
 * Stages are retained across source and output boundaries; done and error are
 * terminal until the reader is reinitialized.
 */

#ifndef LH_COMPRESS_GZIP_READER_PHASE_H
#define LH_COMPRESS_GZIP_READER_PHASE_H

/** @brief Current stage of a streaming gzip reader. */
typedef enum lh_compress_gzip_reader_phase
{
    lh_compress_gzip_reader_phase_header,
    lh_compress_gzip_reader_phase_deflate,
    lh_compress_gzip_reader_phase_trailer,
    lh_compress_gzip_reader_phase_next,
    lh_compress_gzip_reader_phase_done,
    lh_compress_gzip_reader_phase_error
} lh_compress_gzip_reader_phase_t;

#endif /* LH_COMPRESS_GZIP_READER_PHASE_H */
