/**
 * @file place.c
 * @brief One draw call's origin, clip and mask.
 */
#include <lh/gl/place.h>

#include <lh/core/assert/runtime.h>

const lh_gl_point_t *
lh_gl_place_get_origin(const lh_gl_place_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

const lh_gl_rect_t *
lh_gl_place_get_clip(const lh_gl_place_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->clip);
}

const lh_gl_mask_t *
lh_gl_place_get_mask(const lh_gl_place_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mask;
}

lh_void
lh_gl_place_init(lh_gl_place_t *self, lh_gl_point_t origin, lh_gl_rect_t clip,
                 const lh_gl_mask_t *mask)
{
    lh_assert_runtime_ref(self);
    self->origin = origin;
    self->clip = clip;
    self->mask = mask;
}
