/**
 * @file surface.c
 * @brief Implementation of `lh/ui/surface.h`.
 */
#include <lh/assert.h>

#include <lh/os/system/surface.h>
#include <lh/ui/surface.h>
#include <lh/util/return.h>

lh_void
lh_ui_surface_init(lh_ui_surface_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_size_init(lh_ui_surface_get_size_mut(self), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_surface_set_format(self, lh_ui_pixmap_format_argb8888);
    lh_ui_surface_set_handle(self, LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
}

lh_void
lh_ui_surface_deinit(lh_ui_surface_t *self)
{
    lh_assert_runtime_ref(self);
    lh_os_system_surface_destroy(lh_ui_surface_get_handle(self));
    lh_ui_surface_init(self);
}

lh_bool_t
lh_ui_surface_is_valid(const lh_ui_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_null_ne(lh_ui_surface_get_handle(self)) ? lh_bool_true : lh_bool_false;
}

lh_void
lh_ui_surface_copy_size(const lh_ui_surface_t *self, lh_ui_size_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(self);
    lh_ptr_deref(out) = lh_ptr_deref(lh_ui_surface_get_size(self));
}

lh_ui_pixmap_format_t
lh_ui_surface_get_format(const lh_ui_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->format;
}

lh_void
lh_ui_surface_set_format(lh_ui_surface_t *self, lh_ui_pixmap_format_t format)
{
    lh_assert_runtime_ref(self);
    /* Only while empty: a live buffer's format belongs to whatever is drawing into
       it, and swapping it under that would be a frame drawn in two formats. */
    lh_assert_runtime_if(lh_ui_surface_is_valid(self), lh_runtime_error_code_invalid_argument);
    /* A byte-swapped format is for a display controller that wants the words the
       other way round, not for a buffer this side draws into: the OS hands out words
       in its own order, and a swapped pixmap over them would read every pixel
       upside-down in colour. That format belongs to a frame on its way out. */
    lh_assert_runtime_if(format == lh_ui_pixmap_format_rgb565_swapped,
                         lh_runtime_error_code_invalid_argument);
    lh_ui_surface_set_format(self, format);
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
        lh_ui_size_get_width(lh_ui_surface_get_size_mut(self)) == lh_ui_size_get_width(lh_addr_of(size)) &&
        lh_ui_size_get_height(lh_ui_surface_get_size_mut(self)) == lh_ui_size_get_height(lh_addr_of(size)))
    {
        return lh_bool_true;
    }
    lh_os_system_surface_destroy(lh_ui_surface_get_handle(self));
    lh_ui_surface_set_handle(self, LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
    lh_ui_surface_set_size(self, size);
    lh_return_if(width <= 0 || height <= 0, lh_bool_false);
    lh_ui_surface_set_handle(self, lh_os_system_surface_create(width, height, lh_ui_pixmap_format_get_bits(lh_ui_surface_get_format(self))));
    return lh_ui_surface_is_valid(self);
}

lh_bool_t
lh_ui_surface_get_pixmap(const lh_ui_surface_t *self, lh_ui_pixmap_t *pixmap)
{
    const lh_s32_t width = lh_cast_static(lh_s32_t, lh_ui_size_get_width(lh_ui_surface_get_size(self)));
    lh_byte_ptr bits = lh_ptr_rcast(lh_byte_t, lh_os_system_surface_get_pixels(lh_ui_surface_get_handle(self)));

    lh_return_if(lh_null_eq(bits), lh_bool_false);
    lh_ui_pixmap_init(pixmap, bits, width,
                      lh_cast_static(lh_s32_t, lh_ui_size_get_height(lh_ui_surface_get_size(self))),
                      lh_math_mul(width, lh_ui_pixmap_format_get_bytes(lh_ui_surface_get_format(self))),
                      lh_ui_surface_get_format(self));
    return lh_bool_true;
}

lh_bool_t
lh_ui_surface_present_at(const lh_ui_surface_t *self, lh_ptr dest, lh_ui_point_t at)
{
    lh_assert_runtime_ref(self);
    return lh_os_system_surface_present_at(lh_ui_surface_get_handle(self), dest,
                                           lh_cast_static(int, lh_ui_point_get_x(lh_addr_of(at))),
                                           lh_cast_static(int, lh_ui_point_get_y(lh_addr_of(at))));
}

lh_bool_t
lh_ui_surface_present_part(const lh_ui_surface_t *self, lh_ptr dest, const lh_ui_rect_t *part,
                           lh_ui_point_t at)
{
    const lh_ui_point_t *from = lh_ui_rect_get_origin(part);
    const lh_ui_size_t *size = lh_ui_rect_get_size(part);
    /* The part sits inside the surface, so its own corner is added to where the
       surface goes: `at` is where (0, 0) of the surface lands, and a pixel at
       (px, py) of it lands at `at` + (px, py). */
    const int x = lh_cast_static(int, lh_ui_point_get_x(lh_addr_of(at))) +
                  lh_cast_static(int, lh_ui_point_get_x(from));
    const int y = lh_cast_static(int, lh_ui_point_get_y(lh_addr_of(at))) +
                  lh_cast_static(int, lh_ui_point_get_y(from));

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(part);
    return lh_os_system_surface_present_part(lh_ui_surface_get_handle(self), dest, x, y,
                                             lh_cast_static(int, lh_ui_point_get_x(from)),
                                             lh_cast_static(int, lh_ui_point_get_y(from)),
                                             lh_cast_static(int, lh_ui_size_get_width(size)),
                                             lh_cast_static(int, lh_ui_size_get_height(size)));
}

lh_bool_t
lh_ui_surface_present_rects(const lh_ui_surface_t *self, lh_ptr dest, const lh_ui_rects_t *rects,
                            lh_ui_point_t at)
{
    lh_u32_t i;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rects);
    for (i = 0u; i < lh_ui_rects_get_count(rects); ++i)
    {
        lh_return_if(!lh_ui_surface_present_part(self, dest, lh_ui_rects_get(rects, i), at),
                     lh_bool_false);
    }
    return lh_bool_true;
}

lh_os_system_surface_handle_t
lh_ui_surface_get_handle(const lh_ui_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_void
lh_ui_surface_set_handle(lh_ui_surface_t *self, lh_os_system_surface_handle_t handle)
{
    lh_assert_runtime_ref(self);
    self->handle = handle;
}

lh_ui_size_t *
lh_ui_surface_get_size_mut(lh_ui_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

const lh_ui_size_t *
lh_ui_surface_get_size(const lh_ui_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}
