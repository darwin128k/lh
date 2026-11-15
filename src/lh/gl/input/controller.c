/**
 * @file controller.c
 * @brief Feed a controller's events to one held entity.
 */
#include <lh/gl/input/controller.h>

#include <lh/core/assert/runtime.h>

lh_gl_input_t *
lh_gl_input_binding_get_input(const lh_gl_input_binding_t *self)
{
    lh_assert_runtime_ref(self);
    return self->input;
}

lh_void
lh_gl_input_binding_set_input(lh_gl_input_binding_t *self, lh_gl_input_t *input)
{
    lh_assert_runtime_ref(self);
    self->input = input;
}

const lh_gl_layer_t *
lh_gl_input_binding_get_layer(const lh_gl_input_binding_t *self)
{
    lh_assert_runtime_ref(self);
    return self->layer;
}

lh_void
lh_gl_input_binding_set_layer(lh_gl_input_binding_t *self, const lh_gl_layer_t *layer)
{
    lh_assert_runtime_ref(self);
    self->layer = layer;
}

lh_void
lh_gl_input_binding_init(lh_gl_input_binding_t *self, lh_gl_input_t *input,
                         const lh_gl_layer_t *layer)
{
    lh_gl_input_binding_set_input(self, input);
    lh_gl_input_binding_set_layer(self, layer);
}

lh_bool_t
lh_gl_input_controller_handle_event_is_handled(lh_self_ptr self, const lh_os_input_event_ptr event)
{
    lh_gl_input_binding_t *binding = lh_ptr_rcast(lh_gl_input_binding_t, self);
    lh_os_input_kind_t kind;
    const lh_gl_entity_t *found;

    lh_assert_runtime_ref(binding);
    lh_assert_runtime_ref(event);
    if (lh_ptr_is_null(lh_gl_input_binding_get_input(binding)) ||
        lh_ptr_is_null(lh_gl_input_binding_get_layer(binding)))
    {
        return lh_bool_false;
    }
    kind = lh_os_input_event_get_kind(event);
    if (kind == lh_os_input_kind_pointer_down || kind == lh_os_input_kind_pointer_move ||
        kind == lh_os_input_kind_pointer_up || kind == lh_os_input_kind_pointer_cancel)
    {
        found = lh_gl_input_apply(lh_gl_input_binding_get_input(binding),
                                  lh_gl_input_binding_get_layer(binding), event);
        return lh_type_cast(lh_bool_t, lh_ptr_is_set(found));
    }
    if (kind == lh_os_input_kind_click || kind == lh_os_input_kind_double_click ||
        kind == lh_os_input_kind_long_press || kind == lh_os_input_kind_drag_begin ||
        kind == lh_os_input_kind_drag || kind == lh_os_input_kind_drag_end ||
        kind == lh_os_input_kind_swipe)
    {
        return lh_type_cast(
            lh_bool_t, lh_ptr_is_set(lh_gl_input_get_captured(lh_gl_input_binding_get_input(binding))));
    }
    return lh_bool_false;
}

lh_void
lh_gl_input_controller_cancel(lh_self_ptr self)
{
    lh_gl_input_binding_t *binding = lh_ptr_rcast(lh_gl_input_binding_t, self);

    lh_assert_runtime_ref(binding);
    if (lh_ptr_is_set(lh_gl_input_binding_get_input(binding)))
    {
        lh_gl_input_release(lh_gl_input_binding_get_input(binding));
    }
}
