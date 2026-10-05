/**
 * @file entity.c
 * @brief Implementation of `lh/entity.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/entity.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_void
lh_entity_init(lh_entity_t *self, lh_math_rect_t rect)
{
    lh_assert_runtime_ref(self);
    self->rect = rect;
    self->class_p = lh_addr_of(lh_entity_class);
}

lh_math_rect_t
lh_entity_get_rect(const lh_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rect;
}

lh_void
lh_entity_set_rect(lh_entity_t *self, lh_math_rect_t rect)
{
    lh_assert_runtime_ref(self);
    self->rect = rect;
}

const lh_entity_class_t *
lh_entity_get_class(const lh_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->class_p;
}

lh_void
lh_entity_set_class(lh_entity_t *self, const lh_entity_class_t *class_p)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(class_p);
    lh_assert_runtime_ref(class_p->event);
    self->class_p = class_p;
}

lh_void
lh_entity_face_rect(const struct lh_entity *self, const lh_entity_event_t *event)
{
    lh_math_rect_t rect;
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(event);
    lh_return_if(lh_entity_event_get_code(event) != lh_entity_event_draw);
    rect = lh_entity_get_rect(self);
    lh_ui_canvas_fill_rect(lh_entity_event_get_canvas(event), rect, lh_entity_event_get_brush(event));
    lh_ui_canvas_stroke_rect(lh_entity_event_get_canvas(event), rect, lh_entity_event_get_pen(event));
}

lh_void
lh_entity_draw(const lh_entity_t *self, lh_ui_canvas_t *canvas, const lh_ui_brush_t *brush, const lh_ui_pen_t *pen)
{
    lh_entity_event_t event;
    const lh_entity_class_t *class_p;
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(canvas);
    lh_assert_runtime_ref(brush);
    lh_assert_runtime_ref(pen);
    class_p = lh_entity_get_class(self);
    lh_assert_runtime_ref(class_p);
    lh_assert_runtime_ref(class_p->event);
    event.code = lh_entity_event_draw;
    event.canvas = canvas;
    event.brush = brush;
    event.pen = pen;
    class_p->event(self, lh_addr_of(event));
}
