#include <lh/core/crypto/curve25519/field.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/bit/endian.h>
#include <lh/core/crypto/curve25519/field/initializer.h>
#include <lh/core/crypto/is_equal.h>
#include <lh/core/crypto/wipe.h>
#include <lh/core/math.h>

/* Ported from Monocypher 4 (CC0 / BSD-2-Clause, Loup Vaillant): the same limb
   layout, the same carry chain and the same bounds, so its proofs of range in the
   comments there hold here. */

const lh_crypto_curve25519_field_t m_crypto_curve25519_field_sqrt_m1 =
    lh_crypto_curve25519_field_initializer(-32595792, -7943725, 9377950, 3500415, 12389472, -272473,
                                           -25146209, -2005654, 326686, 11406482);

const lh_crypto_curve25519_field_t *
lh_crypto_curve25519_field_get_sqrt_m1(void)
{
    return lh_addr_of(m_crypto_curve25519_field_sqrt_m1);
}

lh_void
lh_crypto_curve25519_field_zero(lh_crypto_curve25519_field_t *h)
{
    for (lh_usize_t i = 0U; i < LH_CRYPTO_CURVE25519_FIELD_LIMBS; i = lh_math_add_one(i))
    {
        h->limb[i] = 0;
    }
}

lh_void
lh_crypto_curve25519_field_assign(lh_crypto_curve25519_field_t *h,
                                  const lh_crypto_curve25519_field_t *f)
{
    for (lh_usize_t i = 0U; i < LH_CRYPTO_CURVE25519_FIELD_LIMBS; i = lh_math_add_one(i))
    {
        h->limb[i] = f->limb[i];
    }
}

lh_void
lh_crypto_curve25519_field_neg(lh_crypto_curve25519_field_t *h,
                               const lh_crypto_curve25519_field_t *f)
{
    for (lh_usize_t i = 0U; i < LH_CRYPTO_CURVE25519_FIELD_LIMBS; i = lh_math_add_one(i))
    {
        h->limb[i] = -f->limb[i];
    }
}

lh_void
lh_crypto_curve25519_field_add(lh_crypto_curve25519_field_t *h,
                               const lh_crypto_curve25519_field_t *f,
                               const lh_crypto_curve25519_field_t *g)
{
    for (lh_usize_t i = 0U; i < LH_CRYPTO_CURVE25519_FIELD_LIMBS; i = lh_math_add_one(i))
    {
        h->limb[i] = f->limb[i] + g->limb[i];
    }
}

lh_void
lh_crypto_curve25519_field_sub(lh_crypto_curve25519_field_t *h,
                               const lh_crypto_curve25519_field_t *f,
                               const lh_crypto_curve25519_field_t *g)
{
    for (lh_usize_t i = 0U; i < LH_CRYPTO_CURVE25519_FIELD_LIMBS; i = lh_math_add_one(i))
    {
        h->limb[i] = f->limb[i] - g->limb[i];
    }
}

lh_void
lh_crypto_curve25519_field_cswap(lh_crypto_curve25519_field_t *f, lh_crypto_curve25519_field_t *g,
                                 lh_s32_t bit)
{
    const lh_s32_t mask = -bit; /* 0 or all ones */
    for (lh_usize_t i = 0U; i < LH_CRYPTO_CURVE25519_FIELD_LIMBS; i = lh_math_add_one(i))
    {
        const lh_s32_t x = (f->limb[i] ^ g->limb[i]) & mask;
        f->limb[i] = f->limb[i] ^ x;
        g->limb[i] = g->limb[i] ^ x;
    }
}

lh_void
lh_crypto_curve25519_field_ccopy(lh_crypto_curve25519_field_t *f,
                                 const lh_crypto_curve25519_field_t *g, lh_s32_t bit)
{
    const lh_s32_t mask = -bit; /* 0 or all ones */
    for (lh_usize_t i = 0U; i < LH_CRYPTO_CURVE25519_FIELD_LIMBS; i = lh_math_add_one(i))
    {
        f->limb[i] = f->limb[i] ^ ((f->limb[i] ^ g->limb[i]) & mask);
    }
}

lh_void
lh_crypto_curve25519_field_carry(lh_crypto_curve25519_field_t *h,
                                 const lh_s64_t t[LH_CRYPTO_CURVE25519_FIELD_LIMBS])
{
    /* Signed carries, alternately at 26 and 25 bits; the last limb wraps around
       with a factor 19 (2^255 = 19 modulo the prime). x >> n is floor(x / 2^n): an
       arithmetic shift, as every supported compiler does. */
    lh_s64_t t0 = t[0];
    lh_s64_t t1 = t[1];
    lh_s64_t t2 = t[2];
    lh_s64_t t3 = t[3];
    lh_s64_t t4 = t[4];
    lh_s64_t t5 = t[5];
    lh_s64_t t6 = t[6];
    lh_s64_t t7 = t[7];
    lh_s64_t t8 = t[8];
    lh_s64_t t9 = t[9];
    lh_s64_t c;
    c = (t0 + ((lh_s64_t)1 << 25)) >> 26;
    t0 -= c * ((lh_s64_t)1 << 26);
    t1 += c;
    c = (t4 + ((lh_s64_t)1 << 25)) >> 26;
    t4 -= c * ((lh_s64_t)1 << 26);
    t5 += c;
    c = (t1 + ((lh_s64_t)1 << 24)) >> 25;
    t1 -= c * ((lh_s64_t)1 << 25);
    t2 += c;
    c = (t5 + ((lh_s64_t)1 << 24)) >> 25;
    t5 -= c * ((lh_s64_t)1 << 25);
    t6 += c;
    c = (t2 + ((lh_s64_t)1 << 25)) >> 26;
    t2 -= c * ((lh_s64_t)1 << 26);
    t3 += c;
    c = (t6 + ((lh_s64_t)1 << 25)) >> 26;
    t6 -= c * ((lh_s64_t)1 << 26);
    t7 += c;
    c = (t3 + ((lh_s64_t)1 << 24)) >> 25;
    t3 -= c * ((lh_s64_t)1 << 25);
    t4 += c;
    c = (t7 + ((lh_s64_t)1 << 24)) >> 25;
    t7 -= c * ((lh_s64_t)1 << 25);
    t8 += c;
    c = (t4 + ((lh_s64_t)1 << 25)) >> 26;
    t4 -= c * ((lh_s64_t)1 << 26);
    t5 += c;
    c = (t8 + ((lh_s64_t)1 << 25)) >> 26;
    t8 -= c * ((lh_s64_t)1 << 26);
    t9 += c;
    c = (t9 + ((lh_s64_t)1 << 24)) >> 25;
    t9 -= c * ((lh_s64_t)1 << 25);
    t0 += c * 19;
    c = (t0 + ((lh_s64_t)1 << 25)) >> 26;
    t0 -= c * ((lh_s64_t)1 << 26);
    t1 += c;
    h->limb[0] = (lh_s32_t)t0;
    h->limb[1] = (lh_s32_t)t1;
    h->limb[2] = (lh_s32_t)t2;
    h->limb[3] = (lh_s32_t)t3;
    h->limb[4] = (lh_s32_t)t4;
    h->limb[5] = (lh_s32_t)t5;
    h->limb[6] = (lh_s32_t)t6;
    h->limb[7] = (lh_s32_t)t7;
    h->limb[8] = (lh_s32_t)t8;
    h->limb[9] = (lh_s32_t)t9;
}

lh_void
lh_crypto_curve25519_field_to_bytes(lh_byte_t s[LH_CRYPTO_CURVE25519_FIELD_SIZE],
                                    const lh_crypto_curve25519_field_t *h)
{
    lh_s32_t t[LH_CRYPTO_CURVE25519_FIELD_LIMBS];
    for (lh_usize_t i = 0U; i < LH_CRYPTO_CURVE25519_FIELD_LIMBS; i = lh_math_add_one(i))
    {
        t[i] = h->limb[i];
    }
    /* |h| < 2^255 - 19: q ends 0 when h >= 0 and -1 when h < 0. */
    lh_s32_t q = (19 * t[9] + ((lh_s32_t)1 << 24)) >> 25;
    for (lh_usize_t i = 0U; i < 5U; i = lh_math_add_one(i))
    {
        q += t[2U * i];
        q >>= 26;
        q += t[2U * i + 1U];
        q >>= 25;
    }
    /* Adding 19 q brings h into [0, p); the carries settle every limb. */
    q *= 19;
    for (lh_usize_t i = 0U; i < 5U; i = lh_math_add_one(i))
    {
        t[2U * i] += q;
        q = t[2U * i] >> 26;
        t[2U * i] -= q * ((lh_s32_t)1 << 26);
        t[2U * i + 1U] += q;
        q = t[2U * i + 1U] >> 25;
        t[2U * i + 1U] -= q * ((lh_s32_t)1 << 25);
    }
    lh_bit_pack_le32(((lh_u32_t)t[0] >> 0) | ((lh_u32_t)t[1] << 26), s + 0);
    lh_bit_pack_le32(((lh_u32_t)t[1] >> 6) | ((lh_u32_t)t[2] << 19), s + 4);
    lh_bit_pack_le32(((lh_u32_t)t[2] >> 13) | ((lh_u32_t)t[3] << 13), s + 8);
    lh_bit_pack_le32(((lh_u32_t)t[3] >> 19) | ((lh_u32_t)t[4] << 6), s + 12);
    lh_bit_pack_le32(((lh_u32_t)t[5] >> 0) | ((lh_u32_t)t[6] << 25), s + 16);
    lh_bit_pack_le32(((lh_u32_t)t[6] >> 7) | ((lh_u32_t)t[7] << 19), s + 20);
    lh_bit_pack_le32(((lh_u32_t)t[7] >> 13) | ((lh_u32_t)t[8] << 12), s + 24);
    lh_bit_pack_le32(((lh_u32_t)t[8] >> 20) | ((lh_u32_t)t[9] << 6), s + 28);
    lh_crypto_wipe(t, sizeof(t));
}

lh_void
lh_crypto_curve25519_field_one(lh_crypto_curve25519_field_t *h)
{
    lh_crypto_curve25519_field_zero(h);
    h->limb[0] = 1;
}

lh_void
lh_crypto_curve25519_field_from_bytes(lh_crypto_curve25519_field_t *h,
                                      const lh_byte_t s[LH_CRYPTO_CURVE25519_FIELD_SIZE])
{
    lh_s64_t t[LH_CRYPTO_CURVE25519_FIELD_LIMBS];
    t[0] = (lh_s64_t)lh_bit_unpack_le32(s);           /* < 2^32 */
    t[1] = (lh_s64_t)lh_bit_unpack_le24(s + 4) << 6;  /* < 2^30 */
    t[2] = (lh_s64_t)lh_bit_unpack_le24(s + 7) << 5;  /* < 2^29 */
    t[3] = (lh_s64_t)lh_bit_unpack_le24(s + 10) << 3; /* < 2^27 */
    t[4] = (lh_s64_t)lh_bit_unpack_le24(s + 13) << 2; /* < 2^26 */
    t[5] = (lh_s64_t)lh_bit_unpack_le32(s + 16);      /* < 2^32 */
    t[6] = (lh_s64_t)lh_bit_unpack_le24(s + 20) << 7; /* < 2^31 */
    t[7] = (lh_s64_t)lh_bit_unpack_le24(s + 23) << 5; /* < 2^29 */
    t[8] = (lh_s64_t)lh_bit_unpack_le24(s + 26) << 4; /* < 2^28 */
    /* The top bit is not part of the value. */
    t[9] = (lh_s64_t)(lh_bit_unpack_le24(s + 29) & 0x7fffffU) << 2; /* < 2^25 */
    lh_crypto_curve25519_field_carry(h, t);
}

lh_void
lh_crypto_curve25519_field_mul_small(lh_crypto_curve25519_field_t *h,
                                     const lh_crypto_curve25519_field_t *f, lh_s32_t g)
{
    lh_s64_t t[LH_CRYPTO_CURVE25519_FIELD_LIMBS];
    for (lh_usize_t i = 0U; i < LH_CRYPTO_CURVE25519_FIELD_LIMBS; i = lh_math_add_one(i))
    {
        t[i] = f->limb[i] * (lh_s64_t)g;
    }
    lh_crypto_curve25519_field_carry(h, t);
}

lh_void
lh_crypto_curve25519_field_mul(lh_crypto_curve25519_field_t *h,
                               const lh_crypto_curve25519_field_t *f,
                               const lh_crypto_curve25519_field_t *g)
{
    /* Unrolled, as Monocypher: a rolled loop makes curve25519 twice as slow. */
    const lh_s32_t f0 = f->limb[0];
    const lh_s32_t f1 = f->limb[1];
    const lh_s32_t f2 = f->limb[2];
    const lh_s32_t f3 = f->limb[3];
    const lh_s32_t f4 = f->limb[4];
    const lh_s32_t f5 = f->limb[5];
    const lh_s32_t f6 = f->limb[6];
    const lh_s32_t f7 = f->limb[7];
    const lh_s32_t f8 = f->limb[8];
    const lh_s32_t f9 = f->limb[9];
    const lh_s32_t g0 = g->limb[0];
    const lh_s32_t g1 = g->limb[1];
    const lh_s32_t g2 = g->limb[2];
    const lh_s32_t g3 = g->limb[3];
    const lh_s32_t g4 = g->limb[4];
    const lh_s32_t g5 = g->limb[5];
    const lh_s32_t g6 = g->limb[6];
    const lh_s32_t g7 = g->limb[7];
    const lh_s32_t g8 = g->limb[8];
    const lh_s32_t g9 = g->limb[9];
    const lh_s32_t F1 = f1 * 2;
    const lh_s32_t F3 = f3 * 2;
    const lh_s32_t F5 = f5 * 2;
    const lh_s32_t F7 = f7 * 2;
    const lh_s32_t F9 = f9 * 2;
    const lh_s32_t G1 = g1 * 19;
    const lh_s32_t G2 = g2 * 19;
    const lh_s32_t G3 = g3 * 19;
    const lh_s32_t G4 = g4 * 19;
    const lh_s32_t G5 = g5 * 19;
    const lh_s32_t G6 = g6 * 19;
    const lh_s32_t G7 = g7 * 19;
    const lh_s32_t G8 = g8 * 19;
    const lh_s32_t G9 = g9 * 19;
    lh_s64_t t[LH_CRYPTO_CURVE25519_FIELD_LIMBS];
    t[0] = f0 * (lh_s64_t)g0 + F1 * (lh_s64_t)G9 + f2 * (lh_s64_t)G8 + F3 * (lh_s64_t)G7 +
           f4 * (lh_s64_t)G6 + F5 * (lh_s64_t)G5 + f6 * (lh_s64_t)G4 + F7 * (lh_s64_t)G3 +
           f8 * (lh_s64_t)G2 + F9 * (lh_s64_t)G1;
    t[1] = f0 * (lh_s64_t)g1 + f1 * (lh_s64_t)g0 + f2 * (lh_s64_t)G9 + f3 * (lh_s64_t)G8 +
           f4 * (lh_s64_t)G7 + f5 * (lh_s64_t)G6 + f6 * (lh_s64_t)G5 + f7 * (lh_s64_t)G4 +
           f8 * (lh_s64_t)G3 + f9 * (lh_s64_t)G2;
    t[2] = f0 * (lh_s64_t)g2 + F1 * (lh_s64_t)g1 + f2 * (lh_s64_t)g0 + F3 * (lh_s64_t)G9 +
           f4 * (lh_s64_t)G8 + F5 * (lh_s64_t)G7 + f6 * (lh_s64_t)G6 + F7 * (lh_s64_t)G5 +
           f8 * (lh_s64_t)G4 + F9 * (lh_s64_t)G3;
    t[3] = f0 * (lh_s64_t)g3 + f1 * (lh_s64_t)g2 + f2 * (lh_s64_t)g1 + f3 * (lh_s64_t)g0 +
           f4 * (lh_s64_t)G9 + f5 * (lh_s64_t)G8 + f6 * (lh_s64_t)G7 + f7 * (lh_s64_t)G6 +
           f8 * (lh_s64_t)G5 + f9 * (lh_s64_t)G4;
    t[4] = f0 * (lh_s64_t)g4 + F1 * (lh_s64_t)g3 + f2 * (lh_s64_t)g2 + F3 * (lh_s64_t)g1 +
           f4 * (lh_s64_t)g0 + F5 * (lh_s64_t)G9 + f6 * (lh_s64_t)G8 + F7 * (lh_s64_t)G7 +
           f8 * (lh_s64_t)G6 + F9 * (lh_s64_t)G5;
    t[5] = f0 * (lh_s64_t)g5 + f1 * (lh_s64_t)g4 + f2 * (lh_s64_t)g3 + f3 * (lh_s64_t)g2 +
           f4 * (lh_s64_t)g1 + f5 * (lh_s64_t)g0 + f6 * (lh_s64_t)G9 + f7 * (lh_s64_t)G8 +
           f8 * (lh_s64_t)G7 + f9 * (lh_s64_t)G6;
    t[6] = f0 * (lh_s64_t)g6 + F1 * (lh_s64_t)g5 + f2 * (lh_s64_t)g4 + F3 * (lh_s64_t)g3 +
           f4 * (lh_s64_t)g2 + F5 * (lh_s64_t)g1 + f6 * (lh_s64_t)g0 + F7 * (lh_s64_t)G9 +
           f8 * (lh_s64_t)G8 + F9 * (lh_s64_t)G7;
    t[7] = f0 * (lh_s64_t)g7 + f1 * (lh_s64_t)g6 + f2 * (lh_s64_t)g5 + f3 * (lh_s64_t)g4 +
           f4 * (lh_s64_t)g3 + f5 * (lh_s64_t)g2 + f6 * (lh_s64_t)g1 + f7 * (lh_s64_t)g0 +
           f8 * (lh_s64_t)G9 + f9 * (lh_s64_t)G8;
    t[8] = f0 * (lh_s64_t)g8 + F1 * (lh_s64_t)g7 + f2 * (lh_s64_t)g6 + F3 * (lh_s64_t)g5 +
           f4 * (lh_s64_t)g4 + F5 * (lh_s64_t)g3 + f6 * (lh_s64_t)g2 + F7 * (lh_s64_t)g1 +
           f8 * (lh_s64_t)g0 + F9 * (lh_s64_t)G9;
    t[9] = f0 * (lh_s64_t)g9 + f1 * (lh_s64_t)g8 + f2 * (lh_s64_t)g7 + f3 * (lh_s64_t)g6 +
           f4 * (lh_s64_t)g5 + f5 * (lh_s64_t)g4 + f6 * (lh_s64_t)g3 + f7 * (lh_s64_t)g2 +
           f8 * (lh_s64_t)g1 + f9 * (lh_s64_t)g0;
    lh_crypto_curve25519_field_carry(h, t);
}

lh_void
lh_crypto_curve25519_field_square(lh_crypto_curve25519_field_t *h,
                                  const lh_crypto_curve25519_field_t *f)
{
    const lh_s32_t f0 = f->limb[0];
    const lh_s32_t f1 = f->limb[1];
    const lh_s32_t f2 = f->limb[2];
    const lh_s32_t f3 = f->limb[3];
    const lh_s32_t f4 = f->limb[4];
    const lh_s32_t f5 = f->limb[5];
    const lh_s32_t f6 = f->limb[6];
    const lh_s32_t f7 = f->limb[7];
    const lh_s32_t f8 = f->limb[8];
    const lh_s32_t f9 = f->limb[9];
    const lh_s32_t f0_2 = f0 * 2;
    const lh_s32_t f1_2 = f1 * 2;
    const lh_s32_t f2_2 = f2 * 2;
    const lh_s32_t f3_2 = f3 * 2;
    const lh_s32_t f4_2 = f4 * 2;
    const lh_s32_t f5_2 = f5 * 2;
    const lh_s32_t f6_2 = f6 * 2;
    const lh_s32_t f7_2 = f7 * 2;
    const lh_s32_t f5_38 = f5 * 38;
    const lh_s32_t f6_19 = f6 * 19;
    const lh_s32_t f7_38 = f7 * 38;
    const lh_s32_t f8_19 = f8 * 19;
    const lh_s32_t f9_38 = f9 * 38;
    lh_s64_t t[LH_CRYPTO_CURVE25519_FIELD_LIMBS];
    t[0] = f0 * (lh_s64_t)f0 + f1_2 * (lh_s64_t)f9_38 + f2_2 * (lh_s64_t)f8_19 +
           f3_2 * (lh_s64_t)f7_38 + f4_2 * (lh_s64_t)f6_19 + f5 * (lh_s64_t)f5_38;
    t[1] = f0_2 * (lh_s64_t)f1 + f2 * (lh_s64_t)f9_38 + f3_2 * (lh_s64_t)f8_19 +
           f4 * (lh_s64_t)f7_38 + f5_2 * (lh_s64_t)f6_19;
    t[2] = f0_2 * (lh_s64_t)f2 + f1_2 * (lh_s64_t)f1 + f3_2 * (lh_s64_t)f9_38 +
           f4_2 * (lh_s64_t)f8_19 + f5_2 * (lh_s64_t)f7_38 + f6 * (lh_s64_t)f6_19;
    t[3] = f0_2 * (lh_s64_t)f3 + f1_2 * (lh_s64_t)f2 + f4 * (lh_s64_t)f9_38 +
           f5_2 * (lh_s64_t)f8_19 + f6 * (lh_s64_t)f7_38;
    t[4] = f0_2 * (lh_s64_t)f4 + f1_2 * (lh_s64_t)f3_2 + f2 * (lh_s64_t)f2 +
           f5_2 * (lh_s64_t)f9_38 + f6_2 * (lh_s64_t)f8_19 + f7 * (lh_s64_t)f7_38;
    t[5] = f0_2 * (lh_s64_t)f5 + f1_2 * (lh_s64_t)f4 + f2_2 * (lh_s64_t)f3 + f6 * (lh_s64_t)f9_38 +
           f7_2 * (lh_s64_t)f8_19;
    t[6] = f0_2 * (lh_s64_t)f6 + f1_2 * (lh_s64_t)f5_2 + f2_2 * (lh_s64_t)f4 + f3_2 * (lh_s64_t)f3 +
           f7_2 * (lh_s64_t)f9_38 + f8 * (lh_s64_t)f8_19;
    t[7] = f0_2 * (lh_s64_t)f7 + f1_2 * (lh_s64_t)f6 + f2_2 * (lh_s64_t)f5 + f3_2 * (lh_s64_t)f4 +
           f8 * (lh_s64_t)f9_38;
    t[8] = f0_2 * (lh_s64_t)f8 + f1_2 * (lh_s64_t)f7_2 + f2_2 * (lh_s64_t)f6 +
           f3_2 * (lh_s64_t)f5_2 + f4 * (lh_s64_t)f4 + f9 * (lh_s64_t)f9_38;
    t[9] = f0_2 * (lh_s64_t)f9 + f1_2 * (lh_s64_t)f8 + f2_2 * (lh_s64_t)f7 + f3_2 * (lh_s64_t)f6 +
           f4 * (lh_s64_t)f5_2;
    lh_crypto_curve25519_field_carry(h, t);
}

lh_bool_t
lh_crypto_curve25519_field_is_odd(const lh_crypto_curve25519_field_t *f)
{
    lh_byte_t s[LH_CRYPTO_CURVE25519_FIELD_SIZE];
    lh_crypto_curve25519_field_to_bytes(s, f);
    const lh_bool_t odd = (lh_bool_t)(s[0] & 1U);
    lh_crypto_wipe(s, sizeof(s));
    return odd;
}

lh_bool_t
lh_crypto_curve25519_field_is_equal(const lh_crypto_curve25519_field_t *f,
                                    const lh_crypto_curve25519_field_t *g)
{
    lh_byte_t fs[LH_CRYPTO_CURVE25519_FIELD_SIZE];
    lh_crypto_curve25519_field_to_bytes(fs, f);
    lh_byte_t gs[LH_CRYPTO_CURVE25519_FIELD_SIZE];
    lh_crypto_curve25519_field_to_bytes(gs, g);
    const lh_bool_t equal = lh_crypto_is_equal(fs, gs, sizeof(fs));
    lh_crypto_wipe(fs, sizeof(fs));
    lh_crypto_wipe(gs, sizeof(gs));
    return equal;
}

lh_bool_t
lh_crypto_curve25519_field_invsqrt(lh_crypto_curve25519_field_t *isr,
                                   const lh_crypto_curve25519_field_t *x)
{
    /* t0 = x^((p-5)/8), by Monocypher's addition chain. */
    lh_crypto_curve25519_field_t t0;
    lh_crypto_curve25519_field_square(lh_addr_of(t0), x);
    lh_crypto_curve25519_field_t t1;
    lh_crypto_curve25519_field_square(lh_addr_of(t1), lh_addr_of(t0));
    lh_crypto_curve25519_field_square(lh_addr_of(t1), lh_addr_of(t1));
    lh_crypto_curve25519_field_mul(lh_addr_of(t1), x, lh_addr_of(t1));
    lh_crypto_curve25519_field_mul(lh_addr_of(t0), lh_addr_of(t0), lh_addr_of(t1));
    lh_crypto_curve25519_field_square(lh_addr_of(t0), lh_addr_of(t0));
    lh_crypto_curve25519_field_mul(lh_addr_of(t0), lh_addr_of(t1), lh_addr_of(t0));
    lh_crypto_curve25519_field_square(lh_addr_of(t1), lh_addr_of(t0));
    for (lh_usize_t i = 1U; i < 5U; i = lh_math_add_one(i))
    {
        lh_crypto_curve25519_field_square(lh_addr_of(t1), lh_addr_of(t1));
    }
    lh_crypto_curve25519_field_mul(lh_addr_of(t0), lh_addr_of(t1), lh_addr_of(t0));
    lh_crypto_curve25519_field_square(lh_addr_of(t1), lh_addr_of(t0));
    for (lh_usize_t i = 1U; i < 10U; i = lh_math_add_one(i))
    {
        lh_crypto_curve25519_field_square(lh_addr_of(t1), lh_addr_of(t1));
    }
    lh_crypto_curve25519_field_mul(lh_addr_of(t1), lh_addr_of(t1), lh_addr_of(t0));
    lh_crypto_curve25519_field_t t2;
    lh_crypto_curve25519_field_square(lh_addr_of(t2), lh_addr_of(t1));
    for (lh_usize_t i = 1U; i < 20U; i = lh_math_add_one(i))
    {
        lh_crypto_curve25519_field_square(lh_addr_of(t2), lh_addr_of(t2));
    }
    lh_crypto_curve25519_field_mul(lh_addr_of(t1), lh_addr_of(t2), lh_addr_of(t1));
    lh_crypto_curve25519_field_square(lh_addr_of(t1), lh_addr_of(t1));
    for (lh_usize_t i = 1U; i < 10U; i = lh_math_add_one(i))
    {
        lh_crypto_curve25519_field_square(lh_addr_of(t1), lh_addr_of(t1));
    }
    lh_crypto_curve25519_field_mul(lh_addr_of(t0), lh_addr_of(t1), lh_addr_of(t0));
    lh_crypto_curve25519_field_square(lh_addr_of(t1), lh_addr_of(t0));
    for (lh_usize_t i = 1U; i < 50U; i = lh_math_add_one(i))
    {
        lh_crypto_curve25519_field_square(lh_addr_of(t1), lh_addr_of(t1));
    }
    lh_crypto_curve25519_field_mul(lh_addr_of(t1), lh_addr_of(t1), lh_addr_of(t0));
    lh_crypto_curve25519_field_square(lh_addr_of(t2), lh_addr_of(t1));
    for (lh_usize_t i = 1U; i < 100U; i = lh_math_add_one(i))
    {
        lh_crypto_curve25519_field_square(lh_addr_of(t2), lh_addr_of(t2));
    }
    lh_crypto_curve25519_field_mul(lh_addr_of(t1), lh_addr_of(t2), lh_addr_of(t1));
    lh_crypto_curve25519_field_square(lh_addr_of(t1), lh_addr_of(t1));
    for (lh_usize_t i = 1U; i < 50U; i = lh_math_add_one(i))
    {
        lh_crypto_curve25519_field_square(lh_addr_of(t1), lh_addr_of(t1));
    }
    lh_crypto_curve25519_field_mul(lh_addr_of(t0), lh_addr_of(t1), lh_addr_of(t0));
    lh_crypto_curve25519_field_square(lh_addr_of(t0), lh_addr_of(t0));
    for (lh_usize_t i = 1U; i < 2U; i = lh_math_add_one(i))
    {
        lh_crypto_curve25519_field_square(lh_addr_of(t0), lh_addr_of(t0));
    }
    lh_crypto_curve25519_field_mul(lh_addr_of(t0), lh_addr_of(t0), x);

    /* quartic = x^((p-1)/4): 1 or -1 when x is a square, ±sqrt(-1) when not. */
    lh_crypto_curve25519_field_t *quartic = lh_addr_of(t1);
    lh_crypto_curve25519_field_square(quartic, lh_addr_of(t0));
    lh_crypto_curve25519_field_mul(quartic, quartic, x);

    lh_crypto_curve25519_field_t *check = lh_addr_of(t2);
    lh_crypto_curve25519_field_zero(check);
    const lh_s32_t z0 = (lh_s32_t)lh_crypto_curve25519_field_is_equal(x, check);
    lh_crypto_curve25519_field_one(check);
    const lh_s32_t p1 = (lh_s32_t)lh_crypto_curve25519_field_is_equal(quartic, check);
    lh_crypto_curve25519_field_neg(check, check);
    const lh_s32_t m1 = (lh_s32_t)lh_crypto_curve25519_field_is_equal(quartic, check);
    lh_crypto_curve25519_field_neg(check, lh_crypto_curve25519_field_get_sqrt_m1());
    const lh_s32_t ms = (lh_s32_t)lh_crypto_curve25519_field_is_equal(quartic, check);

    /* quartic -1 or -sqrt(-1): isr = t0 * sqrt(-1); otherwise isr = t0. */
    lh_crypto_curve25519_field_mul(isr, lh_addr_of(t0), lh_crypto_curve25519_field_get_sqrt_m1());
    lh_crypto_curve25519_field_ccopy(isr, lh_addr_of(t0), 1 - (m1 | ms));

    lh_crypto_wipe(lh_addr_of(t0), sizeof(t0));
    lh_crypto_wipe(lh_addr_of(t1), sizeof(t1));
    lh_crypto_wipe(lh_addr_of(t2), sizeof(t2));
    return (lh_bool_t)(p1 | m1 | z0);
}

lh_void
lh_crypto_curve25519_field_invert(lh_crypto_curve25519_field_t *out,
                                  const lh_crypto_curve25519_field_t *x)
{
    /* 1/x = x * invsqrt(x^2)^2: the square drops the root's sign. */
    lh_crypto_curve25519_field_t tmp;
    lh_crypto_curve25519_field_square(lh_addr_of(tmp), x);
    LH_ATTRIBUTE_UNUSED(lh_crypto_curve25519_field_invsqrt(lh_addr_of(tmp), lh_addr_of(tmp)));
    lh_crypto_curve25519_field_square(lh_addr_of(tmp), lh_addr_of(tmp));
    lh_crypto_curve25519_field_mul(out, lh_addr_of(tmp), x);
    lh_crypto_wipe(lh_addr_of(tmp), sizeof(tmp));
}
