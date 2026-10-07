/**
 * @file gdiplus.h
 * @brief GDI+ process lifetime, frame draw state, and primitives.
 *
 * Win32 lives in `src/lh/os/system/win/gdiplus.c`. A frame (::lh_os_system_gdiplus_frame_t)
 * holds the Graphics, a reusable mask bitmap, path, and solid fill for one
 * canvas begin/end — not one call.
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
 * @brief Open a frame on @p hdc: Graphics, path, solid fill, and empty mask bitmap.
 *
 * Pair with ::lh_os_system_gdiplus_frame_end. ::lh_null on failure.
 */
lh_os_system_gdiplus_frame_t
lh_os_system_gdiplus_frame_begin(lh_ptr hdc);

/**
 * @brief Destroy the Graphics, path, solid fill, and bitmap of @p frame.
 *        No-op on ::lh_null.
 */
lh_void
lh_os_system_gdiplus_frame_end(lh_os_system_gdiplus_frame_t frame);

/**
 * @brief Paint a packed alpha mask in solid RGBA through @p frame.
 *
 * @p bits are high-bit-first @p bpp (1/2/4/8) rows of @p row_bytes. Reuses the
 * frame bitmap; grows it when a glyph is larger.
 *
 * @return ::lh_bool_false when nothing was drawn (bad arguments, no bitmap,
 *         a GDI+ call failed), so the caller can draw it another way.
 */
lh_bool_t
lh_os_system_gdiplus_frame_fill_mask(lh_os_system_gdiplus_frame_t frame, int x, int y, int width,
                                     int height, int bpp, int row_bytes, const lh_byte_t *bits,
                                     lh_byte_t r, lh_byte_t g, lh_byte_t b, lh_byte_t a);

/**
 * @brief Anti-aliased rounded fill of [left,right) × [top,bottom) through @p frame.
 *
 * Reuses the frame path and solid fill. Temporarily sets PixelOffsetMode half
 * and SmoothingMode anti-alias, then restores PixelOffsetMode none (and
 * SmoothingMode none) so mask draws stay aligned.
 *
 * @return ::lh_bool_false when nothing was drawn (no frame objects, the fill
 *         failed), so the caller can draw it another way.
 */
lh_bool_t
lh_os_system_gdiplus_frame_fill_round_rect(lh_os_system_gdiplus_frame_t frame, int left, int top,
                                           int right, int bottom, int radius, lh_byte_t r,
                                           lh_byte_t g, lh_byte_t b, lh_byte_t a);

/**
 * @brief Match the frame Graphics clip to [left,right) × [top,bottom).
 *
 * Needed because `SelectClipRgn` on the HDC after `GdipCreateFromHDC` does not
 * update an existing Graphics clip.
 */
lh_void
lh_os_system_gdiplus_frame_set_clip(lh_os_system_gdiplus_frame_t frame, int left, int top, int right,
                                    int bottom);

/**
 * @brief Clear the frame Graphics clip (no clipping).
 */
lh_void
lh_os_system_gdiplus_frame_clear_clip(lh_os_system_gdiplus_frame_t frame);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_GDIPLUS_H */
