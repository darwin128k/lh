/**
 * @file class.c
 * @brief Implementation of `lh/ui/entity/class.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/entity.h>
#include <lh/ui/entity/class.h>
#include <lh/ui/shadow.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_void
lh_ui_entity_class_fill(const struct lh_ui_entity *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_color_t *color = lh_ui_entity_get_fill_color(self);
    lh_ui_rect_t rect;
    lh_return_if(lh_null_eq(canvas) || lh_null_eq(color));
    rect = lh_ui_entity_get_rect(self);
    /* A fill color implies a style; its radius is clamped by the canvas. It is
       the radius in force, which is also what a clipping parent cuts its
       children with — one answer, so the fill and the clip cannot disagree. */
    lh_ui_canvas_fill_round_rect(canvas, lh_addr_of(rect), lh_ui_entity_get_radius_now(self), color);
}

lh_void
lh_ui_entity_class_shadow(const struct lh_ui_entity *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_style_t *style;
    lh_ui_rect_t rect;

    lh_return_if(lh_null_eq(canvas) || lh_null_eq(lh_ui_entity_get_style_now(self)));
    style = lh_ui_entity_get_style_now(self);
    /* Down first because that is the order of the story: the box casts this,
       then the box is painted. The picture is the same either way — a shadow
       paints nothing inside its own box (::lh_ui_shadow_alpha_at), so there is
       nothing of it left to land on the fill. An empty shadow is the common case
       and costs nothing but this question. */
    lh_return_if(lh_ui_shadow_is_empty(lh_ui_style_get_shadow(style)));
    rect = lh_ui_entity_get_rect(self);
    (void)lh_ui_canvas_shadow(canvas, lh_addr_of(rect), lh_ui_entity_get_radius_now(self),
                              lh_ui_style_get_shadow(style));
}

lh_void
lh_ui_entity_class_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_draw);
    lh_ui_entity_class_shadow(self, lh_ui_entity_event_get_canvas(event));
    lh_ui_entity_class_fill(self, lh_ui_entity_event_get_canvas(event));
}

const lh_ui_entity_class_t lh_ui_entity_class = {lh_ui_entity_class_event, lh_null};

lh_bool_t
lh_ui_entity_class_is(const lh_ui_entity_class_t *kind, const lh_ui_entity_class_t *base)
{
    lh_return_if(lh_null_eq(base), lh_bool_false);
    for (; lh_null_ne(kind); kind = kind->base)
    {
        lh_return_if(kind == base, lh_bool_true);
    }
    return lh_bool_false;
}

lh_void
lh_ui_entity_class_event_base(const lh_ui_entity_class_t *class, const struct lh_ui_entity *self,
                              const lh_ui_entity_event_t *event)
{
    const lh_ui_entity_class_t *base;
    lh_assert_runtime_ref(class);
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(event);
    base = class->base;
    lh_return_if(base == lh_null);
    lh_assert_runtime_ref(base->event);
    base->event(self, event);
}
