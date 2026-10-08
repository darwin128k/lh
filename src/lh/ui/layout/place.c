/**
 * @file place.c
 * @brief Implementation of `lh/ui/layout/place.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/ui/layout/place.h>

lh_void
lh_ui_place_init(lh_ui_place_t *self, lh_ui_place_size_t size_mode, lh_ui_scalar_t size)
{
    lh_assert_runtime_ref(self);
    self->size_mode = size_mode;
    self->size = size;
    self->align = lh_ui_place_align_start;
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