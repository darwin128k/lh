/**
 * @file glass.c
 * @brief A panel that blurs the pixmap behind it and paints a tint.
 */

#include <lh/gl/glass.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/blur.h>
#include <lh/gl/canvas.h>
#include <lh/gl/entity/class/initializer.h>

lh_gl_entity_t *
lh_gl_glass_get_entity_mut(lh_gl_glass_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_entity_t *
lh_gl_glass_get_entity(const lh_gl_glass_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_color_t *
lh_gl_glass_get_tint(const lh_gl_glass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->tint;
}

lh_void
lh_gl_glass_set_tint(lh_gl_glass_t *self, const lh_gl_color_t *tint)
{
    lh_assert_runtime_ref(self);
    self->tint = tint;
    lh_gl_entity_invalidate(lh_gl_glass_get_entity_mut(self));
}

lh_gl_color_t *
lh_gl_glass_get_scratch(const lh_gl_glass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scratch;
}

lh_usize_t
lh_gl_glass_get_scratch_count(const lh_gl_glass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scratch_count;
}

lh_void
lh_gl_glass_set_scratch(lh_gl_glass_t *self, lh_gl_color_t *scratch, lh_usize_t count)
{
    lh_assert_runtime_ref(self);
    self->scratch = scratch;
    self->scratch_count = count;
    lh_gl_entity_invalidate(lh_gl_glass_get_entity_mut(self));
}

lh_gl_scalar_t
lh_gl_glass_get_radius(const lh_gl_glass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_gl_glass_set_radius(lh_gl_glass_t *self, lh_gl_scalar_t radius)
{
    lh_assert_runtime_ref(self);
    self->radius = radius < 0 ? 0 : radius;
    lh_gl_entity_invalidate(lh_gl_glass_get_entity_mut(self));
}

lh_gl_scalar_t
lh_gl_glass_get_blur(const lh_gl_glass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->blur;
}

lh_void
lh_gl_glass_set_blur(lh_gl_glass_t *self, lh_gl_scalar_t blur)
{
    lh_assert_runtime_ref(self);
    self->blur = blur < 0 ? 0 : blur;
    lh_gl_entity_invalidate(lh_gl_glass_get_entity_mut(self));
}

lh_bool_t
lh_gl_glass_get_shown(const lh_gl_glass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shown;
}

lh_void
lh_gl_glass_set_shown(lh_gl_glass_t *self, lh_bool_t shown)
{
    lh_assert_runtime_ref(self);
    self->shown = shown;
    lh_gl_entity_invalidate(lh_gl_glass_get_entity_mut(self));
}

lh_bool_t
lh_gl_glass_get_target(const lh_gl_glass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->target;
}

lh_void
lh_gl_glass_set_target(lh_gl_glass_t *self, lh_bool_t target)
{
    lh_assert_runtime_ref(self);
    self->target = target;
}

const lh_gl_entity_class_t *
lh_gl_glass_get_class(void)
{
    static const lh_gl_entity_class_t glass_class = lh_gl_entity_class_initializer(
        lh_gl_entity_class_kind_entity, lh_gl_glass_draw, lh_gl_glass_pick, lh_bool_false);

    return lh_addr_of(glass_class);
}

lh_void
lh_gl_glass_init(lh_gl_glass_t *self, lh_gl_rect_t rect, const lh_gl_color_t *tint,
                 lh_gl_color_t *scratch, lh_usize_t scratch_count)
{
    lh_gl_entity_init(lh_gl_glass_get_entity_mut(self), rect, lh_gl_glass_get_class());
    lh_gl_glass_set_radius(self, 0);
    lh_gl_glass_set_blur(self, 0);
    lh_gl_glass_set_shown(self, lh_bool_true);
    lh_gl_glass_set_target(self, lh_bool_true);
    lh_gl_glass_set_scratch(self, scratch, scratch_count);
    lh_gl_glass_set_tint(self, tint);
}

lh_void
lh_gl_glass_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas)
{
    const lh_gl_glass_t *glass = lh_ptr_ccast(lh_gl_glass_t, entity);
    const lh_gl_rect_t *area = lh_gl_entity_get_rect(entity);
    const lh_gl_point_t *origin = lh_gl_canvas_get_origin(canvas);

    if (lh_gl_glass_get_shown(glass) == lh_bool_false)
    {
        return;
    }
    lh_gl_rect_t panel;

    lh_gl_rect_init(lh_addr_of(panel), lh_gl_point_get_x(origin), lh_gl_point_get_y(origin),
                    lh_gl_size_get_width(lh_gl_rect_get_size(area)),
                    lh_gl_size_get_height(lh_gl_rect_get_size(area)));
    lh_usize_t need = lh_gl_blur_scratch_count(lh_addr_of(panel), lh_gl_glass_get_blur(glass));
    if (lh_gl_glass_get_blur(glass) > 0 &&
        (lh_ptr_is_null(lh_gl_glass_get_scratch(glass)) ||
         lh_gl_glass_get_scratch_count(glass) < need))
    {
        return;
    }
    lh_gl_glass(lh_ptr_cast(lh_gl_pixmap_t, lh_gl_canvas_get_target(canvas)), lh_addr_of(panel),
                lh_gl_glass_get_radius(glass), lh_gl_glass_get_blur(glass),
                lh_gl_glass_get_tint(glass), lh_gl_glass_get_scratch(glass),
                lh_gl_canvas_get_clip(canvas));
}

lh_bool_t
lh_gl_glass_pick(const lh_gl_entity_t *entity, lh_gl_point_t point, lh_gl_scalar_t slop)
{
    const lh_gl_glass_t *glass = lh_ptr_ccast(lh_gl_glass_t, entity);

    if (lh_gl_glass_get_shown(glass) == lh_bool_false ||
        lh_gl_glass_get_target(glass) == lh_bool_false)
    {
        return lh_bool_false;
    }
    return lh_gl_entity_hit(entity, lh_gl_glass_get_radius(glass), point, slop);
}
