/**
 * @file slide.h
 * @brief ::lh_crypto_ed25519_slide_t — signed sliding windows over a scalar,
 *        read from the top bit down, one step per bit.
 *
 * The digits of a double-and-add ladder that adds an odd multiple of a point
 * only where a window ends (Roberto Maria Avanzi, 2005), computed as the ladder
 * walks down instead of all at once. Used to check signatures: variable time,
 * for public scalars only. Ported from Monocypher 4 (CC0 / BSD-2-Clause, Loup
 * Vaillant).
 */

#ifndef LH_CRYPTO_ED25519_SLIDE_H
#define LH_CRYPTO_ED25519_SLIDE_H

#include <lh/core/crypto/ed25519/scalar.h>
#include <lh/core/crypto/ed25519/slide/fields.h>

/**
 * @brief The state of one sliding window walk.
 */
struct lh_crypto_ed25519_slide
{
    /** Members: ::lh_crypto_ed25519_slide_fields. */
    lh_crypto_ed25519_slide_fields(lh_s16_t, lh_s8_t, lh_u8_t);
};

/** @copybrief lh_crypto_ed25519_slide */
typedef struct lh_crypto_ed25519_slide lh_crypto_ed25519_slide_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Bit position of the next digit (level 1). @param self Walk; not null. */
lh_s16_t
lh_crypto_ed25519_slide_get_next_index(const lh_crypto_ed25519_slide_t *self);

/** @brief Store the bit position of the next digit (level 1). */
lh_void
lh_crypto_ed25519_slide_set_next_index(lh_crypto_ed25519_slide_t *self, lh_s16_t next_index);

/** @brief The next digit (level 1). @param self Walk; not null. */
lh_s8_t
lh_crypto_ed25519_slide_get_next_digit(const lh_crypto_ed25519_slide_t *self);

/** @brief Store the next digit (level 1). */
lh_void
lh_crypto_ed25519_slide_set_next_digit(lh_crypto_ed25519_slide_t *self, lh_s8_t next_digit);

/** @brief The bit at which to look for the next window (level 1). */
lh_u8_t
lh_crypto_ed25519_slide_get_next_check(const lh_crypto_ed25519_slide_t *self);

/** @brief Store the bit at which to look for the next window (level 1). */
lh_void
lh_crypto_ed25519_slide_set_next_check(lh_crypto_ed25519_slide_t *self, lh_u8_t next_check);

/**
 * @brief Start a walk over @p scalar: the first window begins at its top set bit.
 *
 * The scalar is below L (checked or reduced), so bits 253..255 are 0; bit 252
 * is still looked at.
 *
 * @param self   Walk; must not be null.
 * @param scalar 32 bytes below L.
 */
lh_void
lh_crypto_ed25519_slide_init(lh_crypto_ed25519_slide_t *self,
                             const lh_byte_t scalar[LH_CRYPTO_ED25519_SCALAR_SIZE]);

/**
 * @brief The digit at bit @p i, the walk going from the top bit down.
 *
 * Call it once for every @p i, decreasing, from the walk's start to 0.
 *
 * @param self   Walk; must not be null.
 * @param width  Window width in bits (3 for the public key, 5 for the base).
 * @param i      The bit, one less than at the previous call.
 * @param scalar The scalar the walk was started with.
 * @return 0, or an odd digit in -(2^width - 1) .. 2^width - 1: add [digit]P now.
 */
lh_s32_t
lh_crypto_ed25519_slide_step(lh_crypto_ed25519_slide_t *self, lh_s32_t width, lh_s32_t i,
                             const lh_byte_t scalar[LH_CRYPTO_ED25519_SCALAR_SIZE]);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRYPTO_ED25519_SLIDE_H */
