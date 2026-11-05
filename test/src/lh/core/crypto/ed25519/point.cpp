#include <gtest/gtest.h>

#include <lh/core/crypto/ed25519/point.h>
#include <lh/core/crypto/sha.h>

#include <array>

namespace
{

using bytes32 = std::array<lh_byte_t, 32>;
using bytes64 = std::array<lh_byte_t, 64>;

/* Encodings of [k]B from the reference code of RFC 8032 section 6 (Python). */
const bytes32 b1 = {0x58, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
                    0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
                    0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66};
const bytes32 b2 = {0xc9, 0xa3, 0xf8, 0x6a, 0xae, 0x46, 0x5f, 0x0e, 0x56, 0x51, 0x38,
                    0x64, 0x51, 0x0f, 0x39, 0x97, 0x56, 0x1f, 0xa2, 0xc9, 0xe8, 0x5e,
                    0xa2, 0x1d, 0xc2, 0x29, 0x23, 0x09, 0xf3, 0xcd, 0x60, 0x22};
const bytes32 b3 = {0xd4, 0xb4, 0xf5, 0x78, 0x48, 0x68, 0xc3, 0x02, 0x04, 0x03, 0x24,
                    0x67, 0x17, 0xec, 0x16, 0x9f, 0xf7, 0x9e, 0x26, 0x60, 0x8e, 0xa1,
                    0x26, 0xa1, 0xab, 0x69, 0xee, 0x77, 0xd1, 0xb1, 0x67, 0x12};
const bytes32 b7 = {0xb8, 0x62, 0x40, 0x9f, 0xb5, 0xc4, 0xc4, 0x12, 0x3d, 0xf2, 0xab,
                    0xf7, 0x46, 0x2b, 0x88, 0xf0, 0x41, 0xad, 0x36, 0xdd, 0x68, 0x64,
                    0xce, 0x87, 0x2f, 0xd5, 0x47, 0x2b, 0xe3, 0x63, 0xc5, 0xb1};
const bytes32 b_2_252_minus_1 = {0xee, 0x16, 0xe4, 0x09, 0x9c, 0xbf, 0x9b, 0x5d, 0x45, 0x6e, 0xce,
                                 0x25, 0x4d, 0xed, 0x2b, 0x24, 0x1d, 0x1f, 0x5d, 0xe8, 0x47, 0x6d,
                                 0x79, 0xd7, 0x33, 0xcd, 0xe6, 0x87, 0xef, 0x10, 0x25, 0xc9};
const bytes32 b_minus_1 = {0x58, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
                           0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
                           0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xe6};
const bytes32 b_all_ones = {0xdb, 0x27, 0xfe, 0x4b, 0x7a, 0x4b, 0xeb, 0x8c, 0x1b, 0x8c, 0x38,
                            0xa2, 0x1e, 0x94, 0x3a, 0x85, 0x23, 0x04, 0xc9, 0xbb, 0x30, 0x35,
                            0xa5, 0xf3, 0x66, 0x26, 0xb5, 0x11, 0x62, 0xa6, 0x8f, 0x9c};
const bytes32 neutral = {1};

const bytes32 l_bytes = {0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7,
                         0xa2, 0xde, 0xf9, 0xde, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10};

bytes32
scalar_of(lh_u32_t k)
{
    bytes32 s{};
    s[0] = static_cast<lh_byte_t>(k);
    s[1] = static_cast<lh_byte_t>(k >> 8);
    return s;
}

bytes32
scalarbase(const bytes32 &scalar)
{
    bytes32 out{};
    lh_crypto_ed25519_point_scalarbase(out.data(), scalar.data());
    return out;
}

bytes32
encode(const lh_crypto_ed25519_point_t &p)
{
    bytes32 out{};
    lh_crypto_ed25519_point_to_bytes(out.data(), &p);
    return out;
}

TEST(crypto_ed25519_point, scalarbase_small_multiples)
{
    EXPECT_EQ(scalarbase(scalar_of(1)), b1);
    EXPECT_EQ(scalarbase(scalar_of(2)), b2);
    EXPECT_EQ(scalarbase(scalar_of(3)), b3);
    EXPECT_EQ(scalarbase(scalar_of(7)), b7);
}

TEST(crypto_ed25519_point, scalarbase_large_scalars)
{
    bytes32 s;
    s.fill(0xffu);
    s[31] = 0x0fu; /* 2^252 - 1 */
    EXPECT_EQ(scalarbase(s), b_2_252_minus_1);
    bytes32 l_minus_1 = l_bytes;
    l_minus_1[0] -= 1u;
    EXPECT_EQ(scalarbase(l_minus_1), b_minus_1);
    EXPECT_EQ(scalarbase(l_bytes), neutral);
    EXPECT_EQ(scalarbase(scalar_of(0)), neutral);
    s.fill(0xffu); /* 2^256 - 1: any 256-bit scalar is fine */
    EXPECT_EQ(scalarbase(s), b_all_ones);
}

TEST(crypto_ed25519_point, zero_is_the_neutral_point)
{
    lh_crypto_ed25519_point_t p;
    lh_crypto_ed25519_point_zero(&p);
    EXPECT_EQ(encode(p), neutral);
}

TEST(crypto_ed25519_point, from_bytes_gives_the_negation)
{
    lh_crypto_ed25519_point_t minus_b;
    ASSERT_TRUE(lh_crypto_ed25519_point_from_bytes_neg_vartime(&minus_b, b1.data()));
    EXPECT_EQ(encode(minus_b), b_minus_1);
    lh_crypto_ed25519_point_t b;
    ASSERT_TRUE(lh_crypto_ed25519_point_from_bytes_neg_vartime(&b, b_minus_1.data()));
    EXPECT_EQ(encode(b), b1);
}

TEST(crypto_ed25519_point, from_bytes_refuses_a_y_off_the_curve)
{
    /* y = 2 has no x on the curve (the reference code finds none). */
    lh_crypto_ed25519_point_t p;
    EXPECT_FALSE(lh_crypto_ed25519_point_from_bytes_neg_vartime(&p, scalar_of(2).data()));
}

TEST(crypto_ed25519_point, double_add_and_sub_agree_with_multiples)
{
    lh_crypto_ed25519_point_t b;
    lh_crypto_ed25519_point_scalarmult_base(&b, scalar_of(1).data());
    lh_crypto_ed25519_point_t tmp;
    lh_crypto_ed25519_point_t two_b;
    lh_crypto_ed25519_point_double(&two_b, &b, &tmp);
    EXPECT_EQ(encode(two_b), b2);

    lh_crypto_ed25519_point_cached_t cached_b;
    lh_crypto_ed25519_point_cache(&cached_b, &b);
    lh_crypto_ed25519_point_t sum;
    lh_crypto_ed25519_point_add(&sum, &b, &cached_b);
    EXPECT_EQ(encode(sum), b2);
    lh_crypto_ed25519_point_add(&sum, &sum, &cached_b);
    EXPECT_EQ(encode(sum), b3);
    lh_crypto_ed25519_point_sub(&sum, &sum, &cached_b);
    EXPECT_EQ(encode(sum), b2);
}

/* RFC 8032 section 7.1, test 1: the empty message. */
const bytes32 rfc_public = {0xd7, 0x5a, 0x98, 0x01, 0x82, 0xb1, 0x0a, 0xb7, 0xd5, 0x4b, 0xfe,
                            0xd3, 0xc9, 0x64, 0x07, 0x3a, 0x0e, 0xe1, 0x72, 0xf3, 0xda, 0xa6,
                            0x23, 0x25, 0xaf, 0x02, 0x1a, 0x68, 0xf7, 0x07, 0x51, 0x1a};
const bytes64 rfc_signature = {
    0xe5, 0x56, 0x43, 0x00, 0xc3, 0x60, 0xac, 0x72, 0x90, 0x86, 0xe2, 0xcc, 0x80, 0x6e, 0x82, 0x8a,
    0x84, 0x87, 0x7f, 0x1e, 0xb8, 0xe5, 0xd9, 0x74, 0xd8, 0x73, 0xe0, 0x65, 0x22, 0x49, 0x01, 0x55,
    0x5f, 0xb8, 0x82, 0x15, 0x90, 0xa3, 0x3b, 0xac, 0xc6, 0x1e, 0x39, 0x70, 0x1c, 0xf9, 0xb4, 0x6b,
    0xd2, 0x5b, 0xf5, 0xf0, 0x59, 0x5b, 0xbe, 0x24, 0x65, 0x51, 0x41, 0x43, 0x8e, 0x7a, 0x10, 0x0b};

/* h = SHA-512(R || A || M) mod L, M empty. */
bytes32
rfc_h(const bytes64 &signature, const bytes32 &public_key)
{
    std::array<lh_byte_t, 64> input{};
    for (int i = 0; i < 32; ++i)
    {
        input[i] = signature[i];
        input[32 + i] = public_key[i];
    }
    bytes64 hash{};
    EXPECT_TRUE(lh_crypto_sha_hash(lh_crypto_sha_kind_512, input.data(), input.size(), hash.data(),
                                   hash.size()) > 0u);
    bytes32 h{};
    lh_crypto_ed25519_scalar_reduce(h.data(), hash.data());
    return h;
}

TEST(crypto_ed25519_point, check_equation_holds_for_the_rfc_8032_signature)
{
    const bytes32 h = rfc_h(rfc_signature, rfc_public);
    EXPECT_TRUE(
        lh_crypto_ed25519_point_check_equation(rfc_signature.data(), rfc_public.data(), h.data()));
}

TEST(crypto_ed25519_point, check_equation_fails_for_a_changed_s_r_or_h)
{
    const bytes32 h = rfc_h(rfc_signature, rfc_public);
    bytes64 bad_s = rfc_signature;
    bad_s[40] ^= 1u;
    EXPECT_FALSE(lh_crypto_ed25519_point_check_equation(bad_s.data(), rfc_public.data(), h.data()));
    bytes64 bad_r = rfc_signature;
    bad_r[3] ^= 1u;
    EXPECT_FALSE(lh_crypto_ed25519_point_check_equation(bad_r.data(), rfc_public.data(), h.data()));
    bytes32 bad_h = h;
    bad_h[0] ^= 1u;
    EXPECT_FALSE(lh_crypto_ed25519_point_check_equation(rfc_signature.data(), rfc_public.data(),
                                                        bad_h.data()));
}

TEST(crypto_ed25519_point, check_equation_refuses_an_s_at_or_above_l)
{
    const bytes32 h = rfc_h(rfc_signature, rfc_public);
    bytes64 malleable = rfc_signature;
    /* S + L is the same scalar, but must be refused. */
    lh_u64_t carry = 0;
    for (int i = 0; i < 32; ++i)
    {
        carry += static_cast<lh_u64_t>(malleable[32 + i]) + l_bytes[i];
        malleable[32 + i] = static_cast<lh_byte_t>(carry);
        carry >>= 8;
    }
    EXPECT_FALSE(
        lh_crypto_ed25519_point_check_equation(malleable.data(), rfc_public.data(), h.data()));
}

} // namespace
