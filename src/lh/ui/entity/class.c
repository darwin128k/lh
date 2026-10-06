/**
 * @file class.c
 * @brief Implementation of `lh/ui/entity/class.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/entity.h>
#include <lh/ui/entity/class.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_void
lh_ui_entity_class_fill(const struct lh_ui_entity *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_color_t *color = lh_ui_entity_get_fill_color(self);
    lh_ui_rect_t rect;
    lh_return_if(lh_null_eq(canvas) || lh_null_eq(color));
    rect = lh_ui_entity_get_rect(self);
    /* A fill color implies a style; its radius is clamped by the canvas. */
    lh_ui_canvas_fill_round_rect(canvas, lh_addr_of(rect), lh_ui_style_get_radius(lh_ui_entity_get_style(self)),
                                 color);
}

lh_void
lh_ui_entity_class_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_draw);
    lh_ui_entity_class_fill(self, lh_ui_entity_event_get_canvas(event));
}

const lh_ui_entity_class_t lh_ui_entity_class = {lh_ui_entity_class_event, lh_null};

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
