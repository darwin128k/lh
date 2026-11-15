/**
 * @file surface.m
 * @brief AppKit off-screen surface: client memory, presented by drawing an image.
 *
 * The words are top-down `0xAARRGGBB`, the same order a 32-bit little-endian
 * bitmap stores as B, G, R, A. A present draws that image into the paint's
 * graphics context. There is no memory device context: nothing draws into a
 * surface except by writing the words.
 */

#import <AppKit/AppKit.h>

#include <lh/os/system/apple/surface.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/cast/static.h>
#include <lh/core/math/fit.h>
#include <lh/core/math/point.h>
#include <lh/core/math/size.h>
#include <lh/core/memory.h>
#include <lh/core/memory/std.h>
#include <lh/core/runtime/allocator.h>
#include <lh/core/size/limits.h>
#include <lh/core/util/interval.h>
#include <lh/core/util/return.h>
#include <lh/os/system/apple/paint.h>
#include <lh/os/system/posix/surface/rgb565.h>
#include <lh/os/system/surface.h>

#include <limits.h>

lh_byte_ptr
lh_os_system_apple_surface_get_bits(const lh_os_system_apple_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bits;
}

lh_void
lh_os_system_apple_surface_set_bits(lh_os_system_apple_surface_t *self, lh_byte_ptr bits)
{
    lh_assert_runtime_ref(self);
    self->bits = bits;
}

int
lh_os_system_apple_surface_get_width(const lh_os_system_apple_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_os_system_apple_surface_set_width(lh_os_system_apple_surface_t *self, int width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

int
lh_os_system_apple_surface_get_height(const lh_os_system_apple_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_os_system_apple_surface_set_height(lh_os_system_apple_surface_t *self, int height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

int
lh_os_system_apple_surface_get_pixel_bits(const lh_os_system_apple_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pixel_bits;
}

lh_void
lh_os_system_apple_surface_set_pixel_bits(lh_os_system_apple_surface_t *self, int pixel_bits)
{
    lh_assert_runtime_ref(self);
    self->pixel_bits = pixel_bits;
}

lh_u32_t *
lh_os_system_apple_surface_get_staging(const lh_os_system_apple_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->staging;
}

lh_void
lh_os_system_apple_surface_set_staging(lh_os_system_apple_surface_t *self, lh_u32_t *staging)
{
    lh_assert_runtime_ref(self);
    self->staging = staging;
}

lh_usize_t
lh_os_system_apple_surface_get_staging_words(const lh_os_system_apple_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->staging_words;
}

lh_void
lh_os_system_apple_surface_set_staging_words(lh_os_system_apple_surface_t *self,
                                             lh_usize_t staging_words)
{
    lh_assert_runtime_ref(self);
    self->staging_words = staging_words;
}

lh_os_system_apple_surface_t *
lh_os_system_apple_surface_get_fit(const lh_os_system_apple_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fit;
}

lh_void
lh_os_system_apple_surface_set_fit(lh_os_system_apple_surface_t *self,
                                   lh_os_system_apple_surface_t *fit)
{
    lh_assert_runtime_ref(self);
    self->fit = fit;
}

lh_u32_t
lh_os_system_apple_surface_get_pixel(const lh_os_system_apple_surface_t *self, lh_usize_t index)
{
    if (lh_os_system_apple_surface_get_pixel_bits(self) == 16)
    {
        return lh_os_system_posix_surface_from_565(
            lh_ptr_rcast(const lh_u16_t, lh_os_system_apple_surface_get_bits(self))[index]);
    }
    return lh_ptr_rcast(const lh_u32_t, lh_os_system_apple_surface_get_bits(self))[index];
}

lh_u32_t *
lh_os_system_apple_surface_stage(lh_os_system_apple_surface_t *self, int src_x, int src_y,
                                 int width, int height)
{
    const lh_usize_t words = lh_cast_static(lh_usize_t, width) * lh_cast_static(lh_usize_t, height);

    if (lh_os_system_apple_surface_get_staging_words(self) < words)
    {
        lh_u32_t *grown =
            lh_ptr_rcast(lh_u32_t, lh_runtime_allocator_try_alloc(words * sizeof(lh_u32_t)));

        lh_return_if(lh_null_eq(grown), lh_null);
        if (lh_null_ne(lh_os_system_apple_surface_get_staging(self)))
        {
            lh_runtime_allocator_free(lh_os_system_apple_surface_get_staging(self));
        }
        lh_os_system_apple_surface_set_staging(self, grown);
        lh_os_system_apple_surface_set_staging_words(self, words);
    }
    const int source_w = lh_os_system_apple_surface_get_width(self);

    if (lh_os_system_apple_surface_get_pixel_bits(self) == 32)
    {
        const lh_u32_t *src =
            lh_ptr_rcast(const lh_u32_t, lh_os_system_apple_surface_get_bits(self));
        lh_u32_t *dst = lh_os_system_apple_surface_get_staging(self);

        src += lh_cast_static(lh_usize_t, src_y) * lh_cast_static(lh_usize_t, source_w) +
               lh_cast_static(lh_usize_t, src_x);
        for (int y = 0; y < height; ++y)
        {
            lh_memory_std_copy(dst, src, lh_cast_static(lh_usize_t, width) * sizeof(lh_u32_t));
            dst += width;
            src += source_w;
        }
        return lh_os_system_apple_surface_get_staging(self);
    }
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            lh_usize_t index =
                lh_cast_static(lh_usize_t, src_y + y) * lh_cast_static(lh_usize_t, source_w) +
                lh_cast_static(lh_usize_t, src_x + x);

            lh_os_system_apple_surface_get_staging(
                self)[lh_cast_static(lh_usize_t, y) * lh_cast_static(lh_usize_t, width) +
                      lh_cast_static(lh_usize_t, x)] =
                lh_os_system_apple_surface_get_pixel(self, index);
        }
    }
    return lh_os_system_apple_surface_get_staging(self);
}

lh_void
lh_os_system_apple_surface_scale_row(const lh_os_system_apple_surface_t *self, lh_u32_t *row,
                                     int width, int source_y)
{
    int source_w = lh_os_system_apple_surface_get_width(self);

    for (int x = 0; x < width; ++x)
    {
        lh_usize_t sx = lh_cast_static(lh_usize_t, lh_cast_static(lh_s64_t, x) * source_w / width);

        row[x] = lh_os_system_apple_surface_get_pixel(
            self, lh_cast_static(lh_usize_t, source_y) * lh_cast_static(lh_usize_t, source_w) + sx);
    }
}

lh_os_system_apple_surface_t *
lh_os_system_apple_surface_try_prepare_fit(lh_os_system_apple_surface_t *self, int width,
                                           int height, lh_u32_t fill)
{
    lh_return_if(lh_null_eq(self) || width <= 0 || height <= 0 || width > INT_MAX / 4, lh_null);
    int src_w = lh_os_system_apple_surface_get_width(self);
    int src_h = lh_os_system_apple_surface_get_height(self);
    lh_math_rect_t rect;

    lh_return_ifn(lh_math_fit_try_rect(lh_addr_of(rect), src_w, src_h, width, height), lh_null);
    lh_os_system_apple_surface_t *fit = lh_os_system_apple_surface_get_fit(self);

    if (lh_null_eq(fit) || lh_os_system_apple_surface_get_width(fit) != width ||
        lh_os_system_apple_surface_get_height(fit) != height)
    {
        lh_os_system_apple_surface_t *grown = lh_ptr_rcast(
            lh_os_system_apple_surface_t, lh_os_system_surface_create(width, height, 32));

        lh_return_if(lh_null_eq(grown), lh_null);
        lh_os_system_surface_destroy(fit);
        lh_os_system_apple_surface_set_fit(self, grown);
        fit = grown;
    }
    lh_u32_t *pixels = lh_ptr_rcast(lh_u32_t, lh_os_system_surface_get_pixels(fit));
    const lh_u32_t background = ((fill & 0xffU) << 16) | (fill & 0xff00U) | ((fill >> 16) & 0xffU);

    lh_memory_set_u32(pixels,
                      lh_cast_static(lh_usize_t, width) * lh_cast_static(lh_usize_t, height),
                      background);
    int left = lh_math_point_get_x(lh_math_rect_get_origin(lh_addr_of(rect)));
    int top = lh_math_point_get_y(lh_math_rect_get_origin(lh_addr_of(rect)));
    int fit_w = lh_math_size_get_width(lh_math_rect_get_size(lh_addr_of(rect)));
    int fit_h = lh_math_size_get_height(lh_math_rect_get_size(lh_addr_of(rect)));

    for (int y = 0; y < fit_h; ++y)
    {
        int sy = lh_cast_static(int, lh_cast_static(lh_s64_t, y) * src_h / fit_h);
        lh_u32_t *row = lh_ptr_add_by_offset(
            lh_u32_t, pixels,
            (lh_cast_static(lh_usize_t, top + y) * lh_cast_static(lh_usize_t, width) +
             lh_cast_static(lh_usize_t, left)) *
                sizeof(lh_u32_t));

        lh_os_system_apple_surface_scale_row(self, row, fit_w, sy);
    }
    return fit;
}

lh_bool_t
lh_os_system_apple_surface_draw(const lh_u32_t *words, int image_w, int image_h, lh_ptr dest,
                                int x, int y, int width, int height)
{
    const lh_os_system_apple_paint_t *paint =
        lh_ptr_rcast(const lh_os_system_apple_paint_t, dest);
    CGContextRef context = lh_ptr_rcast(CGContextRef, lh_os_system_apple_paint_get_context(paint));

    lh_return_if(lh_null_eq(context) || image_w <= 0 || image_h <= 0, lh_bool_false);
    const lh_usize_t bytes = lh_cast_static(lh_usize_t, image_w) *
                             lh_cast_static(lh_usize_t, image_h) * sizeof(lh_u32_t);
    CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
    CGDataProviderRef provider = CGDataProviderCreateWithData(
        lh_null, words, bytes, lh_cast_static(CGDataProviderReleaseDataCallback, lh_null));
    CGImageRef image = CGImageCreate(
        lh_cast_static(size_t, image_w), lh_cast_static(size_t, image_h), 8U, 32U,
        lh_cast_static(size_t, image_w) * 4U, space,
        kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipFirst, provider,
        lh_cast_static(const CGFloat *, lh_null), false, kCGRenderingIntentDefault);

    if (lh_null_eq(image))
    {
        CGDataProviderRelease(provider);
        CGColorSpaceRelease(space);
        return lh_bool_false;
    }
    NSImage *drawn = [[NSImage alloc] initWithCGImage:image
                                                 size:NSMakeSize(image_w, image_h)];

    [NSGraphicsContext saveGraphicsState];
    NSGraphicsContext *current = [NSGraphicsContext graphicsContextWithCGContext:context
                                                                         flipped:YES];

    [NSGraphicsContext setCurrentContext:current];
    [drawn drawInRect:NSMakeRect(x, y, width, height)
             fromRect:NSZeroRect
            operation:NSCompositingOperationCopy
             fraction:1.0
       respectFlipped:YES
                hints:nil];
    [NSGraphicsContext restoreGraphicsState];
    CGImageRelease(image);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(space);
    return lh_bool_true;
}

lh_os_system_surface_handle_t
lh_os_system_surface_create(int width, int height, int bits)
{
    lh_return_if(width <= 0 || height <= 0, LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
    lh_return_if(bits != 16 && bits != 32, LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
    lh_return_if(lh_interval_closed_is_mul_overflow(lh_cast_static(lh_usize_t, width),
                                                    lh_cast_static(lh_usize_t, height), 0U,
                                                    LH_USIZE_T_MAX / (bits / 8)),
                 LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
    lh_os_system_apple_surface_t *surface = lh_ptr_rcast(
        lh_os_system_apple_surface_t,
        lh_runtime_allocator_try_alloc(sizeof(lh_ptr_deref(surface))));

    lh_return_if(lh_null_eq(surface), LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
    lh_memory_set(surface, sizeof(lh_ptr_deref(surface)), 0);
    lh_os_system_apple_surface_set_width(surface, width);
    lh_os_system_apple_surface_set_height(surface, height);
    lh_os_system_apple_surface_set_pixel_bits(surface, bits);
    lh_usize_t bytes = lh_cast_static(lh_usize_t, width) * lh_cast_static(lh_usize_t, height) *
                       lh_cast_static(lh_usize_t, bits / 8);
    lh_byte_ptr pixels =
        lh_ptr_rcast(lh_byte_t, lh_runtime_allocator_try_alloc(bytes));

    if (lh_null_eq(pixels))
    {
        lh_runtime_allocator_free(surface);
        return LH_OS_SYSTEM_SURFACE_HANDLE_INVALID;
    }
    lh_memory_set(pixels, bytes, 0);
    lh_os_system_apple_surface_set_bits(surface, pixels);
    return surface;
}

lh_void
lh_os_system_surface_destroy(lh_os_system_surface_handle_t handle)
{
    if (lh_null_eq(handle))
    {
        return;
    }
    lh_os_system_apple_surface_t *surface = lh_ptr_rcast(lh_os_system_apple_surface_t, handle);

    lh_os_system_surface_destroy(lh_os_system_apple_surface_get_fit(surface));
    if (lh_null_ne(lh_os_system_apple_surface_get_staging(surface)))
    {
        lh_runtime_allocator_free(lh_os_system_apple_surface_get_staging(surface));
    }
    lh_runtime_allocator_free(lh_os_system_apple_surface_get_bits(surface));
    lh_runtime_allocator_free(surface);
}

int
lh_os_system_surface_get_width(lh_os_system_surface_handle_t handle)
{
    lh_return_if(lh_null_eq(handle), 0);
    return lh_os_system_apple_surface_get_width(
        lh_ptr_rcast(const lh_os_system_apple_surface_t, handle));
}

int
lh_os_system_surface_get_height(lh_os_system_surface_handle_t handle)
{
    lh_return_if(lh_null_eq(handle), 0);
    return lh_os_system_apple_surface_get_height(
        lh_ptr_rcast(const lh_os_system_apple_surface_t, handle));
}

lh_ptr
lh_os_system_surface_get_pixels(lh_os_system_surface_handle_t handle)
{
    lh_return_if(lh_null_eq(handle), lh_null);
    return lh_os_system_apple_surface_get_bits(
        lh_ptr_rcast(const lh_os_system_apple_surface_t, handle));
}

lh_ptr
lh_os_system_surface_get_dc(lh_os_system_surface_handle_t handle)
{
    /* AppKit has no memory device context. A surface is drawn as memory and
       presented into a paint. */
    LH_ATTRIBUTE_UNUSED(handle);
    return lh_null;
}

lh_bool_t
lh_os_system_surface_present_part(lh_os_system_surface_handle_t handle, lh_ptr dest, int x, int y,
                                  int src_x, int src_y, int width, int height)
{
    lh_return_if(lh_null_eq(handle) || lh_null_eq(dest), lh_bool_false);
    lh_os_system_apple_surface_t *surface = lh_ptr_rcast(lh_os_system_apple_surface_t, handle);
    const int surface_w = lh_os_system_apple_surface_get_width(surface);
    const int surface_h = lh_os_system_apple_surface_get_height(surface);

    if (src_x < 0)
    {
        width += src_x;
        x -= src_x;
        src_x = 0;
    }
    if (src_y < 0)
    {
        height += src_y;
        y -= src_y;
        src_y = 0;
    }
    if (src_x + width > surface_w)
    {
        width = surface_w - src_x;
    }
    if (src_y + height > surface_h)
    {
        height = surface_h - src_y;
    }
    lh_return_if(width <= 0 || height <= 0, lh_bool_false);
    const lh_bool_t whole = src_x == 0 && src_y == 0 && width == surface_w && height == surface_h &&
                            lh_os_system_apple_surface_get_pixel_bits(surface) == 32;
    const lh_u32_t *words =
        whole ? lh_ptr_rcast(const lh_u32_t, lh_os_system_apple_surface_get_bits(surface))
              : lh_os_system_apple_surface_stage(surface, src_x, src_y, width, height);

    lh_return_if(lh_null_eq(words), lh_bool_false);
    return lh_os_system_apple_surface_draw(words, width, height, dest, x, y, width, height);
}

lh_bool_t
lh_os_system_surface_present_at(lh_os_system_surface_handle_t handle, lh_ptr dest, int x, int y)
{
    const lh_os_system_apple_surface_t *surface;

    lh_return_if(lh_null_eq(handle) || lh_null_eq(dest), lh_bool_false);
    surface = lh_ptr_rcast(const lh_os_system_apple_surface_t, handle);
    return lh_os_system_surface_present_part(handle, dest, x, y, 0, 0,
                                             lh_os_system_apple_surface_get_width(surface),
                                             lh_os_system_apple_surface_get_height(surface));
}

lh_bool_t
lh_os_system_surface_present_fit(lh_os_system_surface_handle_t handle, lh_ptr dest, int dest_w,
                                 int dest_h, lh_u32_t fill)
{
    lh_return_if(lh_null_eq(handle) || lh_null_eq(dest), lh_bool_false);
    lh_os_system_apple_surface_t *surface = lh_ptr_rcast(lh_os_system_apple_surface_t, handle);
    lh_os_system_apple_surface_t *fit =
        lh_os_system_apple_surface_try_prepare_fit(surface, dest_w, dest_h, fill);

    lh_return_if(lh_null_eq(fit), lh_bool_false);
    return lh_os_system_surface_present_at(fit, dest, 0, 0);
}
