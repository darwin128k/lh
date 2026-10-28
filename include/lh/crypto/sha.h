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

#include <lh/bool.h>
#include <lh/byte/ptr.h>
#include <lh/compiler/extern/c.h>
#include <lh/crypto/sha/kind.h>
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
 * @brief Running SHA hash. Re-init to reuse after ::lh_crypto_sha_finish.
 *
 * Fields are the compression state; treat them as opaque outside this
 * module — read the digest only from ::lh_crypto_sha_finish /
 * ::lh_crypto_sha_hash.
 */
struct lh_crypto_sha
{
    lh_crypto_sha_kind_t kind; /**< Algorithm selected at init. */
    lh_bool_t finished;        /**< Set by finish; update is then invalid. */
    lh_u64_t state[8];         /**< Hash chaining value (low 32 bits for SHA-1/224/256). */
    lh_u64_t bit_count_lo;     /**< Message length in bits, low 64. */
    lh_u64_t bit_count_hi;     /**< Message length in bits, high 64 (SHA-384/512). */
    lh_byte_t block[LH_CRYPTO_SHA_BLOCK_SIZE_MAX]; /**< Partial compression block. */
    lh_usize_t block_fill;                         /**< Bytes currently in @c block. */
};
/** @copybrief lh_crypto_sha */
typedef struct lh_crypto_sha lh_crypto_sha_t;

LH_COMPILER_EXTERN_C_BEGIN

/** True for the 64-bit-word kinds, SHA-384 and SHA-512. */
lh_bool_t
lh_crypto_sha_kind_is_wide(lh_crypto_sha_kind_t kind);

/**
 * @brief Digest length in bytes for @p kind.
 *
 * @param kind Algorithm; one of the `lh_crypto_sha_kind_*` constants.
 * @return Digest size, or 0 if @p kind is not a known algorithm.
 */
lh_usize_t
lh_crypto_sha_digest_size(lh_crypto_sha_kind_t kind);

/**
 * @brief The algorithm selected at init (level 1).
 *
 * @param self The hasher; checked by ::lh_assert_runtime_ref.
 */
lh_crypto_sha_kind_t
lh_crypto_sha_get_kind(const lh_crypto_sha_t *self);

/**
 * @brief Store the algorithm selected at init (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The hasher; checked by ::lh_assert_runtime_ref.
 * @param kind The new value.
 */
lh_void
lh_crypto_sha_set_kind(lh_crypto_sha_t *self, lh_crypto_sha_kind_t kind);

/**
 * @brief Whether ::lh_crypto_sha_finish has run on @p self (level 1).
 *
 * Once it has, ::lh_crypto_sha_update is invalid until the hasher is initialised again.
 *
 * @param self The hasher; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_crypto_sha_is_finished(const lh_crypto_sha_t *self);

/**
 * @brief Store whether the hasher is finished (level 1).
 *
 * A level-1 setter: the flag is stored as given.
 *
 * @param self The hasher; checked by ::lh_assert_runtime_ref.
 * @param finished The new flag.
 */
lh_void
lh_crypto_sha_set_finished(lh_crypto_sha_t *self, lh_bool_t finished);

/**
 * @brief Word @p index of the chaining value (level 1).
 *
 * SHA-1/224/256 use the low 32 bits of words 0..4 or 0..7; SHA-384/512 all
 * 64 bits of words 0..7. @p index is not checked: it must be below 8.
 *
 * @param self The hasher; checked by ::lh_assert_runtime_ref.
 * @param index Word index, 0..7.
 */
lh_u64_t
lh_crypto_sha_get_state_word(const lh_crypto_sha_t *self, lh_usize_t index);

/**
 * @brief Store word @p index of the chaining value (level 1).
 *
 * @p index is not checked: it must be below 8.
 *
 * @param self The hasher; checked by ::lh_assert_runtime_ref.
 * @param index Word index, 0..7.
 * @param word  The new word.
 */
lh_void
lh_crypto_sha_set_state_word(lh_crypto_sha_t *self, lh_usize_t index, lh_u64_t word);

/**
 * @brief The low 64 bits of the message length in bits (level 1).
 *
 * @param self The hasher; checked by ::lh_assert_runtime_ref.
 */
lh_u64_t
lh_crypto_sha_get_bit_count_lo(const lh_crypto_sha_t *self);

/**
 * @brief Store the low 64 bits of the message length in bits (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The hasher; checked by ::lh_assert_runtime_ref.
 * @param bit_count_lo The new value.
 */
lh_void
lh_crypto_sha_set_bit_count_lo(lh_crypto_sha_t *self, lh_u64_t bit_count_lo);

/**
 * @brief The high 64 bits of the message length in bits (level 1).
 *
 * @param self The hasher; checked by ::lh_assert_runtime_ref.
 */
lh_u64_t
lh_crypto_sha_get_bit_count_hi(const lh_crypto_sha_t *self);

/**
 * @brief Store the high 64 bits of the message length in bits (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The hasher; checked by ::lh_assert_runtime_ref.
 * @param bit_count_hi The new value.
 */
lh_void
lh_crypto_sha_set_bit_count_hi(lh_crypto_sha_t *self, lh_u64_t bit_count_hi);

/**
 * @brief How many bytes of the partial block are filled (level 1).
 *
 * @param self The hasher; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_crypto_sha_get_block_fill(const lh_crypto_sha_t *self);

/**
 * @brief Store how many bytes of the partial block are filled (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The hasher; checked by ::lh_assert_runtime_ref.
 * @param block_fill The new value.
 */
lh_void
lh_crypto_sha_set_block_fill(lh_crypto_sha_t *self, lh_usize_t block_fill);

/**
 * @brief The partial compression block (level 1).
 *
 * An array of ::LH_CRYPTO_SHA_BLOCK_SIZE_MAX bytes.
 *
 * @param self The hasher; checked by ::lh_assert_runtime_ref.
 */
const lh_byte_t *
lh_crypto_sha_get_block(const lh_crypto_sha_t *self);

/**
 * @brief The partial compression block, to fill in place (level 1).
 *
 * An array of ::LH_CRYPTO_SHA_BLOCK_SIZE_MAX bytes; nothing about the bounds is checked.
 *
 * @param self The hasher; checked by ::lh_assert_runtime_ref.
 */
lh_byte_t *
lh_crypto_sha_get_block_mut(lh_crypto_sha_t *self);

/**
 * @brief Compression block size in bytes for @p kind.
 *
 * @param kind Algorithm; one of the `lh_crypto_sha_kind_*` constants.
 * @return 64 for SHA-1/224/256, 128 for SHA-384/512, or 0 if @p kind is unknown.
 */
lh_usize_t
lh_crypto_sha_block_size(lh_crypto_sha_kind_t kind);

/** Add @p byte_count bytes to the 128-bit message length of @p self. */
lh_void
lh_crypto_sha_add_bits(lh_crypto_sha_t *self, lh_usize_t byte_count);

/**
 * @brief Initialise @p self for @p kind. Must be called before update/finish.
 *
 * @param self Hasher to initialise.
 * @param kind Algorithm; one of the `lh_crypto_sha_kind_*` constants.
 */
lh_void
lh_crypto_sha_init(lh_crypto_sha_t *self, lh_crypto_sha_kind_t kind);

/**
 * @brief Level 2: add @p word to word @p index of the chaining value, modulo 2^64.
 *
 * The SHA-384/512 step at the end of a block. @p index is not checked: it must be
 * below 8.
 *
 * @param self  The hasher.
 * @param index Word index, 0..7.
 * @param word  What the block's rounds produced for that word.
 */
lh_void
lh_crypto_sha_add_state_word(lh_crypto_sha_t *self, lh_usize_t index, lh_u64_t word);

/**
 * @brief Level 2: add @p word to the low 32 bits of word @p index, modulo 2^32.
 *
 * The SHA-1/224/256 step at the end of a block: those algorithms keep 32-bit words
 * in the low half of each slot. @p index is not checked: it must be below 8.
 *
 * @param self  The hasher.
 * @param index Word index, 0..7.
 * @param word  What the block's rounds produced for that word.
 */
lh_void
lh_crypto_sha_add_state_word_32(lh_crypto_sha_t *self, lh_usize_t index, lh_u32_t word);

/**
 * @brief Level 2: the first byte of the partial block that is not filled yet.
 *
 * ::lh_crypto_sha_get_block_mut plus ::lh_crypto_sha_get_block_fill. Writing past
 * the block size of the hasher's kind is not checked.
 *
 * @param self The hasher.
 */
lh_byte_t *
lh_crypto_sha_get_block_tail_mut(lh_crypto_sha_t *self);

/**
 * @brief Level 2: write the digest of a finished hasher, big-endian, to @p out.
 *
 * @p out must hold ::lh_crypto_sha_digest_size of the kind; that is not checked.
 *
 * @param self The hasher, after ::lh_crypto_sha_pad.
 * @param out  Where the digest goes.
 */
lh_void
lh_crypto_sha_put_digest(const lh_crypto_sha_t *self, lh_byte_ptr out);

/** Fold one 64-byte @p block into the SHA-1 state of @p self. */
lh_void
lh_crypto_sha_process_1(lh_crypto_sha_t *self, const lh_byte_ptr block);

/** Fold one 64-byte @p block into the SHA-224/256 state of @p self. */
lh_void
lh_crypto_sha_process_256(lh_crypto_sha_t *self, const lh_byte_ptr block);

/** Fold one 128-byte @p block into the SHA-384/512 state of @p self. */
lh_void
lh_crypto_sha_process_512(lh_crypto_sha_t *self, const lh_byte_ptr block);

/**
 * @brief Level 3: append @p size bytes of @p bytes to the partial block.
 *
 * Copies them to ::lh_crypto_sha_get_block_tail_mut and moves the fill on. Does not
 * compress a block that becomes full; @p size must fit in what is left of it, and
 * that is not checked.
 *
 * @param self  The hasher.
 * @param bytes What to append.
 * @param size  How many bytes.
 */
lh_void
lh_crypto_sha_fill_block(lh_crypto_sha_t *self, lh_byte_cptr bytes, lh_usize_t size);

/**
 * @brief Level 3: zero the partial block from its fill up to @p end.
 *
 * The fill does not move. @p end must not be below the fill or past the block; that
 * is not checked.
 *
 * @param self The hasher.
 * @param end  Byte offset in the block where the zeros stop.
 */
lh_void
lh_crypto_sha_zero_block_to(lh_crypto_sha_t *self, lh_usize_t end);

/** Fold one @p block into @p self with the compression function of its kind. */
lh_void
lh_crypto_sha_process(lh_crypto_sha_t *self, const lh_byte_ptr block);

/**
 * @brief Absorb @p size bytes from @p data into @p self.
 *
 * May be called any number of times between init and finish.
 *
 * @param self Hasher previously initialised by ::lh_crypto_sha_init.
 * @param data Input span. Ignored when @p size is 0.
 * @param size Number of bytes in @p data.
 */
lh_void
lh_crypto_sha_update(lh_crypto_sha_t *self, const lh_ptr data, lh_usize_t size);

/**
 * @brief Level 4: the final padding — the 0x80 byte, zeros, the message length — and
 *        the last compressions.
 *
 * Leaves the chaining value holding the digest. Does not mark the hasher finished;
 * ::lh_crypto_sha_finish does.
 *
 * @param self The hasher; must not be finished (not checked here).
 */
lh_void
lh_crypto_sha_pad(lh_crypto_sha_t *self);

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
lh_usize_t
lh_crypto_sha_hash(lh_crypto_sha_kind_t kind, const lh_ptr data, lh_usize_t size, lh_ptr out,
                   lh_usize_t out_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRYPTO_SHA_H */
