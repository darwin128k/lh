/**
 * @file transform.c
 * @brief Implementation of `lh/ui/entity/transform.h`.
 */
#include <lh/assert.h>

#include <lh/ui/entity/transform.h>

lh_void
lh_ui_entity_transform_init(lh_ui_entity_transform_t *self)
{
    lh_ui_point_init(lh_ui_entity_transform_get_offset_mut(self), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_entity_transform_set_clip(self, lh_bool_false);
}

lh_void
lh_ui_entity_transform_copy_offset(const lh_ui_entity_transform_t *self, lh_ui_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_ui_entity_transform_get_offset(self));
}

lh_void
lh_ui_entity_transform_set_offset(lh_ui_entity_transform_t *self, lh_ui_point_t offset)
{
    lh_assert_runtime_ref(self);
    self->offset = offset;
}

lh_bool_t
lh_ui_entity_transform_is_clip(const lh_ui_entity_transform_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clip;
}

lh_void
lh_ui_entity_transform_set_clip(lh_ui_entity_transform_t *self, lh_bool_t clip)
{
    lh_assert_runtime_ref(self);
    self->clip = clip;
}

lh_bool_t
lh_ui_entity_transform_is_identity(const lh_ui_entity_transform_t *self)
{
    return !lh_ui_entity_transform_is_clip(self) &&
                   lh_ui_point_get_x(lh_ui_entity_transform_get_offset(self)) == lh_ui_scalar(0) &&
                   lh_ui_point_get_y(lh_ui_entity_transform_get_offset(self)) == lh_ui_scalar(0)
               ? lh_bool_true
               : lh_bool_false;
}

lh_ui_point_t *
lh_ui_entity_transform_get_offset_mut(lh_ui_entity_transform_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->offset);
}

const lh_ui_point_t *
lh_ui_entity_transform_get_offset(const lh_ui_entity_transform_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->offset);
}
