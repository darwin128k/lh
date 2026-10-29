/**
 * @file paint.c
 * @brief Level-1 accessors of ::lh_os_system_posix_paint_t.
 */

#include <lh/os/system/posix/paint.h>

#include <lh/assert/runtime.h>

const lh_os_system_posix_x11_t *
lh_os_system_posix_paint_get_x11(const lh_os_system_posix_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->x11;
}

lh_void
lh_os_system_posix_paint_set_x11(lh_os_system_posix_paint_t *self,
                                 const lh_os_system_posix_x11_t *x11)
{
    lh_assert_runtime_ref(self);
    self->x11 = x11;
}

lh_os_system_posix_display_t *
lh_os_system_posix_paint_get_display(const lh_os_system_posix_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->display;
}

lh_void
lh_os_system_posix_paint_set_display(lh_os_system_posix_paint_t *self,
                                     lh_os_system_posix_display_t *display)
{
    lh_assert_runtime_ref(self);
    self->display = display;
}

lh_os_system_posix_xid_t
lh_os_system_posix_paint_get_window(const lh_os_system_posix_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->window;
}

lh_void
lh_os_system_posix_paint_set_window(lh_os_system_posix_paint_t *self,
                                    lh_os_system_posix_xid_t window)
{
    lh_assert_runtime_ref(self);
    self->window = window;
}

lh_os_system_posix_gc_t *
lh_os_system_posix_paint_get_gc(const lh_os_system_posix_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->gc;
}

lh_void
lh_os_system_posix_paint_set_gc(lh_os_system_posix_paint_t *self, lh_os_system_posix_gc_t *gc)
{
    lh_assert_runtime_ref(self);
    self->gc = gc;
}

int
lh_os_system_posix_paint_get_depth(const lh_os_system_posix_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->depth;
}

lh_void
lh_os_system_posix_paint_set_depth(lh_os_system_posix_paint_t *self, int depth)
{
    lh_assert_runtime_ref(self);
    self->depth = depth;
}

lh_u64_t
lh_os_system_posix_paint_get_area(const lh_os_system_posix_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->area;
}

lh_void
lh_os_system_posix_paint_set_area(lh_os_system_posix_paint_t *self, lh_u64_t area)
{
    lh_assert_runtime_ref(self);
    self->area = area;
}

lh_u32_t
lh_os_system_posix_paint_get_rects(const lh_os_system_posix_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rects;
}

lh_void
lh_os_system_posix_paint_set_rects(lh_os_system_posix_paint_t *self, lh_u32_t rects)
{
    lh_assert_runtime_ref(self);
    self->rects = rects;
}
