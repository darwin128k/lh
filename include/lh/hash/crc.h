/**
 * @file crc.h
 * @brief Reflected CRC kernels and the CRC-32 used by gzip.
 *
 * The reflected kernel accepts the caller's initial register and polynomial,
 * making it usable for both Modbus CRC-16 and gzip CRC-32. The CRC-32 helpers
 * expose a finalized value between calls, so independent input fragments may
 * be processed without keeping a separate object. No input is owned.
 */

#ifndef LH_HASH_CRC_H
#define LH_HASH_CRC_H

#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @brief Standard reflected polynomial for gzip CRC-32.
 *
 * This value is part of the gzip format and is not a configuration point.
 * Other CRC variants pass their own reflected polynomial to
 * ::lh_hash_crc_reflected_byte or ::lh_hash_crc_reflected_update.
 */
#define LH_HASH_CRC32_POLYNOMIAL 0xEDB88320U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fold one byte into a reflected CRC register.
 * @param crc Register before this byte; caller supplies the initial value.
 * @param byte Next byte to fold.
 * @param polynomial Reflected polynomial, without an implicit high bit.
 * @return Updated raw register without final XOR.
 */
lh_u32_t
lh_hash_crc_reflected_byte(lh_u32_t crc, lh_byte_t byte, lh_u32_t polynomial);

/**
 * @brief Fold a byte sequence into a reflected CRC register.
 * @param crc Initial raw register or result of a preceding call.
 * @param data Borrowed bytes; null is permitted only when size is zero.
 * @param size Number of readable bytes.
 * @param polynomial Reflected polynomial.
 * @return Updated raw register without final XOR.
 */
lh_u32_t
lh_hash_crc_reflected_update(lh_u32_t crc, const lh_ptr data, lh_usize_t size, lh_u32_t polynomial);

/**
 * @brief Update the gzip CRC-32 over an additional byte sequence.
 * @param crc Finalized CRC from a preceding call, or zero for the first call.
 * @param data Borrowed input, or null for zero size.
 * @param size Number of input bytes.
 * @return Finalized CRC-32 for all processed bytes.
 */
lh_u32_t
lh_hash_crc32_update(lh_u32_t crc, const lh_ptr data, lh_usize_t size);

/**
 * @brief Calculate the gzip CRC-32 of a complete byte sequence.
 * @param data Borrowed input, or null for zero size.
 * @param size Number of input bytes.
 * @return Finalized CRC-32, zero for an empty sequence.
 */
lh_u32_t
lh_hash_crc32(const lh_ptr data, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HASH_CRC_H */
