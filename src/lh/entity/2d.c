#include <lh/entity/2d.h>
#include <lh/entity/screen.h>
#include <lh/assert.h>
#include <lh/cast/const.h>
#include <lh/null.h>
#include <lh/math/quat.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

static lh_void
lh_entity_2d_construct(lh_entity_t *self)
{
    /* The memory is zeroed: position and angle are already 0. */
    lh_entity_2d_set_scale(lh_ptr_rcast(lh_entity_2d_t, self), lh_math_vec2_make(1.0f, 1.0f));
}

static lh_void
lh_entity_2d_event(lh_entity_t *self, lh_entity_event_t *event)
{
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_GET_LOCAL_MATRIX)
    {
        return;
    }

    const lh_entity_2d_t *const entity = lh_ptr_rcast(const lh_entity_2d_t, self);
    const lh_math_vec2_t position = lh_entity_2d_get_position(entity);
    const lh_math_vec2_t scale = lh_entity_2d_get_scale(entity);
    const lh_math_mat4_t rotate = lh_math_mat4_from_quat(
        lh_math_quat_from_axis_angle(lh_math_vec3_make(0.0f, 0.0f, 1.0f), lh_entity_2d_get_angle(entity)));

    *lh_ptr_rcast(lh_math_mat4_t, lh_entity_event_get_param(event)) =
        lh_math_mat4_mul(lh_math_mat4_from_translation(lh_math_vec3_make(lh_math_vec2_get_x(lh_addr_of(position)), lh_math_vec2_get_y(lh_addr_of(position)), 0.0f)),
                    lh_math_mat4_mul(rotate, lh_math_mat4_from_scale(lh_math_vec3_make(lh_math_vec2_get_x(lh_addr_of(scale)), lh_math_vec2_get_y(lh_addr_of(scale)), 1.0f))));
    lh_entity_event_stop(event);
}

const lh_entity_class_t lh_entity_2d_class =
    lh_entity_class_initializer(&lh_entity_base_class, sizeof(lh_entity_2d_t),
                                lh_entity_2d_construct, lh_null, lh_entity_2d_event);

lh_math_vec2_t
lh_entity_2d_get_position(const lh_entity_2d_t *self)
{
    lh_assert_runtime_ref(self);
    return self->position;
}

lh_void
lh_entity_2d_set_position(lh_entity_2d_t *self, lh_math_vec2_t position)
{
    lh_assert_runtime_ref(self);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self)); /* where it was */
    self->position = position;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self)); /* where it is now */
}

lh_float_t
lh_entity_2d_get_angle(const lh_entity_2d_t *self)
{
    lh_assert_runtime_ref(self);
    return self->angle;
}

lh_void
lh_entity_2d_set_angle(lh_entity_2d_t *self, lh_float_t angle)
{
    lh_assert_runtime_ref(self);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self)); /* where it was */
    self->angle = angle;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self)); /* where it is now */
}

lh_math_vec2_t
lh_entity_2d_get_scale(const lh_entity_2d_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scale;
}

lh_void
lh_entity_2d_set_scale(lh_entity_2d_t *self, lh_math_vec2_t scale)
{
    lh_assert_runtime_ref(self);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self)); /* where it was */
    self->scale = scale;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self)); /* where it is now */
}

lh_math_mat4_t
lh_entity_2d_get_local_matrix(const lh_entity_2d_t *self)
{
    /* Asked as an event so the most derived spatial class answers. */
    lh_math_mat4_t local = lh_math_mat4_identity();
    lh_entity_notify(lh_cast_const(lh_entity_t *, lh_ptr_rcast(const lh_entity_t, self)),
                     LH_ENTITY_EVENT_GET_LOCAL_MATRIX, lh_addr_of(local));
    return local;
}

lh_math_mat4_t
lh_entity_2d_get_world_matrix(const lh_entity_2d_t *self)
{
    lh_math_mat4_t world = lh_entity_2d_get_local_matrix(self);
    for (const lh_entity_t *ancestor = lh_entity_get_parent(lh_ptr_rcast(const lh_entity_t, self));
         lh_ptr_is_set(ancestor); ancestor = lh_entity_get_parent(ancestor))
    {
        const lh_entity_2d_t *const spatial =
            lh_entity_cast(ancestor, lh_addr_of(lh_entity_2d_class));
        if (lh_ptr_is_set(spatial))
        {
            world = lh_math_mat4_mul(lh_entity_2d_get_local_matrix(spatial), world);
        }
    }
    return world;
}
