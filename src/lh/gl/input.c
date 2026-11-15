/**
 * @file input.c
 * @brief One held entity for a press.
 */
#include <lh/gl/input.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/layer.h>

const lh_gl_entity_t *
lh_gl_input_get_captured(const lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->captured;
}

lh_void
lh_gl_input_set_captured(lh_gl_input_t *self, const lh_gl_entity_t *captured)
{
    lh_assert_runtime_ref(self);
    self->captured = captured;
}

lh_u8_t
lh_gl_input_get_pointer(const lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pointer;
}

lh_void
lh_gl_input_set_pointer(lh_gl_input_t *self, lh_u8_t pointer)
{
    lh_assert_runtime_ref(self);
    self->pointer = pointer;
}

lh_bool_t
lh_gl_input_get_tracking(const lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->tracking;
}

lh_void
lh_gl_input_set_tracking(lh_gl_input_t *self, lh_bool_t tracking)
{
    lh_assert_runtime_ref(self);
    self->tracking = tracking;
}

lh_gl_scalar_t
lh_gl_input_get_slop(const lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->slop;
}

lh_void
lh_gl_input_set_slop(lh_gl_input_t *self, lh_gl_scalar_t slop)
{
    lh_assert_runtime_ref(self);
    self->slop = slop < 0 ? 0 : slop;
}

lh_void
lh_gl_input_init(lh_gl_input_t *self)
{
    lh_gl_input_set_captured(self, lh_null);
    lh_gl_input_set_pointer(self, 0U);
    lh_gl_input_set_tracking(self, lh_bool_false);
    lh_gl_input_set_slop(self, 0);
}

const lh_gl_entity_t *
lh_gl_input_press(lh_gl_input_t *self, const lh_gl_layer_t *layer, lh_gl_point_t point)
{
    const lh_gl_entity_t *entity =
        lh_gl_layer_find_entity_slop(layer, point, lh_gl_input_get_slop(self));

    lh_gl_input_set_captured(self, entity);
    lh_gl_input_set_tracking(self, lh_bool_false);
    return entity;
}

const lh_gl_entity_t *
lh_gl_input_move(const lh_gl_input_t *self)
{
    return lh_gl_input_get_captured(self);
}

const lh_gl_entity_t *
lh_gl_input_release(lh_gl_input_t *self)
{
    const lh_gl_entity_t *captured = lh_gl_input_get_captured(self);

    lh_gl_input_set_captured(self, lh_null);
    lh_gl_input_set_tracking(self, lh_bool_false);
    return captured;
}
