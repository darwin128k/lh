#include <lh/core/crypto/ed25519/scalar.h>
#include <lh/core/bit/endian.h>
#include <lh/core/crypto/wipe.h>
#include <lh/core/math.h>
#include <lh/core/memory/std.h>

/* Ported from Monocypher 4 (CC0 / BSD-2-Clause, Loup Vaillant). */

/* L, lowest word first. */
const lh_u32_t m_crypto_ed25519_scalar_l[LH_CRYPTO_ED25519_SCALAR_WORDS] = {
    0x5cf5d3edU, 0x5812631aU, 0xa2f79cd6U, 0x14def9deU,
    0x00000000U, 0x00000000U, 0x00000000U, 0x10000000U,
};

/* floor(2^512 / L), the Barrett constant of ::lh_crypto_ed25519_scalar_mod_l. */
const lh_u32_t m_crypto_ed25519_scalar_barrett[LH_CRYPTO_ED25519_SCALAR_WORDS + 1U] = {
    0x0a2c131bU, 0xed9ce5a3U, 0x086329a7U, 0x2106215dU, 0xffffffebU,
    0xffffffffU, 0xffffffffU, 0xffffffffU, 0x0000000fU,
};

const lh_u32_t *
lh_crypto_ed25519_scalar_get_l(void)
{
    return m_crypto_ed25519_scalar_l;
}

lh_void
lh_crypto_ed25519_scalar_trim(lh_byte_t out[LH_CRYPTO_ED25519_SCALAR_SIZE],
                              const lh_byte_t in[LH_CRYPTO_ED25519_SCALAR_SIZE])
{
    lh_memory_std_move(out, in, LH_CRYPTO_ED25519_SCALAR_SIZE);
    out[0] &= 248U;
    out[31] &= 127U;
    out[31] |= 64U;
}

lh_s32_t
lh_crypto_ed25519_scalar_get_bit(const lh_byte_t s[LH_CRYPTO_ED25519_SCALAR_SIZE], lh_s32_t i)
{
    if (i < 0)
    {
        return 0;
    }
    return (s[i >> 3] >> (i & 7)) & 1;
}

lh_void
lh_crypto_ed25519_scalar_multiply(lh_u32_t p[2U * LH_CRYPTO_ED25519_SCALAR_WORDS],
                                  const lh_u32_t a[LH_CRYPTO_ED25519_SCALAR_WORDS],
                                  const lh_u32_t b[LH_CRYPTO_ED25519_SCALAR_WORDS])
{
    for (lh_usize_t i = 0U; i < LH_CRYPTO_ED25519_SCALAR_WORDS; i = lh_math_add_one(i))
    {
        lh_u64_t carry = 0U;
        for (lh_usize_t j = 0U; j < LH_CRYPTO_ED25519_SCALAR_WORDS; j = lh_math_add_one(j))
        {
            carry += p[i + j] + lh_cast_static(lh_u64_t, a[i]) * b[j];
            p[i + j] = lh_cast_static(lh_u32_t, carry);
            carry >>= 32;
        }
        p[i + LH_CRYPTO_ED25519_SCALAR_WORDS] = lh_cast_static(lh_u32_t, carry);
    }
}

lh_bool_t
lh_crypto_ed25519_scalar_is_above_l(const lh_u32_t x[LH_CRYPTO_ED25519_SCALAR_WORDS])
{
    /* x - L, with -L as ~L + 1: the last carry is 1 exactly when x >= L. */
    const lh_u32_t *l = lh_crypto_ed25519_scalar_get_l();
    lh_u64_t carry = 1U;
    for (lh_usize_t i = 0U; i < LH_CRYPTO_ED25519_SCALAR_WORDS; i = lh_math_add_one(i))
    {
        carry += lh_cast_static(lh_u64_t, x[i]) + (~l[i] & 0xffffffffU);
        carry >>= 32;
    }
    return (lh_bool_t)carry;
}

lh_void
lh_crypto_ed25519_scalar_remove_l(lh_u32_t r[LH_CRYPTO_ED25519_SCALAR_WORDS],
                                  const lh_u32_t x[LH_CRYPTO_ED25519_SCALAR_WORDS])
{
    const lh_u32_t *l = lh_crypto_ed25519_scalar_get_l();
    lh_u64_t carry = lh_cast_static(lh_u64_t, lh_crypto_ed25519_scalar_is_above_l(x));
    const lh_u32_t mask = ~lh_cast_static(lh_u32_t, carry) + 1U; /* 0 or all ones */
    for (lh_usize_t i = 0U; i < LH_CRYPTO_ED25519_SCALAR_WORDS; i = lh_math_add_one(i))
    {
        carry += lh_cast_static(lh_u64_t, x[i]) + (~l[i] & mask);
        r[i] = lh_cast_static(lh_u32_t, carry);
        carry >>= 32;
    }
}

lh_void
lh_crypto_ed25519_scalar_mod_l(lh_byte_t reduced[LH_CRYPTO_ED25519_SCALAR_SIZE],
                               const lh_u32_t x[2U * LH_CRYPTO_ED25519_SCALAR_WORDS])
{
    const lh_u32_t *l = lh_crypto_ed25519_scalar_get_l();
    const lh_u32_t *barrett = m_crypto_ed25519_scalar_barrett;

    /* xr = x * barrett */
    lh_u32_t xr[25] = {0};
    for (lh_usize_t i = 0U; i < 9U; i = lh_math_add_one(i))
    {
        lh_u64_t carry = 0U;
        for (lh_usize_t j = 0U; j < 16U; j = lh_math_add_one(j))
        {
            carry += xr[i + j] + lh_cast_static(lh_u64_t, barrett[i]) * x[j];
            xr[i + j] = lh_cast_static(lh_u32_t, carry);
            carry >>= 32;
        }
        xr[i + 16U] = lh_cast_static(lh_u32_t, carry);
    }
    /* xr = floor(xr / 2^512) * L, its lowest 256 bits only: the result is below
       2L. Reading xr[i + 16] is the division. */
    lh_memory_std_set(xr, 0, 8U * sizeof(lh_u32_t));
    for (lh_usize_t i = 0U; i < 8U; i = lh_math_add_one(i))
    {
        lh_u64_t carry = 0U;
        for (lh_usize_t j = 0U; j < 8U - i; j = lh_math_add_one(j))
        {
            carry += xr[i + j] + lh_cast_static(lh_u64_t, xr[i + 16U]) * l[j];
            xr[i + j] = lh_cast_static(lh_u32_t, carry);
            carry >>= 32;
        }
    }
    /* xr = x - xr */
    lh_u64_t carry = 1U;
    for (lh_usize_t i = 0U; i < 8U; i = lh_math_add_one(i))
    {
        carry += lh_cast_static(lh_u64_t, x[i]) + (~xr[i] & 0xffffffffU);
        xr[i] = lh_cast_static(lh_u32_t, carry);
        carry >>= 32;
    }
    /* Below 2L now: one conditional subtraction ends it. */
    lh_crypto_ed25519_scalar_remove_l(xr, xr);
    lh_bit_pack_le32_words(xr, LH_CRYPTO_ED25519_SCALAR_WORDS, reduced);
    lh_crypto_wipe(xr, sizeof(xr));
}

lh_void
lh_crypto_ed25519_scalar_reduce(lh_byte_t reduced[LH_CRYPTO_ED25519_SCALAR_SIZE],
                                const lh_byte_t expanded[2U * LH_CRYPTO_ED25519_SCALAR_SIZE])
{
    lh_u32_t x[2U * LH_CRYPTO_ED25519_SCALAR_WORDS];
    lh_bit_unpack_le32_words(x, expanded, 2U * LH_CRYPTO_ED25519_SCALAR_WORDS);
    lh_crypto_ed25519_scalar_mod_l(reduced, x);
    lh_crypto_wipe(x, sizeof(x));
}

lh_void
lh_crypto_ed25519_scalar_mul_add(lh_byte_t r[LH_CRYPTO_ED25519_SCALAR_SIZE],
                                 const lh_byte_t a[LH_CRYPTO_ED25519_SCALAR_SIZE],
                                 const lh_byte_t b[LH_CRYPTO_ED25519_SCALAR_SIZE],
                                 const lh_byte_t c[LH_CRYPTO_ED25519_SCALAR_SIZE])
{
    lh_u32_t wa[LH_CRYPTO_ED25519_SCALAR_WORDS];
    lh_bit_unpack_le32_words(wa, a, LH_CRYPTO_ED25519_SCALAR_WORDS);
    lh_u32_t wb[LH_CRYPTO_ED25519_SCALAR_WORDS];
    lh_bit_unpack_le32_words(wb, b, LH_CRYPTO_ED25519_SCALAR_WORDS);
    lh_u32_t p[2U * LH_CRYPTO_ED25519_SCALAR_WORDS] = {0};
    lh_bit_unpack_le32_words(p, c, LH_CRYPTO_ED25519_SCALAR_WORDS);
    lh_crypto_ed25519_scalar_multiply(p, wa, wb);
    lh_crypto_ed25519_scalar_mod_l(r, p);
    lh_crypto_wipe(p, sizeof(p));
    lh_crypto_wipe(wa, sizeof(wa));
    lh_crypto_wipe(wb, sizeof(wb));
}
