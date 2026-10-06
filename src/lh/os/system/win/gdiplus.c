/**
 * @file gdiplus.c
 * @brief Win32 GDI+: process startup, per-frame Graphics, mask and round fills.
 */

#include <lh/bit/packed.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/os/alloc.h>
#include <lh/os/system/gdiplus.h>
#include <lh/os/system/win/gdiplus.h>
#include <lh/runtime/allocator.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* GDI+ is process-wide: started by the first user, stopped by the last. */
static lh_usize_t lh_os_system_gdiplus_token;
static int lh_os_system_gdiplus_users;

struct lh_os_system_gdiplus_frame
{
    lh_os_system_win_gp_graphics_t *graphics;
    lh_os_system_win_gp_path_t *path;
    lh_os_system_win_gp_solid_fill_t *solid;
    lh_os_system_win_gp_bitmap_t *bitmap;
    int bitmap_width;
    int bitmap_height;
};
typedef struct lh_os_system_gdiplus_frame lh_os_system_gdiplus_frame_body_t;

lh_bool_t
lh_os_system_gdiplus_acquire(lh_void)
{
    /* GDI+ 1.0: four fields only (GdiplusStartupInputSize came in 1.1 / Vista). */
    lh_os_system_win_gdiplus_startup_input_t input;

    input.GdiplusVersion = 1U;
    input.DebugEventCallback = lh_null;
    input.SuppressBackgroundThread = LH_OS_SYSTEM_WIN_FALSE;
    input.SuppressExternalCodecs = LH_OS_SYSTEM_WIN_FALSE;
    lh_return_if(lh_os_system_gdiplus_users == 0 &&
                     GdiplusStartup(lh_addr_of(lh_os_system_gdiplus_token), lh_addr_of(input),
                                    lh_null) != lh_os_system_win_gp_ok,
                 lh_bool_false);
    ++lh_os_system_gdiplus_users;
    return lh_bool_true;
}

lh_void
lh_os_system_gdiplus_release(lh_void)
{
    lh_return_if(lh_os_system_gdiplus_users == 0);
    if (--lh_os_system_gdiplus_users == 0)
    {
        GdiplusShutdown(lh_os_system_gdiplus_token);
    }
}

lh_bool_t
lh_os_system_gdiplus_is_ready(lh_void)
{
    return lh_os_system_gdiplus_users > 0 ? lh_bool_true : lh_bool_false;
}

lh_os_system_gdiplus_frame_t
lh_os_system_gdiplus_frame_begin(lh_ptr hdc)
{
    lh_os_system_gdiplus_frame_body_t *frame;
    lh_os_system_win_hdc_t dc;
    lh_os_system_win_gp_graphics_t *graphics = lh_null;
    lh_os_system_win_gp_path_t *path = lh_null;
    lh_os_system_win_gp_solid_fill_t *solid = lh_null;

    lh_return_if(lh_null_eq(hdc), lh_null);
    frame = lh_ptr_rcast(lh_os_system_gdiplus_frame_body_t, lh_os_alloc(sizeof(*frame)));
    lh_return_if(lh_null_eq(frame), lh_null);
    dc = lh_cast_reinterpret(lh_os_system_win_hdc_t, hdc);
    if (GdipCreateFromHDC(dc, lh_addr_of(graphics)) != lh_os_system_win_gp_ok)
    {
        lh_runtime_allocator_free(frame);
        return lh_null;
    }
    GdipSetPixelOffsetMode(graphics, lh_os_system_win_gp_pixel_offset_mode_none);
    GdipSetCompositingMode(graphics, lh_os_system_win_gp_compositing_mode_source_over);
    if (GdipCreatePath(lh_os_system_win_gp_fill_mode_alternate, lh_addr_of(path)) !=
        lh_os_system_win_gp_ok)
    {
        GdipDeleteGraphics(graphics);
        lh_runtime_allocator_free(frame);
        return lh_null;
    }
    if (GdipCreateSolidFill(0, lh_addr_of(solid)) != lh_os_system_win_gp_ok)
    {
        GdipDeletePath(path);
        GdipDeleteGraphics(graphics);
        lh_runtime_allocator_free(frame);
        return lh_null;
    }
    frame->graphics = graphics;
    frame->path = path;
    frame->solid = solid;
    frame->bitmap = lh_null;
    frame->bitmap_width = 0;
    frame->bitmap_height = 0;
    return lh_cast_reinterpret(lh_os_system_gdiplus_frame_t, frame);
}

lh_void
lh_os_system_gdiplus_frame_end(lh_os_system_gdiplus_frame_t handle)
{
    lh_os_system_gdiplus_frame_body_t *frame;

    lh_return_if(lh_null_eq(handle));
    frame = lh_ptr_rcast(lh_os_system_gdiplus_frame_body_t, handle);
    if (lh_null_ne(frame->bitmap))
    {
        GdipDisposeImage(lh_ptr_rcast(lh_os_system_win_gp_image_t, frame->bitmap));
    }
    if (lh_null_ne(frame->solid))
    {
        GdipDeleteBrush(lh_ptr_rcast(lh_os_system_win_gp_brush_t, frame->solid));
    }
    if (lh_null_ne(frame->path))
    {
        GdipDeletePath(frame->path);
    }
    if (lh_null_ne(frame->graphics))
    {
        GdipDeleteGraphics(frame->graphics);
    }
    lh_runtime_allocator_free(frame);
}

static lh_bool_t
lh_os_system_gdiplus_frame_ensure_bitmap(lh_os_system_gdiplus_frame_body_t *frame, int width, int height)
{
    lh_os_system_win_gp_bitmap_t *bitmap = lh_null;

    lh_return_if(width <= 0 || height <= 0, lh_bool_false);
    if (lh_null_ne(frame->bitmap) && frame->bitmap_width >= width && frame->bitmap_height >= height)
    {
        return lh_bool_true;
    }
    if (lh_null_ne(frame->bitmap))
    {
        GdipDisposeImage(lh_ptr_rcast(lh_os_system_win_gp_image_t, frame->bitmap));
        frame->bitmap = lh_null;
    }
    lh_return_if(GdipCreateBitmapFromScan0(width, height, 0, LH_OS_SYSTEM_WIN_PIXEL_FORMAT_32BPP_ARGB,
                                           lh_null, lh_addr_of(bitmap)) != lh_os_system_win_gp_ok,
                 lh_bool_false);
    frame->bitmap = bitmap;
    frame->bitmap_width = width;
    frame->bitmap_height = height;
    return lh_bool_true;
}

lh_void
lh_os_system_gdiplus_frame_fill_mask(lh_os_system_gdiplus_frame_t handle, int x, int y, int width,
                                     int height, int bpp, int row_bytes, const lh_byte_t *bits,
                                     lh_byte_t r, lh_byte_t g, lh_byte_t b, lh_byte_t a)
{
    lh_os_system_gdiplus_frame_body_t *frame;
    lh_os_system_win_gp_bitmap_data_t data;
    lh_os_system_win_gp_rect_t area;
    lh_u32_t max_sample;
    int row;
    int col;

    lh_return_if(lh_null_eq(handle) || lh_null_eq(bits));
    lh_return_if(width <= 0 || height <= 0 || row_bytes <= 0);
    lh_return_if(bpp != 1 && bpp != 2 && bpp != 4 && bpp != 8);
    frame = lh_ptr_rcast(lh_os_system_gdiplus_frame_body_t, handle);
    lh_return_if(lh_null_eq(frame->graphics));
    lh_return_if(!lh_os_system_gdiplus_frame_ensure_bitmap(frame, width, height));

    area.X = 0;
    area.Y = 0;
    area.Width = width;
    area.Height = height;
    lh_return_if(GdipBitmapLockBits(frame->bitmap, lh_addr_of(area),
                                    lh_os_system_win_gp_image_lock_mode_write,
                                    LH_OS_SYSTEM_WIN_PIXEL_FORMAT_32BPP_ARGB,
                                    lh_addr_of(data)) != lh_os_system_win_gp_ok);
    max_sample = lh_bit_packed_max(lh_cast_static(lh_u32_t, bpp));
    for (row = 0; row < height; ++row)
    {
        lh_os_system_win_gdiplus_argb_t *out =
            lh_ptr_rcast(lh_os_system_win_gdiplus_argb_t,
                         lh_cast_reinterpret(lh_byte_t *, data.Scan0) + row * data.Stride);
        const lh_byte_t *in = bits + row * row_bytes;

        for (col = 0; col < width; ++col)
        {
            const lh_u32_t sample = lh_bit_packed_get(in, lh_cast_static(lh_u32_t, col),
                                                     lh_cast_static(lh_u32_t, bpp));
            const lh_u32_t cover = (sample * 255U) / max_sample;
            const lh_u32_t out_a = (lh_cast_static(lh_u32_t, a) * cover) / 255U;

            out[col] = (out_a << 24) | (lh_cast_static(lh_u32_t, r) << 16) |
                       (lh_cast_static(lh_u32_t, g) << 8) | lh_cast_static(lh_u32_t, b);
        }
    }
    GdipBitmapUnlockBits(frame->bitmap, lh_addr_of(data));
    GdipDrawImagePointRectI(frame->graphics, lh_ptr_rcast(lh_os_system_win_gp_image_t, frame->bitmap),
                            x, y, 0, 0, width, height, lh_os_system_win_gp_unit_pixel);
}

lh_void
lh_os_system_gdiplus_frame_fill_round_rect(lh_os_system_gdiplus_frame_t handle, int left, int top,
                                           int right, int bottom, int radius, lh_byte_t r,
                                           lh_byte_t g, lh_byte_t b, lh_byte_t a)
{
    lh_os_system_gdiplus_frame_body_t *frame;
    const lh_os_system_win_gdiplus_real_t d = lh_cast_static(lh_os_system_win_gdiplus_real_t, radius) * 2.0f;
    const lh_os_system_win_gdiplus_real_t x0 = lh_cast_static(lh_os_system_win_gdiplus_real_t, left);
    const lh_os_system_win_gdiplus_real_t y0 = lh_cast_static(lh_os_system_win_gdiplus_real_t, top);
    const lh_os_system_win_gdiplus_real_t x1 = lh_cast_static(lh_os_system_win_gdiplus_real_t, right) - d;
    const lh_os_system_win_gdiplus_real_t y1 = lh_cast_static(lh_os_system_win_gdiplus_real_t, bottom) - d;
    const lh_os_system_win_gdiplus_argb_t argb =
        (lh_cast_static(lh_os_system_win_gdiplus_argb_t, a) << 24) |
        (lh_cast_static(lh_os_system_win_gdiplus_argb_t, r) << 16) |
        (lh_cast_static(lh_os_system_win_gdiplus_argb_t, g) << 8) |
        lh_cast_static(lh_os_system_win_gdiplus_argb_t, b);

    lh_return_if(lh_null_eq(handle));
    frame = lh_ptr_rcast(lh_os_system_gdiplus_frame_body_t, handle);
    lh_return_if(lh_null_eq(frame->graphics) || lh_null_eq(frame->path) || lh_null_eq(frame->solid));

    GdipSetSmoothingMode(frame->graphics, lh_os_system_win_gp_smoothing_mode_anti_alias);
    GdipSetPixelOffsetMode(frame->graphics, lh_os_system_win_gp_pixel_offset_mode_half);
    GdipResetPath(frame->path);
    GdipAddPathArc(frame->path, x0, y0, d, d, 180.0f, 90.0f);
    GdipAddPathArc(frame->path, x1, y0, d, d, 270.0f, 90.0f);
    GdipAddPathArc(frame->path, x1, y1, d, d, 0.0f, 90.0f);
    GdipAddPathArc(frame->path, x0, y1, d, d, 90.0f, 90.0f);
    GdipClosePathFigure(frame->path);
    GdipSetSolidFillColor(frame->solid, argb);
    GdipFillPath(frame->graphics, lh_ptr_rcast(lh_os_system_win_gp_brush_t, frame->solid),
                 frame->path);
    /* Restore frame defaults so subsequent mask draws stay pixel-aligned. */
    GdipSetPixelOffsetMode(frame->graphics, lh_os_system_win_gp_pixel_offset_mode_none);
    GdipSetSmoothingMode(frame->graphics, lh_os_system_win_gp_smoothing_mode_none);
}

lh_void
lh_os_system_gdiplus_frame_set_clip(lh_os_system_gdiplus_frame_t handle, int left, int top, int right,
                                    int bottom)
{
    lh_os_system_gdiplus_frame_body_t *frame;

    lh_return_if(lh_null_eq(handle));
    frame = lh_ptr_rcast(lh_os_system_gdiplus_frame_body_t, handle);
    lh_return_if(lh_null_eq(frame->graphics));
    GdipSetClipRectI(frame->graphics, left, top, right - left, bottom - top,
                     lh_os_system_win_gp_combine_mode_replace);
}

lh_void
lh_os_system_gdiplus_frame_clear_clip(lh_os_system_gdiplus_frame_t handle)
{
    lh_os_system_gdiplus_frame_body_t *frame;

    lh_return_if(lh_null_eq(handle));
    frame = lh_ptr_rcast(lh_os_system_gdiplus_frame_body_t, handle);
    lh_return_if(lh_null_eq(frame->graphics));
    GdipResetClip(frame->graphics);
}
