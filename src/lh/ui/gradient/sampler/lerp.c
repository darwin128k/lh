#include <lh/ui/gradient/sampler/lerp.h>
#include <lh/math.h>
#include <lh/ui/gradient/sampler.h>
#include <lh/ui/gradient/sine.h>
#include <lh/util/return.h>

lh_void
lh_ui_gradient_sampler_lerp_init(lh_ui_gradient_sampler_lerp_t *self, const lh_ui_color_t *from,
                                 const lh_ui_color_t *to, lh_s32_t span, lh_s32_t at)
{
    const lh_s32_t f[4] = {lh_ui_color_get_r(from), lh_ui_color_get_g(from),
                           lh_ui_color_get_b(from), lh_ui_color_get_a(from)};
    const lh_s32_t t[4] = {lh_ui_color_get_r(to), lh_ui_color_get_g(to), lh_ui_color_get_b(to),
                           lh_ui_color_get_a(to)};
    lh_s32_t c;

    lh_ui_gradient_sampler_lerp_set_span(self, span);
    for (c = 0; c < 4; ++c)
    {
        const lh_s32_t moved = t[c] - f[c];
        const lh_s32_t whole = f[c] * (span - at) + t[c] * at;

        lh_ui_gradient_sampler_lerp_get_quotient_mut(self)[c] = whole / span;
        lh_ui_gradient_sampler_lerp_get_rest_mut(self)[c] = whole % span;
        lh_ui_gradient_sampler_lerp_get_step_mut(self)[c] = moved / span;
        lh_ui_gradient_sampler_lerp_get_step_rest_mut(self)[c] = moved % span;
        if (lh_ui_gradient_sampler_lerp_get_step_rest(self)[c] < 0)
        {
            --lh_ui_gradient_sampler_lerp_get_step_mut(self)[c];
            lh_ui_gradient_sampler_lerp_get_step_rest_mut(self)[c] += span;
        }
    }
}

lh_void
lh_ui_gradient_sampler_lerp_next(lh_ui_gradient_sampler_lerp_t *self)
{
    lh_s32_t c;

    for (c = 0; c < 4; ++c)
    {
        lh_ui_gradient_sampler_lerp_get_quotient_mut(self)[c] +=
            lh_ui_gradient_sampler_lerp_get_step(self)[c];
        lh_ui_gradient_sampler_lerp_get_rest_mut(self)[c] +=
            lh_ui_gradient_sampler_lerp_get_step_rest(self)[c];
        if (lh_ui_gradient_sampler_lerp_get_rest(self)[c] >=
            lh_ui_gradient_sampler_lerp_get_span(self))
        {
            lh_ui_gradient_sampler_lerp_get_rest_mut(self)[c] -=
                lh_ui_gradient_sampler_lerp_get_span(self);
            ++lh_ui_gradient_sampler_lerp_get_quotient_mut(self)[c];
        }
    }
}

lh_void
lh_ui_gradient_sampler_lerp_color(const lh_ui_gradient_sampler_lerp_t *self, lh_ui_color_t *out)
{
    lh_ui_color_init(
        out,
        lh_cast_static(lh_ui_color_channel_t, lh_ui_gradient_sampler_lerp_get_quotient(self)[0]),
        lh_cast_static(lh_ui_color_channel_t, lh_ui_gradient_sampler_lerp_get_quotient(self)[1]),
        lh_cast_static(lh_ui_color_channel_t, lh_ui_gradient_sampler_lerp_get_quotient(self)[2]),
        lh_cast_static(lh_ui_color_channel_t, lh_ui_gradient_sampler_lerp_get_quotient(self)[3]));
}

const lh_s32_t *
lh_ui_gradient_sampler_lerp_get_quotient(const lh_ui_gradient_sampler_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->quotient;
}

lh_s32_t *
lh_ui_gradient_sampler_lerp_get_quotient_mut(lh_ui_gradient_sampler_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->quotient;
}

const lh_s32_t *
lh_ui_gradient_sampler_lerp_get_rest(const lh_ui_gradient_sampler_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rest;
}

lh_s32_t *
lh_ui_gradient_sampler_lerp_get_rest_mut(lh_ui_gradient_sampler_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rest;
}

const lh_s32_t *
lh_ui_gradient_sampler_lerp_get_step(const lh_ui_gradient_sampler_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step;
}

lh_s32_t *
lh_ui_gradient_sampler_lerp_get_step_mut(lh_ui_gradient_sampler_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step;
}

const lh_s32_t *
lh_ui_gradient_sampler_lerp_get_step_rest(const lh_ui_gradient_sampler_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step_rest;
}

lh_s32_t *
lh_ui_gradient_sampler_lerp_get_step_rest_mut(lh_ui_gradient_sampler_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step_rest;
}

lh_s32_t
lh_ui_gradient_sampler_lerp_get_span(const lh_ui_gradient_sampler_lerp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->span;
}

lh_void
lh_ui_gradient_sampler_lerp_set_span(lh_ui_gradient_sampler_lerp_t *self, lh_s32_t span)
{
    lh_assert_runtime_ref(self);
    self->span = span;
}
