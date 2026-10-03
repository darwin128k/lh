#include <lh/entity/3d.h>
#include <lh/entity/screen.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/util/ptr.h>

/* The 2D part of @p self, through which x, y and their scale are kept. */
#define lh_entity_3d_as_2d(self) lh_ptr_rcast(lh_entity_2d_t, (self))
#define lh_entity_3d_as_2d_const(self) lh_ptr_rcast(const lh_entity_2d_t, (self))

static lh_void
lh_entity_3d_construct(lh_entity_t *self)
{
    /* The 2D constructor already set the x and y scale; z is zeroed. */
    lh_entity_3d_t *const entity = lh_ptr_rcast(lh_entity_3d_t, self);
    entity->rotation = lh_math_quat_identity();
    entity->scale_z = 1.0f;
}

static lh_void
lh_entity_3d_event(lh_entity_t *self, lh_entity_event_t *event)
{
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_GET_LOCAL_MATRIX)
    {
        return;
    }

    const lh_entity_3d_t *const entity = lh_ptr_rcast(const lh_entity_3d_t, self);
    const lh_math_mat4_t plane_rotate = lh_math_mat4_from_quat(lh_math_quat_from_axis_angle(
        lh_math_vec3_make(0.0f, 0.0f, 1.0f), lh_entity_2d_get_angle(lh_entity_3d_as_2d_const(entity))));
    const lh_math_mat4_t rotate =
        lh_math_mat4_mul(lh_math_mat4_from_quat(lh_entity_3d_get_rotation(entity)), plane_rotate);

    *lh_ptr_rcast(lh_math_mat4_t, lh_entity_event_get_param(event)) =
        lh_math_mat4_mul(lh_math_mat4_from_translation(lh_entity_3d_get_position(entity)),
                    lh_math_mat4_mul(rotate, lh_math_mat4_from_scale(lh_entity_3d_get_scale(entity))));
    lh_entity_event_stop(event); /* the 2D answer below would drop z and rotation */
}

const lh_entity_class_t lh_entity_3d_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_3d_t), lh_entity_3d_construct,
                                lh_null, lh_entity_3d_event);

lh_math_vec3_t
lh_entity_3d_get_position(const lh_entity_3d_t *self)
{
    lh_assert_runtime_ref(self);
    const lh_math_vec2_t xy = lh_entity_2d_get_position(lh_entity_3d_as_2d_const(self));
    return lh_math_vec2_to_vec3(xy, self->z);
}

lh_void
lh_entity_3d_set_position(lh_entity_3d_t *self, lh_math_vec3_t position)
{
    lh_assert_runtime_ref(self);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
    lh_entity_2d_set_position(lh_entity_3d_as_2d(self), lh_math_vec3_to_vec2(position));
    self->z = position.z;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_math_quat_t
lh_entity_3d_get_rotation(const lh_entity_3d_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rotation;
}

lh_void
lh_entity_3d_set_rotation(lh_entity_3d_t *self, lh_math_quat_t rotation)
{
    lh_assert_runtime_ref(self);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
    self->rotation = rotation;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_math_vec3_t
lh_entity_3d_get_scale(const lh_entity_3d_t *self)
{
    lh_assert_runtime_ref(self);
    const lh_math_vec2_t xy = lh_entity_2d_get_scale(lh_entity_3d_as_2d_const(self));
    return lh_math_vec2_to_vec3(xy, self->scale_z);
}

lh_void
lh_entity_3d_set_scale(lh_entity_3d_t *self, lh_math_vec3_t scale)
{
    lh_assert_runtime_ref(self);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
    lh_entity_2d_set_scale(lh_entity_3d_as_2d(self), lh_math_vec3_to_vec2(scale));
    self->scale_z = scale.z;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}
