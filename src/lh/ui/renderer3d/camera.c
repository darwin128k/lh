#include <lh/ui/renderer3d/camera.h>
#include <lh/assert.h>
#include <lh/math/float/finite.h>
#include <lh/math/float/tan.h>
#include <lh/util/return.h>

lh_bool_t
lh_ui_renderer3d_camera_init(lh_ui_renderer3d_camera_t *self, lh_math_vec3_t position,
                             lh_math_vec3_t forward, lh_math_vec3_t up,
                             lh_math_fscalar_t vertical_fov, lh_math_fscalar_t near_plane,
                             lh_math_fscalar_t far_plane)
{
    lh_return_if(lh_null_eq(self) || !lh_math_vec3_is_finite(position) ||
                     !lh_math_vec3_is_finite(forward) || !lh_math_vec3_is_finite(up) ||
                     !lh_math_float_is_finite(vertical_fov) || vertical_fov < .1f ||
                     vertical_fov > 3.f || !lh_math_float_is_finite(near_plane) ||
                     !lh_math_float_is_finite(far_plane) || near_plane <= 0 ||
                     far_plane <= near_plane,
                 lh_bool_false);
    lh_math_vec3_t unit_forward;
    lh_math_vec3_t unit_right;
    lh_math_vec3_t unit_up;
    lh_return_ifn(lh_math_vec3_try_normalize(forward, 1e-6f, &unit_forward), lh_bool_false);
    lh_math_vec3_cross(unit_forward, up, &unit_right);
    lh_return_ifn(lh_math_vec3_try_normalize(unit_right, 1e-6f, &unit_right), lh_bool_false);
    lh_math_vec3_cross(unit_right, unit_forward, &unit_up);
    lh_ui_renderer3d_camera_set_position(self, position);
    lh_ui_renderer3d_camera_set_forward(self, unit_forward);
    lh_ui_renderer3d_camera_set_right(self, unit_right);
    lh_ui_renderer3d_camera_set_up(self, unit_up);
    lh_ui_renderer3d_camera_set_tangent(self, lh_math_float_tan(vertical_fov * .5f));
    lh_ui_renderer3d_camera_set_near_plane(self, near_plane);
    lh_ui_renderer3d_camera_set_far_plane(self, far_plane);
    return lh_bool_true;
}

lh_bool_t
lh_ui_renderer3d_camera_is_usable(const lh_ui_renderer3d_camera_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_float_is_finite(lh_ui_renderer3d_camera_get_tangent(self)) && lh_ui_renderer3d_camera_get_tangent(self) > 0 &&
           lh_ui_renderer3d_camera_get_near_plane(self) > 0 && lh_ui_renderer3d_camera_get_far_plane(self) > lh_ui_renderer3d_camera_get_near_plane(self);
}

lh_void
lh_ui_renderer3d_camera_to_view(const lh_ui_renderer3d_camera_t *self, lh_math_vec3_t world,
                                lh_math_vec3_t *out)
{
    lh_assert_runtime_ref(self);
    lh_math_vec3_t offset;
    lh_math_vec3_sub(world, lh_ptr_deref(lh_ui_renderer3d_camera_get_position(self)), &offset);
    lh_math_vec3_init(out, lh_math_vec3_dot(offset, lh_ptr_deref(lh_ui_renderer3d_camera_get_right(self))),
                      lh_math_vec3_dot(offset, lh_ptr_deref(lh_ui_renderer3d_camera_get_up(self))), lh_math_vec3_dot(offset, lh_ptr_deref(lh_ui_renderer3d_camera_get_forward(self))));
}

const lh_math_vec3_t *
lh_ui_renderer3d_camera_get_position(const lh_ui_renderer3d_camera_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->position);
}

lh_math_vec3_t *
lh_ui_renderer3d_camera_get_position_mut(lh_ui_renderer3d_camera_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->position);
}

lh_void
lh_ui_renderer3d_camera_set_position(lh_ui_renderer3d_camera_t *self, lh_math_vec3_t position)
{
    lh_assert_runtime_ref(self);
    self->position = position;
}

const lh_math_vec3_t *
lh_ui_renderer3d_camera_get_forward(const lh_ui_renderer3d_camera_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->forward);
}

lh_math_vec3_t *
lh_ui_renderer3d_camera_get_forward_mut(lh_ui_renderer3d_camera_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->forward);
}

lh_void
lh_ui_renderer3d_camera_set_forward(lh_ui_renderer3d_camera_t *self, lh_math_vec3_t forward)
{
    lh_assert_runtime_ref(self);
    self->forward = forward;
}

const lh_math_vec3_t *
lh_ui_renderer3d_camera_get_right(const lh_ui_renderer3d_camera_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->right);
}

lh_math_vec3_t *
lh_ui_renderer3d_camera_get_right_mut(lh_ui_renderer3d_camera_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->right);
}

lh_void
lh_ui_renderer3d_camera_set_right(lh_ui_renderer3d_camera_t *self, lh_math_vec3_t right)
{
    lh_assert_runtime_ref(self);
    self->right = right;
}

const lh_math_vec3_t *
lh_ui_renderer3d_camera_get_up(const lh_ui_renderer3d_camera_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->up);
}

lh_math_vec3_t *
lh_ui_renderer3d_camera_get_up_mut(lh_ui_renderer3d_camera_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->up);
}

lh_void
lh_ui_renderer3d_camera_set_up(lh_ui_renderer3d_camera_t *self, lh_math_vec3_t up)
{
    lh_assert_runtime_ref(self);
    self->up = up;
}

lh_math_fscalar_t
lh_ui_renderer3d_camera_get_tangent(const lh_ui_renderer3d_camera_t *self)
{
    lh_assert_runtime_ref(self);
    return self->tangent;
}

lh_void
lh_ui_renderer3d_camera_set_tangent(lh_ui_renderer3d_camera_t *self, lh_math_fscalar_t tangent)
{
    lh_assert_runtime_ref(self);
    self->tangent = tangent;
}

lh_math_fscalar_t
lh_ui_renderer3d_camera_get_near_plane(const lh_ui_renderer3d_camera_t *self)
{
    lh_assert_runtime_ref(self);
    return self->near_plane;
}

lh_void
lh_ui_renderer3d_camera_set_near_plane(lh_ui_renderer3d_camera_t *self, lh_math_fscalar_t near_plane)
{
    lh_assert_runtime_ref(self);
    self->near_plane = near_plane;
}

lh_math_fscalar_t
lh_ui_renderer3d_camera_get_far_plane(const lh_ui_renderer3d_camera_t *self)
{
    lh_assert_runtime_ref(self);
    return self->far_plane;
}

lh_void
lh_ui_renderer3d_camera_set_far_plane(lh_ui_renderer3d_camera_t *self, lh_math_fscalar_t far_plane)
{
    lh_assert_runtime_ref(self);
    self->far_plane = far_plane;
}
