/**
 * @file window.h
 * @brief Size of the DEFLATE sliding window.
 *
 * DEFLATE back-references reach at most 32768 bytes behind the current
 * position (RFC 1951, section 3.2.5). The decoder's history buffer, the
 * back-reference range check and the encoder's match finder all bound
 * themselves by this one value.
 */

#ifndef LH_COMPRESS_DEFLATE_WINDOW_H
#define LH_COMPRESS_DEFLATE_WINDOW_H

/** Largest DEFLATE back-reference distance and history size, in bytes. */
#define LH_COMPRESS_DEFLATE_WINDOW_SIZE 32768U

#endif /* LH_COMPRESS_DEFLATE_WINDOW_H */
