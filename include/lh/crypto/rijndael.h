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
 *
 * On x86 a 16-byte block (all of AES) runs on AES-NI when the toolchain can
 * build it (`LH_LIBRARY_OPTION_SIMD_HAVE_AESNI`) and the CPU has it
 * (::lh_cpu_simd_has_aesni, checked once per process). The result is the same
 * bytes as the table path, which every other block size and CPU uses.
 */

#ifndef LH_CRYPTO_RIJNDAEL_H
#define LH_CRYPTO_RIJNDAEL_H

#include <lh/bool.h>
#include <lh/byte/ptr.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/crypto/rijndael/mode.h>
#include <lh/size.h>

/* The session layout below depends on this option, so a config.h without it would
   silently read it as 0 and disagree with a library built with it on. */
#ifndef LH_LIBRARY_OPTION_SIMD_HAVE_AESNI
#    error                                                                                         \
        "lh/config.h does not define LH_LIBRARY_OPTION_SIMD_HAVE_AESNI (0 or 1); lh_crypto_rijndael_t's layout depends on it"
#endif

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

#if LH_LIBRARY_OPTION_SIMD_HAVE_AESNI
/**
 * @def LH_CRYPTO_RIJNDAEL_AESNI_DEC_KEY_SIZE_MAX
 * @brief Bytes of the AES-NI decryption key schedule: block_min * (rounds_max + 1).
 */
#    define LH_CRYPTO_RIJNDAEL_AESNI_DEC_KEY_SIZE_MAX                                              \
        (LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MIN * (LH_CRYPTO_RIJNDAEL_ROUND_COUNT_MAX + 1U))
#endif

/**
 * @struct lh_crypto_rijndael
 * @brief Rijndael session: expanded key, chaining value, and mode.
 *
 * Fields are the cipher state; treat them as opaque outside this module.
 * The layout depends on ::LH_LIBRARY_OPTION_SIMD_HAVE_AESNI from the shared
 * lh/config.h, so library and consumers must use the same config.h.
 */
struct lh_crypto_rijndael
{
    lh_crypto_rijndael_mode_t mode; /**< Chaining mode selected at init. */
    lh_usize_t block_size;          /**< 16, 24, or 32. */
    lh_usize_t round_count;         /**< Number of rounds for this key/block pair. */
    lh_byte_t expanded_key[LH_CRYPTO_RIJNDAEL_EXPANDED_KEY_SIZE_MAX]; /**< Round keys. */
    lh_byte_t chain[LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MAX]; /**< CBC/CFB chaining block. */
#if LH_LIBRARY_OPTION_SIMD_HAVE_AESNI
    /** AESDEC round keys: InvMixColumns of round keys 1..nr-1 (16-byte blocks on AES-NI only). */
    lh_byte_t aesni_dec_key[LH_CRYPTO_RIJNDAEL_AESNI_DEC_KEY_SIZE_MAX];
#endif
};
/** @copybrief lh_crypto_rijndael */
typedef struct lh_crypto_rijndael lh_crypto_rijndael_t;

LH_COMPILER_EXTERN_C_BEGIN

/** Encrypt (or, with @p decrypt, decrypt) one block @p in into @p out; AES-NI for 16-byte blocks
 * when the CPU has it. */
lh_void
lh_crypto_rijndael_cipher(const lh_crypto_rijndael_t *self, const lh_byte_ptr in, lh_byte_ptr out,
                          lh_bool_t decrypt);

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
lh_bool_t
lh_crypto_rijndael_decrypt_block(lh_crypto_rijndael_t *self, const lh_ptr in, lh_ptr out);

/** True for a key or block size Rijndael takes: 16, 24 or 32 bytes. */
lh_bool_t
lh_crypto_rijndael_size_ok(lh_usize_t n);

/** @p x times 2 in GF(2^8), reduced by the AES polynomial. */
lh_byte_t
lh_crypto_rijndael_xtime(lh_byte_t x);

/** @p a times @p b in GF(2^8). */
lh_byte_t
lh_crypto_rijndael_gf_mul(lh_byte_t a, lh_byte_t b);

/** How far ShiftRows moves rows 1, 2 and 3 for a block of @p nb columns. */
lh_void
lh_crypto_rijndael_shift_offsets(lh_usize_t nb, lh_usize_t *c1, lh_usize_t *c2, lh_usize_t *c3);

/** Rotate row @p row of @p state (@p nb columns) by @p shift, the other way when @p inverse. */
lh_void
lh_crypto_rijndael_shift_row(lh_byte_ptr state, lh_usize_t nb, lh_usize_t row, lh_usize_t shift,
                             lh_bool_t inverse);

/** ShiftRows (or its inverse) on @p state of @p nb columns. */
lh_void
lh_crypto_rijndael_shift_rows(lh_byte_ptr state, lh_usize_t nb, lh_bool_t inverse);

/** SubBytes (or its inverse) on the @p block_size bytes of @p state. */
lh_void
lh_crypto_rijndael_sub_bytes(lh_byte_ptr state, lh_usize_t block_size, lh_bool_t inverse);

/** MixColumns (or its inverse) on @p state of @p nb columns. */
lh_void
lh_crypto_rijndael_mix_columns(lh_byte_ptr state, lh_usize_t nb, lh_bool_t inverse);

/** The S-box applied to each byte of @p w. */
lh_u32_t
lh_crypto_rijndael_sub_word(lh_u32_t w);

/** The round keys of @p key (@p key_size bytes) into @p self. */
lh_void
lh_crypto_rijndael_expand_key(lh_crypto_rijndael_t *self, const lh_byte_ptr key,
                              lh_usize_t key_size);

/**
 * @brief The block length in bytes: 16, 24 or 32 (level 1).
 *
 * Bounded for the compiler: a value past ::LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MAX is
 * declared unreachable, so every block-sized copy and xor is known to fit the
 * state and chain buffers. ::lh_crypto_rijndael_init is what checks it.
 *
 * @param self The session; not checked (the cipher's hot path reads it).
 */
lh_usize_t
lh_crypto_rijndael_get_block_size(const lh_crypto_rijndael_t *self);

/**
 * @brief The chaining mode selected at init (level 1).
 *
 * @param self The session; checked by ::lh_assert_runtime_ref.
 */
lh_crypto_rijndael_mode_t
lh_crypto_rijndael_get_mode(const lh_crypto_rijndael_t *self);

/**
 * @brief Store the chaining mode selected at init (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The session; checked by ::lh_assert_runtime_ref.
 * @param mode The new value.
 */
lh_void
lh_crypto_rijndael_set_mode(lh_crypto_rijndael_t *self, lh_crypto_rijndael_mode_t mode);

/**
 * @brief Store the block length in bytes: 16, 24 or 32 (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The session; checked by ::lh_assert_runtime_ref.
 * @param block_size The new value.
 */
lh_void
lh_crypto_rijndael_set_block_size(lh_crypto_rijndael_t *self, lh_usize_t block_size);

/**
 * @brief The number of rounds for this key and block (level 1).
 *
 * @param self The session; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_crypto_rijndael_get_round_count(const lh_crypto_rijndael_t *self);

/**
 * @brief Store the number of rounds for this key and block (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The session; checked by ::lh_assert_runtime_ref.
 * @param round_count The new value.
 */
lh_void
lh_crypto_rijndael_set_round_count(lh_crypto_rijndael_t *self, lh_usize_t round_count);

/**
 * @brief The round keys (level 1).
 *
 * An array of ::LH_CRYPTO_RIJNDAEL_EXPANDED_KEY_SIZE_MAX bytes.
 *
 * @param self The session; checked by ::lh_assert_runtime_ref.
 */
const lh_byte_t *
lh_crypto_rijndael_get_expanded_key(const lh_crypto_rijndael_t *self);

/**
 * @brief The round keys, to fill in place (level 1).
 *
 * An array of ::LH_CRYPTO_RIJNDAEL_EXPANDED_KEY_SIZE_MAX bytes; nothing about the
 * bounds is checked.
 *
 * @param self The session; checked by ::lh_assert_runtime_ref.
 */
lh_byte_t *
lh_crypto_rijndael_get_expanded_key_mut(lh_crypto_rijndael_t *self);

/**
 * @brief The CBC / CFB chaining block (level 1).
 *
 * An array of ::LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MAX bytes.
 *
 * @param self The session; checked by ::lh_assert_runtime_ref.
 */
const lh_byte_t *
lh_crypto_rijndael_get_chain(const lh_crypto_rijndael_t *self);

/**
 * @brief The CBC / CFB chaining block, to fill in place (level 1).
 *
 * An array of ::LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MAX bytes; nothing about the
 * bounds is checked.
 *
 * @param self The session; checked by ::lh_assert_runtime_ref.
 */
lh_byte_t *
lh_crypto_rijndael_get_chain_mut(lh_crypto_rijndael_t *self);

#if LH_LIBRARY_OPTION_SIMD_HAVE_AESNI
/**
 * @brief The AESDEC round keys (AES-NI builds only) (level 1).
 *
 * An array of ::LH_CRYPTO_RIJNDAEL_AESNI_DEC_KEY_SIZE_MAX bytes.
 *
 * @param self The session; checked by ::lh_assert_runtime_ref.
 */
const lh_byte_t *
lh_crypto_rijndael_get_aesni_dec_key(const lh_crypto_rijndael_t *self);

/**
 * @brief The AESDEC round keys (AES-NI builds only), to fill in place (level 1).
 *
 * An array of ::LH_CRYPTO_RIJNDAEL_AESNI_DEC_KEY_SIZE_MAX bytes; nothing about the
 * bounds is checked.
 *
 * @param self The session; checked by ::lh_assert_runtime_ref.
 */
lh_byte_t *
lh_crypto_rijndael_get_aesni_dec_key_mut(lh_crypto_rijndael_t *self);
#endif

/**
 * @brief Level 2: the key of round @p round, one block long, in the expanded key.
 *
 * @p round is not checked: it must not exceed ::lh_crypto_rijndael_get_round_count.
 *
 * @param self  The session.
 * @param round 0 for the key added first, the round count for the last.
 */
const lh_byte_t *
lh_crypto_rijndael_get_round_key(const lh_crypto_rijndael_t *self, lh_usize_t round);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRYPTO_RIJNDAEL_H */
