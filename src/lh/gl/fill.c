/**
 * @file fill.c
 * @brief Paint an entity rectangle with a borrowed color.
 */
#include <lh/gl/fill.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/canvas.h>
#include <lh/gl/cover.h>
#include <lh/gl/entity/class/initializer.h>

lh_gl_entity_t *
lh_gl_fill_get_entity_mut(lh_gl_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_entity_t *
lh_gl_fill_get_entity(const lh_gl_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_color_t *
lh_gl_fill_get_color(const lh_gl_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return self->color;
}

lh_void
lh_gl_fill_set_color(lh_gl_fill_t *self, const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(self);
    self->color = color;
    lh_gl_entity_invalidate(lh_gl_fill_get_entity_mut(self));
}

lh_gl_scalar_t
lh_gl_fill_get_radius(const lh_gl_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_gl_fill_set_radius(lh_gl_fill_t *self, lh_gl_scalar_t radius)
{
    lh_assert_runtime_ref(self);
    self->radius = radius < 0 ? 0 : radius;
    lh_gl_entity_invalidate(lh_gl_fill_get_entity_mut(self));
}

lh_bool_t
lh_gl_fill_get_shown(const lh_gl_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shown;
}

lh_void
lh_gl_fill_set_shown(lh_gl_fill_t *self, lh_bool_t shown)
{
    lh_assert_runtime_ref(self);
    self->shown = shown;
    lh_gl_entity_invalidate(lh_gl_fill_get_entity_mut(self));
}

lh_bool_t
lh_gl_fill_get_target(const lh_gl_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return self->target;
}

lh_void
lh_gl_fill_set_target(lh_gl_fill_t *self, lh_bool_t target)
{
    lh_assert_runtime_ref(self);
    self->target = target;
}

const lh_gl_entity_class_t *
lh_gl_fill_get_class(void)
{
    static const lh_gl_entity_class_t fill_class = lh_gl_entity_class_initializer(
        lh_gl_entity_class_kind_entity, lh_gl_fill_draw, lh_gl_fill_pick, lh_bool_false);

    return lh_addr_of(fill_class);
}

lh_void
lh_gl_fill_init(lh_gl_fill_t *self, lh_gl_rect_t rect, const lh_gl_color_t *color)
{
    lh_gl_entity_init(lh_gl_fill_get_entity_mut(self), rect, lh_gl_fill_get_class());
    lh_gl_fill_set_radius(self, 0);
    lh_gl_fill_set_shown(self, lh_bool_true);
    lh_gl_fill_set_target(self, lh_bool_true);
    lh_gl_fill_set_color(self, color);
}

lh_void
lh_gl_fill_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas)
{
    const lh_gl_fill_t *fill = lh_ptr_ccast(lh_gl_fill_t, entity);
    const lh_gl_color_t *color = lh_gl_fill_get_color(fill);

    if (lh_gl_fill_get_shown(fill) == lh_bool_false || lh_ptr_is_null(color))
    {
        return;
    }
    if (lh_gl_fill_get_radius(fill) > 0)
    {
        const lh_gl_rect_t *area = lh_gl_entity_get_rect(entity);
        lh_gl_scalar_t height = lh_gl_size_get_height(lh_gl_rect_get_size(area));
        lh_gl_rect_t local;

        lh_gl_rect_init(lh_addr_of(local), 0, 0, lh_gl_size_get_width(lh_gl_rect_get_size(area)),
                        height);
        for (lh_gl_scalar_t y = 0; y < height; ++y)
        {
            lh_gl_scalar_t x0;
            lh_gl_scalar_t x1;

            if (lh_gl_cover_round_rect_span(lh_addr_of(local), lh_gl_fill_get_radius(fill), y,
                                            lh_addr_of(x0), lh_addr_of(x1)) == lh_bool_false)
            {
                continue;
            }
            lh_gl_rect_t row;

            lh_gl_rect_init(lh_addr_of(row), x0, y, x1 - x0, 1);
            lh_gl_canvas_fill_rect(canvas, lh_addr_of(row), color);
        }
        return;
    }
    const lh_gl_rect_t *area = lh_gl_entity_get_rect(entity);
    lh_gl_rect_t local;

    lh_gl_rect_init(lh_addr_of(local), 0, 0, lh_gl_size_get_width(lh_gl_rect_get_size(area)),
                    lh_gl_size_get_height(lh_gl_rect_get_size(area)));
    lh_gl_canvas_fill_rect(canvas, lh_addr_of(local), color);
}

lh_bool_t
lh_gl_fill_pick(const lh_gl_entity_t *entity, lh_gl_point_t point, lh_gl_scalar_t slop)
{
    const lh_gl_fill_t *fill = lh_ptr_ccast(lh_gl_fill_t, entity);

    if (lh_gl_fill_get_shown(fill) == lh_bool_false || lh_gl_fill_get_target(fill) == lh_bool_false)
    {
        return lh_bool_false;
    }
    return lh_gl_entity_hit(entity, lh_gl_fill_get_radius(fill), point, slop);
}
