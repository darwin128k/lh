/**
 * @file paint.c
 * @brief Level-1 accessors for ::lh_os_system_apple_paint_t.
 */

#include <lh/os/system/apple/paint.h>

#include <lh/core/assert/runtime.h>

lh_ptr
lh_os_system_apple_paint_get_context(const lh_os_system_apple_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_os_system_apple_paint_set_context(lh_os_system_apple_paint_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_u64_t
lh_os_system_apple_paint_get_area(const lh_os_system_apple_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->area;
}

lh_void
lh_os_system_apple_paint_set_area(lh_os_system_apple_paint_t *self, lh_u64_t area)
{
    lh_assert_runtime_ref(self);
    self->area = area;
}

lh_u32_t
lh_os_system_apple_paint_get_rects(const lh_os_system_apple_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rects;
}

lh_void
lh_os_system_apple_paint_set_rects(lh_os_system_apple_paint_t *self, lh_u32_t rects)
{
    lh_assert_runtime_ref(self);
    self->rects = rects;
}
