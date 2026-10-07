/**
 * @file surface.c
 * @brief Implementation of `lh/ui/surface.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/os/system/surface.h>
#include <lh/ui/size.h>
#include <lh/ui/surface.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

lh_void
lh_ui_surface_init(lh_ui_surface_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_size_init(lh_addr_of(self->size), lh_ui_scalar(0), lh_ui_scalar(0));
    self->handle = LH_OS_SYSTEM_SURFACE_HANDLE_INVALID;
}

lh_void
lh_ui_surface_deinit(lh_ui_surface_t *self)
{
    lh_assert_runtime_ref(self);
    lh_os_system_surface_destroy(self->handle);
    lh_ui_surface_init(self);
}

lh_bool_t
lh_ui_surface_is_valid(const lh_ui_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_null_ne(self->handle) ? lh_bool_true : lh_bool_false;
}

lh_ui_size_t
lh_ui_surface_get_size(const lh_ui_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_bool_t
lh_ui_surface_set_size(lh_ui_surface_t *self, lh_ui_size_t size)
{
    int width;
    int height;

    lh_assert_runtime_ref(self);
    width = lh_cast_static(int, lh_ui_size_get_width(lh_addr_of(size)));
    height = lh_cast_static(int, lh_ui_size_get_height(lh_addr_of(size)));
    if (lh_ui_surface_is_valid(self) &&
        lh_ui_size_get_width(lh_addr_of(self->size)) == lh_ui_size_get_width(lh_addr_of(size)) &&
        lh_ui_size_get_height(lh_addr_of(self->size)) == lh_ui_size_get_height(lh_addr_of(size)))
    {
        return lh_bool_true;
    }
    lh_os_system_surface_destroy(self->handle);
    self->handle = LH_OS_SYSTEM_SURFACE_HANDLE_INVALID;
    self->size = size;
    lh_return_if(width <= 0 || height <= 0, lh_bool_false);
    self->handle = lh_os_system_surface_create(width, height);
    return lh_ui_surface_is_valid(self);
}

lh_ptr
lh_ui_surface_get_draw_target(const lh_ui_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_os_system_surface_get_draw_target(self->handle);
}

lh_bool_t
lh_ui_surface_get_pixmap(const lh_ui_surface_t *self, lh_ui_pixmap_t *pixmap)
{
    const lh_s32_t width = lh_cast_static(lh_s32_t, lh_ui_size_get_width(lh_addr_of(self->size)));
    lh_u32_t *pixels = lh_ptr_rcast(lh_u32_t, lh_os_system_surface_get_pixels(self->handle));

    lh_return_if(lh_null_eq(pixels), lh_bool_false);
    lh_ui_pixmap_init(pixmap, pixels, width,
                      lh_cast_static(lh_s32_t, lh_ui_size_get_height(lh_addr_of(self->size))), width);
    return lh_bool_true;
}

lh_bool_t
lh_ui_surface_present(const lh_ui_surface_t *self, lh_ptr dest)
{
    lh_assert_runtime_ref(self);
    return lh_os_system_surface_present(self->handle, dest);
}
