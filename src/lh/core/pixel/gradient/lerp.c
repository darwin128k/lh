/**
 * @file lerp.c
 * @brief Implementation of `lh/core/pixel/gradient/lerp.h`.
 */

#include <lh/core/pixel/gradient/lerp.h>

#include <lh/core/assert.h>
#include <lh/core/pixel/argb.h>

const lh_s32_t *
lh_pixel_gradient_lerp_get_quotient(const lh_pixel_gradient_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->quotient;
}

lh_s32_t *
lh_pixel_gradient_lerp_get_quotient_mut(lh_pixel_gradient_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->quotient;
}

const lh_s32_t *
lh_pixel_gradient_lerp_get_rest(const lh_pixel_gradient_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rest;
}

lh_s32_t *
lh_pixel_gradient_lerp_get_rest_mut(lh_pixel_gradient_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rest;
}

const lh_s32_t *
lh_pixel_gradient_lerp_get_step(const lh_pixel_gradient_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step;
}

lh_s32_t *
lh_pixel_gradient_lerp_get_step_mut(lh_pixel_gradient_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step;
}

const lh_s32_t *
lh_pixel_gradient_lerp_get_step_rest(const lh_pixel_gradient_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step_rest;
}

lh_s32_t *
lh_pixel_gradient_lerp_get_step_rest_mut(lh_pixel_gradient_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step_rest;
}

lh_s32_t
lh_pixel_gradient_lerp_get_span(const lh_pixel_gradient_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->span;
}

lh_void
lh_pixel_gradient_lerp_set_span(lh_pixel_gradient_lerp_t *self, lh_s32_t span)
{
    lh_assert_runtime_ref(self);
    self->span = span;
}

lh_void
lh_pixel_gradient_lerp_init(lh_pixel_gradient_lerp_t *self, lh_u32_t from, lh_u32_t to,
                            lh_s32_t span, lh_s32_t at)
{
    lh_pixel_gradient_lerp_set_span(self, span);
    for (lh_u32_t c = 0U; c < 4U; ++c)
    {
        const lh_s32_t f = lh_cast_static(lh_s32_t, lh_pixel_argb_get_channel(from, c * 8U));
        const lh_s32_t t = lh_cast_static(lh_s32_t, lh_pixel_argb_get_channel(to, c * 8U));
        const lh_s32_t moved = t - f;
        const lh_s32_t whole = f * (span - at) + t * at;

        lh_pixel_gradient_lerp_get_quotient_mut(self)[c] = whole / span;
        lh_pixel_gradient_lerp_get_rest_mut(self)[c] = whole % span;
        lh_pixel_gradient_lerp_get_step_mut(self)[c] = moved / span;
        lh_pixel_gradient_lerp_get_step_rest_mut(self)[c] = moved % span;
        /* The remainder of a step down is kept at or above zero: one less whole. */
        if (lh_pixel_gradient_lerp_get_step_rest(self)[c] < 0)
        {
            --lh_pixel_gradient_lerp_get_step_mut(self)[c];
            lh_pixel_gradient_lerp_get_step_rest_mut(self)[c] += span;
        }
    }
}

lh_void
lh_pixel_gradient_lerp_next(lh_pixel_gradient_lerp_t *self)
{
    const lh_s32_t span = lh_pixel_gradient_lerp_get_span(self);

    for (lh_u32_t c = 0U; c < 4U; ++c)
    {
        lh_pixel_gradient_lerp_get_quotient_mut(self)[c] +=
            lh_pixel_gradient_lerp_get_step(self)[c];
        lh_pixel_gradient_lerp_get_rest_mut(self)[c] +=
            lh_pixel_gradient_lerp_get_step_rest(self)[c];
        if (lh_pixel_gradient_lerp_get_rest(self)[c] >= span)
        {
            lh_pixel_gradient_lerp_get_rest_mut(self)[c] -= span;
            ++lh_pixel_gradient_lerp_get_quotient_mut(self)[c];
        }
    }
}

lh_u32_t
lh_pixel_gradient_lerp_get_color(const lh_pixel_gradient_lerp_t *self)
{
    const lh_s32_t *q = lh_pixel_gradient_lerp_get_quotient(self);

    return lh_pixel_argb_make(lh_cast_static(lh_u32_t, q[2]), lh_cast_static(lh_u32_t, q[1]),
                              lh_cast_static(lh_u32_t, q[0]), lh_cast_static(lh_u32_t, q[3]));
}
