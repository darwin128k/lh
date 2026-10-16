/**
 * @file gradient.c
 * @brief Implementation of lh/ui/gradient.h and lh/ui/gradient/stop.h.
 */

#include <lh/math.h>
#include <lh/assert/runtime.h>
#include <lh/byte.h>
#include <lh/byte/ptr.h>
#include <lh/byte/limits.h>
#include <lh/cast/static.h>
#include <lh/config.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/gradient.h>
#include <lh/ui/gradient/sine.h>
#include <lh/ui/gradient/stop/count.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* ── Stop ────────────────────────────────────────────────────────────────── */

const lh_ui_color_t *
lh_ui_gradient_stop_get_color_as_const(const lh_ui_gradient_stop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->color);
}

lh_ui_color_t *
lh_ui_gradient_stop_get_color(lh_ui_gradient_stop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->color);
}

lh_byte_t
lh_ui_gradient_stop_get_frac(const lh_ui_gradient_stop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->frac;
}

lh_void
lh_ui_gradient_stop_set_color(lh_ui_gradient_stop_t *self, lh_ui_color_t color)
{
    lh_assert_runtime_ref(self);
    lh_ptr_deref(lh_ui_gradient_stop_get_color(self)) = color;
}

lh_void
lh_ui_gradient_stop_set_frac(lh_ui_gradient_stop_t *self, lh_byte_t frac)
{
    lh_assert_runtime_ref(self);
    self->frac = frac;
}

lh_void
lh_ui_gradient_stop_init(lh_ui_gradient_stop_t *self, lh_ui_color_t color, lh_byte_t frac)
{
    lh_assert_runtime_ref(self);
    lh_ui_gradient_stop_set_color(self, color);
    lh_ui_gradient_stop_set_frac(self, frac);
}

/* ── Gradient ────────────────────────────────────────────────────────────── */

lh_void
lh_ui_gradient_init(lh_ui_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    self->stops_count = 0;
    self->shape = lh_ui_gradient_shape_linear;
    self->angle = 180;
}

lh_void
lh_ui_gradient_init_stops(lh_ui_gradient_t *self, const lh_ui_color_t *colors, lh_byte_cptr fracs,
                          lh_ui_gradient_stop_count_t num_stops)
{
    lh_ui_gradient_stop_count_t i;
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(colors);
    lh_assert_runtime_ifn(num_stops >= 1U, lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_ifn(num_stops <= LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS,
                          lh_runtime_error_code_invalid_argument);
    self->stops_count = num_stops;
    for (i = 0; i < num_stops; ++i)
    {
        lh_byte_t frac;
        if (lh_null_eq(fracs))
        {
            if (num_stops == 1U)
            {
                frac = 0;
            }
            else
            {
                frac = lh_cast_static(lh_byte_t, (i * LH_BYTE_T_MAX) / (num_stops - 1U));
            }
        }
        else
        {
            frac = fracs[i];
        }
        lh_ui_gradient_stop_init(lh_addr_of(self->stops[i]), colors[i], frac);
    }
}

lh_ui_gradient_stop_count_t
lh_ui_gradient_get_stop_count(const lh_ui_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stops_count;
}

const lh_ui_gradient_stop_t *
lh_ui_gradient_get_stop_as_const(const lh_ui_gradient_t *self, lh_ui_gradient_stop_count_t index)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ifn(index < self->stops_count, lh_runtime_error_code_invalid_argument);
    return lh_addr_of(self->stops[index]);
}

lh_ui_gradient_stop_t *
lh_ui_gradient_get_stop(lh_ui_gradient_t *self, lh_ui_gradient_stop_count_t index)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ifn(index < self->stops_count, lh_runtime_error_code_invalid_argument);
    return lh_addr_of(self->stops[index]);
}

/* ── Over a box ──────────────────────────────────────────────────────────── */

/** The integer square root of @p v: the largest r with r * r <= v. */
static lh_s64_t
lh_ui_gradient_isqrt(lh_s64_t v)
{
    lh_s64_t r = 0;
    lh_s64_t bit = lh_cast_static(lh_s64_t, 1)<< 40;

    while (bit > v)
    {
        bit >>= 2;
    }
    while (bit != 0)
    {
        if (v >= r + bit)
        {
            v -= r + bit;
            r = (r >> 1) + bit;
        }
        else
        {
            r >>= 1;
        }
        bit >>= 2;
    }
    return r;
}

lh_void
lh_ui_gradient_set_linear(lh_ui_gradient_t *self, lh_s32_t degrees)
{
    lh_assert_runtime_ref(self);
    degrees %= 360;
    self->shape = lh_ui_gradient_shape_linear;
    self->angle = lh_cast_static(lh_s16_t, degrees < 0 ? degrees + 360 : degrees);
}

lh_void
lh_ui_gradient_set_radial(lh_ui_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    self->shape = lh_ui_gradient_shape_radial;
}

lh_ui_gradient_shape_t
lh_ui_gradient_get_shape(const lh_ui_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shape;
}

lh_s32_t
lh_ui_gradient_get_angle(const lh_ui_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return self->angle;
}

lh_byte_t
lh_ui_gradient_frac_at(const lh_ui_gradient_t *self, const lh_ui_rect_t *rect, lh_s32_t x, lh_s32_t y)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);
    const lh_s64_t w = lh_ui_scalar_floor_s32(lh_ui_size_get_width(size));
    const lh_s64_t h = lh_ui_scalar_floor_s32(lh_ui_size_get_height(size));
    /* The pixel's centre from the middle of the box, both doubled so no half is lost. */
    const lh_s64_t dx = 2 * lh_cast_static(lh_s64_t, x)+ 1 - (2 * lh_cast_static(lh_s64_t, lh_ui_scalar_floor_s32(lh_ui_point_get_x(origin)))+ w);
    const lh_s64_t dy = 2 * lh_cast_static(lh_s64_t, y)+ 1 - (2 * lh_cast_static(lh_s64_t, lh_ui_scalar_floor_s32(lh_ui_point_get_y(origin)))+ h);
    lh_s64_t frac;

    lh_assert_runtime_ref(self);
    lh_return_if(w <= 0 || h <= 0, 0);
    if (self->shape == lh_ui_gradient_shape_radial)
    {
        /* On an ellipse the shape of the box, through its corners: the middle is 0
           and every corner is 255. Each axis is first put on a scale of 1024 to a
           side, so the square root has room and no product overflows. */
        const lh_s64_t ux = dx * 1024 / w;
        const lh_s64_t uy = dy * 1024 / h;

        frac = lh_ui_gradient_isqrt(ux * ux + uy * uy) * 255 / 1448;
    }
    else
    {
        /* Along (sin a, -cos a), which is up at 0 and right at 90; the line is as long
           as the box seen along it, so 0 and 255 land on the two corners it meets. */
        const lh_s64_t s = lh_ui_gradient_sine(self->angle);
        const lh_s64_t c = lh_ui_gradient_sine(self->angle + 90);
        const lh_s64_t along = dx * s - dy * c;
        const lh_s64_t length = (w * lh_math_abs(s) + h * lh_math_abs(c)) * 2;

        lh_return_if(length == 0, 0);
        frac = ((2 * along + length) * 255) / (2 * length);
    }
    return lh_cast_static(lh_byte_t, lh_math_clamp(frac, 0, 255));
}

lh_void
lh_ui_gradient_color_at(const lh_ui_gradient_t *self, lh_byte_t frac, lh_ui_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_gradient_stop_count_t i;
    lh_ui_color_t result;

    lh_assert_runtime_ref(self);
    lh_ui_color_init(lh_addr_of(result), 0, 0, 0, 0);
    if (self->stops_count == 0)
    {
        *out = result;
        return;
    }
    if (self->stops_count == 1 || frac <= lh_ui_gradient_stop_get_frac(lh_addr_of(self->stops[0])))
    {
        *out = *lh_ui_gradient_stop_get_color_as_const(lh_addr_of(self->stops[0]));
        return;
    }
    for (i = 1; i < self->stops_count; ++i)
    {
        const lh_ui_gradient_stop_t *a = lh_addr_of(self->stops[i - 1]);
        const lh_ui_gradient_stop_t *b = lh_addr_of(self->stops[i]);
        const lh_ui_color_t *from = lh_ui_gradient_stop_get_color_as_const(a);
        const lh_ui_color_t *to = lh_ui_gradient_stop_get_color_as_const(b);
        const lh_s32_t start = lh_ui_gradient_stop_get_frac(a);
        const lh_s32_t end = lh_ui_gradient_stop_get_frac(b);

        if (lh_cast_static(lh_s32_t, frac)<= end)
        {
            const lh_s32_t span = end - start;
            const lh_s32_t at = lh_cast_static(lh_s32_t, frac)- start;

            if (span <= 0)
            {
                *out = *to;
                return;
            }
            /* Each channel, alpha with them, mixed in proportion to how far along the
               span the pixel is. */
            lh_ui_color_init(
                lh_addr_of(result),
                lh_cast_static(lh_ui_color_channel_t,
                               (lh_ui_color_get_r(from) * (span - at) + lh_ui_color_get_r(to) * at) / span),
                lh_cast_static(lh_ui_color_channel_t,
                               (lh_ui_color_get_g(from) * (span - at) + lh_ui_color_get_g(to) * at) / span),
                lh_cast_static(lh_ui_color_channel_t,
                               (lh_ui_color_get_b(from) * (span - at) + lh_ui_color_get_b(to) * at) / span),
                lh_cast_static(lh_ui_color_channel_t,
                               (lh_ui_color_get_a(from) * (span - at) + lh_ui_color_get_a(to) * at) / span));
            *out = result;
            return;
        }
    }
    *out = *lh_ui_gradient_stop_get_color_as_const(lh_addr_of(self->stops[self->stops_count - 1]));
    return;
}
