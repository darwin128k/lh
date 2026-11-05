/**
 * @file field.h
 * @brief ::lh_crypto_curve25519_field_t — an element of GF(2^255 - 19), the field
 *        of Curve25519 and Ed25519.
 *
 * The arithmetic every Curve25519 construction stands on: Ed25519 points
 * (lh/core/crypto/ed25519/point.h) are made of these. A value type with no
 * owner and no teardown; it may live on the stack. Ten signed limbs of 26 and
 * 25 bits (radix 2^25.5): sums and differences are left unreduced, products
 * and squares carry back into range (::lh_crypto_curve25519_field_carry), and
 * only ::lh_crypto_curve25519_field_to_bytes produces the one canonical form.
 *
 * Constant time: no branch and no memory index depends on a value; the
 * conditional forms take the condition as a 0/1 number and mask with it.
 * Temporaries that held secrets are wiped (lh/core/crypto/wipe.h).
 *
 * Every operation allows its output to be one of its inputs. The limb bounds
 * each operation expects and leaves (the carry's postcondition, ±1.1 * 2^25 /
 * 2^24, and the products' precondition, ±1.65 * 2^26 / 2^25) are the ones of
 * Monocypher 4, from which this arithmetic is ported (CC0 / BSD-2-Clause,
 * Loup Vaillant); a sum or difference of two carried elements is still a valid
 * input to a product.
 */

#ifndef LH_CRYPTO_CURVE25519_FIELD_H
#define LH_CRYPTO_CURVE25519_FIELD_H

#include <lh/core/bool.h>
#include <lh/core/byte/ptr.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/crypto/curve25519/field/fields.h>

/**
 * @def LH_CRYPTO_CURVE25519_FIELD_SIZE
 * @brief Bytes of an encoded field element (32, little-endian).
 */
#define LH_CRYPTO_CURVE25519_FIELD_SIZE 32U

/**
 * @brief One element of GF(2^255 - 19).
 */
struct lh_crypto_curve25519_field
{
    /** Members: ::lh_crypto_curve25519_field_fields. */
    lh_crypto_curve25519_field_fields(lh_s32_t);
};

/** @copybrief lh_crypto_curve25519_field */
typedef struct lh_crypto_curve25519_field lh_crypto_curve25519_field_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief sqrt(-1) in the field: one of the two square roots of -1.
 * @return A constant element.
 */
const lh_crypto_curve25519_field_t *
lh_crypto_curve25519_field_get_sqrt_m1(void);

/**
 * @brief @p h = 0.
 * @param h Destination; must not be null.
 */
lh_void
lh_crypto_curve25519_field_zero(lh_crypto_curve25519_field_t *h);

/**
 * @brief @p h = @p f.
 * @param h Destination; must not be null.
 * @param f Source; must not be null.
 */
lh_void
lh_crypto_curve25519_field_assign(lh_crypto_curve25519_field_t *h,
                                  const lh_crypto_curve25519_field_t *f);

/**
 * @brief @p h = -@p f, limb by limb (unreduced).
 * @param h Destination; must not be null.
 * @param f Operand; must not be null.
 */
lh_void
lh_crypto_curve25519_field_neg(lh_crypto_curve25519_field_t *h,
                               const lh_crypto_curve25519_field_t *f);

/**
 * @brief @p h = @p f + @p g, limb by limb (unreduced).
 * @param h Destination; must not be null.
 * @param f Operand; must not be null.
 * @param g Operand; must not be null.
 */
lh_void
lh_crypto_curve25519_field_add(lh_crypto_curve25519_field_t *h,
                               const lh_crypto_curve25519_field_t *f,
                               const lh_crypto_curve25519_field_t *g);

/**
 * @brief @p h = @p f - @p g, limb by limb (unreduced).
 * @param h Destination; must not be null.
 * @param f Operand; must not be null.
 * @param g Operand; must not be null.
 */
lh_void
lh_crypto_curve25519_field_sub(lh_crypto_curve25519_field_t *h,
                               const lh_crypto_curve25519_field_t *f,
                               const lh_crypto_curve25519_field_t *g);

/**
 * @brief Swap @p f and @p g when @p bit is 1; leave both when it is 0. Constant time.
 * @param f   Element; must not be null.
 * @param g   Element; must not be null.
 * @param bit 0 or 1 (any other value mixes the two).
 */
lh_void
lh_crypto_curve25519_field_cswap(lh_crypto_curve25519_field_t *f, lh_crypto_curve25519_field_t *g,
                                 lh_s32_t bit);

/**
 * @brief @p f = @p g when @p bit is 1; leave @p f when it is 0. Constant time.
 * @param f   Destination; must not be null.
 * @param g   Source; must not be null.
 * @param bit 0 or 1.
 */
lh_void
lh_crypto_curve25519_field_ccopy(lh_crypto_curve25519_field_t *f,
                                 const lh_crypto_curve25519_field_t *g, lh_s32_t bit);

/**
 * @brief Carry ten wide limbs back into an element: @p h = @p t, limbs within
 *        ±1.1 * 2^25 (even) and ±1.1 * 2^24 (odd).
 *
 * The last step of every product. Relies on an arithmetic right shift of
 * negative numbers, as every supported compiler does.
 *
 * @param h Destination; must not be null.
 * @param t Ten wide limbs: |t[0]| < 2^63, |t[1..9]| < 2^62.
 */
lh_void
lh_crypto_curve25519_field_carry(lh_crypto_curve25519_field_t *h,
                                 const lh_s64_t t[LH_CRYPTO_CURVE25519_FIELD_LIMBS]);

/**
 * @brief Encode @p h as its canonical 32 little-endian bytes (fully reduced, top
 *        bit 0).
 * @param s Destination of 32 bytes; must not be null.
 * @param h A carried element; must not be null.
 */
lh_void
lh_crypto_curve25519_field_to_bytes(lh_byte_t s[LH_CRYPTO_CURVE25519_FIELD_SIZE],
                                    const lh_crypto_curve25519_field_t *h);

/**
 * @brief @p h = 1.
 * @param h Destination; must not be null.
 */
lh_void
lh_crypto_curve25519_field_one(lh_crypto_curve25519_field_t *h);

/**
 * @brief Decode 32 little-endian bytes, ignoring the top bit (Ed25519 keeps the
 *        sign of x there).
 *
 * A value at or above 2^255 - 19 is taken modulo the prime.
 *
 * @param h Destination; must not be null.
 * @param s 32 bytes; must not be null.
 */
lh_void
lh_crypto_curve25519_field_from_bytes(lh_crypto_curve25519_field_t *h,
                                      const lh_byte_t s[LH_CRYPTO_CURVE25519_FIELD_SIZE]);

/**
 * @brief @p h = @p f * @p g for a small @p g (a curve constant such as 121666).
 * @param h Destination; must not be null.
 * @param f Operand within the product bounds; must not be null.
 * @param g Multiplier, |g| < 2^31.
 */
lh_void
lh_crypto_curve25519_field_mul_small(lh_crypto_curve25519_field_t *h,
                                     const lh_crypto_curve25519_field_t *f, lh_s32_t g);

/**
 * @brief @p h = @p f * @p g.
 * @param h Destination; must not be null.
 * @param f Operand within the product bounds; must not be null.
 * @param g Operand within the product bounds; must not be null.
 */
lh_void
lh_crypto_curve25519_field_mul(lh_crypto_curve25519_field_t *h,
                               const lh_crypto_curve25519_field_t *f,
                               const lh_crypto_curve25519_field_t *g);

/**
 * @brief @p h = @p f^2, faster than ::lh_crypto_curve25519_field_mul of @p f by itself.
 * @param h Destination; must not be null.
 * @param f Operand within the product bounds; must not be null.
 */
lh_void
lh_crypto_curve25519_field_square(lh_crypto_curve25519_field_t *h,
                                  const lh_crypto_curve25519_field_t *f);

/**
 * @brief Whether the canonical value of @p f is odd (Ed25519's "negative").
 * @param f A carried element; must not be null.
 * @return ::lh_bool_true when odd.
 */
lh_bool_t
lh_crypto_curve25519_field_is_odd(const lh_crypto_curve25519_field_t *f);

/**
 * @brief Whether @p f and @p g are the same element, in constant time.
 * @param f A carried element; must not be null.
 * @param g A carried element; must not be null.
 * @return ::lh_bool_true when equal modulo the prime.
 */
lh_bool_t
lh_crypto_curve25519_field_is_equal(const lh_crypto_curve25519_field_t *f,
                                    const lh_crypto_curve25519_field_t *g);

/**
 * @brief Inverse square root: @p isr = sqrt(1 / @p x) when @p x is a non-zero
 *        square, sqrt(sqrt(-1) / @p x) when it is not, 0 when @p x is 0.
 *
 * The sign of the root is not fixed. Constant time.
 *
 * @param isr Destination; must not be null.
 * @param x   A carried element; must not be null.
 * @return ::lh_bool_true when @p x is a square (0 included).
 */
lh_bool_t
lh_crypto_curve25519_field_invsqrt(lh_crypto_curve25519_field_t *isr,
                                   const lh_crypto_curve25519_field_t *x);

/**
 * @brief @p out = 1 / @p x (0 for 0), through the inverse square root of x^2.
 * @param out Destination; must not be null.
 * @param x   A carried element; must not be null.
 */
lh_void
lh_crypto_curve25519_field_invert(lh_crypto_curve25519_field_t *out,
                                  const lh_crypto_curve25519_field_t *x);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRYPTO_CURVE25519_FIELD_H */
