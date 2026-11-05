/**
 * @file point.h
 * @brief ::lh_crypto_ed25519_point_t — a point of the Ed25519 curve, and the
 *        group operations Ed25519 signs and checks with.
 *
 * The twisted Edwards curve -x^2 + y^2 = 1 + d x^2 y^2 over GF(2^255 - 19)
 * (lh/core/crypto/curve25519/field.h), in extended projective coordinates
 * (x = X/Z, y = Y/Z, T = XY/Z). A point is added either as a cached point
 * (::lh_crypto_ed25519_point_cached_t, from ::lh_crypto_ed25519_point_cache) or
 * as a precomputed one with Z = 1 (::lh_crypto_ed25519_point_precomp_t, the
 * base tables of lh/core/crypto/ed25519/base.h).
 *
 * Two kinds of operation. Signing computes [s]B for a secret s:
 * ::lh_crypto_ed25519_point_scalarmult_base is constant time and wipes what it
 * held. Checking works on public data only: ::lh_crypto_ed25519_point_from_bytes_neg_vartime
 * and ::lh_crypto_ed25519_point_check_equation take variable time and wipe
 * nothing; never give them secrets. ::lh_crypto_ed25519_point_add / _sub keep
 * their temporaries too.
 *
 * Ported from Monocypher 4 (CC0 / BSD-2-Clause, Loup Vaillant): the same
 * formulas, the same twin 4-bit signed combs (Mike Hamburg, 2012) and the same
 * merged double-and-add check with sliding windows.
 */

#ifndef LH_CRYPTO_ED25519_POINT_H
#define LH_CRYPTO_ED25519_POINT_H

#include <lh/core/crypto/ed25519/point/cached.h>
#include <lh/core/crypto/ed25519/point/fields.h>
#include <lh/core/crypto/ed25519/point/precomp.h>
#include <lh/core/crypto/ed25519/scalar.h>

/**
 * @def LH_CRYPTO_ED25519_POINT_SIZE
 * @brief Bytes of an encoded point (32: y, and the sign of x in the top bit).
 */
#define LH_CRYPTO_ED25519_POINT_SIZE 32U

/**
 * @brief A point of the Ed25519 curve in extended projective coordinates.
 */
struct lh_crypto_ed25519_point
{
    /** Members: ::lh_crypto_ed25519_point_fields. */
    lh_crypto_ed25519_point_fields(lh_crypto_curve25519_field_t);
};

/** @copybrief lh_crypto_ed25519_point */
typedef struct lh_crypto_ed25519_point lh_crypto_ed25519_point_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The curve constant d = -121665 / 121666.
 * @return A constant field element.
 */
const lh_crypto_curve25519_field_t *
lh_crypto_ed25519_point_get_d(void);

/**
 * @brief 2d, the factor of a cached point's `t2`.
 * @return A constant field element.
 */
const lh_crypto_curve25519_field_t *
lh_crypto_ed25519_point_get_d2(void);

/**
 * @brief @p p = the neutral point (0, 1).
 * @param p Destination; must not be null.
 */
lh_void
lh_crypto_ed25519_point_zero(lh_crypto_ed25519_point_t *p);

/**
 * @brief Encode @p h: y, with the sign (parity) of x in the top bit. Constant time.
 * @param s Destination of 32 bytes.
 * @param h Point; must not be null.
 */
lh_void
lh_crypto_ed25519_point_to_bytes(lh_byte_t s[LH_CRYPTO_ED25519_POINT_SIZE],
                                 const lh_crypto_ed25519_point_t *h);

/**
 * @brief @p s = @p p + @p q. Temporaries are not wiped: public data only.
 * @param s Destination; may be @p p.
 * @param p Point.
 * @param q Cached point.
 */
lh_void
lh_crypto_ed25519_point_add(lh_crypto_ed25519_point_t *s, const lh_crypto_ed25519_point_t *p,
                            const lh_crypto_ed25519_point_cached_t *q);

/**
 * @brief @p s = @p p + @p q for a precomputed @p q (Z = 1), with the caller's
 *        temporaries @p a and @p b (wiped by the caller when secret).
 * @param s Destination; may be @p p.
 * @param p Point.
 * @param q Precomputed point.
 * @param a Temporary.
 * @param b Temporary.
 */
lh_void
lh_crypto_ed25519_point_madd(lh_crypto_ed25519_point_t *s, const lh_crypto_ed25519_point_t *p,
                             const lh_crypto_ed25519_point_precomp_t *q,
                             lh_crypto_curve25519_field_t *a, lh_crypto_curve25519_field_t *b);

/**
 * @brief @p s = 2 @p p, with the caller's temporary point @p q.
 * @param s Destination; may be @p p.
 * @param p Point.
 * @param q Temporary; not @p s or @p p.
 */
lh_void
lh_crypto_ed25519_point_double(lh_crypto_ed25519_point_t *s, const lh_crypto_ed25519_point_t *p,
                               lh_crypto_ed25519_point_t *q);

/**
 * @brief Decode 32 bytes as the NEGATION of the point they encode. Variable time:
 *        public data only.
 *
 * x = sqrt((y^2 - 1) / (d y^2 + 1)); a y with no such x is not on the curve.
 * Non-canonical encodings of y are accepted, as Monocypher does.
 *
 * @param h Destination; must not be null. Undefined when the result is false.
 * @param s 32 bytes.
 * @return ::lh_bool_true when @p s is a point of the curve.
 */
lh_bool_t
lh_crypto_ed25519_point_from_bytes_neg_vartime(lh_crypto_ed25519_point_t *h,
                                               const lh_byte_t s[LH_CRYPTO_ED25519_POINT_SIZE]);

/**
 * @brief Prepare @p p for addition.
 * @param c Destination; must not be null.
 * @param p Point; must not be null.
 */
lh_void
lh_crypto_ed25519_point_cache(lh_crypto_ed25519_point_cached_t *c,
                              const lh_crypto_ed25519_point_t *p);

/**
 * @brief @p s = @p p - @p q. Temporaries are not wiped: public data only.
 * @param s Destination; may be @p p.
 * @param p Point.
 * @param q Cached point.
 */
lh_void
lh_crypto_ed25519_point_sub(lh_crypto_ed25519_point_t *s, const lh_crypto_ed25519_point_t *p,
                            const lh_crypto_ed25519_point_cached_t *q);

/**
 * @brief @p s = @p p - @p q for a precomputed @p q. Public data only.
 * @param s Destination; may be @p p.
 * @param p Point.
 * @param q Precomputed point.
 * @param a Temporary.
 * @param b Temporary.
 */
lh_void
lh_crypto_ed25519_point_msub(lh_crypto_ed25519_point_t *s, const lh_crypto_ed25519_point_t *p,
                             const lh_crypto_ed25519_point_precomp_t *q,
                             lh_crypto_curve25519_field_t *a, lh_crypto_curve25519_field_t *b);

/**
 * @brief Add the comb entry the four "teeth" bits i, i+32, i+64, i+96 of
 *        @p scalar select, in constant time (every entry is read).
 *
 * One step of ::lh_crypto_ed25519_point_scalarmult_base; the scalar is in
 * all-bits-set form (1 means +1, 0 means -1).
 *
 * @param p      Point to add to.
 * @param tmp_c  Temporary precomputed point.
 * @param tmp_a  Temporary.
 * @param tmp_b  Temporary.
 * @param comb   ::LH_CRYPTO_ED25519_BASE_TABLE_SIZE comb entries
 *               (lh/core/crypto/ed25519/base.h).
 * @param scalar 32 bytes.
 * @param i      The lowest tooth: 0..31 for the low comb, 128..159 for the high.
 */
lh_void
lh_crypto_ed25519_point_comb_add(lh_crypto_ed25519_point_t *p,
                                 lh_crypto_ed25519_point_precomp_t *tmp_c,
                                 lh_crypto_curve25519_field_t *tmp_a,
                                 lh_crypto_curve25519_field_t *tmp_b,
                                 const lh_crypto_ed25519_point_precomp_t *comb,
                                 const lh_byte_t scalar[LH_CRYPTO_ED25519_SCALAR_SIZE], lh_s32_t i);

/**
 * @brief @p p = [@p scalar]B, B the base point. Constant time; wipes its
 *        temporaries.
 *
 * Any 256-bit scalar, not only one below 2^252.
 *
 * @param p      Destination; must not be null.
 * @param scalar 32 bytes, secret.
 */
lh_void
lh_crypto_ed25519_point_scalarmult_base(lh_crypto_ed25519_point_t *p,
                                        const lh_byte_t scalar[LH_CRYPTO_ED25519_SCALAR_SIZE]);

/**
 * @brief The Ed25519 check: [8]([S]B - [h]A - R) = 0, where @p signature is R || S
 *        and @p public_key is A. Variable time: public data only.
 *
 * Refuses an A or R that is not on the curve and an S at or above L (no
 * malleability); accepts non-canonical encodings of A and R, as Monocypher does.
 * The cofactor 8 makes it agree with batch verification.
 *
 * @param signature  64 bytes: R, then S.
 * @param public_key 32 bytes: A.
 * @param h          32 bytes: SHA-512(R || A || message) reduced modulo L.
 * @return ::lh_bool_true when the equation holds.
 */
lh_bool_t
lh_crypto_ed25519_point_check_equation(const lh_byte_t signature[2U * LH_CRYPTO_ED25519_POINT_SIZE],
                                       const lh_byte_t public_key[LH_CRYPTO_ED25519_POINT_SIZE],
                                       const lh_byte_t h[LH_CRYPTO_ED25519_SCALAR_SIZE]);

/**
 * @brief The encoding of [@p scalar]B: a public key from a trimmed secret scalar.
 * @param point  Destination of 32 bytes.
 * @param scalar 32 bytes, secret.
 */
lh_void
lh_crypto_ed25519_point_scalarbase(lh_byte_t point[LH_CRYPTO_ED25519_POINT_SIZE],
                                   const lh_byte_t scalar[LH_CRYPTO_ED25519_SCALAR_SIZE]);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRYPTO_ED25519_POINT_H */
