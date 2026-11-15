/**
 * @file fill.c
 * @brief Paint an entity rectangle from a gradient, top to bottom.
 */
#include <lh/gl/gradient/fill.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/canvas.h>
#include <lh/gl/cover.h>
#include <lh/gl/entity/class/initializer.h>
#include <lh/gl/gradient/sine.h>

lh_gl_entity_t *
lh_gl_gradient_fill_get_entity_mut(lh_gl_gradient_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_entity_t *
lh_gl_gradient_fill_get_entity(const lh_gl_gradient_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_gradient_t *
lh_gl_gradient_fill_get_gradient(const lh_gl_gradient_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return self->gradient;
}

lh_void
lh_gl_gradient_fill_set_gradient(lh_gl_gradient_fill_t *self, const lh_gl_gradient_t *gradient)
{
    lh_assert_runtime_ref(self);
    self->gradient = gradient;
    lh_gl_entity_invalidate(lh_gl_gradient_fill_get_entity_mut(self));
}

lh_gl_scalar_t
lh_gl_gradient_fill_get_angle(const lh_gl_gradient_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return self->angle;
}

lh_void
lh_gl_gradient_fill_set_angle(lh_gl_gradient_fill_t *self, lh_gl_scalar_t angle)
{
    lh_assert_runtime_ref(self);
    self->angle = angle;
    lh_gl_entity_invalidate(lh_gl_gradient_fill_get_entity_mut(self));
}

lh_gl_scalar_t
lh_gl_gradient_fill_get_radius(const lh_gl_gradient_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_gl_gradient_fill_set_radius(lh_gl_gradient_fill_t *self, lh_gl_scalar_t radius)
{
    lh_assert_runtime_ref(self);
    self->radius = radius < 0 ? 0 : radius;
    lh_gl_entity_invalidate(lh_gl_gradient_fill_get_entity_mut(self));
}

lh_bool_t
lh_gl_gradient_fill_is_radial(const lh_gl_gradient_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radial;
}

lh_void
lh_gl_gradient_fill_set_radial(lh_gl_gradient_fill_t *self, lh_bool_t radial)
{
    lh_assert_runtime_ref(self);
    self->radial = radial;
    lh_gl_entity_invalidate(lh_gl_gradient_fill_get_entity_mut(self));
}

lh_bool_t
lh_gl_gradient_fill_get_shown(const lh_gl_gradient_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shown;
}

lh_void
lh_gl_gradient_fill_set_shown(lh_gl_gradient_fill_t *self, lh_bool_t shown)
{
    lh_assert_runtime_ref(self);
    self->shown = shown;
    lh_gl_entity_invalidate(lh_gl_gradient_fill_get_entity_mut(self));
}

lh_bool_t
lh_gl_gradient_fill_get_target(const lh_gl_gradient_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return self->target;
}

lh_void
lh_gl_gradient_fill_set_target(lh_gl_gradient_fill_t *self, lh_bool_t target)
{
    lh_assert_runtime_ref(self);
    self->target = target;
}

const lh_gl_entity_class_t *
lh_gl_gradient_fill_get_class(void)
{
    static const lh_gl_entity_class_t gradient_fill_class = lh_gl_entity_class_initializer(
        lh_gl_entity_class_kind_entity, lh_gl_gradient_fill_draw, lh_gl_gradient_fill_pick,
        lh_bool_false);

    return lh_addr_of(gradient_fill_class);
}

lh_void
lh_gl_gradient_fill_init(lh_gl_gradient_fill_t *self, lh_gl_rect_t rect,
                         const lh_gl_gradient_t *gradient)
{
    lh_gl_entity_init(lh_gl_gradient_fill_get_entity_mut(self), rect,
                      lh_gl_gradient_fill_get_class());
    lh_gl_gradient_fill_set_gradient(self, gradient);
    lh_gl_gradient_fill_set_angle(self, 180);
    lh_gl_gradient_fill_set_radius(self, 0);
    lh_gl_gradient_fill_set_radial(self, lh_bool_false);
    lh_gl_gradient_fill_set_shown(self, lh_bool_true);
    lh_gl_gradient_fill_set_target(self, lh_bool_true);
}

lh_void
lh_gl_gradient_fill_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas)
{
    const lh_gl_gradient_t *gradient =
        lh_gl_gradient_fill_get_gradient(lh_ptr_ccast(lh_gl_gradient_fill_t, entity));

    if (lh_gl_gradient_fill_get_shown(lh_ptr_ccast(lh_gl_gradient_fill_t, entity)) == lh_bool_false ||
        lh_ptr_is_null(gradient))
    {
        return;
    }
    const lh_gl_rect_t *area = lh_gl_entity_get_rect(entity);
    lh_gl_scalar_t width = lh_gl_size_get_width(lh_gl_rect_get_size(area));
    lh_gl_scalar_t height = lh_gl_size_get_height(lh_gl_rect_get_size(area));

    if (width <= 0 || height <= 0)
    {
        return;
    }
    lh_gl_scalar_t radius =
        lh_gl_gradient_fill_get_radius(lh_ptr_ccast(lh_gl_gradient_fill_t, entity));
    lh_gl_scalar_t angle =
        lh_gl_gradient_fill_get_angle(lh_ptr_ccast(lh_gl_gradient_fill_t, entity));
    lh_gl_rect_t shape;

    lh_gl_rect_init(lh_addr_of(shape), 0, 0, width, height);
    if (lh_gl_gradient_fill_is_radial(lh_ptr_ccast(lh_gl_gradient_fill_t, entity)) == lh_bool_true)
    {
        lh_gl_color_t table[256];
        lh_u64_t reach[256];

        reach[0] = 0;
        for (lh_gl_scalar_t frac = 0; frac < 256; ++frac)
        {
            lh_gl_gradient_color_at(gradient, lh_cast_static(lh_gl_color_channel_t, frac),
                                    lh_ptr_add_by_offset(lh_gl_color_t, table,
                                                         (lh_uaddr_t)frac * sizeof(lh_gl_color_t)));
            if (frac == 0)
            {
                continue;
            }
            lh_u64_t need = (lh_cast_static(lh_u64_t, frac) * 1448U + 254U) / 255U;

            reach[frac] = need * need;
        }
        lh_u64_t column_square[256];
        lh_bool_t columns = width <= 256 ? lh_bool_true : lh_bool_false;

        if (columns == lh_bool_true)
        {
            for (lh_gl_scalar_t x = 0; x < width; ++x)
            {
                lh_s64_t ux = lh_gl_gradient_radial_axis(x, width);

                column_square[x] = lh_cast_static(lh_u64_t, ux * ux);
            }
        }
        for (lh_gl_scalar_t y = 0; y < height; ++y)
        {
            lh_gl_scalar_t x0 = 0;
            lh_gl_scalar_t x1 = width;

            if (radius > 0 &&
                lh_gl_cover_round_rect_span(lh_addr_of(shape), radius, y, lh_addr_of(x0),
                                            lh_addr_of(x1)) == lh_bool_false)
            {
                continue;
            }
            lh_s64_t uy = lh_gl_gradient_radial_axis(y, height);
            lh_u64_t row_square = lh_cast_static(lh_u64_t, uy * uy);

            if (columns == lh_bool_true)
            {
                /* `axis(x)` and `axis(width - 1 - x)` are the same distance:
                   the division cuts toward zero, and the squares match. The
                   right half is walked; the left half copies it. */
                lh_gl_color_t row[256];
                lh_gl_scalar_t centre = width / 2;
                lh_gl_scalar_t right = x0 > centre ? x0 : centre;
                lh_gl_scalar_t level = 0;

                for (lh_gl_scalar_t x = right; x < x1; ++x)
                {
                    lh_u64_t square = row_square + column_square[x];

                    while (level < 255 && square >= reach[level + 1])
                    {
                        level = level + 1;
                    }
                    while (level > 0 && square < reach[level])
                    {
                        level = level - 1;
                    }
                    row[x] = table[level];
                }
                for (lh_gl_scalar_t x = x0; x < centre && x < x1; ++x)
                {
                    lh_gl_scalar_t mirror = width - 1 - x;

                    if (mirror >= right && mirror < x1)
                    {
                        row[x] = row[mirror];
                        continue;
                    }
                    lh_u64_t square = row_square + column_square[x];
                    lh_gl_scalar_t lone = 0;

                    while (lone < 255 && square >= reach[lone + 1])
                    {
                        lone = lone + 1;
                    }
                    row[x] = table[lone];
                }
                lh_gl_canvas_fill_colors(canvas, x0, y, row + x0, x1 - x0);
                continue;
            }
            lh_gl_scalar_t level = 0;

            for (lh_gl_scalar_t x = x0; x < x1;)
            {
                lh_gl_color_t chunk[256];
                lh_gl_scalar_t n = x1 - x;

                if (n > 256)
                {
                    n = 256;
                }
                for (lh_gl_scalar_t i = 0; i < n; ++i)
                {
                    lh_s64_t ux = lh_gl_gradient_radial_axis(x + i, width);
                    lh_u64_t square = row_square + lh_cast_static(lh_u64_t, ux * ux);

                    while (level < 255 && square >= reach[level + 1])
                    {
                        level = level + 1;
                    }
                    while (level > 0 && square < reach[level])
                    {
                        level = level - 1;
                    }
                    chunk[i] = table[level];
                }
                lh_gl_canvas_fill_colors(canvas, x, y, chunk, n);
                x = x + n;
            }
        }
        return;
    }
    if (lh_gl_gradient_sine(lh_cast_static(lh_s32_t, angle)) == 0)
    {
        lh_gl_scalar_t run_y = 0;
        lh_gl_scalar_t run_x0 = 0;
        lh_gl_scalar_t run_x1 = 0;
        lh_gl_color_t run_color;
        lh_bool_t open = lh_bool_false;

        for (lh_gl_scalar_t y = 0; y < height; ++y)
        {
            lh_gl_scalar_t x0 = 0;
            lh_gl_scalar_t x1 = width;

            if (radius > 0 &&
                lh_gl_cover_round_rect_span(lh_addr_of(shape), radius, y, lh_addr_of(x0),
                                            lh_addr_of(x1)) == lh_bool_false)
            {
                if (open == lh_bool_true)
                {
                    lh_gl_rect_t strip;

                    lh_gl_rect_init(lh_addr_of(strip), run_x0, run_y, run_x1 - run_x0, y - run_y);
                    lh_gl_canvas_fill_rect(canvas, lh_addr_of(strip), lh_addr_of(run_color));
                    open = lh_bool_false;
                }
                continue;
            }
            lh_gl_color_t color;

            lh_gl_gradient_color_at(gradient,
                                    lh_gl_gradient_linear_frac(angle, width, height, 0, y),
                                    lh_addr_of(color));
            lh_bool_t same = lh_bool_false;

            if (open == lh_bool_true && x0 == run_x0 && x1 == run_x1)
            {
                same = lh_gl_color_get_r(lh_addr_of(color)) ==
                               lh_gl_color_get_r(lh_addr_of(run_color)) &&
                           lh_gl_color_get_g(lh_addr_of(color)) ==
                               lh_gl_color_get_g(lh_addr_of(run_color)) &&
                           lh_gl_color_get_b(lh_addr_of(color)) ==
                               lh_gl_color_get_b(lh_addr_of(run_color)) &&
                           lh_gl_color_get_a(lh_addr_of(color)) ==
                               lh_gl_color_get_a(lh_addr_of(run_color))
                       ? lh_bool_true
                       : lh_bool_false;
            }
            if (same == lh_bool_true)
            {
                continue;
            }
            if (open == lh_bool_true)
            {
                lh_gl_rect_t strip;

                lh_gl_rect_init(lh_addr_of(strip), run_x0, run_y, run_x1 - run_x0, y - run_y);
                lh_gl_canvas_fill_rect(canvas, lh_addr_of(strip), lh_addr_of(run_color));
            }
            run_y = y;
            run_x0 = x0;
            run_x1 = x1;
            run_color = color;
            open = lh_bool_true;
        }
        if (open == lh_bool_true)
        {
            lh_gl_rect_t strip;

            lh_gl_rect_init(lh_addr_of(strip), run_x0, run_y, run_x1 - run_x0, height - run_y);
            lh_gl_canvas_fill_rect(canvas, lh_addr_of(strip), lh_addr_of(run_color));
        }
        return;
    }
    if (radius == 0 && lh_gl_gradient_sine(lh_cast_static(lh_s32_t, angle + 90)) == 0)
    {
        for (lh_gl_scalar_t x = 0; x < width; ++x)
        {
            lh_gl_color_t color;

            lh_gl_gradient_color_at(gradient,
                                    lh_gl_gradient_linear_frac(angle, width, height, x, 0),
                                    lh_addr_of(color));
            lh_gl_rect_t strip;

            lh_gl_rect_init(lh_addr_of(strip), x, 0, 1, height);
            lh_gl_canvas_fill_rect(canvas, lh_addr_of(strip), lh_addr_of(color));
        }
        return;
    }
    for (lh_gl_scalar_t y = 0; y < height; ++y)
    {
        lh_gl_scalar_t x0 = 0;
        lh_gl_scalar_t x1 = width;

        if (radius > 0 && lh_gl_cover_round_rect_span(lh_addr_of(shape), radius, y, lh_addr_of(x0),
                                                      lh_addr_of(x1)) == lh_bool_false)
        {
            continue;
        }
        for (lh_gl_scalar_t x = x0; x < x1; ++x)
        {
            lh_gl_color_t color;

            lh_gl_gradient_color_at(gradient,
                                    lh_gl_gradient_linear_frac(angle, width, height, x, y),
                                    lh_addr_of(color));
            lh_gl_rect_t strip;

            lh_gl_rect_init(lh_addr_of(strip), x, y, 1, 1);
            lh_gl_canvas_fill_rect(canvas, lh_addr_of(strip), lh_addr_of(color));
        }
    }
}

lh_bool_t
lh_gl_gradient_fill_pick(const lh_gl_entity_t *entity, lh_gl_point_t point, lh_gl_scalar_t slop)
{
    const lh_gl_gradient_fill_t *fill = lh_ptr_ccast(lh_gl_gradient_fill_t, entity);

    if (lh_gl_gradient_fill_get_shown(fill) == lh_bool_false ||
        lh_gl_gradient_fill_get_target(fill) == lh_bool_false)
    {
        return lh_bool_false;
    }
    return lh_gl_entity_hit(entity, lh_gl_gradient_fill_get_radius(fill), point, slop);
}
