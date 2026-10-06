/**
 * @file gdiplus.h
 * @brief Backend-private: the GDI+ 1.0 flat C API we call, declared by us
 *        instead of `<windows.h>` / `<gdiplus.h>`.
 *
 * GDI+ ships with XP (gdiplus.dll redistributable / in-box on XP SP2+). Only
 * the 1.0 surface: `GdiplusStartupInput` has four fields — the fifth
 * (`GdiplusStartupInputSize`) arrived in GDI+ 1.1 / Vista. Must not share a
 * translation unit with `<windows.h>`.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_GDIPLUS_H
#define LH_SRC_OS_SYSTEM_WIN_GDIPLUS_H

#include <lh/byte.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/os/system/win/types.h>
#include <lh/os/system/win/user32.h>
#include <lh/ptr.h>
#include <lh/size.h>
#include <lh/void.h>

/* GDI+ calling convention. Present since GDI+ 1.0 / Windows XP. */
#if defined(_ARM_)
#    define LH_OS_SYSTEM_WIN_GDIPAPI
#else
#    define LH_OS_SYSTEM_WIN_GDIPAPI __stdcall
#endif

/** @brief `REAL`. GDI+ 1.0. */
typedef float lh_os_system_win_gdiplus_real_t;

/** @brief `ARGB`. GDI+ 1.0. */
typedef lh_os_system_win_dword_t lh_os_system_win_gdiplus_argb_t;

/** @brief `PixelFormat`. GDI+ 1.0. */
typedef lh_int_t lh_os_system_win_gdiplus_pixel_format_t;

/** @brief Opaque GDI+ objects. GDI+ 1.0. */
typedef struct lh_os_system_win_gp_graphics lh_os_system_win_gp_graphics_t;
typedef struct lh_os_system_win_gp_image lh_os_system_win_gp_image_t;
typedef struct lh_os_system_win_gp_bitmap lh_os_system_win_gp_bitmap_t;
typedef struct lh_os_system_win_gp_brush lh_os_system_win_gp_brush_t;
typedef struct lh_os_system_win_gp_solid_fill lh_os_system_win_gp_solid_fill_t;
typedef struct lh_os_system_win_gp_path lh_os_system_win_gp_path_t;

/**
 * @brief `GpStatus`. GDI+ 1.0 — only `Ok` is tested.
 */
enum lh_os_system_win_gp_status
{
    lh_os_system_win_gp_ok = 0
};
typedef enum lh_os_system_win_gp_status lh_os_system_win_gp_status_t;

/** @brief `Unit`. GDI+ 1.0. */
enum lh_os_system_win_gp_unit
{
    lh_os_system_win_gp_unit_pixel = 2
};
typedef enum lh_os_system_win_gp_unit lh_os_system_win_gp_unit_t;

/** @brief `FillMode`. GDI+ 1.0. */
enum lh_os_system_win_gp_fill_mode
{
    lh_os_system_win_gp_fill_mode_alternate = 0
};
typedef enum lh_os_system_win_gp_fill_mode lh_os_system_win_gp_fill_mode_t;

/** @brief `CompositingMode`. GDI+ 1.0. */
enum lh_os_system_win_gp_compositing_mode
{
    lh_os_system_win_gp_compositing_mode_source_over = 0
};
typedef enum lh_os_system_win_gp_compositing_mode lh_os_system_win_gp_compositing_mode_t;

/** @brief `PixelOffsetMode`. GDI+ 1.0. */
enum lh_os_system_win_gp_pixel_offset_mode
{
    lh_os_system_win_gp_pixel_offset_mode_none = 3,
    lh_os_system_win_gp_pixel_offset_mode_half = 4
};
typedef enum lh_os_system_win_gp_pixel_offset_mode lh_os_system_win_gp_pixel_offset_mode_t;

/** @brief `SmoothingMode`. GDI+ 1.0. */
enum lh_os_system_win_gp_smoothing_mode
{
    lh_os_system_win_gp_smoothing_mode_anti_alias = 4
};
typedef enum lh_os_system_win_gp_smoothing_mode lh_os_system_win_gp_smoothing_mode_t;

/** @brief `ImageLockMode`. GDI+ 1.0. */
enum lh_os_system_win_gp_image_lock_mode
{
    lh_os_system_win_gp_image_lock_mode_write = 2
};
typedef enum lh_os_system_win_gp_image_lock_mode lh_os_system_win_gp_image_lock_mode_t;

/**
 * @struct lh_os_system_win_gp_rect
 * @brief `GpRect` / `Rect`. GDI+ 1.0.
 */
struct lh_os_system_win_gp_rect
{
    lh_int_t X;
    lh_int_t Y;
    lh_int_t Width;
    lh_int_t Height;
};
typedef struct lh_os_system_win_gp_rect lh_os_system_win_gp_rect_t;

/**
 * @struct lh_os_system_win_gp_bitmap_data
 * @brief `BitmapData`. GDI+ 1.0.
 */
struct lh_os_system_win_gp_bitmap_data
{
    lh_uint_t Width;
    lh_uint_t Height;
    lh_int_t Stride;
    lh_int_t PixelFormat;
    lh_ptr Scan0;
    lh_usize_t Reserved;
};
typedef struct lh_os_system_win_gp_bitmap_data lh_os_system_win_gp_bitmap_data_t;

/**
 * @struct lh_os_system_win_gdiplus_startup_input
 * @brief `GdiplusStartupInput` — GDI+ **1.0** (four fields).
 *
 * Do not add a fifth field: `GdiplusStartupInputSize` is GDI+ 1.1 / Vista.
 */
struct lh_os_system_win_gdiplus_startup_input
{
    lh_uint_t GdiplusVersion;
    lh_ptr DebugEventCallback; /* `DebugEventProc`; always null here. */
    lh_os_system_win_bool_t SuppressBackgroundThread;
    lh_os_system_win_bool_t SuppressExternalCodecs;
};
typedef struct lh_os_system_win_gdiplus_startup_input lh_os_system_win_gdiplus_startup_input_t;

/* `PixelFormat32bppARGB`. GDI+ 1.0. */
#define LH_OS_SYSTEM_WIN_PIXEL_FORMAT_32BPP_ARGB                                                   \
    ((lh_os_system_win_gdiplus_pixel_format_t)(10 | (32 << 8) | 0x00040000 | 0x00020000 |           \
                                               0x00200000))

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdiplusStartup(lh_usize_t *token, const lh_os_system_win_gdiplus_startup_input_t *input,
               lh_ptr output);

LH_OS_SYSTEM_WIN_IMPORT lh_void LH_OS_SYSTEM_WIN_GDIPAPI
GdiplusShutdown(lh_usize_t token);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipCreateFromHDC(lh_os_system_win_hdc_t hdc, lh_os_system_win_gp_graphics_t **graphics);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipDeleteGraphics(lh_os_system_win_gp_graphics_t *graphics);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipSetCompositingMode(lh_os_system_win_gp_graphics_t *graphics,
                       lh_os_system_win_gp_compositing_mode_t mode);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipSetPixelOffsetMode(lh_os_system_win_gp_graphics_t *graphics,
                       lh_os_system_win_gp_pixel_offset_mode_t mode);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipSetSmoothingMode(lh_os_system_win_gp_graphics_t *graphics,
                     lh_os_system_win_gp_smoothing_mode_t mode);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipCreateBitmapFromScan0(lh_int_t width, lh_int_t height, lh_int_t stride,
                          lh_os_system_win_gdiplus_pixel_format_t format, lh_byte_t *scan0,
                          lh_os_system_win_gp_bitmap_t **bitmap);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipDisposeImage(lh_os_system_win_gp_image_t *image);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipBitmapLockBits(lh_os_system_win_gp_bitmap_t *bitmap, const lh_os_system_win_gp_rect_t *rect,
                   lh_uint_t flags, lh_os_system_win_gdiplus_pixel_format_t format,
                   lh_os_system_win_gp_bitmap_data_t *locked_data);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipBitmapUnlockBits(lh_os_system_win_gp_bitmap_t *bitmap,
                     lh_os_system_win_gp_bitmap_data_t *locked_data);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipDrawImagePointRectI(lh_os_system_win_gp_graphics_t *graphics, lh_os_system_win_gp_image_t *image,
                        lh_int_t x, lh_int_t y, lh_int_t srcx, lh_int_t srcy, lh_int_t srcwidth,
                        lh_int_t srcheight, lh_os_system_win_gp_unit_t src_unit);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipCreatePath(lh_os_system_win_gp_fill_mode_t fill_mode, lh_os_system_win_gp_path_t **path);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipDeletePath(lh_os_system_win_gp_path_t *path);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipAddPathArc(lh_os_system_win_gp_path_t *path, lh_os_system_win_gdiplus_real_t x,
               lh_os_system_win_gdiplus_real_t y, lh_os_system_win_gdiplus_real_t width,
               lh_os_system_win_gdiplus_real_t height, lh_os_system_win_gdiplus_real_t start_angle,
               lh_os_system_win_gdiplus_real_t sweep_angle);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipClosePathFigure(lh_os_system_win_gp_path_t *path);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipCreateSolidFill(lh_os_system_win_gdiplus_argb_t color, lh_os_system_win_gp_solid_fill_t **brush);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipDeleteBrush(lh_os_system_win_gp_brush_t *brush);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_gp_status_t LH_OS_SYSTEM_WIN_GDIPAPI
GdipFillPath(lh_os_system_win_gp_graphics_t *graphics, lh_os_system_win_gp_brush_t *brush,
             lh_os_system_win_gp_path_t *path);

#endif /* LH_SRC_OS_SYSTEM_WIN_GDIPLUS_H */
