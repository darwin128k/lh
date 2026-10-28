/**
 * @file fnv.h
 * @brief FNV-1a, 32-bit: a fast non-cryptographic hash of bytes.
 *
 * For lookup tables and change checks (glyph covers, label text), not for
 * anything an attacker controls. Built bottom-up: one byte
 * (::lh_hash_fnv1a_32_byte), a run of bytes onto a running hash
 * (::lh_hash_fnv1a_32_update), and the one-shot hash from the standard offset
 * basis (::lh_hash_fnv1a_32).
 */

#ifndef LH_HASH_FNV_H
#define LH_HASH_FNV_H

#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @def LH_HASH_FNV1A_32_OFFSET
 * @brief The 32-bit FNV offset basis: the hash of no bytes.
 */
#define LH_HASH_FNV1A_32_OFFSET 2166136261U

/**
 * @def LH_HASH_FNV1A_32_PRIME
 * @brief The 32-bit FNV prime.
 */
#define LH_HASH_FNV1A_32_PRIME 16777619U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fold one byte into a running hash.
 * @param hash The hash so far.
 * @param byte The next byte.
 * @return `(hash ^ byte) * prime`.
 */
lh_u32_t
lh_hash_fnv1a_32_byte(lh_u32_t hash, lh_byte_t byte);

/**
 * @brief Fold @p size bytes of @p data into a running hash.
 * @param hash The hash so far (::LH_HASH_FNV1A_32_OFFSET to start, or a seed).
 * @param data Bytes to hash; may be ::lh_null when @p size is zero.
 * @param size Number of bytes.
 * @return The hash after the last byte.
 */
lh_u32_t
lh_hash_fnv1a_32_update(lh_u32_t hash, const lh_ptr data, lh_usize_t size);

/**
 * @brief The FNV-1a hash of @p size bytes of @p data.
 * @param data Bytes to hash; may be ::lh_null when @p size is zero.
 * @param size Number of bytes.
 * @return The hash; ::LH_HASH_FNV1A_32_OFFSET for no bytes.
 */
lh_u32_t
lh_hash_fnv1a_32(const lh_ptr data, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HASH_FNV_H */
