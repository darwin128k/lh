/**
 * @file base.h
 * @brief The precomputed multiples of the Ed25519 base point B.
 *
 * Three constant tables of eight points each, in the form
 * ::lh_crypto_ed25519_point_precomp_t: the signed 5-bit window used to check a
 * signature (odd multiples of B), and the two 4-bit signed combs (Mike Hamburg,
 * "Fast and compact elliptic-curve cryptography", 2012) used to compute [s]B
 * in constant time when signing. The values are Monocypher 4's (CC0 /
 * BSD-2-Clause, Loup Vaillant).
 */

#ifndef LH_CRYPTO_ED25519_BASE_H
#define LH_CRYPTO_ED25519_BASE_H

#include <lh/core/crypto/ed25519/point/precomp.h>

/**
 * @def LH_CRYPTO_ED25519_BASE_TABLE_SIZE
 * @brief Points in each table (8).
 */
#define LH_CRYPTO_ED25519_BASE_TABLE_SIZE 8U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The window table: [1]B, [3]B, ..., [15]B (entry k is [2k + 1]B).
 * @return ::LH_CRYPTO_ED25519_BASE_TABLE_SIZE constant points.
 */
const lh_crypto_ed25519_point_precomp_t *
lh_crypto_ed25519_base_get_window(void);

/**
 * @brief The comb for bits 0..127 of the scalar.
 * @return ::LH_CRYPTO_ED25519_BASE_TABLE_SIZE constant points.
 */
const lh_crypto_ed25519_point_precomp_t *
lh_crypto_ed25519_base_get_comb_low(void);

/**
 * @brief The comb for bits 128..255 of the scalar.
 * @return ::LH_CRYPTO_ED25519_BASE_TABLE_SIZE constant points.
 */
const lh_crypto_ed25519_point_precomp_t *
lh_crypto_ed25519_base_get_comb_high(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRYPTO_ED25519_BASE_H */
