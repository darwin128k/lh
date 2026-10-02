/**
 * @file color.h
 * @brief OS-portable 8-bit RGBA color.
 *
 * A single value type `lh_color_t` with four `lh_uchar_t` channels
 * (red, green, blue, alpha). The alpha is "straight" (not premultiplied)
 * to match the convention every OS native paint API uses on input
 * (`COLORREF` ignores alpha entirely; `NSColor` separates color from
 * alpha but takes both as 0..1 floats; Xlib's `XSetForeground` takes a
 * 24-bit pixel).
 *
 * `lh_color_t` is OS-portable. Each backend converts to its native type at
 * the boundary: Win32 `COLORREF` (`0x00BBGGRR`), X11 `unsigned long`
 * pixel (24-bit RGB), Cocoa `NSColor *`. The conversion helpers live in
 * `src/lh/os/system/{win,posix,macos}/geom.h`.
 *
 * Requires nothing from `lh/os`. Safe in STM/embedded.
 */

#ifndef LH_COLOR_H
#define LH_COLOR_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @struct lh_color
 * @typedef lh_color_t
 * @brief 8-bit RGBA color, straight alpha. Range per channel: `0..255`.
 */
struct lh_color
{
    lh_uchar_t r;
    lh_uchar_t g;
    lh_uchar_t b;
    lh_uchar_t a;
};
typedef struct lh_color lh_color_t;

/**
 * @brief Build a `::lh_color_t` from explicit 0..255 channel values.
 */
lh_color_t
lh_color_make(lh_uchar_t r, lh_uchar_t g, lh_uchar_t b, lh_uchar_t a);

/**
 * @brief Build a `::lh_color_t` from a packed `0xAARRGGBB` `lh_uint_t`.
 *        Alpha `0xFF` is fully opaque.
 */
lh_color_t
lh_color_from_argb(lh_uint_t argb);

/**
 * @brief Pack a `::lh_color_t` as `0xAARRGGBB` `lh_uint_t`.
 */
lh_uint_t
lh_color_to_argb(lh_color_t self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_COLOR_H */