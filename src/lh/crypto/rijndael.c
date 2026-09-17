#include <lh/crypto/rijndael.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/memory/std.h>
#include <lh/null.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/util/bit/endian.h>
#include <lh/util/bit/rotate.h>
#include <lh/util/ptr.h>

static const lh_uchar_t m_sbox[256] = {
    0x63U, 0x7CU, 0x77U, 0x7BU, 0xF2U, 0x6BU, 0x6FU, 0xC5U, 0x30U, 0x01U, 0x67U, 0x2BU, 0xFEU,
    0xD7U, 0xABU, 0x76U, 0xCAU, 0x82U, 0xC9U, 0x7DU, 0xFAU, 0x59U, 0x47U, 0xF0U, 0xADU, 0xD4U,
    0xA2U, 0xAFU, 0x9CU, 0xA4U, 0x72U, 0xC0U, 0xB7U, 0xFDU, 0x93U, 0x26U, 0x36U, 0x3FU, 0xF7U,
    0xCCU, 0x34U, 0xA5U, 0xE5U, 0xF1U, 0x71U, 0xD8U, 0x31U, 0x15U, 0x04U, 0xC7U, 0x23U, 0xC3U,
    0x18U, 0x96U, 0x05U, 0x9AU, 0x07U, 0x12U, 0x80U, 0xE2U, 0xEBU, 0x27U, 0xB2U, 0x75U, 0x09U,
    0x83U, 0x2CU, 0x1AU, 0x1BU, 0x6EU, 0x5AU, 0xA0U, 0x52U, 0x3BU, 0xD6U, 0xB3U, 0x29U, 0xE3U,
    0x2FU, 0x84U, 0x53U, 0xD1U, 0x00U, 0xEDU, 0x20U, 0xFCU, 0xB1U, 0x5BU, 0x6AU, 0xCBU, 0xBEU,
    0x39U, 0x4AU, 0x4CU, 0x58U, 0xCFU, 0xD0U, 0xEFU, 0xAAU, 0xFBU, 0x43U, 0x4DU, 0x33U, 0x85U,
    0x45U, 0xF9U, 0x02U, 0x7FU, 0x50U, 0x3CU, 0x9FU, 0xA8U, 0x51U, 0xA3U, 0x40U, 0x8FU, 0x92U,
    0x9DU, 0x38U, 0xF5U, 0xBCU, 0xB6U, 0xDAU, 0x21U, 0x10U, 0xFFU, 0xF3U, 0xD2U, 0xCDU, 0x0CU,
    0x13U, 0xECU, 0x5FU, 0x97U, 0x44U, 0x17U, 0xC4U, 0xA7U, 0x7EU, 0x3DU, 0x64U, 0x5DU, 0x19U,
    0x73U, 0x60U, 0x81U, 0x4FU, 0xDCU, 0x22U, 0x2AU, 0x90U, 0x88U, 0x46U, 0xEEU, 0xB8U, 0x14U,
    0xDEU, 0x5EU, 0x0BU, 0xDBU, 0xE0U, 0x32U, 0x3AU, 0x0AU, 0x49U, 0x06U, 0x24U, 0x5CU, 0xC2U,
    0xD3U, 0xACU, 0x62U, 0x91U, 0x95U, 0xE4U, 0x79U, 0xE7U, 0xC8U, 0x37U, 0x6DU, 0x8DU, 0xD5U,
    0x4EU, 0xA9U, 0x6CU, 0x56U, 0xF4U, 0xEAU, 0x65U, 0x7AU, 0xAEU, 0x08U, 0xBAU, 0x78U, 0x25U,
    0x2EU, 0x1CU, 0xA6U, 0xB4U, 0xC6U, 0xE8U, 0xDDU, 0x74U, 0x1FU, 0x4BU, 0xBDU, 0x8BU, 0x8AU,
    0x70U, 0x3EU, 0xB5U, 0x66U, 0x48U, 0x03U, 0xF6U, 0x0EU, 0x61U, 0x35U, 0x57U, 0xB9U, 0x86U,
    0xC1U, 0x1DU, 0x9EU, 0xE1U, 0xF8U, 0x98U, 0x11U, 0x69U, 0xD9U, 0x8EU, 0x94U, 0x9BU, 0x1EU,
    0x87U, 0xE9U, 0xCEU, 0x55U, 0x28U, 0xDFU, 0x8CU, 0xA1U, 0x89U, 0x0DU, 0xBFU, 0xE6U, 0x42U,
    0x68U, 0x41U, 0x99U, 0x2DU, 0x0FU, 0xB0U, 0x54U, 0xBBU, 0x16U};

static const lh_uchar_t m_inv_sbox[256] = {
    0x52U, 0x09U, 0x6AU, 0xD5U, 0x30U, 0x36U, 0xA5U, 0x38U, 0xBFU, 0x40U, 0xA3U, 0x9EU, 0x81U,
    0xF3U, 0xD7U, 0xFBU, 0x7CU, 0xE3U, 0x39U, 0x82U, 0x9BU, 0x2FU, 0xFFU, 0x87U, 0x34U, 0x8EU,
    0x43U, 0x44U, 0xC4U, 0xDEU, 0xE9U, 0xCBU, 0x54U, 0x7BU, 0x94U, 0x32U, 0xA6U, 0xC2U, 0x23U,
    0x3DU, 0xEEU, 0x4CU, 0x95U, 0x0BU, 0x42U, 0xFAU, 0xC3U, 0x4EU, 0x08U, 0x2EU, 0xA1U, 0x66U,
    0x28U, 0xD9U, 0x24U, 0xB2U, 0x76U, 0x5BU, 0xA2U, 0x49U, 0x6DU, 0x8BU, 0xD1U, 0x25U, 0x72U,
    0xF8U, 0xF6U, 0x64U, 0x86U, 0x68U, 0x98U, 0x16U, 0xD4U, 0xA4U, 0x5CU, 0xCCU, 0x5DU, 0x65U,
    0xB6U, 0x92U, 0x6CU, 0x70U, 0x48U, 0x50U, 0xFDU, 0xEDU, 0xB9U, 0xDAU, 0x5EU, 0x15U, 0x46U,
    0x57U, 0xA7U, 0x8DU, 0x9DU, 0x84U, 0x90U, 0xD8U, 0xABU, 0x00U, 0x8CU, 0xBCU, 0xD3U, 0x0AU,
    0xF7U, 0xE4U, 0x58U, 0x05U, 0xB8U, 0xB3U, 0x45U, 0x06U, 0xD0U, 0x2CU, 0x1EU, 0x8FU, 0xCAU,
    0x3FU, 0x0FU, 0x02U, 0xC1U, 0xAFU, 0xBDU, 0x03U, 0x01U, 0x13U, 0x8AU, 0x6BU, 0x3AU, 0x91U,
    0x11U, 0x41U, 0x4FU, 0x67U, 0xDCU, 0xEAU, 0x97U, 0xF2U, 0xCFU, 0xCEU, 0xF0U, 0xB4U, 0xE6U,
    0x73U, 0x96U, 0xACU, 0x74U, 0x22U, 0xE7U, 0xADU, 0x35U, 0x85U, 0xE2U, 0xF9U, 0x37U, 0xE8U,
    0x1CU, 0x75U, 0xDFU, 0x6EU, 0x47U, 0xF1U, 0x1AU, 0x71U, 0x1DU, 0x29U, 0xC5U, 0x89U, 0x6FU,
    0xB7U, 0x62U, 0x0EU, 0xAAU, 0x18U, 0xBEU, 0x1BU, 0xFCU, 0x56U, 0x3EU, 0x4BU, 0xC6U, 0xD2U,
    0x79U, 0x20U, 0x9AU, 0xDBU, 0xC0U, 0xFEU, 0x78U, 0xCDU, 0x5AU, 0xF4U, 0x1FU, 0xDDU, 0xA8U,
    0x33U, 0x88U, 0x07U, 0xC7U, 0x31U, 0xB1U, 0x12U, 0x10U, 0x59U, 0x27U, 0x80U, 0xECU, 0x5FU,
    0x60U, 0x51U, 0x7FU, 0xA9U, 0x19U, 0xB5U, 0x4AU, 0x0DU, 0x2DU, 0xE5U, 0x7AU, 0x9FU, 0x93U,
    0xC9U, 0x9CU, 0xEFU, 0xA0U, 0xE0U, 0x3BU, 0x4DU, 0xAEU, 0x2AU, 0xF5U, 0xB0U, 0xC8U, 0xEBU,
    0xBBU, 0x3CU, 0x83U, 0x53U, 0x99U, 0x61U, 0x17U, 0x2BU, 0x04U, 0x7EU, 0xBAU, 0x77U, 0xD6U,
    0x26U, 0xE1U, 0x69U, 0x14U, 0x63U, 0x55U, 0x21U, 0x0CU, 0x7DU};

static lh_bool_t
lh_crypto_rijndael_size_ok(lh_usize_t n)
{
    return (n == 16U || n == 24U || n == 32U) ? lh_bool_true : lh_bool_false;
}

static lh_uchar_t
lh_crypto_rijndael_xtime(lh_uchar_t x)
{
    return lh_cast_static(lh_uchar_t, ((x << 1) ^ ((x & 0x80U) ? 0x1BU : 0U)));
}

static lh_uchar_t
lh_crypto_rijndael_gf_mul(lh_uchar_t a, lh_uchar_t b)
{
    lh_uchar_t p = 0;
    lh_usize_t i;

    for (i = 0; i < 8U; ++i)
    {
        if (b & 1U)
        {
            p = lh_cast_static(lh_uchar_t, (p ^ a));
        }
        a = lh_crypto_rijndael_xtime(a);
        b = lh_cast_static(lh_uchar_t, (b >> 1));
    }
    return p;
}

static void
lh_crypto_rijndael_shift_offsets(lh_usize_t nb, lh_usize_t *c1, lh_usize_t *c2, lh_usize_t *c3)
{
    if (nb == 8U)
    {
        *c1 = 1U;
        *c2 = 3U;
        *c3 = 4U;
    }
    else
    {
        *c1 = 1U;
        *c2 = 2U;
        *c3 = 3U;
    }
}

static void
lh_crypto_rijndael_shift_row(lh_uchar_t *state, lh_usize_t nb, lh_usize_t row, lh_usize_t shift,
                             lh_bool_t inverse)
{
    lh_uchar_t tmp[8];
    lh_usize_t i;
    lh_usize_t n = nb;

    for (i = 0; i < n; ++i)
    {
        tmp[i] = state[row + 4U * i];
    }
    for (i = 0; i < n; ++i)
    {
        lh_usize_t src = inverse ? (i + n - (shift % n)) % n : (i + shift) % n;
        state[row + 4U * i] = tmp[src];
    }
}

static void
lh_crypto_rijndael_shift_rows(lh_uchar_t *state, lh_usize_t nb, lh_bool_t inverse)
{
    lh_usize_t c1;
    lh_usize_t c2;
    lh_usize_t c3;

    lh_crypto_rijndael_shift_offsets(nb, lh_addr_of(c1), lh_addr_of(c2), lh_addr_of(c3));
    lh_crypto_rijndael_shift_row(state, nb, 1U, c1, inverse);
    lh_crypto_rijndael_shift_row(state, nb, 2U, c2, inverse);
    lh_crypto_rijndael_shift_row(state, nb, 3U, c3, inverse);
}

static void
lh_crypto_rijndael_sub_bytes(lh_uchar_t *state, lh_usize_t block_size, lh_bool_t inverse)
{
    const lh_uchar_t *box = inverse ? m_inv_sbox : m_sbox;
    lh_usize_t i;

    for (i = 0; i < block_size; ++i)
    {
        state[i] = box[state[i]];
    }
}

static void
lh_crypto_rijndael_mix_columns(lh_uchar_t *state, lh_usize_t nb, lh_bool_t inverse)
{
    lh_usize_t c;

    for (c = 0; c < nb; ++c)
    {
        lh_uchar_t *col = state + c * 4U;
        lh_uchar_t a0 = col[0];
        lh_uchar_t a1 = col[1];
        lh_uchar_t a2 = col[2];
        lh_uchar_t a3 = col[3];

        if (inverse)
        {
            col[0] = lh_cast_static(lh_uchar_t, (lh_crypto_rijndael_gf_mul(a0, 0x0EU) ^
                                  lh_crypto_rijndael_gf_mul(a1, 0x0BU) ^
                                  lh_crypto_rijndael_gf_mul(a2, 0x0DU) ^
                                  lh_crypto_rijndael_gf_mul(a3, 0x09U)));
            col[1] = lh_cast_static(lh_uchar_t, (lh_crypto_rijndael_gf_mul(a0, 0x09U) ^
                                  lh_crypto_rijndael_gf_mul(a1, 0x0EU) ^
                                  lh_crypto_rijndael_gf_mul(a2, 0x0BU) ^
                                  lh_crypto_rijndael_gf_mul(a3, 0x0DU)));
            col[2] = lh_cast_static(lh_uchar_t, (lh_crypto_rijndael_gf_mul(a0, 0x0DU) ^
                                  lh_crypto_rijndael_gf_mul(a1, 0x09U) ^
                                  lh_crypto_rijndael_gf_mul(a2, 0x0EU) ^
                                  lh_crypto_rijndael_gf_mul(a3, 0x0BU)));
            col[3] = lh_cast_static(lh_uchar_t, (lh_crypto_rijndael_gf_mul(a0, 0x0BU) ^
                                  lh_crypto_rijndael_gf_mul(a1, 0x0DU) ^
                                  lh_crypto_rijndael_gf_mul(a2, 0x09U) ^
                                  lh_crypto_rijndael_gf_mul(a3, 0x0EU)));
        }
        else
        {
            lh_uchar_t t = lh_cast_static(lh_uchar_t, (a0 ^ a1 ^ a2 ^ a3));
            col[0] = lh_cast_static(
                lh_uchar_t, a0 ^ t ^ lh_crypto_rijndael_xtime(lh_cast_static(lh_uchar_t, a0 ^ a1)));
            col[1] = lh_cast_static(
                lh_uchar_t, a1 ^ t ^ lh_crypto_rijndael_xtime(lh_cast_static(lh_uchar_t, a1 ^ a2)));
            col[2] = lh_cast_static(
                lh_uchar_t, a2 ^ t ^ lh_crypto_rijndael_xtime(lh_cast_static(lh_uchar_t, a2 ^ a3)));
            col[3] = lh_cast_static(
                lh_uchar_t, a3 ^ t ^ lh_crypto_rijndael_xtime(lh_cast_static(lh_uchar_t, a3 ^ a0)));
        }
    }
}

static void
lh_crypto_rijndael_add_round_key(lh_uchar_t *state, const lh_uchar_t *round_key,
                                 lh_usize_t block_size)
{
    lh_usize_t i;

    for (i = 0; i < block_size; ++i)
    {
        state[i] = lh_cast_static(lh_uchar_t, (state[i] ^ round_key[i]));
    }
}

static lh_u32_t
lh_crypto_rijndael_sub_word(lh_u32_t w)
{
    return (lh_cast_static(lh_u32_t, m_sbox[(w >> 24) & 0xFFU]) << 24) |
           (lh_cast_static(lh_u32_t, m_sbox[(w >> 16) & 0xFFU]) << 16) | (lh_cast_static(lh_u32_t, m_sbox[(w >> 8) & 0xFFU]) << 8) |
           lh_cast_static(lh_u32_t, m_sbox[w & 0xFFU]);
}

static lh_u32_t
lh_crypto_rijndael_rot_word(lh_u32_t w)
{
    return lh_bit_rotate_left_u32(w, 8U);
}

static void
lh_crypto_rijndael_expand_key(lh_crypto_rijndael_t *self, const lh_uchar_t *key,
                              lh_usize_t key_size)
{
    lh_usize_t nk = key_size / 4U;
    lh_usize_t nb = self->block_size / 4U;
    lh_usize_t nr = self->round_count;
    lh_usize_t total = nb * (nr + 1U);
    lh_u32_t w[120];
    lh_usize_t i;
    lh_u32_t rcon = 0x01000000U;

    for (i = 0; i < nk; ++i)
    {
        w[i] = lh_bit_unpack_be32(key + 4U * i);
    }

    for (i = nk; i < total; ++i)
    {
        lh_u32_t temp = w[i - 1U];

        if (i % nk == 0)
        {
            temp = lh_crypto_rijndael_sub_word(lh_crypto_rijndael_rot_word(temp)) ^ rcon;
            rcon = lh_cast_static(lh_u32_t, lh_crypto_rijndael_xtime(
                                               lh_cast_static(lh_uchar_t, rcon >> 24)))
                   << 24;
        }
        else if (nk > 6U && (i % nk) == 4U)
        {
            temp = lh_crypto_rijndael_sub_word(temp);
        }
        w[i] = w[i - nk] ^ temp;
    }

    for (i = 0; i < total; ++i)
    {
        lh_bit_pack_be32(w[i], self->expanded_key + 4U * i);
    }
}

static void
lh_crypto_rijndael_cipher(const lh_crypto_rijndael_t *self, const lh_uchar_t *in, lh_uchar_t *out,
                          lh_bool_t decrypt)
{
    lh_uchar_t state[LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MAX];
    lh_usize_t nb = self->block_size / 4U;
    lh_usize_t nr = self->round_count;
    lh_usize_t round;

    lh_memory_std_copy(state, in, self->block_size);

    if (!decrypt)
    {
        lh_crypto_rijndael_add_round_key(state, self->expanded_key, self->block_size);
        for (round = 1; round < nr; ++round)
        {
            lh_crypto_rijndael_sub_bytes(state, self->block_size, lh_bool_false);
            lh_crypto_rijndael_shift_rows(state, nb, lh_bool_false);
            lh_crypto_rijndael_mix_columns(state, nb, lh_bool_false);
            lh_crypto_rijndael_add_round_key(state, self->expanded_key + round * self->block_size,
                                             self->block_size);
        }
        lh_crypto_rijndael_sub_bytes(state, self->block_size, lh_bool_false);
        lh_crypto_rijndael_shift_rows(state, nb, lh_bool_false);
        lh_crypto_rijndael_add_round_key(state, self->expanded_key + nr * self->block_size,
                                         self->block_size);
    }
    else
    {
        lh_crypto_rijndael_add_round_key(state, self->expanded_key + nr * self->block_size,
                                         self->block_size);
        for (round = nr; round > 1U; --round)
        {
            lh_crypto_rijndael_shift_rows(state, nb, lh_bool_true);
            lh_crypto_rijndael_sub_bytes(state, self->block_size, lh_bool_true);
            lh_crypto_rijndael_add_round_key(
                state, self->expanded_key + (round - 1U) * self->block_size, self->block_size);
            lh_crypto_rijndael_mix_columns(state, nb, lh_bool_true);
        }
        lh_crypto_rijndael_shift_rows(state, nb, lh_bool_true);
        lh_crypto_rijndael_sub_bytes(state, self->block_size, lh_bool_true);
        lh_crypto_rijndael_add_round_key(state, self->expanded_key, self->block_size);
    }

    lh_memory_std_copy(out, state, self->block_size);
}

static void
lh_crypto_rijndael_xor(lh_uchar_t *dst, const lh_uchar_t *a, const lh_uchar_t *b, lh_usize_t n)
{
    lh_usize_t i;

    for (i = 0; i < n; ++i)
    {
        dst[i] = lh_cast_static(lh_uchar_t, (a[i] ^ b[i]));
    }
}

lh_bool_t
lh_crypto_rijndael_init(lh_crypto_rijndael_t *self, const lh_ptr key, lh_usize_t key_size,
                        const lh_ptr chain, lh_usize_t chain_size, lh_usize_t block_size,
                        lh_crypto_rijndael_mode_t mode)
{
    lh_usize_t nk;
    lh_usize_t nb;
    lh_usize_t nr;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(key);

    if (!lh_crypto_rijndael_size_ok(key_size) || !lh_crypto_rijndael_size_ok(block_size) ||
        (mode != lh_crypto_rijndael_mode_ecb && mode != lh_crypto_rijndael_mode_cbc &&
         mode != lh_crypto_rijndael_mode_cfb))
    {
        return lh_bool_false;
    }

    if (mode != lh_crypto_rijndael_mode_ecb)
    {
        if (lh_null_eq(chain) || chain_size != block_size)
        {
            return lh_bool_false;
        }
    }

    nk = key_size / 4U;
    nb = block_size / 4U;
    nr = (nk > nb ? nk : nb) + 6U;

    self->mode = mode;
    self->block_size = block_size;
    self->round_count = nr;
    lh_memory_std_set(self->expanded_key, 0, LH_CRYPTO_RIJNDAEL_EXPANDED_KEY_SIZE_MAX);
    lh_memory_std_set(self->chain, 0, LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MAX);
    lh_crypto_rijndael_expand_key(self, lh_ptr_rcast(const lh_uchar_t, key), key_size);

    if (mode != lh_crypto_rijndael_mode_ecb)
    {
        lh_memory_std_copy(self->chain, lh_ptr_rcast(const lh_uchar_t, chain), block_size);
    }

    return lh_bool_true;
}

lh_bool_t
lh_crypto_rijndael_encrypt_block(lh_crypto_rijndael_t *self, const lh_ptr in, lh_ptr out)
{
    const lh_uchar_t *in_bytes;
    lh_uchar_t *out_bytes;
    lh_uchar_t block[LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MAX];

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(in);
    lh_assert_runtime_ref(out);

    in_bytes = lh_ptr_rcast(const lh_uchar_t, in);
    out_bytes = lh_ptr_rcast(lh_uchar_t, out);

    if (self->mode == lh_crypto_rijndael_mode_ecb)
    {
        lh_crypto_rijndael_cipher(self, in_bytes, out_bytes, lh_bool_false);
        return lh_bool_true;
    }

    if (self->mode == lh_crypto_rijndael_mode_cbc)
    {
        lh_crypto_rijndael_xor(block, in_bytes, self->chain, self->block_size);
        lh_crypto_rijndael_cipher(self, block, out_bytes, lh_bool_false);
        lh_memory_std_copy(self->chain, out_bytes, self->block_size);
        return lh_bool_true;
    }

    /* CFB: encrypt chain, xor with plaintext, chain <- ciphertext */
    lh_crypto_rijndael_cipher(self, self->chain, block, lh_bool_false);
    lh_crypto_rijndael_xor(out_bytes, in_bytes, block, self->block_size);
    lh_memory_std_copy(self->chain, out_bytes, self->block_size);
    return lh_bool_true;
}

lh_bool_t
lh_crypto_rijndael_decrypt_block(lh_crypto_rijndael_t *self, const lh_ptr in, lh_ptr out)
{
    const lh_uchar_t *in_bytes;
    lh_uchar_t *out_bytes;
    lh_uchar_t block[LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MAX];
    lh_uchar_t saved[LH_CRYPTO_RIJNDAEL_BLOCK_SIZE_MAX];

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(in);
    lh_assert_runtime_ref(out);

    in_bytes = lh_ptr_rcast(const lh_uchar_t, in);
    out_bytes = lh_ptr_rcast(lh_uchar_t, out);

    if (self->mode == lh_crypto_rijndael_mode_ecb)
    {
        lh_crypto_rijndael_cipher(self, in_bytes, out_bytes, lh_bool_true);
        return lh_bool_true;
    }

    if (self->mode == lh_crypto_rijndael_mode_cbc)
    {
        lh_memory_std_copy(saved, in_bytes, self->block_size);
        lh_crypto_rijndael_cipher(self, in_bytes, block, lh_bool_true);
        lh_crypto_rijndael_xor(out_bytes, block, self->chain, self->block_size);
        lh_memory_std_copy(self->chain, saved, self->block_size);
        return lh_bool_true;
    }

    /* CFB decrypt: encrypt chain, xor with ciphertext, chain <- ciphertext */
    lh_memory_std_copy(saved, in_bytes, self->block_size);
    lh_crypto_rijndael_cipher(self, self->chain, block, lh_bool_false);
    lh_crypto_rijndael_xor(out_bytes, in_bytes, block, self->block_size);
    lh_memory_std_copy(self->chain, saved, self->block_size);
    return lh_bool_true;
}
