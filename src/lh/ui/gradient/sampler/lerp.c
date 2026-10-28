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

    lh_assert_runtime_ref(self);
    self->span = span;
    for (c = 0; c < 4; ++c)
    {
        const lh_s32_t moved = t[c] - f[c];
        const lh_s32_t whole = f[c] * (span - at) + t[c] * at;

        self->quotient[c] = whole / span;
        self->rest[c] = whole % span;
        self->step[c] = moved / span;
        self->step_rest[c] = moved % span;
        if (self->step_rest[c] < 0)
        {
            --self->step[c];
            self->step_rest[c] += span;
        }
    }
}

lh_void
lh_ui_gradient_sampler_lerp_next(lh_ui_gradient_sampler_lerp_t *self)
{
    lh_s32_t c;

    lh_assert_runtime_ref(self);
    for (c = 0; c < 4; ++c)
    {
        self->quotient[c] += self->step[c];
        self->rest[c] += self->step_rest[c];
        if (self->rest[c] >= self->span)
        {
            self->rest[c] -= self->span;
            ++self->quotient[c];
        }
    }
}

lh_void
lh_ui_gradient_sampler_lerp_color(const lh_ui_gradient_sampler_lerp_t *self, lh_ui_color_t *out)
{
    lh_assert_runtime_ref(self);
    lh_ui_color_init(out, lh_cast_static(lh_ui_color_channel_t, self->quotient[0]),
                     lh_cast_static(lh_ui_color_channel_t, self->quotient[1]),
                     lh_cast_static(lh_ui_color_channel_t, self->quotient[2]),
                     lh_cast_static(lh_ui_color_channel_t, self->quotient[3]));
}
