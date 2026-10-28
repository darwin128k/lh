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
    self->position = position;
    self->forward = unit_forward;
    self->right = unit_right;
    self->up = unit_up;
    self->tangent = lh_math_float_tan(vertical_fov * .5f);
    self->near_plane = near_plane;
    self->far_plane = far_plane;
    return lh_bool_true;
}

lh_bool_t
lh_ui_renderer3d_camera_is_usable(const lh_ui_renderer3d_camera_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_float_is_finite(self->tangent) && self->tangent > 0 &&
           self->near_plane > 0 && self->far_plane > self->near_plane;
}

lh_void
lh_ui_renderer3d_camera_to_view(const lh_ui_renderer3d_camera_t *self, lh_math_vec3_t world,
                                lh_math_vec3_t *out)
{
    lh_assert_runtime_ref(self);
    lh_math_vec3_t offset;
    lh_math_vec3_sub(world, self->position, &offset);
    lh_math_vec3_init(out, lh_math_vec3_dot(offset, self->right),
                      lh_math_vec3_dot(offset, self->up), lh_math_vec3_dot(offset, self->forward));
}
