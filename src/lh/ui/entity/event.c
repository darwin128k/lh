/**
 * @file event.c
 * @brief Implementation of `lh/ui/entity/event.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/canvas.h>
#include <lh/ui/entity/event.h>
#include <lh/ui/point.h>
#include <lh/util/ptr.h>

lh_void
lh_ui_entity_event_init(lh_ui_entity_event_t *self, lh_ui_entity_event_code_t code, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->code = code;
    self->context = context;
}

lh_ui_entity_event_code_t
lh_ui_entity_event_get_code(const lh_ui_entity_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->code;
}

lh_ptr
lh_ui_entity_event_get_context(const lh_ui_entity_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

struct lh_ui_canvas *
lh_ui_entity_event_get_canvas(const lh_ui_entity_event_t *self)
{
    lh_assert_runtime_ifn(lh_ui_entity_event_get_code(self) == lh_ui_entity_event_draw,
                          lh_runtime_error_code_invalid_argument);
    return lh_ptr_rcast(lh_ui_canvas_t, self->context);
}

lh_ui_point_t
lh_ui_entity_event_get_point(const lh_ui_entity_event_t *self)
{
    const lh_ui_point_t *point;

    lh_assert_runtime_ifn(lh_ui_entity_event_get_code(self) == lh_ui_entity_event_click,
                          lh_runtime_error_code_invalid_argument);
    point = lh_ptr_rcast(const lh_ui_point_t, self->context);
    lh_assert_runtime_ref(point);
    return *point;
}
