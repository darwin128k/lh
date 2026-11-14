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

const lh_gl_entity_class_t *
lh_gl_gradient_fill_get_class(void)
{
    static const lh_gl_entity_class_t gradient_fill_class = lh_gl_entity_class_initializer(
        lh_gl_entity_class_kind_entity, lh_gl_gradient_fill_draw, lh_bool_false);

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
}

lh_void
lh_gl_gradient_fill_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas)
{
    const lh_gl_gradient_t *gradient =
        lh_gl_gradient_fill_get_gradient(lh_ptr_ccast(lh_gl_gradient_fill_t, entity));

    if (lh_ptr_is_null(gradient))
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

    if (radius > 0)
    {
        lh_gl_scalar_t angle =
            lh_gl_gradient_fill_get_angle(lh_ptr_ccast(lh_gl_gradient_fill_t, entity));
        lh_gl_rect_t shape;

        lh_gl_rect_init(lh_addr_of(shape), 0, 0, width, height);
        for (lh_gl_scalar_t y = 0; y < height; ++y)
        {
            for (lh_gl_scalar_t x = 0; x < width; ++x)
            {
                lh_gl_color_channel_t cover =
                    lh_gl_cover_round_rect(lh_addr_of(shape), radius, x, y);

                if (cover == 0)
                {
                    continue;
                }
                lh_gl_color_t color;

                lh_gl_gradient_color_at(gradient,
                                        lh_gl_gradient_linear_frac(angle, width, height, x, y),
                                        lh_addr_of(color));
                lh_gl_color_scale_alpha(lh_addr_of(color), cover, lh_addr_of(color));
                lh_gl_rect_t pixel;

                lh_gl_rect_init(lh_addr_of(pixel), x, y, 1, 1);
                lh_gl_canvas_fill_rect(canvas, lh_addr_of(pixel), lh_addr_of(color));
            }
        }
        return;
    }
    lh_gl_scalar_t angle =
        lh_gl_gradient_fill_get_angle(lh_ptr_ccast(lh_gl_gradient_fill_t, entity));

    if (lh_gl_gradient_sine(lh_cast_static(lh_s32_t, angle)) == 0)
    {
        for (lh_gl_scalar_t y = 0; y < height; ++y)
        {
            lh_gl_color_t color;

            lh_gl_gradient_color_at(gradient,
                                    lh_gl_gradient_linear_frac(angle, width, height, 0, y),
                                    lh_addr_of(color));
            lh_gl_rect_t strip;

            lh_gl_rect_init(lh_addr_of(strip), 0, y, width, 1);
            lh_gl_canvas_fill_rect(canvas, lh_addr_of(strip), lh_addr_of(color));
        }
        return;
    }
    if (lh_gl_gradient_sine(lh_cast_static(lh_s32_t, angle + 90)) == 0)
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
        for (lh_gl_scalar_t x = 0; x < width; ++x)
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
