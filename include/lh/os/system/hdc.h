/**
 * @file hdc.h
 * @brief Platform draw-target ops behind the GDI canvas backend.
 *
 * @p hdc is the opaque paint / memory DC (::lh_ptr). Win32 lives in
 * `src/lh/os/system/win/hdc.c`.
 *
 * Requires ::LH_LIBRARY_OPTION_OS_WINDOW.
 */

#ifndef LH_OS_SYSTEM_HDC_H
#define LH_OS_SYSTEM_HDC_H

#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/ptr.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_OS_WINDOW
#    error "lh/os/system/hdc.h requires LH_LIBRARY_OPTION_OS_WINDOW"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill [left,right) × [top,bottom) on @p hdc with opaque RGB.
 */
lh_void
lh_os_system_hdc_fill_rect(lh_ptr hdc, int left, int top, int right, int bottom, lh_byte_t r,
                           lh_byte_t g, lh_byte_t b);

/**
 * @brief Restrict drawing on @p hdc to [left,right) × [top,bottom).
 */
lh_void
lh_os_system_hdc_set_clip(lh_ptr hdc, int left, int top, int right, int bottom);

/**
 * @brief Remove the clip region of @p hdc.
 */
lh_void
lh_os_system_hdc_clear_clip(lh_ptr hdc);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_HDC_H */
