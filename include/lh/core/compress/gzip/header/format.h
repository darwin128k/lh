/**
 * @file format.h
 * @brief Fixed byte values and sizes of the gzip member format (RFC 1952).
 *
 * The header decoder checks these values and the gzip writer emits them, so
 * both name them from here. A minimal member header is the fixed ten-byte
 * part with no optional fields; every member ends with an eight-byte trailer
 * of CRC-32 and size.
 */

#ifndef LH_COMPRESS_GZIP_HEADER_FORMAT_H
#define LH_COMPRESS_GZIP_HEADER_FORMAT_H

/** First identification byte of every gzip member. */
#define LH_COMPRESS_GZIP_HEADER_ID1 31U

/** Second identification byte of every gzip member. */
#define LH_COMPRESS_GZIP_HEADER_ID2 139U

/** Compression method byte for DEFLATE, the only method gzip defines. */
#define LH_COMPRESS_GZIP_HEADER_METHOD_DEFLATE 8U

/** Operating-system byte meaning "unknown". */
#define LH_COMPRESS_GZIP_HEADER_OS_UNKNOWN 255U

/** Size of the fixed header part, without optional fields. */
#define LH_COMPRESS_GZIP_HEADER_SIZE 10U

/** Size of the member trailer: CRC-32 and size modulo 2^32, little endian. */
#define LH_COMPRESS_GZIP_TRAILER_SIZE 8U

#endif /* LH_COMPRESS_GZIP_HEADER_FORMAT_H */
