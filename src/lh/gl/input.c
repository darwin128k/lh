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

lh_void
lh_gl_input_init(lh_gl_input_t *self)
{
    lh_gl_input_set_captured(self, lh_null);
}

const lh_gl_entity_t *
lh_gl_input_press(lh_gl_input_t *self, const lh_gl_layer_t *layer, lh_gl_point_t point)
{
    const lh_gl_entity_t *entity = lh_gl_layer_find_entity(layer, point);

    lh_gl_input_set_captured(self, entity);
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
    return captured;
}
