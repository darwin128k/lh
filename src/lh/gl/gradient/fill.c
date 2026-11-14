/**
 * @file fill.c
 * @brief Paint an entity rectangle from a gradient, top to bottom.
 */
#include <lh/gl/gradient/fill.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/byte/limits.h>
#include <lh/gl/canvas.h>
#include <lh/gl/entity/class/initializer.h>

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
    lh_u32_t span = height == 1 ? 0U : lh_cast_static(lh_u32_t, height - 1);

    for (lh_gl_scalar_t y = 0; y < height; ++y)
    {
        lh_gl_color_channel_t frac =
            span == 0U ? 0 : lh_cast_static(lh_gl_color_channel_t,
                                            lh_cast_static(lh_u32_t, y) * LH_BYTE_T_MAX / span);
        lh_gl_color_t color;

        lh_gl_gradient_color_at(gradient, frac, lh_addr_of(color));
        lh_gl_rect_t row;

        lh_gl_rect_init(lh_addr_of(row), 0, y, width, 1);
        lh_gl_canvas_fill_rect(canvas, lh_addr_of(row), lh_addr_of(color));
    }
}
