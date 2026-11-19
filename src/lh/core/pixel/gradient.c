/**
 * @file gradient.c
 * @brief Implementation of `lh/core/pixel/gradient.h`.
 */

#include <lh/core/pixel/gradient.h>

#include <lh/core/assert.h>
#include <lh/core/pixel/argb.h>
#include <lh/core/pixel/gradient/sine.h>

const lh_pixel_gradient_stop_t *
lh_pixel_gradient_get_stops(const lh_pixel_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stops;
}

lh_pixel_gradient_stop_t *
lh_pixel_gradient_get_stops_mut(lh_pixel_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stops;
}

lh_pixel_gradient_stop_count_t
lh_pixel_gradient_get_stops_count(const lh_pixel_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stops_count;
}

lh_void
lh_pixel_gradient_set_stops_count(lh_pixel_gradient_t *self,
                                  lh_pixel_gradient_stop_count_t stops_count)
{
    lh_assert_runtime_ref(self);
    self->stops_count = stops_count;
}

lh_pixel_gradient_shape_t
lh_pixel_gradient_get_shape(const lh_pixel_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shape;
}

lh_void
lh_pixel_gradient_set_shape(lh_pixel_gradient_t *self, lh_pixel_gradient_shape_t shape)
{
    lh_assert_runtime_ref(self);
    self->shape = shape;
}

lh_s32_t
lh_pixel_gradient_get_angle(const lh_pixel_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return self->angle;
}

lh_void
lh_pixel_gradient_set_angle(lh_pixel_gradient_t *self, lh_s16_t angle)
{
    lh_assert_runtime_ref(self);
    self->angle = angle;
}

const lh_pixel_gradient_stop_t *
lh_pixel_gradient_get_stop(const lh_pixel_gradient_t *self, lh_pixel_gradient_stop_count_t index)
{
    lh_assert_runtime_ifn(index < lh_pixel_gradient_get_stops_count(self),
                          lh_runtime_error_code_invalid_argument);
    return lh_addr_of(lh_pixel_gradient_get_stops(self)[index]);
}

lh_pixel_gradient_stop_t *
lh_pixel_gradient_get_stop_mut(lh_pixel_gradient_t *self, lh_pixel_gradient_stop_count_t index)
{
    lh_assert_runtime_ifn(index < lh_pixel_gradient_get_stops_count(self),
                          lh_runtime_error_code_invalid_argument);
    return lh_addr_of(lh_pixel_gradient_get_stops_mut(self)[index]);
}

lh_void
lh_pixel_gradient_init(lh_pixel_gradient_t *self)
{
    lh_pixel_gradient_set_stops_count(self, 0);
    lh_pixel_gradient_set_shape(self, lh_pixel_gradient_shape_linear);
    lh_pixel_gradient_set_angle(self, 180);
}

lh_void
lh_pixel_gradient_set_linear(lh_pixel_gradient_t *self, lh_s32_t degrees)
{
    const lh_s32_t folded = degrees % 360;

    lh_pixel_gradient_set_shape(self, lh_pixel_gradient_shape_linear);
    lh_pixel_gradient_set_angle(self, lh_cast_static(lh_s16_t, folded < 0 ? folded + 360 : folded));
}

lh_void
lh_pixel_gradient_set_radial(lh_pixel_gradient_t *self)
{
    lh_pixel_gradient_set_shape(self, lh_pixel_gradient_shape_radial);
}

lh_byte_t
lh_pixel_gradient_get_frac_at(const lh_pixel_gradient_t *self, const lh_math_rect_t *rect,
                              lh_s32_t x, lh_s32_t y)
{
    const lh_math_point_t *origin = lh_math_rect_get_origin(rect);
    const lh_math_size_t *size = lh_math_rect_get_size(rect);
    const lh_s64_t w = lh_math_size_get_width(size);
    const lh_s64_t h = lh_math_size_get_height(size);

    lh_return_if(w <= 0 || h <= 0, 0);
    /* The pixel's centre from the middle of the box, both doubled so no half is lost. */
    const lh_s64_t dx = 2 * lh_cast_static(lh_s64_t, x) + 1 -
                        (2 * lh_cast_static(lh_s64_t, lh_math_point_get_x(origin)) + w);
    const lh_s64_t dy = 2 * lh_cast_static(lh_s64_t, y) + 1 -
                        (2 * lh_cast_static(lh_s64_t, lh_math_point_get_y(origin)) + h);
    lh_s64_t frac;

    if (lh_pixel_gradient_get_shape(self) == lh_pixel_gradient_shape_radial)
    {
        /* On an ellipse the shape of the box, through its corners: the middle is 0
           and every corner is 255. Each axis is first put on a scale of 1024 to a
           side, so the square root has room and no product overflows. */
        const lh_s64_t ux = dx * 1024 / w;
        const lh_s64_t uy = dy * 1024 / h;
        const lh_u64_t square = lh_cast_static(lh_u64_t, ux * ux + uy * uy);

        frac = lh_cast_static(lh_s64_t, lh_math_isqrt_u64(square)) * 255 / 1448;
    }
    else
    {
        /* Along (sin a, -cos a), which is up at 0 and right at 90; the line is as long
           as the box seen along it, so 0 and 255 land on the two corners it meets. */
        const lh_s64_t s = lh_pixel_gradient_sine(lh_pixel_gradient_get_angle(self));
        const lh_s64_t c = lh_pixel_gradient_sine(lh_pixel_gradient_get_angle(self) + 90);
        const lh_s64_t along = dx * s - dy * c;
        const lh_s64_t length = (w * lh_math_abs(s) + h * lh_math_abs(c)) * 2;

        lh_return_if(length == 0, 0);
        frac = ((2 * along + length) * 255) / (2 * length);
    }
    return lh_cast_static(lh_byte_t, lh_math_clamp(frac, 0, 255));
}

lh_s32_t
lh_pixel_gradient_get_segment_at(const lh_pixel_gradient_t *self, lh_s32_t segment, lh_s32_t frac)
{
    const lh_s32_t count = lh_pixel_gradient_get_stops_count(self);

    /* The first stop at or past this step only moves on as the step does. */
    while (segment < count &&
           frac > lh_pixel_gradient_stop_get_frac(lh_pixel_gradient_get_stop(
                      self, lh_cast_static(lh_pixel_gradient_stop_count_t, segment))))
    {
        ++segment;
    }
    return segment;
}

lh_void
lh_pixel_gradient_init_stops(lh_pixel_gradient_t *self, const lh_u32_t *colors, lh_byte_cptr fracs,
                             lh_pixel_gradient_stop_count_t num_stops)
{
    lh_assert_runtime_ref(colors);
    lh_assert_runtime_ifn(num_stops >= 1U, lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_ifn(num_stops <= LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS,
                          lh_runtime_error_code_invalid_argument);
    lh_pixel_gradient_set_stops_count(self, num_stops);
    for (lh_pixel_gradient_stop_count_t i = 0; i < num_stops; ++i)
    {
        lh_byte_t frac = 0;

        if (lh_null_ne(fracs))
        {
            frac = fracs[i];
        }
        else if (num_stops > 1U)
        {
            frac = lh_cast_static(lh_byte_t, (i * LH_BYTE_T_MAX) / (num_stops - 1U));
        }
        lh_pixel_gradient_stop_init(lh_pixel_gradient_get_stop_mut(self, i), colors[i], frac);
    }
}

lh_u32_t
lh_pixel_gradient_get_color_at(const lh_pixel_gradient_t *self, lh_byte_t frac)
{
    const lh_s32_t count = lh_pixel_gradient_get_stops_count(self);

    lh_return_if(count == 0, 0U);
    const lh_pixel_gradient_stop_t *first = lh_pixel_gradient_get_stop(self, 0);

    lh_return_if(count == 1 || frac <= lh_pixel_gradient_stop_get_frac(first),
                 lh_pixel_gradient_stop_get_color(first));
    const lh_s32_t segment = lh_pixel_gradient_get_segment_at(self, 1, frac);

    lh_return_if(segment >= count,
                 lh_pixel_gradient_stop_get_color(lh_pixel_gradient_get_stop(
                     self, lh_cast_static(lh_pixel_gradient_stop_count_t, count - 1))));
    const lh_pixel_gradient_stop_t *a = lh_pixel_gradient_get_stop(
        self, lh_cast_static(lh_pixel_gradient_stop_count_t, segment - 1));
    const lh_pixel_gradient_stop_t *b =
        lh_pixel_gradient_get_stop(self, lh_cast_static(lh_pixel_gradient_stop_count_t, segment));
    const lh_u32_t from = lh_pixel_gradient_stop_get_color(a);
    const lh_u32_t to = lh_pixel_gradient_stop_get_color(b);
    const lh_s32_t span = lh_pixel_gradient_stop_get_frac(b) - lh_pixel_gradient_stop_get_frac(a);
    const lh_s32_t at = lh_cast_static(lh_s32_t, frac) - lh_pixel_gradient_stop_get_frac(a);

    lh_return_if(span <= 0, to);
    /* Each channel, alpha with them, mixed in proportion to how far along the span
       the pixel is. */
    lh_u32_t mixed[4];

    for (lh_u32_t c = 0U; c < 4U; ++c)
    {
        const lh_s32_t f = lh_cast_static(lh_s32_t, lh_pixel_argb_get_channel(from, c * 8U));
        const lh_s32_t t = lh_cast_static(lh_s32_t, lh_pixel_argb_get_channel(to, c * 8U));

        mixed[c] = lh_cast_static(lh_u32_t, (f * (span - at) + t * at) / span);
    }
    return lh_pixel_argb_make(mixed[2], mixed[1], mixed[0], mixed[3]);
}
