#include <lh/ui/effect.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/util/ptr.h>

lh_void
lh_ui_effect_init(lh_ui_effect_t *self, const lh_ui_effect_class_t *effect_class)
{
    lh_assert_runtime_ref(self);
    self->class_ptr = effect_class;
    self->next = lh_null;
}

const lh_ui_effect_t *
lh_ui_effect_get_next(const lh_ui_effect_t *self)
{
    lh_assert_runtime_ref(self);
    return self->next;
}

lh_void
lh_ui_effect_set_next(lh_ui_effect_t *self, const lh_ui_effect_t *next)
{
    lh_assert_runtime_ref(self);
    self->next = next;
}

lh_void
lh_ui_effect_draw(const lh_ui_effect_t *self, lh_ui_canvas_t *canvas, lh_math_rect_t box,
                  lh_int_t corner)
{
    const lh_ui_effect_t *effect = self;
    lh_assert_runtime_ref(canvas);
    while (lh_ptr_is_set(effect))
    {
        if (lh_ptr_is_set(effect->class_ptr) && lh_ptr_is_set(effect->class_ptr->draw))
        {
            effect->class_ptr->draw(effect, canvas, box, corner);
        }
        effect = effect->next;
    }
}

lh_int_t
lh_ui_effect_outset(const lh_ui_effect_t *self)
{
    const lh_ui_effect_t *effect = self;
    lh_int_t farthest = 0;
    while (lh_ptr_is_set(effect))
    {
        if (lh_ptr_is_set(effect->class_ptr) && lh_ptr_is_set(effect->class_ptr->outset))
        {
            const lh_int_t outset = effect->class_ptr->outset(effect);
            if (outset > farthest)
            {
                farthest = outset;
            }
        }
        effect = effect->next;
    }
    return farthest;
}
