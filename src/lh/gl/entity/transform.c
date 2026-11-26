/**
 * @file transform.c
 * @brief Implementation of `lh/gl/entity/transform.h`.
 */
#include <lh/core/assert.h>

#include <lh/gl/entity/transform.h>

lh_void
lh_gl_entity_transform_set_offset(lh_gl_entity_transform_t *self, lh_gl_point_t offset)
{
    lh_assert_runtime_ref(self);
    self->offset = offset;
}

lh_bool_t
lh_gl_entity_transform_is_clip(const lh_gl_entity_transform_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clip;
}

lh_void
lh_gl_entity_transform_set_clip(lh_gl_entity_transform_t *self, lh_bool_t clip)
{
    lh_assert_runtime_ref(self);
    self->clip = clip;
}

lh_gl_point_t *
lh_gl_entity_transform_get_offset_mut(lh_gl_entity_transform_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->offset);
}

const lh_gl_point_t *
lh_gl_entity_transform_get_offset(const lh_gl_entity_transform_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->offset);
}

lh_void
lh_gl_entity_transform_init(lh_gl_entity_transform_t *self)
{
    lh_gl_point_init(lh_gl_entity_transform_get_offset_mut(self), lh_gl_scalar(0), lh_gl_scalar(0));
    lh_gl_entity_transform_set_clip(self, lh_bool_false);
}

lh_void
lh_gl_entity_transform_copy_offset(const lh_gl_entity_transform_t *self, lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_gl_entity_transform_get_offset(self));
}

lh_bool_t
lh_gl_entity_transform_is_identity(const lh_gl_entity_transform_t *self)
{
    return !lh_gl_entity_transform_is_clip(self) &&
                   lh_gl_point_get_x(lh_gl_entity_transform_get_offset(self)) == lh_gl_scalar(0) &&
                   lh_gl_point_get_y(lh_gl_entity_transform_get_offset(self)) == lh_gl_scalar(0)
               ? lh_bool_true
               : lh_bool_false;
}
