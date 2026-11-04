/**
 * @file encoder.c
 * @brief DEFLATE encoder state.
 */

#include <lh/core/compress/deflate/encoder.h>
#include <lh/core/assert.h>

const lh_compress_deflate_matcher_t *
lh_compress_deflate_encoder_get_matcher(const lh_compress_deflate_encoder_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->matcher);
}

lh_compress_deflate_matcher_t *
lh_compress_deflate_encoder_get_matcher_mut(lh_compress_deflate_encoder_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->matcher);
}

const lh_compress_deflate_tokens_t *
lh_compress_deflate_encoder_get_tokens(const lh_compress_deflate_encoder_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->tokens);
}

lh_compress_deflate_tokens_t *
lh_compress_deflate_encoder_get_tokens_mut(lh_compress_deflate_encoder_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->tokens);
}

lh_void
lh_compress_deflate_encoder_init(lh_compress_deflate_encoder_t *self, lh_usize_t chain_limit)
{
    lh_compress_deflate_matcher_init(lh_compress_deflate_encoder_get_matcher_mut(self),
                                     chain_limit);
    lh_compress_deflate_tokens_reset(lh_compress_deflate_encoder_get_tokens_mut(self));
}
