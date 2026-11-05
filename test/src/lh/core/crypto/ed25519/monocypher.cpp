#include <gtest/gtest.h>

#include <lh/core/crypto/ed25519.h>

/* The vendored copy LH's Ed25519 was ported from: while it is still in the tree,
   every result must match it bit for bit. Removed together with lib/monocypher. */
#include <monocypher-ed25519.h>

#include <array>
#include <cstring>
#include <vector>

namespace
{

struct rng
{
    lh_u32_t x;

    lh_byte_t
    next()
    {
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return static_cast<lh_byte_t>(x >> 3);
    }

    template <typename T>
    void
    fill(T &buffer)
    {
        for (auto &b : buffer)
        {
            b = next();
        }
    }
};

TEST(crypto_ed25519_monocypher, keys_and_signatures_match_bit_for_bit)
{
    rng r{0x9e3779b9u};
    for (int round = 0; round < 64; ++round)
    {
        std::array<lh_byte_t, 32> seed{};
        r.fill(seed);
        std::vector<lh_byte_t> message(static_cast<std::size_t>(round * 7 % 300));
        r.fill(message);

        std::array<lh_byte_t, 32> seed_lh = seed;
        std::array<lh_byte_t, 64> sk_lh{};
        std::array<lh_byte_t, 32> pk_lh{};
        lh_crypto_ed25519_key_pair(sk_lh.data(), pk_lh.data(), seed_lh.data());

        std::array<lh_byte_t, 32> seed_mono = seed;
        std::array<lh_byte_t, 64> sk_mono{};
        std::array<lh_byte_t, 32> pk_mono{};
        crypto_ed25519_key_pair(sk_mono.data(), pk_mono.data(), seed_mono.data());

        ASSERT_EQ(sk_lh, sk_mono) << round;
        ASSERT_EQ(pk_lh, pk_mono) << round;
        EXPECT_EQ(seed_lh, seed_mono) << round; /* both wipe the seed */

        std::array<lh_byte_t, 64> sig_lh{};
        std::array<lh_byte_t, 64> sig_mono{};
        lh_crypto_ed25519_sign(sig_lh.data(), sk_lh.data(), message.data(), message.size());
        crypto_ed25519_sign(sig_mono.data(), sk_mono.data(), message.data(), message.size());
        ASSERT_EQ(sig_lh, sig_mono) << round;

        EXPECT_TRUE(
            lh_crypto_ed25519_check(sig_mono.data(), pk_lh.data(), message.data(), message.size()))
            << round;
        EXPECT_EQ(
            crypto_ed25519_check(sig_lh.data(), pk_mono.data(), message.data(), message.size()), 0)
            << round;
    }
}

TEST(crypto_ed25519_monocypher, checks_agree_on_tampered_and_random_signatures)
{
    rng r{12345u};
    int agreed = 0;
    for (int round = 0; round < 200; ++round)
    {
        std::array<lh_byte_t, 32> seed{};
        r.fill(seed);
        std::array<lh_byte_t, 64> sk{};
        std::array<lh_byte_t, 32> pk{};
        lh_crypto_ed25519_key_pair(sk.data(), pk.data(), seed.data());
        std::array<lh_byte_t, 40> message{};
        r.fill(message);
        std::array<lh_byte_t, 64> sig{};
        lh_crypto_ed25519_sign(sig.data(), sk.data(), message.data(), message.size());

        std::array<lh_byte_t, 64> bad = sig;
        switch (round % 4)
        {
        case 0: /* one flipped bit of R or S */
            bad[r.next() % 64] ^= static_cast<lh_byte_t>(1u << (r.next() % 8));
            break;
        case 1: /* a random signature */
            r.fill(bad);
            break;
        case 2: /* a flipped message bit */
            message[r.next() % message.size()] ^= 1u;
            break;
        default: /* a random public key */
            r.fill(pk);
            break;
        }
        const bool lh_ok = lh_crypto_ed25519_check(bad.data(), pk.data(), message.data(),
                                                   message.size()) == lh_bool_true;
        const bool mono_ok =
            crypto_ed25519_check(bad.data(), pk.data(), message.data(), message.size()) == 0;
        EXPECT_EQ(lh_ok, mono_ok) << round;
        agreed += lh_ok == mono_ok ? 1 : 0;
    }
    EXPECT_EQ(agreed, 200);
}

} // namespace
