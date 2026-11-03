/**
 * @file matcher.cpp
 * @brief Hash-chain match finder bricks.
 */

#include <gtest/gtest.h>
#include <lh/compress/deflate/matcher.h>
#include <lh/expect/death.h>
#include <memory>
#include <string>

namespace
{

const lh_byte_t *
bytes(const std::string &text)
{
    return reinterpret_cast<const lh_byte_t *>(text.data());
}

std::unique_ptr<lh_compress_deflate_matcher_t>
make_matcher(lh_usize_t chain_limit)
{
    std::unique_ptr<lh_compress_deflate_matcher_t> matcher(new lh_compress_deflate_matcher_t);
    lh_compress_deflate_matcher_init(matcher.get(), chain_limit);
    return matcher;
}

TEST(compress_deflate_matcher, init_sets_chain_limit_and_clears_heads)
{
    std::unique_ptr<lh_compress_deflate_matcher_t> matcher(new lh_compress_deflate_matcher_t);
    lh_compress_deflate_matcher_get_head_mut(matcher.get())[7] = 99U;
    lh_compress_deflate_matcher_init(matcher.get(), 5U);
    EXPECT_EQ(lh_compress_deflate_matcher_get_chain_limit(matcher.get()), 5U);
    EXPECT_EQ(lh_compress_deflate_matcher_get_head(matcher.get())[7], 0U);
    lh_compress_deflate_matcher_set_chain_limit(matcher.get(), 9U);
    lh_compress_deflate_matcher_get_head_mut(matcher.get())[7] = 99U;
    lh_compress_deflate_matcher_reset(matcher.get());
    EXPECT_EQ(lh_compress_deflate_matcher_get_chain_limit(matcher.get()), 9U);
    EXPECT_EQ(lh_compress_deflate_matcher_get_head(matcher.get())[7], 0U);
}

TEST(compress_deflate_matcher_get_hash, depends_on_three_bytes_and_stays_in_range)
{
    const std::string text = "abcabd\xff\xff\xff";
    const std::string other = "abcZ";
    EXPECT_EQ(lh_compress_deflate_matcher_get_hash(bytes(text)),
              lh_compress_deflate_matcher_get_hash(bytes(other)));
    EXPECT_NE(lh_compress_deflate_matcher_get_hash(bytes(text)),
              lh_compress_deflate_matcher_get_hash(bytes(text) + 3U));
    EXPECT_LT(lh_compress_deflate_matcher_get_hash(bytes(text) + 6U),
              LH_COMPRESS_DEFLATE_MATCHER_HASH_SIZE);
}

TEST(compress_deflate_matcher_get_match_size, stops_at_difference_end_and_maximum)
{
    const std::string text = "abcdabcx";
    EXPECT_EQ(lh_compress_deflate_matcher_get_match_size(bytes(text), text.size(), 0U, 4U), 3U);
    const std::string run(300U, 'z');
    EXPECT_EQ(lh_compress_deflate_matcher_get_match_size(bytes(run), run.size(), 0U, 1U), 258U);
    EXPECT_EQ(lh_compress_deflate_matcher_get_match_size(bytes(run), run.size(), 0U, 295U), 5U);
}

TEST(compress_deflate_matcher_can_improve, compares_the_byte_after_the_length)
{
    const std::string text = "abcdabcx";
    EXPECT_TRUE(lh_compress_deflate_matcher_can_improve(bytes(text), 0U, 4U, 0U));
    EXPECT_TRUE(lh_compress_deflate_matcher_can_improve(bytes(text), 0U, 4U, 2U));
    EXPECT_FALSE(lh_compress_deflate_matcher_can_improve(bytes(text), 0U, 4U, 3U));
    EXPECT_FALSE(lh_compress_deflate_matcher_can_improve(bytes(text), 1U, 4U, 0U));
}

TEST(compress_deflate_matcher, stops_at_a_match_reaching_the_input_end)
{
    const std::string text = "abcdabcdabcd";
    std::unique_ptr<lh_compress_deflate_matcher_t> matcher = make_matcher(8U);
    for (lh_usize_t i = 0U; i < 8U; ++i)
    {
        lh_compress_deflate_matcher_insert(matcher.get(), bytes(text), text.size(), i);
    }
    lh_usize_t length = 0U;
    lh_usize_t distance = 0U;
    lh_compress_deflate_matcher_find(matcher.get(), bytes(text), text.size(), 8U, &length,
                                     &distance);
    EXPECT_EQ(length, 4U);
    EXPECT_EQ(distance, 4U);
}

TEST(compress_deflate_matcher, finds_longest_inserted_match_within_window)
{
    const std::string text = "abcXabcdYabcdZ";
    std::unique_ptr<lh_compress_deflate_matcher_t> matcher = make_matcher(8U);
    for (lh_usize_t i = 0U; i < 9U; ++i)
    {
        lh_compress_deflate_matcher_insert(matcher.get(), bytes(text), text.size(), i);
    }
    lh_usize_t length = 0U;
    lh_usize_t distance = 0U;
    lh_compress_deflate_matcher_find(matcher.get(), bytes(text), text.size(), 9U, &length,
                                     &distance);
    EXPECT_EQ(length, 4U);
    EXPECT_EQ(distance, 5U);
}

TEST(compress_deflate_matcher, chain_limit_bounds_the_search)
{
    const std::string text = "abcdeXabcYabcde";
    std::unique_ptr<lh_compress_deflate_matcher_t> matcher = make_matcher(1U);
    for (lh_usize_t i = 0U; i < 10U; ++i)
    {
        lh_compress_deflate_matcher_insert(matcher.get(), bytes(text), text.size(), i);
    }
    lh_usize_t length = 0U;
    lh_usize_t distance = 0U;
    ASSERT_TRUE(lh_compress_deflate_matcher_try_find(matcher.get(), bytes(text), text.size(), 10U,
                                                     &length, &distance));
    EXPECT_EQ(length, 3U);
    EXPECT_EQ(distance, 4U);
    lh_compress_deflate_matcher_set_chain_limit(matcher.get(), 8U);
    ASSERT_TRUE(lh_compress_deflate_matcher_try_find(matcher.get(), bytes(text), text.size(), 10U,
                                                     &length, &distance));
    EXPECT_EQ(length, 5U);
    EXPECT_EQ(distance, 10U);
}

TEST(compress_deflate_matcher, ignores_positions_beyond_window_and_reports_misses)
{
    std::string text(LH_COMPRESS_DEFLATE_WINDOW_SIZE + 8U, '.');
    for (lh_usize_t i = 0U; i < text.size(); ++i)
    {
        text[i] = static_cast<char>('a' + (i * 7U + i / 26U) % 26U);
    }
    text.replace(0U, 4U, "QRST");
    text.replace(LH_COMPRESS_DEFLATE_WINDOW_SIZE + 1U, 4U, "QRST");
    std::unique_ptr<lh_compress_deflate_matcher_t> matcher = make_matcher(4096U);
    lh_compress_deflate_matcher_insert(matcher.get(), bytes(text), text.size(), 0U);
    lh_usize_t length = 7U;
    lh_usize_t distance = 7U;
    EXPECT_FALSE(lh_compress_deflate_matcher_try_find(matcher.get(), bytes(text), text.size(),
                                                      LH_COMPRESS_DEFLATE_WINDOW_SIZE + 1U, &length,
                                                      &distance));
    EXPECT_EQ(length, 7U);
    EXPECT_EQ(distance, 7U);
    EXPECT_FALSE(lh_compress_deflate_matcher_try_find(matcher.get(), bytes(text), 2U, 0U, &length,
                                                      &distance));
    LH_EXPECT_CHECK_DEATH(
        lh_compress_deflate_matcher_find(matcher.get(), bytes(text), 2U, 0U, &length, &distance));
}

TEST(compress_deflate_matcher, zero_chain_limit_disables_matching)
{
    const std::string text = "abcabc";
    std::unique_ptr<lh_compress_deflate_matcher_t> matcher = make_matcher(0U);
    for (lh_usize_t i = 0U; i < 3U; ++i)
    {
        lh_compress_deflate_matcher_insert(matcher.get(), bytes(text), text.size(), i);
    }
    lh_usize_t length = 0U;
    lh_usize_t distance = 0U;
    EXPECT_FALSE(lh_compress_deflate_matcher_try_find(matcher.get(), bytes(text), text.size(), 3U,
                                                      &length, &distance));
}

} // namespace
