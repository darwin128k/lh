/**
 * @file place.c
 * @brief Implementation of `lh/ui/layout/place.h`.
 */
#include <lh/assert.h>

#include <lh/ui/layout/place.h>

lh_void
lh_ui_place_init(lh_ui_place_t *self, lh_ui_place_size_t size_mode, lh_ui_scalar_t size)
{
    lh_ui_place_set_size_mode(self, size_mode);
    lh_ui_place_set_size(self, size);
    lh_ui_place_set_align(self, lh_ui_place_align_start);
    lh_ui_insets_init_all(lh_ui_place_get_margin_mut(self), lh_ui_scalar(0));
    lh_ui_size_init(lh_ui_place_get_min_mut(self), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_size_init(lh_ui_place_get_max_mut(self), lh_ui_scalar(0), lh_ui_scalar(0));
}

lh_void
lh_ui_place_set_sizing(lh_ui_place_t *self, lh_ui_place_size_t size_mode, lh_ui_scalar_t size)
{
    lh_ui_place_set_size_mode(self, size_mode);
    lh_ui_place_set_size(self, size);
}

lh_ui_place_size_t
lh_ui_place_get_size_mode(const lh_ui_place_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size_mode;
}

lh_ui_scalar_t
lh_ui_place_get_size(const lh_ui_place_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_void
lh_ui_place_set_align(lh_ui_place_t *self, lh_ui_place_align_t align)
{
    lh_assert_runtime_ref(self);
    self->align = align;
}

lh_ui_place_align_t
lh_ui_place_get_align(const lh_ui_place_t *self)
{
    lh_assert_runtime_ref(self);
    return self->align;
}

lh_void
lh_ui_place_assign_margin(lh_ui_place_t *self, const lh_ui_insets_t *margin)
{
    lh_assert_runtime_ref(margin);
    lh_ui_place_set_margin(self, lh_ptr_deref(margin));
}

const lh_ui_insets_t *
lh_ui_place_get_margin(const lh_ui_place_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->margin);
}

lh_void
lh_ui_place_set_min_size(lh_ui_place_t *self, lh_ui_scalar_t width, lh_ui_scalar_t height)
{
    lh_ui_size_init(lh_ui_place_get_min_mut(self), width, height);
}

lh_void
lh_ui_place_copy_min(const lh_ui_place_t *self, lh_ui_size_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_ui_place_get_min(self));
}

lh_void
lh_ui_place_set_max_size(lh_ui_place_t *self, lh_ui_scalar_t width, lh_ui_scalar_t height)
{
    lh_ui_size_init(lh_ui_place_get_max_mut(self), width, height);
}

lh_void
lh_ui_place_copy_max(const lh_ui_place_t *self, lh_ui_size_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_ui_place_get_max(self));
}

lh_ui_scalar_t
lh_ui_place_clamp(const lh_ui_place_t *self, lh_ui_axis_t axis, lh_ui_scalar_t length)
{

    lh_ui_scalar_t most = lh_ui_size_get_along(lh_ui_place_get_max(self), axis);
    lh_ui_scalar_t least = lh_ui_size_get_along(lh_ui_place_get_min(self), axis);
    /* The maximum first and the minimum last, so that a minimum above a maximum is
       the minimum, as CSS has it. */
    if (most > lh_ui_scalar(0) && length > most)
    {
        length = most;
    }
    if (length < least)
    {
        length = least;
    }
    return length;
}

lh_void
lh_ui_place_set_size_mode(lh_ui_place_t *self, lh_ui_place_size_t size_mode)
{
    lh_assert_runtime_ref(self);
    self->size_mode = size_mode;
}

lh_ui_insets_t *
lh_ui_place_get_margin_mut(lh_ui_place_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->margin);
}

lh_ui_size_t *
lh_ui_place_get_min_mut(lh_ui_place_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->min);
}

lh_ui_size_t *
lh_ui_place_get_max_mut(lh_ui_place_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->max);
}

lh_void
lh_ui_place_set_size(lh_ui_place_t *self, lh_ui_scalar_t size)
{
    lh_assert_runtime_ref(self);
    self->size = size;
}

const lh_ui_size_t *
lh_ui_place_get_min(const lh_ui_place_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->min);
}

lh_void
lh_ui_place_set_min(lh_ui_place_t *self, lh_ui_size_t min)
{
    lh_assert_runtime_ref(self);
    self->min = min;
}

const lh_ui_size_t *
lh_ui_place_get_max(const lh_ui_place_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->max);
}

lh_void
lh_ui_place_set_max(lh_ui_place_t *self, lh_ui_size_t max)
{
    lh_assert_runtime_ref(self);
    self->max = max;
}

lh_void
lh_ui_place_set_margin(lh_ui_place_t *self, lh_ui_insets_t margin)
{
    lh_assert_runtime_ref(self);
    self->margin = margin;
}
