/**
 * @file encoder.cpp
 * @brief DEFLATE encoder state bricks.
 */

#include <gtest/gtest.h>
#include <lh/compress/deflate/encoder.h>
#include <memory>

namespace
{

TEST(compress_deflate_encoder, init_prepares_the_matcher_and_empties_the_tokens)
{
    std::unique_ptr<lh_compress_deflate_encoder_t> encoder(new lh_compress_deflate_encoder_t);
    lh_compress_deflate_tokens_set_count(lh_compress_deflate_encoder_get_tokens_mut(encoder.get()),
                                         5U);
    lh_compress_deflate_matcher_get_head_mut(
        lh_compress_deflate_encoder_get_matcher_mut(encoder.get()))[3] = 9U;
    lh_compress_deflate_encoder_init(encoder.get(), 12U);
    const lh_compress_deflate_encoder_t *view = encoder.get();
    EXPECT_EQ(
        lh_compress_deflate_matcher_get_chain_limit(lh_compress_deflate_encoder_get_matcher(view)),
        12U);
    EXPECT_EQ(
        lh_compress_deflate_matcher_get_head(lh_compress_deflate_encoder_get_matcher(view))[3], 0U);
    EXPECT_EQ(lh_compress_deflate_tokens_get_count(lh_compress_deflate_encoder_get_tokens(view)),
              0U);
    EXPECT_EQ(lh_compress_deflate_encoder_get_matcher(view),
              lh_compress_deflate_encoder_get_matcher_mut(encoder.get()));
    EXPECT_EQ(lh_compress_deflate_encoder_get_tokens(view),
              lh_compress_deflate_encoder_get_tokens_mut(encoder.get()));
}

} // namespace
