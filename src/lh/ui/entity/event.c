/**
 * @file event.c
 * @brief Implementation of `lh/ui/entity/event.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/canvas.h>
#include <lh/ui/entity/event.h>
#include <lh/ui/entity/transform.h>
#include <lh/ui/key.h>
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

lh_bool_t
lh_ui_entity_event_is_pointer(const lh_ui_entity_event_t *self)
{
    const lh_ui_entity_event_code_t code = lh_ui_entity_event_get_code(self);

    return code == lh_ui_entity_event_click || code == lh_ui_entity_event_press ||
                   code == lh_ui_entity_event_release
               ? lh_bool_true
               : lh_bool_false;
}

lh_bool_t *
lh_ui_entity_event_get_focusable(const lh_ui_entity_event_t *self)
{
    lh_bool_t *focusable;

    lh_assert_runtime_ifn(lh_ui_entity_event_get_code(self) == lh_ui_entity_event_focusable,
                          lh_runtime_error_code_invalid_argument);
    focusable = lh_ptr_rcast(lh_bool_t, self->context);
    lh_assert_runtime_ref(focusable);
    return focusable;
}

lh_bool_t *
lh_ui_entity_event_get_clickable(const lh_ui_entity_event_t *self)
{
    lh_bool_t *clickable;

    lh_assert_runtime_ifn(lh_ui_entity_event_get_code(self) == lh_ui_entity_event_clickable,
                          lh_runtime_error_code_invalid_argument);
    clickable = lh_ptr_rcast(lh_bool_t, self->context);
    lh_assert_runtime_ref(clickable);
    return clickable;
}

const struct lh_ui_key_input *
lh_ui_entity_event_get_key(const lh_ui_entity_event_t *self)
{
    const struct lh_ui_key_input *input;

    lh_assert_runtime_ifn(lh_ui_entity_event_get_code(self) == lh_ui_entity_event_key,
                          lh_runtime_error_code_invalid_argument);
    input = lh_ptr_rcast(const struct lh_ui_key_input, self->context);
    lh_assert_runtime_ref(input);
    return input;
}

lh_ui_point_t
lh_ui_entity_event_get_point(const lh_ui_entity_event_t *self)
{
    const lh_ui_point_t *point;

    lh_assert_runtime_ifn(lh_ui_entity_event_is_pointer(self), lh_runtime_error_code_invalid_argument);
    point = lh_ptr_rcast(const lh_ui_point_t, self->context);
    lh_assert_runtime_ref(point);
    return *point;
}

struct lh_ui_entity_transform *
lh_ui_entity_event_get_transform(const lh_ui_entity_event_t *self)
{
    lh_ui_entity_transform_t *transform;

    lh_assert_runtime_ifn(lh_ui_entity_event_get_code(self) == lh_ui_entity_event_children,
                          lh_runtime_error_code_invalid_argument);
    transform = lh_ptr_rcast(lh_ui_entity_transform_t, self->context);
    lh_assert_runtime_ref(transform);
    return transform;
}

lh_bool_t *
lh_ui_entity_event_get_visible(const lh_ui_entity_event_t *self)
{
    lh_bool_t *visible;

    lh_assert_runtime_ifn(lh_ui_entity_event_get_code(self) == lh_ui_entity_event_visible,
                          lh_runtime_error_code_invalid_argument);
    visible = lh_ptr_rcast(lh_bool_t, self->context);
    lh_assert_runtime_ref(visible);
    return visible;
}

lh_ui_rect_t *
lh_ui_entity_event_get_bounds(const lh_ui_entity_event_t *self)
{
    lh_ui_rect_t *bounds;

    lh_assert_runtime_ifn(lh_ui_entity_event_get_code(self) == lh_ui_entity_event_measure,
                          lh_runtime_error_code_invalid_argument);
    bounds = lh_ptr_rcast(lh_ui_rect_t, self->context);
    lh_assert_runtime_ref(bounds);
    return bounds;
}

lh_ui_scalar_t *
lh_ui_entity_event_get_baseline(const lh_ui_entity_event_t *self)
{
    lh_ui_scalar_t *baseline;

    lh_assert_runtime_ifn(lh_ui_entity_event_get_code(self) == lh_ui_entity_event_baseline,
                          lh_runtime_error_code_invalid_argument);
    baseline = lh_ptr_rcast(lh_ui_scalar_t, self->context);
    lh_assert_runtime_ref(baseline);
    return baseline;
}
