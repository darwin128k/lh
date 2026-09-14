/**
 * @file mode.h
 * @brief Block-cipher mode selector for ::lh_crypto_rijndael_t.
 */

#ifndef LH_CRYPTO_RIJNDAEL_MODE_H
#define LH_CRYPTO_RIJNDAEL_MODE_H

#include <lh/byte.h>

/**
 * @def lh_crypto_rijndael_mode_ecb
 * @brief Electronic Code Book: each block is encrypted independently.
 */
#define lh_crypto_rijndael_mode_ecb 0

/**
 * @def lh_crypto_rijndael_mode_cbc
 * @brief Cipher Block Chaining: each plaintext block is xored with the
 *        previous ciphertext block before encryption.
 */
#define lh_crypto_rijndael_mode_cbc 1

/**
 * @def lh_crypto_rijndael_mode_cfb
 * @brief Cipher Feedback: the previous ciphertext block is encrypted and
 *        xored with the plaintext.
 */
#define lh_crypto_rijndael_mode_cfb 2

/**
 * @typedef lh_crypto_rijndael_mode_t
 * @brief Rijndael chaining mode; one of the `lh_crypto_rijndael_mode_*`
 *        constants.
 */
typedef lh_byte_t lh_crypto_rijndael_mode_t;

#endif /* LH_CRYPTO_RIJNDAEL_MODE_H */
