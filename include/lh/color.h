/**
 * @file color.h
 * @brief OS-portable 8-bit RGBA color.
 *
 * A single value type `lh_color_t` with four `lh_uchar_t` channels
 * (red, green, blue, alpha), straight (not premultiplied) alpha.
 *
 * A plain lh value type for layers above the OS. The `lh/os/system`
 * backends do not use it: each works in its own native color type, and
 * any mapping between the two belongs to the lh layer that needs it.
 *
 * Requires nothing from `lh/os`. Safe in STM/embedded.
 */

#ifndef LH_COLOR_H
#define LH_COLOR_H

#include <lh/char.h>
#include <lh/color/fields.h>
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
    lh_color_fields(lh_uchar_t);
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