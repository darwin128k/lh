#include <lh/core/crypto/ed25519/point.h>
#include <lh/core/bit/endian.h>
#include <lh/core/crypto/curve25519/field/initializer.h>
#include <lh/core/crypto/ed25519/base.h>
#include <lh/core/crypto/ed25519/slide.h>
#include <lh/core/crypto/is_equal.h>
#include <lh/core/crypto/wipe.h>
#include <lh/core/math.h>

/* Ported from Monocypher 4 (CC0 / BSD-2-Clause, Loup Vaillant). */

const lh_crypto_curve25519_field_t m_crypto_ed25519_point_d =
    lh_crypto_curve25519_field_initializer(-10913610, 13857413, -15372611, 6949391, 114729,
                                           -8787816, -6275908, -3247719, -18696448, -12055116);

const lh_crypto_curve25519_field_t m_crypto_ed25519_point_d2 =
    lh_crypto_curve25519_field_initializer(-21827239, -5839606, -30745221, 13898782, 229458,
                                           15978800, -12551817, -6495438, 29715968, 9444199);

/* 1/2 modulo L, and (2^256 - 1)/2 modulo L: they turn a scalar into the
   all-bits-set form of the combs. */
const lh_byte_t m_crypto_ed25519_point_half_mod_l[LH_CRYPTO_ED25519_SCALAR_SIZE] = {
    247, 233, 122, 46, 141, 49, 9, 44, 107, 206, 123, 81, 239, 124, 111, 10,
    0,   0,   0,   0,  0,   0,  0, 0,  0,   0,   0,   0,  0,   0,   0,   8,
};

const lh_byte_t m_crypto_ed25519_point_half_ones[LH_CRYPTO_ED25519_SCALAR_SIZE] = {
    142, 74,  204, 70,  186, 24,  118, 107, 184, 231, 190, 57,  250, 173, 119, 99,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 7,
};

/* The neutral point's encoding: y = 1, x = 0. */
const lh_byte_t m_crypto_ed25519_point_zero_encoding[LH_CRYPTO_ED25519_POINT_SIZE] = {1};

/* Window widths of the check: 3 bits for A (a small table on the stack), 5 for
   B (the constant window table). */
#define LH_CRYPTO_ED25519_POINT_A_WIDTH 3
#define LH_CRYPTO_ED25519_POINT_B_WIDTH 5
#define LH_CRYPTO_ED25519_POINT_A_TABLE (1 << (LH_CRYPTO_ED25519_POINT_A_WIDTH - 2))

const lh_crypto_curve25519_field_t *
lh_crypto_ed25519_point_get_d(void)
{
    return lh_addr_of(m_crypto_ed25519_point_d);
}

const lh_crypto_curve25519_field_t *
lh_crypto_ed25519_point_get_d2(void)
{
    return lh_addr_of(m_crypto_ed25519_point_d2);
}

lh_void
lh_crypto_ed25519_point_zero(lh_crypto_ed25519_point_t *p)
{
    lh_crypto_curve25519_field_zero(lh_addr_of(p->x));
    lh_crypto_curve25519_field_one(lh_addr_of(p->y));
    lh_crypto_curve25519_field_one(lh_addr_of(p->z));
    lh_crypto_curve25519_field_zero(lh_addr_of(p->t));
}

lh_void
lh_crypto_ed25519_point_to_bytes(lh_byte_t s[LH_CRYPTO_ED25519_POINT_SIZE],
                                 const lh_crypto_ed25519_point_t *h)
{
    lh_crypto_curve25519_field_t recip;
    lh_crypto_curve25519_field_invert(lh_addr_of(recip), lh_addr_of(h->z));
    lh_crypto_curve25519_field_t x;
    lh_crypto_curve25519_field_mul(lh_addr_of(x), lh_addr_of(h->x), lh_addr_of(recip));
    lh_crypto_curve25519_field_t y;
    lh_crypto_curve25519_field_mul(lh_addr_of(y), lh_addr_of(h->y), lh_addr_of(recip));
    lh_crypto_curve25519_field_to_bytes(s, lh_addr_of(y));
    s[31] ^= (lh_byte_t)(lh_crypto_curve25519_field_is_odd(lh_addr_of(x)) << 7);
    lh_crypto_wipe(lh_addr_of(recip), sizeof(recip));
    lh_crypto_wipe(lh_addr_of(x), sizeof(x));
    lh_crypto_wipe(lh_addr_of(y), sizeof(y));
}

lh_void
lh_crypto_ed25519_point_add(lh_crypto_ed25519_point_t *s, const lh_crypto_ed25519_point_t *p,
                            const lh_crypto_ed25519_point_cached_t *q)
{
    lh_crypto_curve25519_field_t a;
    lh_crypto_curve25519_field_add(lh_addr_of(a), lh_addr_of(p->y), lh_addr_of(p->x));
    lh_crypto_curve25519_field_t b;
    lh_crypto_curve25519_field_sub(lh_addr_of(b), lh_addr_of(p->y), lh_addr_of(p->x));
    lh_crypto_curve25519_field_mul(lh_addr_of(a), lh_addr_of(a), lh_addr_of(q->yp));
    lh_crypto_curve25519_field_mul(lh_addr_of(b), lh_addr_of(b), lh_addr_of(q->ym));
    lh_crypto_curve25519_field_add(lh_addr_of(s->y), lh_addr_of(a), lh_addr_of(b));
    lh_crypto_curve25519_field_sub(lh_addr_of(s->x), lh_addr_of(a), lh_addr_of(b));

    lh_crypto_curve25519_field_add(lh_addr_of(s->z), lh_addr_of(p->z), lh_addr_of(p->z));
    lh_crypto_curve25519_field_mul(lh_addr_of(s->z), lh_addr_of(s->z), lh_addr_of(q->z));
    lh_crypto_curve25519_field_mul(lh_addr_of(s->t), lh_addr_of(p->t), lh_addr_of(q->t2));
    lh_crypto_curve25519_field_add(lh_addr_of(a), lh_addr_of(s->z), lh_addr_of(s->t));
    lh_crypto_curve25519_field_sub(lh_addr_of(b), lh_addr_of(s->z), lh_addr_of(s->t));

    lh_crypto_curve25519_field_mul(lh_addr_of(s->t), lh_addr_of(s->x), lh_addr_of(s->y));
    lh_crypto_curve25519_field_mul(lh_addr_of(s->x), lh_addr_of(s->x), lh_addr_of(b));
    lh_crypto_curve25519_field_mul(lh_addr_of(s->y), lh_addr_of(s->y), lh_addr_of(a));
    lh_crypto_curve25519_field_mul(lh_addr_of(s->z), lh_addr_of(a), lh_addr_of(b));
}

lh_void
lh_crypto_ed25519_point_madd(lh_crypto_ed25519_point_t *s, const lh_crypto_ed25519_point_t *p,
                             const lh_crypto_ed25519_point_precomp_t *q,
                             lh_crypto_curve25519_field_t *a, lh_crypto_curve25519_field_t *b)
{
    lh_crypto_curve25519_field_add(a, lh_addr_of(p->y), lh_addr_of(p->x));
    lh_crypto_curve25519_field_sub(b, lh_addr_of(p->y), lh_addr_of(p->x));
    lh_crypto_curve25519_field_mul(a, a, lh_addr_of(q->yp));
    lh_crypto_curve25519_field_mul(b, b, lh_addr_of(q->ym));
    lh_crypto_curve25519_field_add(lh_addr_of(s->y), a, b);
    lh_crypto_curve25519_field_sub(lh_addr_of(s->x), a, b);

    lh_crypto_curve25519_field_add(lh_addr_of(s->z), lh_addr_of(p->z), lh_addr_of(p->z));
    lh_crypto_curve25519_field_mul(lh_addr_of(s->t), lh_addr_of(p->t), lh_addr_of(q->t2));
    lh_crypto_curve25519_field_add(a, lh_addr_of(s->z), lh_addr_of(s->t));
    lh_crypto_curve25519_field_sub(b, lh_addr_of(s->z), lh_addr_of(s->t));

    lh_crypto_curve25519_field_mul(lh_addr_of(s->t), lh_addr_of(s->x), lh_addr_of(s->y));
    lh_crypto_curve25519_field_mul(lh_addr_of(s->x), lh_addr_of(s->x), b);
    lh_crypto_curve25519_field_mul(lh_addr_of(s->y), lh_addr_of(s->y), a);
    lh_crypto_curve25519_field_mul(lh_addr_of(s->z), a, b);
}

lh_void
lh_crypto_ed25519_point_double(lh_crypto_ed25519_point_t *s, const lh_crypto_ed25519_point_t *p,
                               lh_crypto_ed25519_point_t *q)
{
    lh_crypto_curve25519_field_square(lh_addr_of(q->x), lh_addr_of(p->x));
    lh_crypto_curve25519_field_square(lh_addr_of(q->y), lh_addr_of(p->y));
    lh_crypto_curve25519_field_square(lh_addr_of(q->z), lh_addr_of(p->z));
    lh_crypto_curve25519_field_mul_small(lh_addr_of(q->z), lh_addr_of(q->z), 2);
    lh_crypto_curve25519_field_add(lh_addr_of(q->t), lh_addr_of(p->x), lh_addr_of(p->y));
    lh_crypto_curve25519_field_square(lh_addr_of(s->t), lh_addr_of(q->t));
    lh_crypto_curve25519_field_add(lh_addr_of(q->t), lh_addr_of(q->y), lh_addr_of(q->x));
    lh_crypto_curve25519_field_sub(lh_addr_of(q->y), lh_addr_of(q->y), lh_addr_of(q->x));
    lh_crypto_curve25519_field_sub(lh_addr_of(q->x), lh_addr_of(s->t), lh_addr_of(q->t));
    lh_crypto_curve25519_field_sub(lh_addr_of(q->z), lh_addr_of(q->z), lh_addr_of(q->y));

    lh_crypto_curve25519_field_mul(lh_addr_of(s->x), lh_addr_of(q->x), lh_addr_of(q->z));
    lh_crypto_curve25519_field_mul(lh_addr_of(s->y), lh_addr_of(q->t), lh_addr_of(q->y));
    lh_crypto_curve25519_field_mul(lh_addr_of(s->z), lh_addr_of(q->y), lh_addr_of(q->z));
    lh_crypto_curve25519_field_mul(lh_addr_of(s->t), lh_addr_of(q->x), lh_addr_of(q->t));
}

lh_bool_t
lh_crypto_ed25519_point_from_bytes_neg_vartime(lh_crypto_ed25519_point_t *h,
                                               const lh_byte_t s[LH_CRYPTO_ED25519_POINT_SIZE])
{
    lh_crypto_curve25519_field_from_bytes(lh_addr_of(h->y), s);
    lh_crypto_curve25519_field_one(lh_addr_of(h->z));
    /* t = y^2, x = d y^2 */
    lh_crypto_curve25519_field_square(lh_addr_of(h->t), lh_addr_of(h->y));
    lh_crypto_curve25519_field_mul(lh_addr_of(h->x), lh_addr_of(h->t),
                                   lh_crypto_ed25519_point_get_d());
    /* t = y^2 - 1, x = d y^2 + 1 */
    lh_crypto_curve25519_field_sub(lh_addr_of(h->t), lh_addr_of(h->t), lh_addr_of(h->z));
    lh_crypto_curve25519_field_add(lh_addr_of(h->x), lh_addr_of(h->x), lh_addr_of(h->z));
    /* x = sqrt(num / den) = num * invsqrt(num * den) */
    lh_crypto_curve25519_field_mul(lh_addr_of(h->x), lh_addr_of(h->t), lh_addr_of(h->x));
    if (!lh_crypto_curve25519_field_invsqrt(lh_addr_of(h->x), lh_addr_of(h->x)))
    {
        return lh_bool_false; /* not on the curve */
    }
    lh_crypto_curve25519_field_mul(lh_addr_of(h->x), lh_addr_of(h->t), lh_addr_of(h->x));
    /* The negation: x takes the sign opposite to the encoded one. */
    if ((lh_s32_t)lh_crypto_curve25519_field_is_odd(lh_addr_of(h->x)) == (s[31] >> 7))
    {
        lh_crypto_curve25519_field_neg(lh_addr_of(h->x), lh_addr_of(h->x));
    }
    lh_crypto_curve25519_field_mul(lh_addr_of(h->t), lh_addr_of(h->x), lh_addr_of(h->y));
    return lh_bool_true;
}

lh_void
lh_crypto_ed25519_point_cache(lh_crypto_ed25519_point_cached_t *c,
                              const lh_crypto_ed25519_point_t *p)
{
    lh_crypto_curve25519_field_add(lh_addr_of(c->yp), lh_addr_of(p->y), lh_addr_of(p->x));
    lh_crypto_curve25519_field_sub(lh_addr_of(c->ym), lh_addr_of(p->y), lh_addr_of(p->x));
    lh_crypto_curve25519_field_assign(lh_addr_of(c->z), lh_addr_of(p->z));
    lh_crypto_curve25519_field_mul(lh_addr_of(c->t2), lh_addr_of(p->t),
                                   lh_crypto_ed25519_point_get_d2());
}

lh_void
lh_crypto_ed25519_point_sub(lh_crypto_ed25519_point_t *s, const lh_crypto_ed25519_point_t *p,
                            const lh_crypto_ed25519_point_cached_t *q)
{
    /* -q: swap y + x and y - x, negate t2. */
    lh_crypto_ed25519_point_cached_t neg;
    lh_crypto_curve25519_field_assign(lh_addr_of(neg.ym), lh_addr_of(q->yp));
    lh_crypto_curve25519_field_assign(lh_addr_of(neg.yp), lh_addr_of(q->ym));
    lh_crypto_curve25519_field_assign(lh_addr_of(neg.z), lh_addr_of(q->z));
    lh_crypto_curve25519_field_neg(lh_addr_of(neg.t2), lh_addr_of(q->t2));
    lh_crypto_ed25519_point_add(s, p, lh_addr_of(neg));
}

lh_void
lh_crypto_ed25519_point_msub(lh_crypto_ed25519_point_t *s, const lh_crypto_ed25519_point_t *p,
                             const lh_crypto_ed25519_point_precomp_t *q,
                             lh_crypto_curve25519_field_t *a, lh_crypto_curve25519_field_t *b)
{
    lh_crypto_ed25519_point_precomp_t neg;
    lh_crypto_curve25519_field_assign(lh_addr_of(neg.ym), lh_addr_of(q->yp));
    lh_crypto_curve25519_field_assign(lh_addr_of(neg.yp), lh_addr_of(q->ym));
    lh_crypto_curve25519_field_neg(lh_addr_of(neg.t2), lh_addr_of(q->t2));
    lh_crypto_ed25519_point_madd(s, p, lh_addr_of(neg), a, b);
}

lh_void
lh_crypto_ed25519_point_comb_add(lh_crypto_ed25519_point_t *p,
                                 lh_crypto_ed25519_point_precomp_t *tmp_c,
                                 lh_crypto_curve25519_field_t *tmp_a,
                                 lh_crypto_curve25519_field_t *tmp_b,
                                 const lh_crypto_ed25519_point_precomp_t *comb,
                                 const lh_byte_t scalar[LH_CRYPTO_ED25519_SCALAR_SIZE], lh_s32_t i)
{
    const lh_u8_t teeth = (lh_u8_t)((lh_crypto_ed25519_scalar_get_bit(scalar, i)) +
                                    (lh_crypto_ed25519_scalar_get_bit(scalar, i + 32) << 1) +
                                    (lh_crypto_ed25519_scalar_get_bit(scalar, i + 64) << 2) +
                                    (lh_crypto_ed25519_scalar_get_bit(scalar, i + 96) << 3));
    const lh_u8_t high = (lh_u8_t)(teeth >> 3);
    const lh_u8_t index = (lh_u8_t)((teeth ^ (high - 1)) & 7);
    /* Read every entry, keep the one selected: no index depends on the secret. */
    for (lh_s32_t j = 0; j < (lh_s32_t)LH_CRYPTO_ED25519_BASE_TABLE_SIZE; j = lh_math_add_one(j))
    {
        const lh_s32_t select = 1 & (((j ^ index) - 1) >> 8);
        lh_crypto_curve25519_field_ccopy(lh_addr_of(tmp_c->yp), lh_addr_of(comb[j].yp), select);
        lh_crypto_curve25519_field_ccopy(lh_addr_of(tmp_c->ym), lh_addr_of(comb[j].ym), select);
        lh_crypto_curve25519_field_ccopy(lh_addr_of(tmp_c->t2), lh_addr_of(comb[j].t2), select);
    }
    /* A clear top tooth negates the entry. */
    lh_crypto_curve25519_field_neg(tmp_a, lh_addr_of(tmp_c->t2));
    lh_crypto_curve25519_field_cswap(lh_addr_of(tmp_c->t2), tmp_a, high ^ 1);
    lh_crypto_curve25519_field_cswap(lh_addr_of(tmp_c->yp), lh_addr_of(tmp_c->ym), high ^ 1);
    lh_crypto_ed25519_point_madd(p, p, tmp_c, tmp_a, tmp_b);
}

lh_void
lh_crypto_ed25519_point_scalarmult_base(lh_crypto_ed25519_point_t *p,
                                        const lh_byte_t scalar[LH_CRYPTO_ED25519_SCALAR_SIZE])
{
    /* All-bits-set form: s' = (s + (2^256 - 1)) / 2 modulo L; 1 means +1, 0 means -1. */
    lh_byte_t s_scalar[LH_CRYPTO_ED25519_SCALAR_SIZE];
    lh_crypto_ed25519_scalar_mul_add(s_scalar, scalar, m_crypto_ed25519_point_half_mod_l,
                                     m_crypto_ed25519_point_half_ones);

    lh_crypto_ed25519_point_precomp_t tmp_c;
    lh_crypto_curve25519_field_one(lh_addr_of(tmp_c.yp));
    lh_crypto_curve25519_field_one(lh_addr_of(tmp_c.ym));
    lh_crypto_curve25519_field_zero(lh_addr_of(tmp_c.t2));

    /* The first round saves a double. */
    lh_crypto_ed25519_point_zero(p);
    /* The two temporaries of every addition. */
    lh_crypto_curve25519_field_t tmp_ab[2];
    lh_crypto_ed25519_point_comb_add(p, lh_addr_of(tmp_c), lh_addr_of(tmp_ab[0]),
                                     lh_addr_of(tmp_ab[1]), lh_crypto_ed25519_base_get_comb_low(),
                                     s_scalar, 31);
    lh_crypto_ed25519_point_comb_add(p, lh_addr_of(tmp_c), lh_addr_of(tmp_ab[0]),
                                     lh_addr_of(tmp_ab[1]), lh_crypto_ed25519_base_get_comb_high(),
                                     s_scalar, 31 + 128);
    lh_crypto_ed25519_point_t tmp_d;
    for (lh_s32_t i = 30; i >= 0; i = lh_math_sub(i, 1))
    {
        lh_crypto_ed25519_point_double(p, p, lh_addr_of(tmp_d));
        lh_crypto_ed25519_point_comb_add(p, lh_addr_of(tmp_c), lh_addr_of(tmp_ab[0]),
                                         lh_addr_of(tmp_ab[1]),
                                         lh_crypto_ed25519_base_get_comb_low(), s_scalar, i);
        lh_crypto_ed25519_point_comb_add(p, lh_addr_of(tmp_c), lh_addr_of(tmp_ab[0]),
                                         lh_addr_of(tmp_ab[1]),
                                         lh_crypto_ed25519_base_get_comb_high(), s_scalar, i + 128);
    }
    lh_crypto_wipe(tmp_ab, sizeof(tmp_ab));
    lh_crypto_wipe(lh_addr_of(tmp_c), sizeof(tmp_c));
    lh_crypto_wipe(lh_addr_of(tmp_d), sizeof(tmp_d));
    lh_crypto_wipe(s_scalar, sizeof(s_scalar));
}

lh_bool_t
lh_crypto_ed25519_point_check_equation(const lh_byte_t signature[2U * LH_CRYPTO_ED25519_POINT_SIZE],
                                       const lh_byte_t public_key[LH_CRYPTO_ED25519_POINT_SIZE],
                                       const lh_byte_t h[LH_CRYPTO_ED25519_SCALAR_SIZE])
{
    const lh_byte_t *s = signature + LH_CRYPTO_ED25519_POINT_SIZE;

    /* A and R on the curve, 0 <= S < L. */
    lh_u32_t s32[LH_CRYPTO_ED25519_SCALAR_WORDS];
    lh_bit_unpack_le32_words(s32, s, LH_CRYPTO_ED25519_SCALAR_WORDS);
    lh_crypto_ed25519_point_t minus_a;
    if (!lh_crypto_ed25519_point_from_bytes_neg_vartime(lh_addr_of(minus_a), public_key) ||
        lh_crypto_ed25519_scalar_is_above_l(s32))
    {
        return lh_bool_false;
    }
    lh_crypto_ed25519_point_t minus_r;
    if (!lh_crypto_ed25519_point_from_bytes_neg_vartime(lh_addr_of(minus_r), signature))
    {
        return lh_bool_false;
    }

    /* -A, -3A: the odd multiples of the 3-bit window. */
    lh_crypto_ed25519_point_cached_t lut_a[LH_CRYPTO_ED25519_POINT_A_TABLE];
    {
        lh_crypto_ed25519_point_t minus_a2;
        lh_crypto_ed25519_point_t tmp;
        lh_crypto_ed25519_point_double(lh_addr_of(minus_a2), lh_addr_of(minus_a), lh_addr_of(tmp));
        lh_crypto_ed25519_point_cache(lh_addr_of(lut_a[0]), lh_addr_of(minus_a));
        for (lh_s32_t i = 1; i < LH_CRYPTO_ED25519_POINT_A_TABLE; i = lh_math_add_one(i))
        {
            lh_crypto_ed25519_point_add(lh_addr_of(tmp), lh_addr_of(minus_a2),
                                        lh_addr_of(lut_a[i - 1]));
            lh_crypto_ed25519_point_cache(lh_addr_of(lut_a[i]), lh_addr_of(tmp));
        }
    }

    /* sum = [s]B - [h]A: one double-and-add ladder over both sliding windows. */
    lh_crypto_ed25519_slide_t h_slide;
    lh_crypto_ed25519_slide_init(lh_addr_of(h_slide), h);
    lh_crypto_ed25519_slide_t s_slide;
    lh_crypto_ed25519_slide_init(lh_addr_of(s_slide), s);
    lh_s32_t i = lh_math_max((lh_s32_t)lh_crypto_ed25519_slide_get_next_check(lh_addr_of(h_slide)),
                             (lh_s32_t)lh_crypto_ed25519_slide_get_next_check(lh_addr_of(s_slide)));
    lh_crypto_ed25519_point_t *sum = lh_addr_of(minus_a); /* -A is in the table now */
    lh_crypto_ed25519_point_zero(sum);
    const lh_crypto_ed25519_point_precomp_t *window = lh_crypto_ed25519_base_get_window();
    while (i >= 0)
    {
        lh_crypto_ed25519_point_t tmp;
        lh_crypto_ed25519_point_double(sum, sum, lh_addr_of(tmp));
        const lh_s32_t h_digit = lh_crypto_ed25519_slide_step(
            lh_addr_of(h_slide), LH_CRYPTO_ED25519_POINT_A_WIDTH, i, h);
        const lh_s32_t s_digit = lh_crypto_ed25519_slide_step(
            lh_addr_of(s_slide), LH_CRYPTO_ED25519_POINT_B_WIDTH, i, s);
        if (h_digit > 0)
        {
            lh_crypto_ed25519_point_add(sum, sum, lh_addr_of(lut_a[h_digit / 2]));
        }
        if (h_digit < 0)
        {
            lh_crypto_ed25519_point_sub(sum, sum, lh_addr_of(lut_a[-h_digit / 2]));
        }
        lh_crypto_curve25519_field_t t1;
        lh_crypto_curve25519_field_t t2;
        if (s_digit > 0)
        {
            lh_crypto_ed25519_point_madd(sum, sum, window + s_digit / 2, lh_addr_of(t1),
                                         lh_addr_of(t2));
        }
        if (s_digit < 0)
        {
            lh_crypto_ed25519_point_msub(sum, sum, window + -s_digit / 2, lh_addr_of(t1),
                                         lh_addr_of(t2));
        }
        i = lh_math_sub(i, 1);
    }

    /* [8](sum - R) must be the neutral point: the cofactor removes any small-order
       part and agrees with batch verification. */
    lh_crypto_ed25519_point_cached_t cached;
    lh_crypto_ed25519_point_cache(lh_addr_of(cached), lh_addr_of(minus_r));
    lh_crypto_ed25519_point_add(sum, sum, lh_addr_of(cached));
    lh_crypto_ed25519_point_double(sum, sum, lh_addr_of(minus_r));
    lh_crypto_ed25519_point_double(sum, sum, lh_addr_of(minus_r));
    lh_crypto_ed25519_point_double(sum, sum, lh_addr_of(minus_r));
    lh_byte_t check[LH_CRYPTO_ED25519_POINT_SIZE];
    lh_crypto_ed25519_point_to_bytes(check, sum);
    return lh_crypto_is_equal(check, m_crypto_ed25519_point_zero_encoding, sizeof(check));
}

lh_void
lh_crypto_ed25519_point_scalarbase(lh_byte_t point[LH_CRYPTO_ED25519_POINT_SIZE],
                                   const lh_byte_t scalar[LH_CRYPTO_ED25519_SCALAR_SIZE])
{
    lh_crypto_ed25519_point_t p;
    lh_crypto_ed25519_point_scalarmult_base(lh_addr_of(p), scalar);
    lh_crypto_ed25519_point_to_bytes(point, lh_addr_of(p));
    lh_crypto_wipe(lh_addr_of(p), sizeof(p));
}
