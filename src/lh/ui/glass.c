#include <lh/ui/glass.h>
#include <lh/assert.h>
#include <lh/ui/blur.h>
#include <lh/util/ptr.h>

lh_void
lh_ui_glass_draw(const lh_ui_effect_t *self, lh_ui_canvas_t *canvas, lh_math_rect_t box,
                 lh_int_t corner)
{
    const lh_ui_glass_t *const glass = lh_ptr_rcast(const lh_ui_glass_t, self);
    (void)corner;
    lh_ui_blur_apply(canvas, box, glass->radius);
    lh_ui_canvas_fill_rect(canvas, box, glass->tint);
}

lh_int_t
lh_ui_glass_outset(const lh_ui_effect_t *self)
{
    (void)self;
    return 0;
}

const lh_ui_effect_class_t lh_ui_glass_class = {lh_ui_glass_draw, lh_ui_glass_outset};

lh_void
lh_ui_glass_init(lh_ui_glass_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_effect_init(lh_ptr_rcast(lh_ui_effect_t, self), lh_addr_of(lh_ui_glass_class));
    self->radius = 0;
    self->tint = lh_ui_color_make(0, 0, 0, 0);
}

const lh_ui_effect_t *
lh_ui_glass_effect(const lh_ui_glass_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ptr_rcast(const lh_ui_effect_t, self);
}

lh_int_t
lh_ui_glass_get_radius(const lh_ui_glass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_ui_glass_set_radius(lh_ui_glass_t *self, lh_int_t radius)
{
    lh_assert_runtime_ref(self);
    self->radius = radius < 0 ? 0 : radius;
}

lh_ui_color_t
lh_ui_glass_get_tint(const lh_ui_glass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->tint;
}

lh_void
lh_ui_glass_set_tint(lh_ui_glass_t *self, lh_ui_color_t tint)
{
    lh_assert_runtime_ref(self);
    self->tint = tint;
}
