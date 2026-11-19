/**
 * @file sampler.c
 * @brief Implementation of `lh/core/pixel/gradient/sampler.h`.
 */

#include <lh/core/pixel/gradient/sampler.h>

#include <lh/core/assert.h>
#include <lh/core/math.h>
#include <lh/core/pixel/gradient/sine.h>
#include <lh/core/util/return.h>

/** `ceil(1448 * k / 255)` squared: the least squared distance, on the 1024-to-a-side
   scale ::lh_pixel_gradient_get_frac_at puts a radial gradient on, whose whole square
   root reaches step `k` of the axis (`isqrt(n) * 255 / 1448 >= k`). */
static const lh_u32_t lh_pixel_gradient_sampler_reach[256] = {
    0U,       36U,      144U,     324U,     529U,     841U,     1225U,    1600U,    2116U,
    2704U,    3249U,    3969U,    4761U,    5476U,    6400U,    7396U,    8281U,    9409U,
    10609U,   11664U,   12996U,   14400U,   15625U,   17161U,   18769U,   20164U,   21904U,
    23716U,   25281U,   27225U,   29241U,   31329U,   33124U,   35344U,   37636U,   39601U,
    42025U,   44521U,   46656U,   49284U,   51984U,   54289U,   57121U,   60025U,   62500U,
    65536U,   68644U,   71289U,   74529U,   77841U,   80656U,   84100U,   87616U,   90601U,
    94249U,   97969U,   101124U,  104976U,  108900U,  112896U,  116281U,  120409U,  124609U,
    128164U,  132496U,  136900U,  140625U,  145161U,  149769U,  153664U,  158404U,  163216U,
    167281U,  172225U,  177241U,  181476U,  186624U,  191844U,  196249U,  201601U,  207025U,
    211600U,  217156U,  222784U,  227529U,  233289U,  239121U,  245025U,  250000U,  256036U,
    262144U,  267289U,  273529U,  279841U,  285156U,  291600U,  298116U,  303601U,  310249U,
    316969U,  322624U,  329476U,  336400U,  342225U,  349281U,  356409U,  362404U,  369664U,
    376996U,  383161U,  390625U,  398161U,  404496U,  412164U,  419904U,  427716U,  434281U,
    442225U,  450241U,  456976U,  465124U,  473344U,  480249U,  488601U,  497025U,  504100U,
    512656U,  521284U,  528529U,  537289U,  546121U,  553536U,  562500U,  571536U,  579121U,
    588289U,  597529U,  605284U,  614656U,  624100U,  632025U,  641601U,  651249U,  660969U,
    669124U,  678976U,  688900U,  697225U,  707281U,  717409U,  725904U,  736164U,  746496U,
    755161U,  765625U,  776161U,  784996U,  795664U,  806404U,  815409U,  826281U,  837225U,
    846400U,  857476U,  868624U,  877969U,  889249U,  900601U,  910116U,  921600U,  933156U,
    944784U,  954529U,  966289U,  978121U,  988036U,  1000000U, 1012036U, 1022121U, 1034289U,
    1046529U, 1056784U, 1069156U, 1081600U, 1092025U, 1104601U, 1117249U, 1127844U, 1140624U,
    1153476U, 1164241U, 1177225U, 1190281U, 1201216U, 1214404U, 1227664U, 1238769U, 1252161U,
    1265625U, 1279161U, 1290496U, 1304164U, 1317904U, 1329409U, 1343281U, 1357225U, 1368900U,
    1382976U, 1397124U, 1408969U, 1423249U, 1437601U, 1449616U, 1464100U, 1478656U, 1490841U,
    1505529U, 1520289U, 1532644U, 1547536U, 1562500U, 1575025U, 1590121U, 1605289U, 1617984U,
    1633284U, 1648656U, 1664100U, 1677025U, 1692601U, 1708249U, 1721344U, 1737124U, 1752976U,
    1766241U, 1782225U, 1798281U, 1811716U, 1827904U, 1844164U, 1857769U, 1874161U, 1890625U,
    1904400U, 1920996U, 1937664U, 1951609U, 1968409U, 1985281U, 1999396U, 2016400U, 2033476U,
    2047761U, 2064969U, 2082249U, 2096704U,
};

lh_pixel_gradient_shape_t
lh_pixel_gradient_sampler_get_shape(const lh_pixel_gradient_sampler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shape;
}

lh_void
lh_pixel_gradient_sampler_set_shape(lh_pixel_gradient_sampler_t *self,
                                    lh_pixel_gradient_shape_t shape)
{
    lh_assert_runtime_ref(self);
    self->shape = shape;
}

lh_s64_t
lh_pixel_gradient_sampler_get_width(const lh_pixel_gradient_sampler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_pixel_gradient_sampler_set_width(lh_pixel_gradient_sampler_t *self, lh_s64_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_s64_t
lh_pixel_gradient_sampler_get_height(const lh_pixel_gradient_sampler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_pixel_gradient_sampler_set_height(lh_pixel_gradient_sampler_t *self, lh_s64_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

lh_s64_t
lh_pixel_gradient_sampler_get_middle_x(const lh_pixel_gradient_sampler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->middle_x;
}

lh_void
lh_pixel_gradient_sampler_set_middle_x(lh_pixel_gradient_sampler_t *self, lh_s64_t middle_x)
{
    lh_assert_runtime_ref(self);
    self->middle_x = middle_x;
}

lh_s64_t
lh_pixel_gradient_sampler_get_middle_y(const lh_pixel_gradient_sampler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->middle_y;
}

lh_void
lh_pixel_gradient_sampler_set_middle_y(lh_pixel_gradient_sampler_t *self, lh_s64_t middle_y)
{
    lh_assert_runtime_ref(self);
    self->middle_y = middle_y;
}

lh_s64_t
lh_pixel_gradient_sampler_get_sine(const lh_pixel_gradient_sampler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->sine;
}

lh_void
lh_pixel_gradient_sampler_set_sine(lh_pixel_gradient_sampler_t *self, lh_s64_t sine)
{
    lh_assert_runtime_ref(self);
    self->sine = sine;
}

lh_s64_t
lh_pixel_gradient_sampler_get_cosine(const lh_pixel_gradient_sampler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->cosine;
}

lh_void
lh_pixel_gradient_sampler_set_cosine(lh_pixel_gradient_sampler_t *self, lh_s64_t cosine)
{
    lh_assert_runtime_ref(self);
    self->cosine = cosine;
}

lh_s64_t
lh_pixel_gradient_sampler_get_length(const lh_pixel_gradient_sampler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->length;
}

lh_void
lh_pixel_gradient_sampler_set_length(lh_pixel_gradient_sampler_t *self, lh_s64_t length)
{
    lh_assert_runtime_ref(self);
    self->length = length;
}

lh_void
lh_pixel_gradient_sampler_flat_run(const lh_pixel_gradient_sampler_t *self, lh_s32_t count,
                                   lh_byte_t *out)
{
    (void)self;
    for (lh_s32_t i = 0; i < count; ++i)
    {
        out[i] = 0U;
    }
}

lh_void
lh_pixel_gradient_sampler_init(lh_pixel_gradient_sampler_t *self,
                               const lh_pixel_gradient_t *gradient, const lh_math_rect_t *rect)
{
    const lh_math_point_t *origin = lh_math_rect_get_origin(rect);
    const lh_s64_t width = lh_math_size_get_width(lh_math_rect_get_size(rect));
    const lh_s64_t height = lh_math_size_get_height(lh_math_rect_get_size(rect));
    const lh_s64_t sine = lh_pixel_gradient_sine(lh_pixel_gradient_get_angle(gradient));
    const lh_s64_t cosine = lh_pixel_gradient_sine(lh_pixel_gradient_get_angle(gradient) + 90);

    lh_pixel_gradient_sampler_set_shape(self, lh_pixel_gradient_get_shape(gradient));
    lh_pixel_gradient_sampler_set_width(self, width);
    lh_pixel_gradient_sampler_set_height(self, height);
    lh_pixel_gradient_sampler_set_middle_x(
        self, 2 * lh_cast_static(lh_s64_t, lh_math_point_get_x(origin)) + width);
    lh_pixel_gradient_sampler_set_middle_y(
        self, 2 * lh_cast_static(lh_s64_t, lh_math_point_get_y(origin)) + height);
    lh_pixel_gradient_sampler_set_sine(self, sine);
    lh_pixel_gradient_sampler_set_cosine(self, cosine);
    lh_pixel_gradient_sampler_set_length(
        self, (width * lh_math_abs(sine) + height * lh_math_abs(cosine)) * 2);
}

lh_bool_t
lh_pixel_gradient_sampler_is_flat(const lh_pixel_gradient_sampler_t *self)
{
    return (lh_pixel_gradient_sampler_get_width(self) <= 0 ||
            lh_pixel_gradient_sampler_get_height(self) <= 0 ||
            (lh_pixel_gradient_sampler_get_shape(self) != lh_pixel_gradient_shape_radial &&
             lh_pixel_gradient_sampler_get_length(self) == 0))
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_pixel_gradient_sampler_linear_run(const lh_pixel_gradient_sampler_t *self, lh_s64_t dx,
                                     lh_s64_t dy, lh_s64_t move, lh_s32_t count, lh_byte_t *out)
{
    /* `(2 * along + length) * 255 / (2 * length)`, the along the line moving by
       @p move / 255 / 2 a pixel: the quotient rounded down and carried with its
       remainder. */
    const lh_s64_t length = lh_pixel_gradient_sampler_get_length(self);
    const lh_s64_t divisor = 2 * length;
    const lh_s64_t at = (2 * (dx * lh_pixel_gradient_sampler_get_sine(self) -
                              dy * lh_pixel_gradient_sampler_get_cosine(self)) +
                         length) *
                        255;
    const lh_s64_t step = lh_math_floor_div(move, divisor);
    const lh_s64_t step_rest = lh_math_floor_mod(move, divisor);
    lh_s64_t quotient = lh_math_floor_div(at, divisor);
    lh_s64_t rest = lh_math_floor_mod(at, divisor);

    for (lh_s32_t i = 0; i < count; ++i)
    {
        out[i] = lh_cast_static(lh_byte_t, lh_math_clamp(quotient, 0, 255));
        quotient += step;
        rest += step_rest;
        if (rest >= divisor)
        {
            rest -= divisor;
            ++quotient;
        }
    }
}

lh_void
lh_pixel_gradient_sampler_radial_run(const lh_pixel_gradient_sampler_t *self, lh_s32_t x0,
                                     lh_s32_t x1, lh_s64_t dy, lh_byte_t *out)
{
    lh_return_if(x1 <= x0);
    const lh_s64_t width = lh_pixel_gradient_sampler_get_width(self);
    const lh_s64_t uy = dy * 1024 / lh_pixel_gradient_sampler_get_height(self);
    const lh_s64_t uy2 = uy * uy;
    const lh_s64_t step = lh_math_floor_div(2048, width);
    const lh_s64_t step_rest = lh_math_floor_mod(2048, width);
    lh_s64_t along =
        (2 * lh_cast_static(lh_s64_t, x0) + 1 - lh_pixel_gradient_sampler_get_middle_x(self)) *
        1024;
    lh_s64_t quotient = lh_math_floor_div(along, width);
    lh_s64_t rest = lh_math_floor_mod(along, width);
    lh_s32_t level = 0;

    for (lh_s32_t x = x0; x < x1; ++x)
    {
        /* `dx * 1024 / width` cut toward zero, as ::lh_pixel_gradient_get_frac_at has it. */
        const lh_s64_t ux = (along < 0 && rest != 0) ? quotient + 1 : quotient;
        const lh_u64_t square = lh_cast_static(lh_u64_t, ux * ux + uy2);

        while (level < 255 && square >= lh_pixel_gradient_sampler_reach[level + 1])
        {
            ++level;
        }
        while (level > 0 && square < lh_pixel_gradient_sampler_reach[level])
        {
            --level;
        }
        out[x - x0] = lh_cast_static(lh_byte_t, level);
        along += 2048;
        quotient += step;
        rest += step_rest;
        if (rest >= width)
        {
            rest -= width;
            ++quotient;
        }
    }
}

lh_void
lh_pixel_gradient_sampler_radial_row(const lh_pixel_gradient_sampler_t *self, lh_s32_t x0,
                                     lh_s32_t x1, lh_s64_t dy, lh_byte_t *out)
{
    /* `dx * 1024 / w` is cut toward zero, which gives `-dx` the opposite of what it
       gives `dx`: the two halves mirror each other exactly. */
    const lh_s64_t middle = lh_pixel_gradient_sampler_get_middle_x(self);
    const lh_s32_t centre =
        lh_cast_static(lh_s32_t, (middle >= 0 ? middle / 2 : lh_math_neg((1 - middle) / 2)));
    const lh_s32_t right = lh_math_max(x0, centre);
    const lh_s32_t lone =
        lh_math_min(lh_math_min(x1, centre), lh_cast_static(lh_s32_t, (middle - x1)));

    lh_pixel_gradient_sampler_radial_run(self, right, x1, dy, out + (right - x0));
    if (x0 < lone)
    {
        lh_pixel_gradient_sampler_radial_run(self, x0, lone, dy, out);
    }
    for (lh_s32_t x = lh_math_max(x0, lone); x < lh_math_min(x1, centre); ++x)
    {
        out[x - x0] = out[lh_cast_static(lh_s32_t, (middle - 1 - x)) - x0];
    }
}

lh_void
lh_pixel_gradient_sampler_row(const lh_pixel_gradient_sampler_t *self, lh_s32_t x0, lh_s32_t x1,
                              lh_s32_t y, lh_byte_t *out)
{
    lh_assert_runtime_ref(out);
    if (lh_pixel_gradient_sampler_is_flat(self))
    {
        lh_pixel_gradient_sampler_flat_run(self, x1 - x0, out);
        return;
    }
    /* The pixel's centre from the middle of the box, doubled, as
       ::lh_pixel_gradient_get_frac_at has it; each pixel to the right is two further. */
    const lh_s64_t dy =
        2 * lh_cast_static(lh_s64_t, y) + 1 - lh_pixel_gradient_sampler_get_middle_y(self);

    if (lh_pixel_gradient_sampler_get_shape(self) == lh_pixel_gradient_shape_radial)
    {
        lh_pixel_gradient_sampler_radial_row(self, x0, x1, dy, out);
        return;
    }
    const lh_s64_t dx =
        2 * lh_cast_static(lh_s64_t, x0) + 1 - lh_pixel_gradient_sampler_get_middle_x(self);

    lh_pixel_gradient_sampler_linear_run(
        self, dx, dy, 4 * lh_pixel_gradient_sampler_get_sine(self) * 255, x1 - x0, out);
}

lh_void
lh_pixel_gradient_sampler_column(const lh_pixel_gradient_sampler_t *self, lh_s32_t x, lh_s32_t y0,
                                 lh_s32_t y1, lh_byte_t *out)
{
    lh_assert_runtime_ref(out);
    if (lh_pixel_gradient_sampler_get_shape(self) == lh_pixel_gradient_shape_radial ||
        lh_pixel_gradient_sampler_is_flat(self))
    {
        for (lh_s32_t y = y0; y < y1; ++y)
        {
            lh_pixel_gradient_sampler_row(self, x, x + 1, y, out + (y - y0));
        }
        return;
    }
    /* As along a row, but a pixel down moves the along the line by `-2 * cosine`. */
    const lh_s64_t dx =
        2 * lh_cast_static(lh_s64_t, x) + 1 - lh_pixel_gradient_sampler_get_middle_x(self);
    const lh_s64_t dy =
        2 * lh_cast_static(lh_s64_t, y0) + 1 - lh_pixel_gradient_sampler_get_middle_y(self);

    lh_pixel_gradient_sampler_linear_run(
        self, dx, dy, -4 * lh_pixel_gradient_sampler_get_cosine(self) * 255, y1 - y0, out);
}
