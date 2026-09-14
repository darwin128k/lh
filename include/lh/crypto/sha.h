/**
 * @file sha.h
 * @brief SHA-1 / SHA-224 / SHA-256 / SHA-384 / SHA-512 (FIPS 180-4).
 *
 * Incremental (`init` / `update` / `finish`) and one-shot (::lh_crypto_sha_hash)
 * hashing. Pure computation — no OS, no allocator. The running state lives
 * in ::lh_crypto_sha_t so callers can stack-allocate it.
 */

#ifndef LH_CRYPTO_SHA_H
#define LH_CRYPTO_SHA_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/crypto/sha/kind.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @def LH_CRYPTO_SHA1_DIGEST_SIZE
 * @brief SHA-1 digest length in bytes (20).
 */
#define LH_CRYPTO_SHA1_DIGEST_SIZE 20U

/**
 * @def LH_CRYPTO_SHA224_DIGEST_SIZE
 * @brief SHA-224 digest length in bytes (28).
 */
#define LH_CRYPTO_SHA224_DIGEST_SIZE 28U

/**
 * @def LH_CRYPTO_SHA256_DIGEST_SIZE
 * @brief SHA-256 digest length in bytes (32).
 */
#define LH_CRYPTO_SHA256_DIGEST_SIZE 32U

/**
 * @def LH_CRYPTO_SHA384_DIGEST_SIZE
 * @brief SHA-384 digest length in bytes (48).
 */
#define LH_CRYPTO_SHA384_DIGEST_SIZE 48U

/**
 * @def LH_CRYPTO_SHA512_DIGEST_SIZE
 * @brief SHA-512 digest length in bytes (64).
 */
#define LH_CRYPTO_SHA512_DIGEST_SIZE 64U

/**
 * @def LH_CRYPTO_SHA_DIGEST_SIZE_MAX
 * @brief Longest digest this hasher can produce (::LH_CRYPTO_SHA512_DIGEST_SIZE).
 */
#define LH_CRYPTO_SHA_DIGEST_SIZE_MAX LH_CRYPTO_SHA512_DIGEST_SIZE

/**
 * @def LH_CRYPTO_SHA_BLOCK_SIZE_MAX
 * @brief Longest compression block (SHA-384/512 use 128 bytes).
 */
#define LH_CRYPTO_SHA_BLOCK_SIZE_MAX 128U

/**
 * @struct lh_crypto_sha
 * @typedef lh_crypto_sha_t
 * @brief Running SHA hash. Re-init to reuse after ::lh_crypto_sha_finish.
 *
 * Fields are the compression state; treat them as opaque outside this
 * module — read the digest only from ::lh_crypto_sha_finish /
 * ::lh_crypto_sha_hash.
 */
struct lh_crypto_sha
{
    lh_crypto_sha_kind_t kind;                      /**< Algorithm selected at init. */
    lh_bool_t finished;                             /**< Set by finish; update is then invalid. */
    lh_u64_t state[8];                              /**< Hash chaining value (low 32 bits for SHA-1/224/256). */
    lh_u64_t bit_count_lo;                          /**< Message length in bits, low 64. */
    lh_u64_t bit_count_hi;                          /**< Message length in bits, high 64 (SHA-384/512). */
    lh_uchar_t block[LH_CRYPTO_SHA_BLOCK_SIZE_MAX]; /**< Partial compression block. */
    lh_usize_t block_fill;                          /**< Bytes currently in @c block. */
};
typedef struct lh_crypto_sha lh_crypto_sha_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Digest length in bytes for @p kind.
 *
 * @param kind Algorithm; one of the `lh_crypto_sha_kind_*` constants.
 * @return Digest size, or 0 if @p kind is not a known algorithm.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_crypto_sha_digest_size(lh_crypto_sha_kind_t kind);

/**
 * @brief Compression block size in bytes for @p kind.
 *
 * @param kind Algorithm; one of the `lh_crypto_sha_kind_*` constants.
 * @return 64 for SHA-1/224/256, 128 for SHA-384/512, or 0 if @p kind is unknown.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_crypto_sha_block_size(lh_crypto_sha_kind_t kind);

/**
 * @brief Initialise @p self for @p kind. Must be called before update/finish.
 *
 * @param self Hasher to initialise.
 * @param kind Algorithm; one of the `lh_crypto_sha_kind_*` constants.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_crypto_sha_init(lh_crypto_sha_t *self, lh_crypto_sha_kind_t kind);

/**
 * @brief Absorb @p size bytes from @p data into @p self.
 *
 * May be called any number of times between init and finish.
 *
 * @param self Hasher previously initialised by ::lh_crypto_sha_init.
 * @param data Input span. Ignored when @p size is 0.
 * @param size Number of bytes in @p data.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_crypto_sha_update(lh_crypto_sha_t *self, const lh_ptr data, lh_usize_t size);

/**
 * @brief Complete the hash and write the digest to @p out.
 *
 * @p self cannot be updated afterwards; call ::lh_crypto_sha_init to reuse it.
 *
 * @param self     Hasher to finish.
 * @param out      Destination for the digest.
 * @param out_size Capacity of @p out in bytes.
 *
 * @return Bytes written (the digest size for this kind), or 0 if @p out_size
 *         was too small.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_crypto_sha_finish(lh_crypto_sha_t *self, lh_ptr out, lh_usize_t out_size);

/**
 * @brief One-shot hash of @p data into @p out.
 *
 * Equivalent to init / update / finish on a stack hasher.
 *
 * @param kind     Algorithm; one of the `lh_crypto_sha_kind_*` constants.
 * @param data     Input span. Ignored when @p size is 0.
 * @param size     Number of bytes in @p data.
 * @param out      Destination for the digest.
 * @param out_size Capacity of @p out in bytes.
 *
 * @return Bytes written, or 0 if @p out_size was too small or @p kind is unknown.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_crypto_sha_hash(lh_crypto_sha_kind_t kind, const lh_ptr data, lh_usize_t size, lh_ptr out,
                   lh_usize_t out_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRYPTO_SHA_H */
