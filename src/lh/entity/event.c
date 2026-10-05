/**
 * @file event.c
 * @brief Implementation of `lh/entity/event.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/entity/event.h>

lh_entity_event_code_t
lh_entity_event_get_code(const lh_entity_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->code;
}

lh_ui_canvas_t *
lh_entity_event_get_canvas(const lh_entity_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->canvas;
}

const lh_ui_brush_t *
lh_entity_event_get_brush(const lh_entity_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->brush;
}

const lh_ui_pen_t *
lh_entity_event_get_pen(const lh_entity_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pen;
}
