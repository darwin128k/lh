/**
 * @file mask.c
 * @brief Rounded coverage and the three combinations.
 */
#include <lh/gl/mask.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/cover.h>

lh_gl_mask_kind_t
lh_gl_mask_get_kind(const lh_gl_mask_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

const lh_gl_rect_t *
lh_gl_mask_get_rect(const lh_gl_mask_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->rect);
}

lh_gl_scalar_t
lh_gl_mask_get_radius(const lh_gl_mask_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

const lh_gl_mask_t *
lh_gl_mask_get_left(const lh_gl_mask_t *self)
{
    lh_assert_runtime_ref(self);
    return self->left;
}

const lh_gl_mask_t *
lh_gl_mask_get_right(const lh_gl_mask_t *self)
{
    lh_assert_runtime_ref(self);
    return self->right;
}

lh_void
lh_gl_mask_init_round(lh_gl_mask_t *self, lh_gl_rect_t rect, lh_gl_scalar_t radius)
{
    lh_assert_runtime_ref(self);
    self->kind = lh_gl_mask_kind_round;
    self->rect = rect;
    self->radius = radius < 0 ? 0 : radius;
    self->left = lh_null;
    self->right = lh_null;
}

lh_void
lh_gl_mask_init_intersect(lh_gl_mask_t *self, const lh_gl_mask_t *left, const lh_gl_mask_t *right)
{
    lh_assert_runtime_ref(self);
    self->kind = lh_gl_mask_kind_intersect;
    lh_gl_rect_init(lh_addr_of(self->rect), 0, 0, 0, 0);
    self->radius = 0;
    self->left = left;
    self->right = right;
}

lh_void
lh_gl_mask_init_unite(lh_gl_mask_t *self, const lh_gl_mask_t *left, const lh_gl_mask_t *right)
{
    lh_assert_runtime_ref(self);
    self->kind = lh_gl_mask_kind_unite;
    lh_gl_rect_init(lh_addr_of(self->rect), 0, 0, 0, 0);
    self->radius = 0;
    self->left = left;
    self->right = right;
}

lh_void
lh_gl_mask_init_exclude(lh_gl_mask_t *self, const lh_gl_mask_t *left, const lh_gl_mask_t *right)
{
    lh_assert_runtime_ref(self);
    self->kind = lh_gl_mask_kind_exclude;
    lh_gl_rect_init(lh_addr_of(self->rect), 0, 0, 0, 0);
    self->radius = 0;
    self->left = left;
    self->right = right;
}

lh_gl_color_channel_t
lh_gl_mask_coverage(const lh_gl_mask_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y)
{
    lh_assert_runtime_ref(self);
    if (lh_gl_mask_get_kind(self) == lh_gl_mask_kind_round)
    {
        const lh_gl_rect_t *rect = lh_gl_mask_get_rect(self);

        return lh_gl_cover_round_rect_band(rect, lh_gl_mask_get_radius(self), x, y);
    }
    const lh_gl_mask_t *left = lh_gl_mask_get_left(self);
    const lh_gl_mask_t *right = lh_gl_mask_get_right(self);
    lh_gl_color_channel_t left_cover = 0;

    if (lh_ptr_is_set(left))
    {
        left_cover = lh_gl_mask_coverage(left, x, y);
    }
    lh_gl_color_channel_t right_cover = 0;

    if (lh_ptr_is_set(right))
    {
        right_cover = lh_gl_mask_coverage(right, x, y);
    }

    if (lh_gl_mask_get_kind(self) == lh_gl_mask_kind_intersect)
    {
        return left_cover < right_cover ? left_cover : right_cover;
    }
    if (lh_gl_mask_get_kind(self) == lh_gl_mask_kind_unite)
    {
        return left_cover > right_cover ? left_cover : right_cover;
    }
    return lh_type_cast(lh_gl_color_channel_t,
                        lh_type_cast(lh_u16_t, left_cover) * (255U - right_cover) / 255U);
}
