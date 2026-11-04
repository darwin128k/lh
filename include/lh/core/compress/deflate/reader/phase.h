/**
 * @file phase.h
 * @brief Stages of a streaming DEFLATE reader.
 *
 * The reader retains its stage across source and output boundaries. Done and
 * error are terminal until reinitialization.
 */

#ifndef LH_COMPRESS_DEFLATE_READER_PHASE_H
#define LH_COMPRESS_DEFLATE_READER_PHASE_H

/** @brief Current stage of a streaming DEFLATE reader. */
typedef enum lh_compress_deflate_reader_phase
{
    lh_compress_deflate_reader_phase_block,
    lh_compress_deflate_reader_phase_stored_header,
    lh_compress_deflate_reader_phase_stored,
    lh_compress_deflate_reader_phase_dynamic,
    lh_compress_deflate_reader_phase_codes,
    lh_compress_deflate_reader_phase_match,
    lh_compress_deflate_reader_phase_done,
    lh_compress_deflate_reader_phase_error
} lh_compress_deflate_reader_phase_t;

#endif /* LH_COMPRESS_DEFLATE_READER_PHASE_H */
