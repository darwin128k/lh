/**
 * @file tokens.cpp
 * @brief DEFLATE block token buffer bricks.
 */

#include <gtest/gtest.h>
#include <lh/compress/deflate/tokens.h>
#include <lh/expect/death.h>
#include <memory>

namespace
{

std::unique_ptr<lh_compress_deflate_tokens_t>
make_tokens()
{
    std::unique_ptr<lh_compress_deflate_tokens_t> tokens(new lh_compress_deflate_tokens_t);
    lh_compress_deflate_tokens_reset(tokens.get());
    return tokens;
}

TEST(compress_deflate_tokens, reset_empties_and_count_round_trips)
{
    std::unique_ptr<lh_compress_deflate_tokens_t> tokens(new lh_compress_deflate_tokens_t);
    lh_compress_deflate_tokens_set_count(tokens.get(), 7U);
    EXPECT_EQ(lh_compress_deflate_tokens_get_count(tokens.get()), 7U);
    lh_compress_deflate_tokens_reset(tokens.get());
    EXPECT_EQ(lh_compress_deflate_tokens_get_count(tokens.get()), 0U);
    EXPECT_FALSE(lh_compress_deflate_tokens_is_full(tokens.get()));
    EXPECT_EQ(lh_compress_deflate_tokens_get_values(tokens.get()),
              lh_compress_deflate_tokens_get_values_mut(tokens.get()));
    EXPECT_EQ(lh_compress_deflate_tokens_get_distances(tokens.get()),
              lh_compress_deflate_tokens_get_distances_mut(tokens.get()));
}

TEST(compress_deflate_tokens, stores_literals_and_matches_in_order)
{
    std::unique_ptr<lh_compress_deflate_tokens_t> tokens = make_tokens();
    lh_compress_deflate_tokens_push_literal(tokens.get(), 0xffU);
    lh_compress_deflate_tokens_push_match(tokens.get(), 3U, 1U);
    lh_compress_deflate_tokens_push_match(tokens.get(), 258U, 32768U);
    ASSERT_EQ(lh_compress_deflate_tokens_get_count(tokens.get()), 3U);
    EXPECT_TRUE(lh_compress_deflate_tokens_is_literal(tokens.get(), 0U));
    EXPECT_EQ(lh_compress_deflate_tokens_get_literal(tokens.get(), 0U), 0xffU);
    EXPECT_EQ(lh_compress_deflate_tokens_get_distance(tokens.get(), 0U), 0U);
    EXPECT_FALSE(lh_compress_deflate_tokens_is_literal(tokens.get(), 1U));
    EXPECT_EQ(lh_compress_deflate_tokens_get_length(tokens.get(), 1U), 3U);
    EXPECT_EQ(lh_compress_deflate_tokens_get_distance(tokens.get(), 1U), 1U);
    EXPECT_EQ(lh_compress_deflate_tokens_get_length(tokens.get(), 2U), 258U);
    EXPECT_EQ(lh_compress_deflate_tokens_get_distance(tokens.get(), 2U), 32768U);
}

TEST(compress_deflate_tokens, rejects_out_of_range_matches)
{
    std::unique_ptr<lh_compress_deflate_tokens_t> tokens = make_tokens();
    EXPECT_FALSE(lh_compress_deflate_tokens_try_push_match(tokens.get(), 2U, 1U));
    EXPECT_FALSE(lh_compress_deflate_tokens_try_push_match(tokens.get(), 259U, 1U));
    EXPECT_FALSE(lh_compress_deflate_tokens_try_push_match(tokens.get(), 3U, 0U));
    EXPECT_FALSE(lh_compress_deflate_tokens_try_push_match(tokens.get(), 3U, 32769U));
    EXPECT_EQ(lh_compress_deflate_tokens_get_count(tokens.get()), 0U);
    LH_EXPECT_CHECK_DEATH(lh_compress_deflate_tokens_push_match(tokens.get(), 2U, 1U));
}

TEST(compress_deflate_tokens, refuses_a_token_past_the_limit)
{
    std::unique_ptr<lh_compress_deflate_tokens_t> tokens = make_tokens();
    for (lh_usize_t i = 0U; i < LH_COMPRESS_DEFLATE_TOKENS_LIMIT; ++i)
    {
        lh_compress_deflate_tokens_push_literal(tokens.get(), 'x');
    }
    EXPECT_TRUE(lh_compress_deflate_tokens_is_full(tokens.get()));
    EXPECT_FALSE(lh_compress_deflate_tokens_try_push_literal(tokens.get(), 'y'));
    EXPECT_FALSE(lh_compress_deflate_tokens_try_push_match(tokens.get(), 3U, 1U));
    EXPECT_EQ(lh_compress_deflate_tokens_get_count(tokens.get()), LH_COMPRESS_DEFLATE_TOKENS_LIMIT);
    LH_EXPECT_CHECK_DEATH(lh_compress_deflate_tokens_push_literal(tokens.get(), 'y'));
}

} // namespace
