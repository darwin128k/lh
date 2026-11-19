/**
 * @file corners.c
 * @brief Implementation of `lh/gx/corners.h`.
 */

#include <lh/gx/corners.h>

lh_gx_scalar_t
lh_gx_corners_get_top_left(const lh_gx_corners_t *self)
{
    lh_assert_runtime_ref(self);
    return self->top_left;
}

lh_void
lh_gx_corners_set_top_left(lh_gx_corners_t *self, lh_gx_scalar_t top_left)
{
    lh_assert_runtime_ref(self);
    self->top_left = top_left;
}

lh_gx_scalar_t
lh_gx_corners_get_top_right(const lh_gx_corners_t *self)
{
    lh_assert_runtime_ref(self);
    return self->top_right;
}

lh_void
lh_gx_corners_set_top_right(lh_gx_corners_t *self, lh_gx_scalar_t top_right)
{
    lh_assert_runtime_ref(self);
    self->top_right = top_right;
}

lh_gx_scalar_t
lh_gx_corners_get_bottom_right(const lh_gx_corners_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bottom_right;
}

lh_void
lh_gx_corners_set_bottom_right(lh_gx_corners_t *self, lh_gx_scalar_t bottom_right)
{
    lh_assert_runtime_ref(self);
    self->bottom_right = bottom_right;
}

lh_gx_scalar_t
lh_gx_corners_get_bottom_left(const lh_gx_corners_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bottom_left;
}

lh_void
lh_gx_corners_set_bottom_left(lh_gx_corners_t *self, lh_gx_scalar_t bottom_left)
{
    lh_assert_runtime_ref(self);
    self->bottom_left = bottom_left;
}

lh_void
lh_gx_corners_init(lh_gx_corners_t *self, lh_gx_scalar_t top_left, lh_gx_scalar_t top_right,
                   lh_gx_scalar_t bottom_right, lh_gx_scalar_t bottom_left)
{
    lh_gx_corners_set_top_left(self, top_left);
    lh_gx_corners_set_top_right(self, top_right);
    lh_gx_corners_set_bottom_right(self, bottom_right);
    lh_gx_corners_set_bottom_left(self, bottom_left);
}

lh_void
lh_gx_corners_init_all(lh_gx_corners_t *self, lh_gx_scalar_t length)
{
    lh_gx_corners_init(self, length, length, length, length);
}
