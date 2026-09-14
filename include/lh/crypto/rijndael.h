/**
 * @file rijndael.h
 * @brief Rijndael block cipher (AES is the 128-bit-block subset).
 *
 * Key and block lengths of 16, 24, or 32 bytes (all nine combinations).
 * AES is Rijndael with a 16-byte block and a 16/24/32-byte key. Modes
 * ECB, CBC, and CFB operate one block at a time; the caller supplies any
 * chaining value for CBC/CFB.
 *
 * Pure computation — no OS, no allocator.
 */

#ifndef LH_CRYPTO_RIJNDAEL_H
#define LH_CRYPTO_RIJNDAEL_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/crypto/rijndael/mode.h>
#include <lh/null.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @def LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MIN
 * @brief Smallest supported block length in bytes (16 — AES).
 */
#define LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MIN 16U

/**
 * @def LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MAX
 * @brief Largest supported block length in bytes (32).
 */
#define LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MAX 32U

/**
 * @def LH_CRYPTO_RIJNDAEL_KEY_SIZE_MIN
 * @brief Smallest supported key length in bytes (16).
 */
#define LH_CRYPTO_RIJNDAEL_KEY_SIZE_MIN 16U

/**
 * @def LH_CRYPTO_RIJNDAEL_KEY_SIZE_MAX
 * @brief Largest supported key length in bytes (32).
 */
#define LH_CRYPTO_RIJNDAEL_KEY_SIZE_MAX 32U

/**
 * @def LH_CRYPTO_RIJNDAEL_ROUND_COUNT_MAX
 * @brief Largest number of rounds (14, when key or block is 32 bytes).
 */
#define LH_CRYPTO_RIJNDAEL_ROUND_COUNT_MAX 14U

/**
 * @def LH_CRYPTO_RIJNDAEL_EXPANDED_KEY_SIZE_MAX
 * @brief Bytes of expanded key: block_max * (rounds_max + 1).
 */
#define LH_CRYPTO_RIJNDAEL_EXPANDED_KEY_SIZE_MAX                                                   \
    (LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MAX * (LH_CRYPTO_RIJNDAEL_ROUND_COUNT_MAX + 1U))

/**
 * @struct lh_crypto_rijndael
 * @typedef lh_crypto_rijndael_t
 * @brief Rijndael session: expanded key, chaining value, and mode.
 *
 * Fields are the cipher state; treat them as opaque outside this module.
 */
struct lh_crypto_rijndael
{
    lh_crypto_rijndael_mode_t mode; /**< Chaining mode selected at init. */
    lh_usize_t block_size;          /**< 16, 24, or 32. */
    lh_usize_t round_count;         /**< Number of rounds for this key/block pair. */
    lh_uchar_t expanded_key[LH_CRYPTO_RIJNDAEL_EXPANDED_KEY_SIZE_MAX]; /**< Round keys. */
    lh_uchar_t chain[LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MAX]; /**< CBC/CFB chaining block. */
};
typedef struct lh_crypto_rijndael lh_crypto_rijndael_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Expand @p key and arm @p self for encrypt/decrypt of @p block_size
 *        blocks in @p mode.
 *
 * @p key_size and @p block_size must each be 16, 24, or 32. For CBC and CFB,
 * @p chain must be @p block_size bytes (the initial chaining value). For ECB,
 * @p chain is ignored and may be ::lh_null.
 *
 * @param self       Session to initialise.
 * @param key        Raw key bytes.
 * @param key_size   Key length in bytes (16, 24, or 32).
 * @param chain      Initial chaining value, or ::lh_null for ECB.
 * @param chain_size Length of @p chain; must equal @p block_size for CBC/CFB.
 * @param block_size Block length in bytes (16, 24, or 32).
 * @param mode       One of the `lh_crypto_rijndael_mode_*` constants.
 *
 * @return ::lh_bool_true on success, ::lh_bool_false if a length or @p mode
 *         is invalid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_crypto_rijndael_init(lh_crypto_rijndael_t *self, const lh_ptr key, lh_usize_t key_size,
                        const lh_ptr chain, lh_usize_t chain_size, lh_usize_t block_size,
                        lh_crypto_rijndael_mode_t mode);

/**
 * @brief Encrypt one block from @p in into @p out.
 *
 * @p in and @p out may overlap. For CBC/CFB the session chaining value is
 * updated. Both buffers are @c self->block_size bytes.
 *
 * @param self Session previously initialised by ::lh_crypto_rijndael_init.
 * @param in   Plaintext block.
 * @param out  Ciphertext block.
 *
 * @return ::lh_bool_true on success.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_crypto_rijndael_encrypt_block(lh_crypto_rijndael_t *self, const lh_ptr in, lh_ptr out);

/**
 * @brief Decrypt one block from @p in into @p out.
 *
 * @p in and @p out may overlap. For CBC/CFB the session chaining value is
 * updated. Both buffers are @c self->block_size bytes.
 *
 * @param self Session previously initialised by ::lh_crypto_rijndael_init.
 * @param in   Ciphertext block.
 * @param out  Plaintext block.
 *
 * @return ::lh_bool_true on success.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_crypto_rijndael_decrypt_block(lh_crypto_rijndael_t *self, const lh_ptr in, lh_ptr out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRYPTO_RIJNDAEL_H */
