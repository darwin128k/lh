/**
 * @file fill.c
 * @brief Paint an entity rectangle with a borrowed color.
 */
#include <lh/gl/fill.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/canvas.h>
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

const lh_gl_entity_class_t *
lh_gl_fill_get_class(void)
{
    static const lh_gl_entity_class_t fill_class = lh_gl_entity_class_initializer(
        lh_gl_entity_class_kind_entity, lh_gl_fill_draw, lh_bool_false);

    return lh_addr_of(fill_class);
}

lh_void
lh_gl_fill_init(lh_gl_fill_t *self, lh_gl_rect_t rect, const lh_gl_color_t *color)
{
    lh_gl_entity_init(lh_gl_fill_get_entity_mut(self), rect, lh_gl_fill_get_class());
    lh_gl_fill_set_color(self, color);
}

lh_void
lh_gl_fill_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas)
{
    const lh_gl_color_t *color = lh_gl_fill_get_color(lh_ptr_ccast(lh_gl_fill_t, entity));

    if (lh_ptr_is_null(color))
    {
        return;
    }
    const lh_gl_rect_t *area = lh_gl_entity_get_rect(entity);
    lh_gl_rect_t local;

    lh_gl_rect_init(lh_addr_of(local), 0, 0, lh_gl_size_get_width(lh_gl_rect_get_size(area)),
                    lh_gl_size_get_height(lh_gl_rect_get_size(area)));
    lh_gl_canvas_fill_rect(canvas, lh_addr_of(local), color);
}
