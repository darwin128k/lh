#include <lh/crypto/sha.h>
#include <lh/assert.h>
#include <lh/memory/std.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/util/bit/endian.h>
#include <lh/util/bit/rotate.h>
#include <lh/util/ptr.h>

#define LH_CRYPTO_SHA_BLOCK_SIZE_32 64U
#define LH_CRYPTO_SHA_BLOCK_SIZE_64 128U

static const lh_u32_t m_k256[64] = {
    0x428A2F98U, 0x71374491U, 0xB5C0FBCFU, 0xE9B5DBA5U, 0x3956C25BU, 0x59F111F1U, 0x923F82A4U,
    0xAB1C5ED5U, 0xD807AA98U, 0x12835B01U, 0x243185BEU, 0x550C7DC3U, 0x72BE5D74U, 0x80DEB1FEU,
    0x9BDC06A7U, 0xC19BF174U, 0xE49B69C1U, 0xEFBE4786U, 0x0FC19DC6U, 0x240CA1CCU, 0x2DE92C6FU,
    0x4A7484AAU, 0x5CB0A9DCU, 0x76F988DAU, 0x983E5152U, 0xA831C66DU, 0xB00327C8U, 0xBF597FC7U,
    0xC6E00BF3U, 0xD5A79147U, 0x06CA6351U, 0x14292967U, 0x27B70A85U, 0x2E1B2138U, 0x4D2C6DFCU,
    0x53380D13U, 0x650A7354U, 0x766A0ABBU, 0x81C2C92EU, 0x92722C85U, 0xA2BFE8A1U, 0xA81A664BU,
    0xC24B8B70U, 0xC76C51A3U, 0xD192E819U, 0xD6990624U, 0xF40E3585U, 0x106AA070U, 0x19A4C116U,
    0x1E376C08U, 0x2748774CU, 0x34B0BCB5U, 0x391C0CB3U, 0x4ED8AA4AU, 0x5B9CCA4FU, 0x682E6FF3U,
    0x748F82EEU, 0x78A5636FU, 0x84C87814U, 0x8CC70208U, 0x90BEFFFAU, 0xA4506CEBU, 0xBEF9A3F7U,
    0xC67178F2U};

static const lh_u64_t m_k512[80] = {
    0x428A2F98D728AE22ULL, 0x7137449123EF65CDULL, 0xB5C0FBCFEC4D3B2FULL, 0xE9B5DBA58189DBBCULL,
    0x3956C25BF348B538ULL, 0x59F111F1B605D019ULL, 0x923F82A4AF194F9BULL, 0xAB1C5ED5DA6D8118ULL,
    0xD807AA98A3030242ULL, 0x12835B0145706FBEULL, 0x243185BE4EE4B28CULL, 0x550C7DC3D5FFB4E2ULL,
    0x72BE5D74F27B896FULL, 0x80DEB1FE3B1696B1ULL, 0x9BDC06A725C71235ULL, 0xC19BF174CF692694ULL,
    0xE49B69C19EF14AD2ULL, 0xEFBE4786384F25E3ULL, 0x0FC19DC68B8CD5B5ULL, 0x240CA1CC77AC9C65ULL,
    0x2DE92C6F592B0275ULL, 0x4A7484AA6EA6E483ULL, 0x5CB0A9DCBD41FBD4ULL, 0x76F988DA831153B5ULL,
    0x983E5152EE66DFABULL, 0xA831C66D2DB43210ULL, 0xB00327C898FB213FULL, 0xBF597FC7BEEF0EE4ULL,
    0xC6E00BF33DA88FC2ULL, 0xD5A79147930AA725ULL, 0x06CA6351E003826FULL, 0x142929670A0E6E70ULL,
    0x27B70A8546D22FFCULL, 0x2E1B21385C26C926ULL, 0x4D2C6DFC5AC42AEDULL, 0x53380D139D95B3DFULL,
    0x650A73548BAF63DEULL, 0x766A0ABB3C77B2A8ULL, 0x81C2C92E47EDAEE6ULL, 0x92722C851482353BULL,
    0xA2BFE8A14CF10364ULL, 0xA81A664BBC423001ULL, 0xC24B8B70D0F89791ULL, 0xC76C51A30654BE30ULL,
    0xD192E819D6EF5218ULL, 0xD69906245565A910ULL, 0xF40E35855771202AULL, 0x106AA07032BBD1B8ULL,
    0x19A4C116B8D2D0C8ULL, 0x1E376C085141AB53ULL, 0x2748774CDF8EEB99ULL, 0x34B0BCB5E19B48A8ULL,
    0x391C0CB3C5C95A63ULL, 0x4ED8AA4AE3418ACBULL, 0x5B9CCA4F7763E373ULL, 0x682E6FF3D6B2B8A3ULL,
    0x748F82EE5DEFB2FCULL, 0x78A5636F43172F60ULL, 0x84C87814A1F0AB72ULL, 0x8CC702081A6439ECULL,
    0x90BEFFFA23631E28ULL, 0xA4506CEBDE82BDE9ULL, 0xBEF9A3F7B2C67915ULL, 0xC67178F2E372532BULL,
    0xCA273ECEEA26619CULL, 0xD186B8C721C0C207ULL, 0xEADA7DD6CDE0EB1EULL, 0xF57D4F7FEE6ED178ULL,
    0x06F067AA72176FBAULL, 0x0A637DC5A2C898A6ULL, 0x113F9804BEF90DAEULL, 0x1B710B35131C471BULL,
    0x28DB77F523047D84ULL, 0x32CAAB7B40C72493ULL, 0x3C9EBE0A15C9BEBCULL, 0x431D67C49C100D4CULL,
    0x4CC5D4BECB3E42B6ULL, 0x597F299CFC657E2AULL, 0x5FCB6FAB3AD6FAECULL, 0x6C44198C4A475817ULL};

static lh_bool_t
lh_crypto_sha_kind_is_wide(lh_crypto_sha_kind_t kind)
{
    return (kind == lh_crypto_sha_kind_384 || kind == lh_crypto_sha_kind_512) ? lh_bool_true
                                                                              : lh_bool_false;
}

lh_usize_t
lh_crypto_sha_digest_size(lh_crypto_sha_kind_t kind)
{
    switch (kind)
    {
    case lh_crypto_sha_kind_1:
        return LH_CRYPTO_SHA1_DIGEST_SIZE;
    case lh_crypto_sha_kind_224:
        return LH_CRYPTO_SHA224_DIGEST_SIZE;
    case lh_crypto_sha_kind_256:
        return LH_CRYPTO_SHA256_DIGEST_SIZE;
    case lh_crypto_sha_kind_384:
        return LH_CRYPTO_SHA384_DIGEST_SIZE;
    case lh_crypto_sha_kind_512:
        return LH_CRYPTO_SHA512_DIGEST_SIZE;
    default:
        return 0;
    }
}

lh_usize_t
lh_crypto_sha_block_size(lh_crypto_sha_kind_t kind)
{
    if (lh_crypto_sha_digest_size(kind) == 0)
    {
        return 0;
    }
    return lh_crypto_sha_kind_is_wide(kind) ? LH_CRYPTO_SHA_BLOCK_SIZE_64
                                            : LH_CRYPTO_SHA_BLOCK_SIZE_32;
}

static void
lh_crypto_sha_process_1(lh_crypto_sha_t *self, const lh_uchar_t *block)
{
    lh_u32_t w[80];
    lh_u32_t a;
    lh_u32_t b;
    lh_u32_t c;
    lh_u32_t d;
    lh_u32_t e;
    lh_usize_t i;

    for (i = 0; i < 16U; ++i)
    {
        w[i] = lh_bit_unpack_be32(block + i * 4U);
    }
    for (i = 16; i < 80U; ++i)
    {
        w[i] = lh_bit_rotate_left_u32(w[i - 3U] ^ w[i - 8U] ^ w[i - 14U] ^ w[i - 16U], 1U);
    }

    a = (lh_u32_t)self->state[0];
    b = (lh_u32_t)self->state[1];
    c = (lh_u32_t)self->state[2];
    d = (lh_u32_t)self->state[3];
    e = (lh_u32_t)self->state[4];

    for (i = 0; i < 80U; ++i)
    {
        lh_u32_t f;
        lh_u32_t k;
        lh_u32_t temp;

        if (i < 20U)
        {
            f = (b & c) | ((~b) & d);
            k = 0x5A827999U;
        }
        else if (i < 40U)
        {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1U;
        }
        else if (i < 60U)
        {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDCU;
        }
        else
        {
            f = b ^ c ^ d;
            k = 0xCA62C1D6U;
        }

        temp = lh_bit_rotate_left_u32(a, 5U) + f + e + k + w[i];
        e = d;
        d = c;
        c = lh_bit_rotate_left_u32(b, 30U);
        b = a;
        a = temp;
    }

    self->state[0] = (lh_u32_t)self->state[0] + a;
    self->state[1] = (lh_u32_t)self->state[1] + b;
    self->state[2] = (lh_u32_t)self->state[2] + c;
    self->state[3] = (lh_u32_t)self->state[3] + d;
    self->state[4] = (lh_u32_t)self->state[4] + e;
}

static void
lh_crypto_sha_process_256(lh_crypto_sha_t *self, const lh_uchar_t *block)
{
    lh_u32_t w[64];
    lh_u32_t a;
    lh_u32_t b;
    lh_u32_t c;
    lh_u32_t d;
    lh_u32_t e;
    lh_u32_t f;
    lh_u32_t g;
    lh_u32_t h;
    lh_usize_t i;

    for (i = 0; i < 16U; ++i)
    {
        w[i] = lh_bit_unpack_be32(block + i * 4U);
    }
    for (i = 16; i < 64U; ++i)
    {
        lh_u32_t s0 = lh_bit_rotate_right_u32(w[i - 15U], 7U) ^ lh_bit_rotate_right_u32(w[i - 15U], 18U)
                      ^ (w[i - 15U] >> 3);
        lh_u32_t s1 = lh_bit_rotate_right_u32(w[i - 2U], 17U) ^ lh_bit_rotate_right_u32(w[i - 2U], 19U)
                      ^ (w[i - 2U] >> 10);
        w[i] = w[i - 16U] + s0 + w[i - 7U] + s1;
    }

    a = (lh_u32_t)self->state[0];
    b = (lh_u32_t)self->state[1];
    c = (lh_u32_t)self->state[2];
    d = (lh_u32_t)self->state[3];
    e = (lh_u32_t)self->state[4];
    f = (lh_u32_t)self->state[5];
    g = (lh_u32_t)self->state[6];
    h = (lh_u32_t)self->state[7];

    for (i = 0; i < 64U; ++i)
    {
        lh_u32_t s1 = lh_bit_rotate_right_u32(e, 6U) ^ lh_bit_rotate_right_u32(e, 11U)
                      ^ lh_bit_rotate_right_u32(e, 25U);
        lh_u32_t ch = (e & f) ^ ((~e) & g);
        lh_u32_t temp1 = h + s1 + ch + m_k256[i] + w[i];
        lh_u32_t s0 = lh_bit_rotate_right_u32(a, 2U) ^ lh_bit_rotate_right_u32(a, 13U)
                      ^ lh_bit_rotate_right_u32(a, 22U);
        lh_u32_t maj = (a & b) ^ (a & c) ^ (b & c);
        lh_u32_t temp2 = s0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    self->state[0] = (lh_u32_t)self->state[0] + a;
    self->state[1] = (lh_u32_t)self->state[1] + b;
    self->state[2] = (lh_u32_t)self->state[2] + c;
    self->state[3] = (lh_u32_t)self->state[3] + d;
    self->state[4] = (lh_u32_t)self->state[4] + e;
    self->state[5] = (lh_u32_t)self->state[5] + f;
    self->state[6] = (lh_u32_t)self->state[6] + g;
    self->state[7] = (lh_u32_t)self->state[7] + h;
}

static void
lh_crypto_sha_process_512(lh_crypto_sha_t *self, const lh_uchar_t *block)
{
    lh_u64_t w[80];
    lh_u64_t a;
    lh_u64_t b;
    lh_u64_t c;
    lh_u64_t d;
    lh_u64_t e;
    lh_u64_t f;
    lh_u64_t g;
    lh_u64_t h;
    lh_usize_t i;

    for (i = 0; i < 16U; ++i)
    {
        w[i] = lh_bit_unpack_be64(block + i * 8U);
    }
    for (i = 16; i < 80U; ++i)
    {
        lh_u64_t s0 = lh_bit_rotate_right_u64(w[i - 15U], 1U) ^ lh_bit_rotate_right_u64(w[i - 15U], 8U)
                      ^ (w[i - 15U] >> 7);
        lh_u64_t s1 = lh_bit_rotate_right_u64(w[i - 2U], 19U) ^ lh_bit_rotate_right_u64(w[i - 2U], 61U)
                      ^ (w[i - 2U] >> 6);
        w[i] = w[i - 16U] + s0 + w[i - 7U] + s1;
    }

    a = self->state[0];
    b = self->state[1];
    c = self->state[2];
    d = self->state[3];
    e = self->state[4];
    f = self->state[5];
    g = self->state[6];
    h = self->state[7];

    for (i = 0; i < 80U; ++i)
    {
        lh_u64_t s1 = lh_bit_rotate_right_u64(e, 14U) ^ lh_bit_rotate_right_u64(e, 18U)
                      ^ lh_bit_rotate_right_u64(e, 41U);
        lh_u64_t ch = (e & f) ^ ((~e) & g);
        lh_u64_t temp1 = h + s1 + ch + m_k512[i] + w[i];
        lh_u64_t s0 = lh_bit_rotate_right_u64(a, 28U) ^ lh_bit_rotate_right_u64(a, 34U)
                      ^ lh_bit_rotate_right_u64(a, 39U);
        lh_u64_t maj = (a & b) ^ (a & c) ^ (b & c);
        lh_u64_t temp2 = s0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    self->state[0] += a;
    self->state[1] += b;
    self->state[2] += c;
    self->state[3] += d;
    self->state[4] += e;
    self->state[5] += f;
    self->state[6] += g;
    self->state[7] += h;
}

static void
lh_crypto_sha_process(lh_crypto_sha_t *self, const lh_uchar_t *block)
{
    if (self->kind == lh_crypto_sha_kind_1)
    {
        lh_crypto_sha_process_1(self, block);
    }
    else if (lh_crypto_sha_kind_is_wide(self->kind))
    {
        lh_crypto_sha_process_512(self, block);
    }
    else
    {
        lh_crypto_sha_process_256(self, block);
    }
}

static void
lh_crypto_sha_add_bits(lh_crypto_sha_t *self, lh_usize_t byte_count)
{
    lh_u64_t bits = (lh_u64_t)byte_count << 3;
    lh_u64_t lo = self->bit_count_lo + bits;

    if (lo < self->bit_count_lo)
    {
        self->bit_count_hi += 1U;
    }
    self->bit_count_lo = lo;
    self->bit_count_hi += (lh_u64_t)byte_count >> 61;
}

void
lh_crypto_sha_init(lh_crypto_sha_t *self, lh_crypto_sha_kind_t kind)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(lh_crypto_sha_digest_size(kind) == 0,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    self->kind = kind;
    self->finished = lh_bool_false;
    self->bit_count_lo = 0;
    self->bit_count_hi = 0;
    self->block_fill = 0;
    lh_memory_std_set(self->block, 0, LH_CRYPTO_SHA_BLOCK_SIZE_MAX);

    switch (kind)
    {
    case lh_crypto_sha_kind_1:
        self->state[0] = 0x67452301U;
        self->state[1] = 0xEFCDAB89U;
        self->state[2] = 0x98BADCFEU;
        self->state[3] = 0x10325476U;
        self->state[4] = 0xC3D2E1F0U;
        self->state[5] = 0;
        self->state[6] = 0;
        self->state[7] = 0;
        break;
    case lh_crypto_sha_kind_224:
        self->state[0] = 0xC1059ED8U;
        self->state[1] = 0x367CD507U;
        self->state[2] = 0x3070DD17U;
        self->state[3] = 0xF70E5939U;
        self->state[4] = 0xFFC00B31U;
        self->state[5] = 0x68581511U;
        self->state[6] = 0x64F98FA7U;
        self->state[7] = 0xBEFA4FA4U;
        break;
    case lh_crypto_sha_kind_256:
        self->state[0] = 0x6A09E667U;
        self->state[1] = 0xBB67AE85U;
        self->state[2] = 0x3C6EF372U;
        self->state[3] = 0xA54FF53AU;
        self->state[4] = 0x510E527FU;
        self->state[5] = 0x9B05688CU;
        self->state[6] = 0x1F83D9ABU;
        self->state[7] = 0x5BE0CD19U;
        break;
    case lh_crypto_sha_kind_384:
        self->state[0] = 0xCBBB9D5DC1059ED8ULL;
        self->state[1] = 0x629A292A367CD507ULL;
        self->state[2] = 0x9159015A3070DD17ULL;
        self->state[3] = 0x152FECD8F70E5939ULL;
        self->state[4] = 0x67332667FFC00B31ULL;
        self->state[5] = 0x8EB44A8768581511ULL;
        self->state[6] = 0xDB0C2E0D64F98FA7ULL;
        self->state[7] = 0x47B5481DBEFA4FA4ULL;
        break;
    case lh_crypto_sha_kind_512:
        self->state[0] = 0x6A09E667F3BCC908ULL;
        self->state[1] = 0xBB67AE8584CAA73BULL;
        self->state[2] = 0x3C6EF372FE94F82BULL;
        self->state[3] = 0xA54FF53A5F1D36F1ULL;
        self->state[4] = 0x510E527FADE682D1ULL;
        self->state[5] = 0x9B05688C2B3E6C1FULL;
        self->state[6] = 0x1F83D9ABFB41BD6BULL;
        self->state[7] = 0x5BE0CD19137E2179ULL;
        break;
    default:
        break;
    }
}

void
lh_crypto_sha_update(lh_crypto_sha_t *self, const lh_ptr data, lh_usize_t size)
{
    const lh_uchar_t *bytes;
    lh_usize_t block_size;
    lh_usize_t remaining;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(self->finished,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    if (size == 0)
    {
        return;
    }
    lh_assert_runtime_ref(data);

    bytes = lh_ptr_rcast(const lh_uchar_t, data);
    block_size = lh_crypto_sha_block_size(self->kind);
    remaining = size;

    lh_crypto_sha_add_bits(self, size);

    if (self->block_fill != 0)
    {
        lh_usize_t room = block_size - self->block_fill;
        lh_usize_t take = remaining < room ? remaining : room;

        lh_memory_std_copy(self->block + self->block_fill, bytes, take);
        self->block_fill += take;
        bytes += take;
        remaining -= take;
        if (self->block_fill == block_size)
        {
            lh_crypto_sha_process(self, self->block);
            self->block_fill = 0;
        }
    }

    while (remaining >= block_size)
    {
        lh_crypto_sha_process(self, bytes);
        bytes += block_size;
        remaining -= block_size;
    }

    if (remaining != 0)
    {
        lh_memory_std_copy(self->block, bytes, remaining);
        self->block_fill = remaining;
    }
}

lh_usize_t
lh_crypto_sha_finish(lh_crypto_sha_t *self, lh_ptr out, lh_usize_t out_size)
{
    lh_usize_t digest_size;
    lh_usize_t block_size;
    lh_usize_t length_size;
    lh_usize_t i;
    lh_uchar_t *out_bytes;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    lh_assert_runtime_if(self->finished,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    digest_size = lh_crypto_sha_digest_size(self->kind);
    if (digest_size > out_size)
    {
        return 0;
    }

    block_size = lh_crypto_sha_block_size(self->kind);
    length_size = lh_crypto_sha_kind_is_wide(self->kind) ? 16U : 8U;

    self->block[self->block_fill++] = 0x80U;
    if (self->block_fill > block_size - length_size)
    {
        lh_memory_std_set(self->block + self->block_fill, 0, block_size - self->block_fill);
        lh_crypto_sha_process(self, self->block);
        self->block_fill = 0;
    }
    lh_memory_std_set(self->block + self->block_fill, 0, block_size - length_size - self->block_fill);

    if (lh_crypto_sha_kind_is_wide(self->kind))
    {
        lh_bit_pack_be64(self->bit_count_hi, self->block + block_size - 16U);
        lh_bit_pack_be64(self->bit_count_lo, self->block + block_size - 8U);
    }
    else
    {
        lh_bit_pack_be64(self->bit_count_lo, self->block + block_size - 8U);
    }
    lh_crypto_sha_process(self, self->block);
    self->finished = lh_bool_true;

    out_bytes = lh_ptr_rcast(lh_uchar_t, out);
    if (lh_crypto_sha_kind_is_wide(self->kind))
    {
        lh_usize_t words = digest_size / 8U;
        for (i = 0; i < words; ++i)
        {
            lh_bit_pack_be64(self->state[i], out_bytes + i * 8U);
        }
    }
    else if (self->kind == lh_crypto_sha_kind_1)
    {
        for (i = 0; i < 5U; ++i)
        {
            lh_bit_pack_be32((lh_u32_t)self->state[i], out_bytes + i * 4U);
        }
    }
    else
    {
        lh_usize_t words = digest_size / 4U;
        for (i = 0; i < words; ++i)
        {
            lh_bit_pack_be32((lh_u32_t)self->state[i], out_bytes + i * 4U);
        }
    }

    return digest_size;
}

lh_usize_t
lh_crypto_sha_hash(lh_crypto_sha_kind_t kind, const lh_ptr data, lh_usize_t size, lh_ptr out,
                   lh_usize_t out_size)
{
    lh_crypto_sha_t hasher;

    if (lh_crypto_sha_digest_size(kind) == 0)
    {
        return 0;
    }
    lh_crypto_sha_init(lh_addr_of(hasher), kind);
    lh_crypto_sha_update(lh_addr_of(hasher), data, size);
    return lh_crypto_sha_finish(lh_addr_of(hasher), out, out_size);
}
