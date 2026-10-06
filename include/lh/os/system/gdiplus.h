/**
 * @file gdiplus.h
 * @brief GDI+ process lifetime, frame draw state, and primitives.
 *
 * Win32 lives in `src/lh/os/system/win/gdiplus.c`. A frame (::lh_os_system_gdiplus_frame_t)
 * holds the Graphics (and a reusable mask bitmap) for one canvas begin/end —
 * not one call. Round rect still takes an HDC (rare); masks must go through a
 * frame.
 *
 * Requires ::LH_LIBRARY_OPTION_OS_WINDOW.
 */

#ifndef LH_OS_SYSTEM_GDIPLUS_H
#define LH_OS_SYSTEM_GDIPLUS_H

#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/ptr.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_OS_WINDOW
#    error "lh/os/system/gdiplus.h requires LH_LIBRARY_OPTION_OS_WINDOW"
#endif

/**
 * @typedef lh_os_system_gdiplus_frame_t
 * @brief Opaque GDI+ draw state for one canvas frame (Graphics + mask bitmap).
 */
typedef lh_ptr lh_os_system_gdiplus_frame_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Start GDI+ for one more user (only the first call starts it).
 *
 * @return ::lh_bool_true when GDI+ is running afterwards.
 */
lh_bool_t
lh_os_system_gdiplus_acquire(lh_void);

/**
 * @brief Drop one user; the last one stops GDI+.
 */
lh_void
lh_os_system_gdiplus_release(lh_void);

/**
 * @brief True while GDI+ is running.
 */
lh_bool_t
lh_os_system_gdiplus_is_ready(lh_void);

/**
 * @brief Open a frame on @p hdc: Graphics for the DC and an empty mask bitmap.
 *
 * Pair with ::lh_os_system_gdiplus_frame_end. ::lh_null on failure.
 */
lh_os_system_gdiplus_frame_t
lh_os_system_gdiplus_frame_begin(lh_ptr hdc);

/**
 * @brief Destroy the Graphics and bitmap of @p frame. No-op on ::lh_null.
 */
lh_void
lh_os_system_gdiplus_frame_end(lh_os_system_gdiplus_frame_t frame);

/**
 * @brief Paint a packed alpha mask in solid RGBA through @p frame.
 *
 * @p bits are high-bit-first @p bpp (1/2/4/8) rows of @p row_bytes. Reuses the
 * frame bitmap; grows it when a glyph is larger. Nothing on failure.
 */
lh_void
lh_os_system_gdiplus_frame_fill_mask(lh_os_system_gdiplus_frame_t frame, int x, int y, int width,
                                     int height, int bpp, int row_bytes, const lh_byte_t *bits,
                                     lh_byte_t r, lh_byte_t g, lh_byte_t b, lh_byte_t a);

/**
 * @brief Anti-aliased rounded fill of [left,right) × [top,bottom) on @p hdc.
 *
 * @p radius is the corner radius in pixels. Nothing on failure. Rare path:
 * builds its own Graphics for the call (masks must not).
 */
lh_void
lh_os_system_gdiplus_fill_round_rect(lh_ptr hdc, int left, int top, int right, int bottom,
                                     int radius, lh_byte_t r, lh_byte_t g, lh_byte_t b, lh_byte_t a);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_GDIPLUS_H */
