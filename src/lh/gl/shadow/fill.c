/**
 * @file fill.c
 * @brief Paint the soft shadow around an entity rectangle.
 */
#include <lh/gl/shadow/fill.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/memory/std.h>
#include <lh/gl/canvas.h>
#include <lh/gl/entity/class/initializer.h>

lh_gl_entity_t *
lh_gl_shadow_fill_get_entity_mut(lh_gl_shadow_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_entity_t *
lh_gl_shadow_fill_get_entity(const lh_gl_shadow_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_shadow_t *
lh_gl_shadow_fill_get_shadow(const lh_gl_shadow_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shadow;
}

lh_void
lh_gl_shadow_fill_set_shadow(lh_gl_shadow_fill_t *self, const lh_gl_shadow_t *shadow)
{
    lh_assert_runtime_ref(self);
    self->shadow = shadow;
    lh_gl_shadow_fill_place(self);
}

const lh_gl_rect_t *
lh_gl_shadow_fill_get_box(const lh_gl_shadow_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->box);
}

lh_void
lh_gl_shadow_fill_set_box(lh_gl_shadow_fill_t *self, lh_gl_rect_t box)
{
    lh_assert_runtime_ref(self);
    self->box = box;
    lh_gl_shadow_fill_place(self);
}

lh_gl_scalar_t
lh_gl_shadow_fill_get_radius(const lh_gl_shadow_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_gl_shadow_fill_set_radius(lh_gl_shadow_fill_t *self, lh_gl_scalar_t radius)
{
    lh_assert_runtime_ref(self);
    self->radius = radius < 0 ? 0 : radius;
    lh_gl_entity_invalidate(lh_gl_shadow_fill_get_entity_mut(self));
}

lh_void
lh_gl_shadow_fill_place(lh_gl_shadow_fill_t *self)
{
    const lh_gl_rect_t *box = lh_gl_shadow_fill_get_box(self);
    const lh_gl_shadow_t *shadow = lh_gl_shadow_fill_get_shadow(self);

    if (lh_ptr_is_null(shadow) || lh_gl_shadow_is_empty(shadow))
    {
        lh_gl_entity_set_rect(lh_gl_shadow_fill_get_entity_mut(self), lh_ptr_deref(box));
        return;
    }
    lh_gl_scalar_t spread = lh_gl_shadow_get_spread(shadow);
    lh_gl_scalar_t shift_x = lh_gl_shadow_get_offset_x(shadow);
    lh_gl_scalar_t shift_y = lh_gl_shadow_get_offset_y(shadow);
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(box);
    lh_gl_scalar_t width = lh_gl_size_get_width(lh_gl_rect_get_size(box));
    lh_gl_scalar_t height = lh_gl_size_get_height(lh_gl_rect_get_size(box));
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), lh_gl_point_get_x(origin) + lh_math_min(0, shift_x) - spread,
                    lh_gl_point_get_y(origin) + lh_math_min(0, shift_y) - spread,
                    width + lh_math_abs(shift_x) + spread * 2, height + lh_math_abs(shift_y) + spread * 2);
    lh_gl_entity_set_rect(lh_gl_shadow_fill_get_entity_mut(self), bounds);
}

const lh_gl_entity_class_t *
lh_gl_shadow_fill_get_class(void)
{
    static const lh_gl_entity_class_t shadow_fill_class = lh_gl_entity_class_initializer(
        lh_gl_entity_class_kind_entity, lh_gl_shadow_fill_draw, lh_bool_false);

    return lh_addr_of(shadow_fill_class);
}

lh_void
lh_gl_shadow_fill_init(lh_gl_shadow_fill_t *self, lh_gl_rect_t rect, const lh_gl_shadow_t *shadow)
{
    lh_memory_std_set(self, 0, sizeof(lh_gl_shadow_fill_t));
    lh_gl_entity_init(lh_gl_shadow_fill_get_entity_mut(self), rect, lh_gl_shadow_fill_get_class());
    lh_gl_shadow_fill_set_box(self, rect);
    lh_gl_shadow_fill_set_shadow(self, shadow);
    lh_gl_shadow_fill_set_radius(self, 0);
}

lh_void
lh_gl_shadow_fill_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas)
{
    const lh_gl_shadow_t *shadow =
        lh_gl_shadow_fill_get_shadow(lh_ptr_ccast(lh_gl_shadow_fill_t, entity));

    if (lh_ptr_is_null(shadow) || lh_gl_shadow_is_empty(shadow))
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
    const lh_gl_rect_t *box = lh_gl_shadow_fill_get_box(lh_ptr_ccast(lh_gl_shadow_fill_t, entity));
    const lh_gl_point_t *entity_origin = lh_gl_rect_get_origin(area);
    const lh_gl_point_t *box_origin = lh_gl_rect_get_origin(box);
    lh_gl_rect_t local;

    lh_gl_rect_init(lh_addr_of(local), lh_gl_point_get_x(box_origin) - lh_gl_point_get_x(entity_origin),
                    lh_gl_point_get_y(box_origin) - lh_gl_point_get_y(entity_origin),
                    lh_gl_size_get_width(lh_gl_rect_get_size(box)),
                    lh_gl_size_get_height(lh_gl_rect_get_size(box)));
    for (lh_gl_scalar_t y = 0; y < height; ++y)
    {
        for (lh_gl_scalar_t x = 0; x < width; ++x)
        {
            lh_gl_color_channel_t coverage = lh_gl_shadow_alpha_at(
                shadow, x, y, lh_addr_of(local),
                lh_gl_shadow_fill_get_radius(lh_ptr_ccast(lh_gl_shadow_fill_t, entity)));

            if (coverage == 0)
            {
                continue;
            }
            lh_gl_color_t ink;

            lh_gl_color_init(lh_addr_of(ink), lh_gl_color_get_r(lh_gl_shadow_get_color(shadow)),
                             lh_gl_color_get_g(lh_gl_shadow_get_color(shadow)),
                             lh_gl_color_get_b(lh_gl_shadow_get_color(shadow)), coverage);
            lh_gl_rect_t pixel;

            lh_gl_rect_init(lh_addr_of(pixel), x, y, 1, 1);
            lh_gl_canvas_fill_rect(canvas, lh_addr_of(pixel), lh_addr_of(ink));
        }
    }
}
