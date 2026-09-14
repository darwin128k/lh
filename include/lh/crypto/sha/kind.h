/**
 * @file kind.h
 * @brief SHA algorithm selector for ::lh_crypto_sha_t.
 */

#ifndef LH_CRYPTO_SHA_KIND_H
#define LH_CRYPTO_SHA_KIND_H

#include <lh/byte.h>

/**
 * @def lh_crypto_sha_kind_1
 * @brief SHA-1 (160-bit digest).
 */
#define lh_crypto_sha_kind_1 0

/**
 * @def lh_crypto_sha_kind_224
 * @brief SHA-224 (SHA-256 construction, 224-bit digest).
 */
#define lh_crypto_sha_kind_224 1

/**
 * @def lh_crypto_sha_kind_256
 * @brief SHA-256 (256-bit digest).
 */
#define lh_crypto_sha_kind_256 2

/**
 * @def lh_crypto_sha_kind_384
 * @brief SHA-384 (SHA-512 construction, 384-bit digest).
 */
#define lh_crypto_sha_kind_384 3

/**
 * @def lh_crypto_sha_kind_512
 * @brief SHA-512 (512-bit digest).
 */
#define lh_crypto_sha_kind_512 4

/**
 * @typedef lh_crypto_sha_kind_t
 * @brief SHA algorithm kind; one of the `lh_crypto_sha_kind_*` constants.
 */
typedef lh_byte_t lh_crypto_sha_kind_t;

#endif /* LH_CRYPTO_SHA_KIND_H */
