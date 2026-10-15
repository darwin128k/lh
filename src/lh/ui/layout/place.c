/**
 * @file place.c
 * @brief Implementation of `lh/ui/layout/place.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/ui/layout/place.h>
#include <lh/util/addr.h>

lh_void
lh_ui_place_init(lh_ui_place_t *self, lh_ui_place_size_t size_mode, lh_ui_scalar_t size)
{
    lh_assert_runtime_ref(self);
    self->size_mode = size_mode;
    self->size = size;
    self->align = lh_ui_place_align_start;
    lh_ui_insets_init_all(lh_addr_of(self->margin), lh_ui_scalar(0));
    lh_ui_size_init(lh_addr_of(self->min), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_size_init(lh_addr_of(self->max), lh_ui_scalar(0), lh_ui_scalar(0));
}

lh_void
lh_ui_place_set_size(lh_ui_place_t *self, lh_ui_place_size_t size_mode, lh_ui_scalar_t size)
{
    lh_assert_runtime_ref(self);
    self->size_mode = size_mode;
    self->size = size;
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
lh_ui_place_set_margin(lh_ui_place_t *self, const lh_ui_insets_t *margin)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(margin);
    self->margin = *margin;
}

const lh_ui_insets_t *
lh_ui_place_get_margin(const lh_ui_place_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->margin);
}

lh_void
lh_ui_place_set_min(lh_ui_place_t *self, lh_ui_scalar_t width, lh_ui_scalar_t height)
{
    lh_assert_runtime_ref(self);
    lh_ui_size_init(lh_addr_of(self->min), width, height);
}

lh_void
lh_ui_place_get_min(const lh_ui_place_t *self, lh_ui_size_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(self);
    *out = self->min;
    return;
}

lh_void
lh_ui_place_set_max(lh_ui_place_t *self, lh_ui_scalar_t width, lh_ui_scalar_t height)
{
    lh_assert_runtime_ref(self);
    lh_ui_size_init(lh_addr_of(self->max), width, height);
}

lh_void
lh_ui_place_get_max(const lh_ui_place_t *self, lh_ui_size_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(self);
    *out = self->max;
    return;
}

lh_ui_scalar_t
lh_ui_place_clamp(const lh_ui_place_t *self, lh_ui_axis_t axis, lh_ui_scalar_t length)
{
    lh_ui_scalar_t most;
    lh_ui_scalar_t least;

    lh_assert_runtime_ref(self);
    most = lh_ui_size_get_along(lh_addr_of(self->max), axis);
    least = lh_ui_size_get_along(lh_addr_of(self->min), axis);
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
