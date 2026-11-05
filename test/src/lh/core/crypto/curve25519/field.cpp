#include <gtest/gtest.h>

#include <lh/core/crypto/curve25519/field.h>

#include <array>
#include <cstring>

namespace
{

using bytes = std::array<lh_byte_t, LH_CRYPTO_CURVE25519_FIELD_SIZE>;

/* A field element from a small non-negative number. */
lh_crypto_curve25519_field_t
small(lh_u32_t value)
{
    bytes s{};
    s[0] = static_cast<lh_byte_t>(value);
    s[1] = static_cast<lh_byte_t>(value >> 8);
    s[2] = static_cast<lh_byte_t>(value >> 16);
    s[3] = static_cast<lh_byte_t>(value >> 24);
    lh_crypto_curve25519_field_t f;
    lh_crypto_curve25519_field_from_bytes(&f, s.data());
    return f;
}

bytes
encode(const lh_crypto_curve25519_field_t &f)
{
    bytes s{};
    lh_crypto_curve25519_field_to_bytes(s.data(), &f);
    return s;
}

/* Deterministic "random" canonical encodings: xorshift bytes, top bit cleared
   (a value at or above 2^255 - 19 has probability 2^-250). */
bytes
pseudo_random(lh_u32_t seed)
{
    bytes s{};
    lh_u32_t x = seed * 2654435761u + 1u;
    for (auto &b : s)
    {
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        b = static_cast<lh_byte_t>(x);
    }
    s[31] &= 0x7fu;
    return s;
}

lh_crypto_curve25519_field_t
decode(const bytes &s)
{
    lh_crypto_curve25519_field_t f;
    lh_crypto_curve25519_field_from_bytes(&f, s.data());
    return f;
}

TEST(crypto_curve25519_field, canonical_encodings_round_trip)
{
    for (lh_u32_t seed = 0; seed < 64; ++seed)
    {
        const bytes s = pseudo_random(seed);
        EXPECT_EQ(encode(decode(s)), s) << seed;
    }
}

TEST(crypto_curve25519_field, the_prime_reduces_to_zero_and_the_top_bit_is_ignored)
{
    bytes p{};
    p.fill(0xffu);
    p[0] = 0xedu;
    p[31] = 0x7fu;
    EXPECT_EQ(encode(decode(p)), bytes{});
    bytes one_with_top_bit{};
    one_with_top_bit[0] = 1u;
    one_with_top_bit[31] = 0x80u;
    EXPECT_EQ(encode(decode(one_with_top_bit)), encode(small(1)));
}

TEST(crypto_curve25519_field, zero_and_one)
{
    lh_crypto_curve25519_field_t f;
    lh_crypto_curve25519_field_zero(&f);
    EXPECT_EQ(encode(f), bytes{});
    lh_crypto_curve25519_field_one(&f);
    EXPECT_EQ(encode(f), encode(small(1)));
}

TEST(crypto_curve25519_field, add_sub_neg)
{
    const lh_crypto_curve25519_field_t a = decode(pseudo_random(1));
    const lh_crypto_curve25519_field_t b = decode(pseudo_random(2));
    lh_crypto_curve25519_field_t sum;
    lh_crypto_curve25519_field_add(&sum, &a, &b);
    lh_crypto_curve25519_field_t back;
    lh_crypto_curve25519_field_sub(&back, &sum, &b);
    EXPECT_EQ(encode(back), encode(a));
    lh_crypto_curve25519_field_t minus;
    lh_crypto_curve25519_field_neg(&minus, &a);
    lh_crypto_curve25519_field_add(&sum, &a, &minus);
    EXPECT_EQ(encode(sum), bytes{});
}

TEST(crypto_curve25519_field, small_products)
{
    const lh_crypto_curve25519_field_t two = small(2);
    const lh_crypto_curve25519_field_t three = small(3);
    lh_crypto_curve25519_field_t h;
    lh_crypto_curve25519_field_mul(&h, &two, &three);
    EXPECT_EQ(encode(h), encode(small(6)));
    lh_crypto_curve25519_field_mul_small(&h, &three, 121666);
    EXPECT_EQ(encode(h), encode(small(3u * 121666u)));
    lh_crypto_curve25519_field_square(&h, &three);
    EXPECT_EQ(encode(h), encode(small(9)));
}

TEST(crypto_curve25519_field, square_is_mul_by_itself_and_mul_commutes)
{
    for (lh_u32_t seed = 10; seed < 30; ++seed)
    {
        const lh_crypto_curve25519_field_t a = decode(pseudo_random(seed));
        const lh_crypto_curve25519_field_t b = decode(pseudo_random(seed + 100));
        lh_crypto_curve25519_field_t sq;
        lh_crypto_curve25519_field_t aa;
        lh_crypto_curve25519_field_square(&sq, &a);
        lh_crypto_curve25519_field_mul(&aa, &a, &a);
        EXPECT_EQ(encode(sq), encode(aa));
        lh_crypto_curve25519_field_t ab;
        lh_crypto_curve25519_field_t ba;
        lh_crypto_curve25519_field_mul(&ab, &a, &b);
        lh_crypto_curve25519_field_mul(&ba, &b, &a);
        EXPECT_EQ(encode(ab), encode(ba));
    }
}

TEST(crypto_curve25519_field, an_element_times_its_inverse_is_one)
{
    for (lh_u32_t seed = 40; seed < 60; ++seed)
    {
        const lh_crypto_curve25519_field_t a = decode(pseudo_random(seed));
        lh_crypto_curve25519_field_t inv;
        lh_crypto_curve25519_field_invert(&inv, &a);
        lh_crypto_curve25519_field_t one;
        lh_crypto_curve25519_field_mul(&one, &a, &inv);
        EXPECT_EQ(encode(one), encode(small(1))) << seed;
    }
    lh_crypto_curve25519_field_t zero;
    lh_crypto_curve25519_field_zero(&zero);
    lh_crypto_curve25519_field_t inv;
    lh_crypto_curve25519_field_invert(&inv, &zero);
    EXPECT_EQ(encode(inv), bytes{});
}

TEST(crypto_curve25519_field, sqrt_m1_squares_to_minus_one)
{
    lh_crypto_curve25519_field_t sq;
    lh_crypto_curve25519_field_square(&sq, lh_crypto_curve25519_field_get_sqrt_m1());
    lh_crypto_curve25519_field_t minus_one = small(1);
    lh_crypto_curve25519_field_neg(&minus_one, &minus_one);
    EXPECT_TRUE(lh_crypto_curve25519_field_is_equal(&sq, &minus_one));
}

TEST(crypto_curve25519_field, invsqrt_of_a_square)
{
    for (lh_u32_t seed = 70; seed < 80; ++seed)
    {
        const lh_crypto_curve25519_field_t a = decode(pseudo_random(seed));
        lh_crypto_curve25519_field_t x;
        lh_crypto_curve25519_field_square(&x, &a);
        lh_crypto_curve25519_field_t isr;
        EXPECT_TRUE(lh_crypto_curve25519_field_invsqrt(&isr, &x));
        /* isr^2 * x = 1 */
        lh_crypto_curve25519_field_t check;
        lh_crypto_curve25519_field_square(&check, &isr);
        lh_crypto_curve25519_field_mul(&check, &check, &x);
        EXPECT_EQ(encode(check), encode(small(1))) << seed;
    }
}

TEST(crypto_curve25519_field, invsqrt_of_a_non_square_and_of_zero)
{
    /* 2 is not a square modulo 2^255 - 19 (the prime is 5 modulo 8). */
    const lh_crypto_curve25519_field_t two = small(2);
    lh_crypto_curve25519_field_t isr;
    EXPECT_FALSE(lh_crypto_curve25519_field_invsqrt(&isr, &two));
    /* isr^2 * 2 = sqrt(-1) */
    lh_crypto_curve25519_field_t check;
    lh_crypto_curve25519_field_square(&check, &isr);
    lh_crypto_curve25519_field_mul(&check, &check, &two);
    EXPECT_TRUE(
        lh_crypto_curve25519_field_is_equal(&check, lh_crypto_curve25519_field_get_sqrt_m1()));

    lh_crypto_curve25519_field_t zero;
    lh_crypto_curve25519_field_zero(&zero);
    EXPECT_TRUE(lh_crypto_curve25519_field_invsqrt(&isr, &zero));
    EXPECT_EQ(encode(isr), bytes{});
}

TEST(crypto_curve25519_field, parity_is_of_the_canonical_value)
{
    EXPECT_TRUE(lh_crypto_curve25519_field_is_odd(
        &static_cast<const lh_crypto_curve25519_field_t &>(small(1))));
    EXPECT_FALSE(lh_crypto_curve25519_field_is_odd(
        &static_cast<const lh_crypto_curve25519_field_t &>(small(2))));
    /* -1 is p - 1: even. */
    lh_crypto_curve25519_field_t minus_one = small(1);
    lh_crypto_curve25519_field_neg(&minus_one, &minus_one);
    EXPECT_FALSE(lh_crypto_curve25519_field_is_odd(&minus_one));
}

TEST(crypto_curve25519_field, conditional_swap_and_copy)
{
    lh_crypto_curve25519_field_t f = small(5);
    lh_crypto_curve25519_field_t g = small(7);
    lh_crypto_curve25519_field_cswap(&f, &g, 0);
    EXPECT_EQ(encode(f), encode(small(5)));
    lh_crypto_curve25519_field_cswap(&f, &g, 1);
    EXPECT_EQ(encode(f), encode(small(7)));
    EXPECT_EQ(encode(g), encode(small(5)));
    lh_crypto_curve25519_field_ccopy(&f, &g, 0);
    EXPECT_EQ(encode(f), encode(small(7)));
    lh_crypto_curve25519_field_ccopy(&f, &g, 1);
    EXPECT_EQ(encode(f), encode(small(5)));
    lh_crypto_curve25519_field_t h;
    lh_crypto_curve25519_field_assign(&h, &f);
    EXPECT_TRUE(lh_crypto_curve25519_field_is_equal(&h, &f));
    EXPECT_FALSE(lh_crypto_curve25519_field_is_equal(
        &h, &static_cast<const lh_crypto_curve25519_field_t &>(small(6))));
}

} // namespace
