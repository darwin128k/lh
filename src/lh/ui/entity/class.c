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

static lh_void
lh_ui_entity_class_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    lh_ui_canvas_t *canvas;
    const lh_ui_color_t *color;
    lh_ui_rect_t rect;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(event);
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_draw);
    canvas = lh_ui_entity_event_get_canvas(event);
    lh_return_if(lh_null_eq(canvas));
    color = lh_ui_entity_get_fill_color(self);
    lh_return_if(lh_null_eq(color));
    rect = lh_ui_entity_get_rect(self);
    lh_ui_canvas_fill_rect(canvas, lh_addr_of(rect), color);
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
