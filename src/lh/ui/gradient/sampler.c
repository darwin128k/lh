/**
 * @file sampler.c
 * @brief Implementation of `lh/ui/gradient/sampler.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/math.h>
#include <lh/ui/gradient/sampler.h>
#include <lh/ui/gradient/sine.h>
#include <lh/ui/point.h>
#include <lh/ui/scalar.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>
#include <lh/cast/static.h>

/** `ceil(1448 * k / 255)` squared: the least squared distance, on the 1024-to-a-side
   scale ::lh_ui_gradient_frac_at puts a radial gradient on, whose whole square root
   reaches step `k` of the axis (`isqrt(n) * 255 / 1448 >= k`). */
static const lh_u32_t lh_ui_gradient_sampler_reach[256] = {
    0U, 36U, 144U, 324U, 529U, 841U, 1225U, 1600U,
    2116U, 2704U, 3249U, 3969U, 4761U, 5476U, 6400U, 7396U,
    8281U, 9409U, 10609U, 11664U, 12996U, 14400U, 15625U, 17161U,
    18769U, 20164U, 21904U, 23716U, 25281U, 27225U, 29241U, 31329U,
    33124U, 35344U, 37636U, 39601U, 42025U, 44521U, 46656U, 49284U,
    51984U, 54289U, 57121U, 60025U, 62500U, 65536U, 68644U, 71289U,
    74529U, 77841U, 80656U, 84100U, 87616U, 90601U, 94249U, 97969U,
    101124U, 104976U, 108900U, 112896U, 116281U, 120409U, 124609U, 128164U,
    132496U, 136900U, 140625U, 145161U, 149769U, 153664U, 158404U, 163216U,
    167281U, 172225U, 177241U, 181476U, 186624U, 191844U, 196249U, 201601U,
    207025U, 211600U, 217156U, 222784U, 227529U, 233289U, 239121U, 245025U,
    250000U, 256036U, 262144U, 267289U, 273529U, 279841U, 285156U, 291600U,
    298116U, 303601U, 310249U, 316969U, 322624U, 329476U, 336400U, 342225U,
    349281U, 356409U, 362404U, 369664U, 376996U, 383161U, 390625U, 398161U,
    404496U, 412164U, 419904U, 427716U, 434281U, 442225U, 450241U, 456976U,
    465124U, 473344U, 480249U, 488601U, 497025U, 504100U, 512656U, 521284U,
    528529U, 537289U, 546121U, 553536U, 562500U, 571536U, 579121U, 588289U,
    597529U, 605284U, 614656U, 624100U, 632025U, 641601U, 651249U, 660969U,
    669124U, 678976U, 688900U, 697225U, 707281U, 717409U, 725904U, 736164U,
    746496U, 755161U, 765625U, 776161U, 784996U, 795664U, 806404U, 815409U,
    826281U, 837225U, 846400U, 857476U, 868624U, 877969U, 889249U, 900601U,
    910116U, 921600U, 933156U, 944784U, 954529U, 966289U, 978121U, 988036U,
    1000000U, 1012036U, 1022121U, 1034289U, 1046529U, 1056784U, 1069156U, 1081600U,
    1092025U, 1104601U, 1117249U, 1127844U, 1140624U, 1153476U, 1164241U, 1177225U,
    1190281U, 1201216U, 1214404U, 1227664U, 1238769U, 1252161U, 1265625U, 1279161U,
    1290496U, 1304164U, 1317904U, 1329409U, 1343281U, 1357225U, 1368900U, 1382976U,
    1397124U, 1408969U, 1423249U, 1437601U, 1449616U, 1464100U, 1478656U, 1490841U,
    1505529U, 1520289U, 1532644U, 1547536U, 1562500U, 1575025U, 1590121U, 1605289U,
    1617984U, 1633284U, 1648656U, 1664100U, 1677025U, 1692601U, 1708249U, 1721344U,
    1737124U, 1752976U, 1766241U, 1782225U, 1798281U, 1811716U, 1827904U, 1844164U,
    1857769U, 1874161U, 1890625U, 1904400U, 1920996U, 1937664U, 1951609U, 1968409U,
    1985281U, 1999396U, 2016400U, 2033476U, 2047761U, 2064969U, 2082249U, 2096704U,
};

/** `a / b` rounded down, for @p b above zero, and what is left over in @p rest
   (`0 .. b - 1`). */
static lh_s64_t
lh_ui_gradient_sampler_floor_div(lh_s64_t a, lh_s64_t b, lh_s64_t *rest)
{
    lh_s64_t quotient = a / b;
    lh_s64_t remainder = a % b;

    if (remainder < 0)
    {
        --quotient;
        remainder += b;
    }
    *rest = remainder;
    return quotient;
}

/** The colour of every step of the axis, as ::lh_ui_gradient_color_at has it. Between
   two stops each channel is `(from * (span - at) + to * at) / span`, and one step on
   adds `to - from` to what is divided: the quotient and its remainder are carried
   instead of dividing again. A step before its stop's start -- stops out of order --
   is asked of ::lh_ui_gradient_color_at itself. */
static lh_void
lh_ui_gradient_sampler_fill(lh_ui_gradient_sampler_t *self)
{
    const lh_ui_gradient_t *gradient = self->gradient;
    const lh_s32_t count = lh_ui_gradient_get_stop_count(gradient);
    lh_s32_t quotient[4];
    lh_s32_t rest[4];
    lh_s32_t step[4];
    lh_s32_t step_rest[4];
    lh_s32_t carried = 0;
    lh_s32_t segment = 1;
    lh_s32_t frac;
    lh_s32_t c;

    for (frac = 0; frac < 256; ++frac)
    {
        const lh_ui_gradient_stop_t *a;
        const lh_ui_gradient_stop_t *b;
        lh_s32_t start;
        lh_s32_t span;
        lh_s32_t at;

        if (count <= 1 || frac <= lh_ui_gradient_stop_get_frac(lh_ui_gradient_get_stop_as_const(gradient, 0)))
        {
            lh_ui_gradient_color_at(gradient, lh_cast_static(lh_byte_t, frac), lh_addr_of(self->colors[frac]));
            continue;
        }
        /* The first stop at or past this step only moves on as the step does. */
        while (segment < count &&
               frac > lh_ui_gradient_stop_get_frac(lh_ui_gradient_get_stop_as_const(gradient,
                                                                                    lh_cast_static(lh_ui_gradient_stop_count_t, segment))))
        {
            ++segment;
        }
        if (segment >= count)
        {
            lh_ui_gradient_color_at(gradient, lh_cast_static(lh_byte_t, frac), lh_addr_of(self->colors[frac]));
            continue;
        }
        a = lh_ui_gradient_get_stop_as_const(gradient, lh_cast_static(lh_ui_gradient_stop_count_t, (segment - 1)));
        b = lh_ui_gradient_get_stop_as_const(gradient, lh_cast_static(lh_ui_gradient_stop_count_t, segment));
        start = lh_ui_gradient_stop_get_frac(a);
        span = lh_ui_gradient_stop_get_frac(b) - start;
        at = frac - start;
        if (span <= 0 || at < 0)
        {
            lh_ui_gradient_color_at(gradient, lh_cast_static(lh_byte_t, frac), lh_addr_of(self->colors[frac]));
            carried = 0;
            continue;
        }
        if (carried != segment)
        {
            const lh_ui_color_t *from = lh_ui_gradient_stop_get_color_as_const(a);
            const lh_ui_color_t *to = lh_ui_gradient_stop_get_color_as_const(b);
            const lh_s32_t f[4] = {lh_ui_color_get_r(from), lh_ui_color_get_g(from), lh_ui_color_get_b(from),
                                   lh_ui_color_get_a(from)};
            const lh_s32_t t[4] = {lh_ui_color_get_r(to), lh_ui_color_get_g(to), lh_ui_color_get_b(to),
                                   lh_ui_color_get_a(to)};

            for (c = 0; c < 4; ++c)
            {
                const lh_s32_t moved = t[c] - f[c];
                const lh_s32_t whole = f[c] * (span - at) + t[c] * at;

                quotient[c] = whole / span;
                rest[c] = whole % span;
                step[c] = moved / span;
                step_rest[c] = moved % span;
                if (step_rest[c] < 0)
                {
                    --step[c];
                    step_rest[c] += span;
                }
            }
            carried = segment;
        }
        else
        {
            for (c = 0; c < 4; ++c)
            {
                quotient[c] += step[c];
                rest[c] += step_rest[c];
                if (rest[c] >= span)
                {
                    rest[c] -= span;
                    ++quotient[c];
                }
            }
        }
        lh_ui_color_init(lh_addr_of(self->colors[frac]), lh_cast_static(lh_ui_color_channel_t, quotient[0]),
                         lh_cast_static(lh_ui_color_channel_t, quotient[1]), lh_cast_static(lh_ui_color_channel_t, quotient[2]),
                         lh_cast_static(lh_ui_color_channel_t, quotient[3]));
    }
}

/** The colours of pixels @p x0 .. @p x1 - 1 of a radial gradient's row whose doubled
   distance from the middle is @p dy: `dx * 1024 / w` cut toward zero, carried from
   pixel to pixel as a quotient rounded down and its remainder. The squared distance
   then crosses the steps of the axis at ::lh_ui_gradient_sampler_reach, and the step
   is walked to from the last pixel's. */
static lh_void
lh_ui_gradient_sampler_radial_run(const lh_ui_gradient_sampler_t *self, lh_s32_t x0, lh_s32_t x1, lh_s64_t dy,
                                  lh_ui_color_t *out)
{
    const lh_s64_t uy = dy * 1024 / self->height;
    const lh_s64_t uy2 = uy * uy;
    lh_s64_t along = (2 * lh_cast_static(lh_s64_t, x0)+ 1 - self->middle_x) * 1024;
    lh_s64_t rest;
    lh_s64_t quotient;
    lh_s64_t step_rest;
    lh_s64_t step;
    lh_s32_t level = 0;
    lh_s32_t x;

    lh_return_if(x1 <= x0);
    quotient = lh_ui_gradient_sampler_floor_div(along, self->width, lh_addr_of(rest));
    step = lh_ui_gradient_sampler_floor_div(2048, self->width, lh_addr_of(step_rest));
    for (x = x0; x < x1; ++x)
    {
        const lh_s64_t ux = (along < 0 && rest != 0) ? quotient + 1 : quotient;
        const lh_u64_t square = lh_cast_static(lh_u64_t, (ux * ux + uy2));

        while (level < 255 && square >= lh_ui_gradient_sampler_reach[level + 1])
        {
            ++level;
        }
        while (level > 0 && square < lh_ui_gradient_sampler_reach[level])
        {
            --level;
        }
        out[x - x0] = self->colors[level];
        along += 2048;
        quotient += step;
        rest += step_rest;
        if (rest >= self->width)
        {
            rest -= self->width;
            ++quotient;
        }
    }
}

lh_void
lh_ui_gradient_sampler_init(lh_ui_gradient_sampler_t *self, const lh_ui_gradient_t *gradient,
                            const lh_ui_rect_t *rect)
{
    const lh_ui_point_t *origin;
    const lh_ui_size_t *size;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(gradient);
    lh_assert_runtime_ref(rect);
    origin = lh_ui_rect_get_origin_as_const(rect);
    size = lh_ui_rect_get_size_as_const(rect);
    self->gradient = gradient;
    lh_ui_gradient_sampler_fill(self);
    self->shape = lh_ui_gradient_get_shape(gradient);
    self->width = lh_ui_scalar_floor_s32(lh_ui_size_get_width(size));
    self->height = lh_ui_scalar_floor_s32(lh_ui_size_get_height(size));
    self->middle_x = 2 * lh_cast_static(lh_s64_t, lh_ui_scalar_floor_s32(lh_ui_point_get_x(origin)))+ self->width;
    self->middle_y = 2 * lh_cast_static(lh_s64_t, lh_ui_scalar_floor_s32(lh_ui_point_get_y(origin)))+ self->height;
    self->sine = lh_ui_gradient_sine(lh_ui_gradient_get_angle(gradient));
    self->cosine = lh_ui_gradient_sine(lh_ui_gradient_get_angle(gradient) + 90);
    self->length = (self->width * (self->sine < 0 ? -self->sine : self->sine) +
                    self->height * (self->cosine < 0 ? -self->cosine : self->cosine)) *
                   2;
}

lh_void
lh_ui_gradient_sampler_row(lh_ui_gradient_sampler_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                           lh_ui_color_t *out)
{
    /* The pixel's centre from the middle of the box, doubled, as ::lh_ui_gradient_frac_at
       has it; each pixel to the right is two further along. */
    const lh_s64_t dx = 2 * lh_cast_static(lh_s64_t, x0)+ 1 - self->middle_x;
    const lh_s64_t dy = 2 * lh_cast_static(lh_s64_t, y)+ 1 - self->middle_y;
    lh_s32_t x;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    if (self->width <= 0 || self->height <= 0 ||
        (self->shape != lh_ui_gradient_shape_radial && self->length == 0))
    {
        const lh_ui_color_t first = self->colors[0];

        for (x = x0; x < x1; ++x)
        {
            out[x - x0] = first;
        }
        return;
    }
    if (self->shape == lh_ui_gradient_shape_radial)
    {
        /* Two pixels the same distance either side of the middle are the same colour:
           `dx * 1024 / w` is cut toward zero, which gives `-dx` the opposite of what it
           gives `dx`. So the half from the middle on is walked, and each pixel left of
           the middle is its mirror's -- walked too when the mirror is past the end of
           the row. */
        const lh_s64_t middle = self->middle_x;
        const lh_s32_t centre = lh_cast_static(lh_s32_t, (middle >= 0 ? middle / 2 : -((1 - middle) / 2)));
        const lh_s32_t right = lh_math_max(x0, centre);
        const lh_s32_t lone = lh_math_min(lh_math_min(x1, centre), lh_cast_static(lh_s32_t, (middle - x1)));

        lh_ui_gradient_sampler_radial_run(self, right, x1, dy, out + (right - x0));
        if (x0 < lone)
        {
            lh_ui_gradient_sampler_radial_run(self, x0, lone, dy, out);
        }
        for (x = lh_math_max(x0, lone); x < lh_math_min(x1, centre); ++x)
        {
            out[x - x0] = out[lh_cast_static(lh_s32_t, (middle - 1 - x))- x0];
        }
        return;
    }
    {
        /* `(2 * along + length) * 255 / (2 * length)`, the along the line moving by
           `2 * sine` a pixel: the quotient rounded down and carried with its remainder.
           Where ::lh_ui_gradient_frac_at cuts toward zero instead the two differ only
           below zero, and both are then the first step. */
        const lh_s64_t divisor = 2 * self->length;
        lh_s64_t rest;
        lh_s64_t quotient = lh_ui_gradient_sampler_floor_div(
            (2 * (dx * self->sine - dy * self->cosine) + self->length) * 255, divisor, lh_addr_of(rest));
        lh_s64_t step_rest;
        const lh_s64_t step =
            lh_ui_gradient_sampler_floor_div(4 * self->sine * 255, divisor, lh_addr_of(step_rest));

        for (x = x0; x < x1; ++x)
        {
            out[x - x0] = self->colors[quotient < 0 ? 0 : (quotient > 255 ? 255 : lh_cast_static(lh_s32_t, quotient))];
            quotient += step;
            rest += step_rest;
            if (rest >= divisor)
            {
                rest -= divisor;
                ++quotient;
            }
        }
    }
}

lh_void
lh_ui_gradient_sampler_column(lh_ui_gradient_sampler_t *self, lh_s32_t x, lh_s32_t y0, lh_s32_t y1,
                              lh_ui_color_t *out)
{
    const lh_s64_t dx = 2 * lh_cast_static(lh_s64_t, x)+ 1 - self->middle_x;
    const lh_s64_t dy = 2 * lh_cast_static(lh_s64_t, y0)+ 1 - self->middle_y;
    lh_s32_t y;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    if (self->shape == lh_ui_gradient_shape_radial || self->width <= 0 || self->height <= 0 || self->length == 0)
    {
        for (y = y0; y < y1; ++y)
        {
            lh_ui_gradient_sampler_row(self, x, x + 1, y, out + (y - y0));
        }
        return;
    }
    {
        /* As along a row, but a pixel down moves the along the line by `-2 * cosine`. */
        const lh_s64_t divisor = 2 * self->length;
        lh_s64_t rest;
        lh_s64_t quotient = lh_ui_gradient_sampler_floor_div(
            (2 * (dx * self->sine - dy * self->cosine) + self->length) * 255, divisor, lh_addr_of(rest));
        lh_s64_t step_rest;
        const lh_s64_t step =
            lh_ui_gradient_sampler_floor_div(-4 * self->cosine * 255, divisor, lh_addr_of(step_rest));

        for (y = y0; y < y1; ++y)
        {
            out[y - y0] = self->colors[quotient < 0 ? 0 : (quotient > 255 ? 255 : lh_cast_static(lh_s32_t, quotient))];
            quotient += step;
            rest += step_rest;
            if (rest >= divisor)
            {
                rest -= divisor;
                ++quotient;
            }
        }
    }
}
