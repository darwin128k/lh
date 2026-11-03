/**
 * @file decoder.h
 * @brief Bounded, incremental parsing of a gzip member header.
 *
 * A decoder owns only scalar state. Each feed borrows input for the call, and
 * may end between any two bytes of the fixed or optional fields. A configured
 * byte limit bounds header metadata, including optional fields and CRC. Once
 * complete or invalid the decoder stays in that terminal state until reset.
 */

#ifndef LH_COMPRESS_GZIP_HEADER_DECODER_H
#define LH_COMPRESS_GZIP_HEADER_DECODER_H

#include <lh/compress/gzip/header/decoder/fields.h>
#include <lh/compress/gzip/header/decoder/phase.h>
#include <lh/compress/gzip/header/format.h>
#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/size.h>

/** @brief State of a gzip member-header parser. */
struct lh_compress_gzip_header_decoder
{
    /** Members: ::lh_compress_gzip_header_decoder_fields. */
    lh_compress_gzip_header_decoder_fields(lh_usize_t, lh_u32_t, lh_u16_t, lh_u8_t,
                                           lh_compress_gzip_header_decoder_phase_t);
};
/** @copybrief lh_compress_gzip_header_decoder */
typedef struct lh_compress_gzip_header_decoder lh_compress_gzip_header_decoder_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Return the stored size; self must be valid. */
lh_usize_t
lh_compress_gzip_header_decoder_get_size(const lh_compress_gzip_header_decoder_t *self);

/** @brief Store size without validation; self must be valid. */
lh_void
lh_compress_gzip_header_decoder_set_size(lh_compress_gzip_header_decoder_t *self, lh_usize_t value);

/** @brief Return the stored limit; self must be valid. */
lh_usize_t
lh_compress_gzip_header_decoder_get_limit(const lh_compress_gzip_header_decoder_t *self);

/** @brief Store limit without validation; self must be valid. */
lh_void
lh_compress_gzip_header_decoder_set_limit(lh_compress_gzip_header_decoder_t *self,
                                          lh_usize_t value);

/** @brief Return the stored remaining; self must be valid. */
lh_usize_t
lh_compress_gzip_header_decoder_get_remaining(const lh_compress_gzip_header_decoder_t *self);

/** @brief Store remaining without validation; self must be valid. */
lh_void
lh_compress_gzip_header_decoder_set_remaining(lh_compress_gzip_header_decoder_t *self,
                                              lh_usize_t value);

/** @brief Return the stored crc; self must be valid. */
lh_u32_t
lh_compress_gzip_header_decoder_get_crc(const lh_compress_gzip_header_decoder_t *self);

/** @brief Store crc without validation; self must be valid. */
lh_void
lh_compress_gzip_header_decoder_set_crc(lh_compress_gzip_header_decoder_t *self, lh_u32_t value);

/** @brief Return the stored value; self must be valid. */
lh_u16_t
lh_compress_gzip_header_decoder_get_value(const lh_compress_gzip_header_decoder_t *self);

/** @brief Store value without validation; self must be valid. */
lh_void
lh_compress_gzip_header_decoder_set_value(lh_compress_gzip_header_decoder_t *self, lh_u16_t value);

/** @brief Return the stored flags; self must be valid. */
lh_u8_t
lh_compress_gzip_header_decoder_get_flags(const lh_compress_gzip_header_decoder_t *self);

/** @brief Store flags without validation; self must be valid. */
lh_void
lh_compress_gzip_header_decoder_set_flags(lh_compress_gzip_header_decoder_t *self, lh_u8_t value);

/** @brief Return the stored fixed; self must be valid. */
lh_u8_t
lh_compress_gzip_header_decoder_get_fixed(const lh_compress_gzip_header_decoder_t *self);

/** @brief Store fixed without validation; self must be valid. */
lh_void
lh_compress_gzip_header_decoder_set_fixed(lh_compress_gzip_header_decoder_t *self, lh_u8_t value);

/** @brief Return the stored phase; self must be valid. */
lh_compress_gzip_header_decoder_phase_t
lh_compress_gzip_header_decoder_get_phase(const lh_compress_gzip_header_decoder_t *self);

/** @brief Store phase without validation; self must be valid. */
lh_void
lh_compress_gzip_header_decoder_set_phase(lh_compress_gzip_header_decoder_t *self,
                                          lh_compress_gzip_header_decoder_phase_t value);

/**
 * @brief Initialize for a new member with an explicit header-size limit.
 * @param self Decoder to initialize; remains caller-owned.
 * @param limit Maximum header bytes, including optional fields and header CRC.
 * @pre Self is valid. A limit below the header length makes parsing fail.
 */
lh_void
lh_compress_gzip_header_decoder_init(lh_compress_gzip_header_decoder_t *self, lh_usize_t limit);

/**
 * @brief Select the next present optional field after completing one field.
 * @param self Valid initialized decoder with its flags containing pending fields.
 * @pre Self is valid; called after the fixed header or an optional field.
 */
lh_void
lh_compress_gzip_header_decoder_next(lh_compress_gzip_header_decoder_t *self);

/**
 * @brief Consume a fragment of one gzip header.
 * @param self Initialized decoder, retained for the next fragment.
 * @param input Borrowed bytes; null is permitted only for a zero size.
 * @param size Number of readable bytes.
 * @param consumed Receives bytes consumed; an invalid consumed byte is counted.
 *                 A byte rejected by the limit is not consumed.
 * @return Current phase: done on a valid header, error for invalid syntax or a
 *         limit violation, otherwise the phase awaiting further bytes. The
 *         decoder stops at the header boundary and leaves later bytes unread.
 * @pre Self and consumed are valid; input is valid for its nonzero size.
 */
lh_compress_gzip_header_decoder_phase_t
lh_compress_gzip_header_decoder_try_feed(lh_compress_gzip_header_decoder_t *self,
                                         const lh_byte_t *input, lh_usize_t size,
                                         lh_usize_t *consumed);

/**
 * @brief Feed a header fragment and check ::lh_compress_gzip_header_decoder_try_feed.
 * Nonterminal phases and done are returned normally; an invalid header or
 * limit violation fails the runtime check. Input remains caller-owned.
 * @copydetails lh_compress_gzip_header_decoder_try_feed
 * @fails ::lh_runtime_error_code_invalid_range The header is invalid or oversized.
 */
lh_compress_gzip_header_decoder_phase_t
lh_compress_gzip_header_decoder_feed(lh_compress_gzip_header_decoder_t *self,
                                     const lh_byte_t *input, lh_usize_t size, lh_usize_t *consumed);

LH_COMPILER_EXTERN_C_END

#endif /* LH_COMPRESS_GZIP_HEADER_DECODER_H */
