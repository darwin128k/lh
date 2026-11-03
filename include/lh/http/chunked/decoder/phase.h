/**
 * @file phase.h
 * @brief State of an incremental HTTP chunked-body decoder.
 */

#ifndef LH_HTTP_CHUNKED_DECODER_PHASE_H
#define LH_HTTP_CHUNKED_DECODER_PHASE_H

/** @brief What the decoder expects next. */
typedef enum lh_http_chunked_decoder_phase
{
    lh_http_chunked_decoder_phase_size,
    lh_http_chunked_decoder_phase_size_lf,
    lh_http_chunked_decoder_phase_data,
    lh_http_chunked_decoder_phase_data_cr,
    lh_http_chunked_decoder_phase_data_lf,
    lh_http_chunked_decoder_phase_trailer,
    lh_http_chunked_decoder_phase_trailer_lf,
    lh_http_chunked_decoder_phase_done,
    lh_http_chunked_decoder_phase_error
} lh_http_chunked_decoder_phase_t;

#endif /* LH_HTTP_CHUNKED_DECODER_PHASE_H */
