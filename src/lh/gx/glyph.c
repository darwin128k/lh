/**
 * @file glyph.c
 * @brief Implementation of `lh/gx/glyph.h`.
 */

#include <lh/gx/glyph.h>

#include <lh/core/assert/runtime.h>
const lh_pixel_mask_t *
lh_gx_glyph_get_mask(const lh_gx_glyph_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->mask);
}

lh_void
lh_gx_glyph_set_mask(lh_gx_glyph_t *self, lh_pixel_mask_t mask)
{
    lh_assert_runtime_ref(self);
    self->mask = mask;
}

lh_s32_t
lh_gx_glyph_get_advance(const lh_gx_glyph_t *self)
{
    lh_assert_runtime_ref(self);
    return self->advance;
}

lh_void
lh_gx_glyph_set_advance(lh_gx_glyph_t *self, lh_s32_t advance)
{
    lh_assert_runtime_ref(self);
    self->advance = advance;
}

lh_s32_t
lh_gx_glyph_get_top(const lh_gx_glyph_t *self)
{
    lh_assert_runtime_ref(self);
    return self->top;
}

lh_void
lh_gx_glyph_set_top(lh_gx_glyph_t *self, lh_s32_t top)
{
    lh_assert_runtime_ref(self);
    self->top = top;
}

lh_void
lh_gx_glyph_init(lh_gx_glyph_t *self, lh_pixel_mask_t mask, lh_s32_t advance, lh_s32_t top)
{
    lh_gx_glyph_set_mask(self, mask);
    lh_gx_glyph_set_advance(self, advance);
    lh_gx_glyph_set_top(self, top);
}
